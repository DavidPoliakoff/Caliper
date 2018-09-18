/** End of day note: DO NOT MOVE TO STD::ARRAY'S FOR THE MULTI-ATTRIBUTE CASE. THE NUMBER OF METRICS IS A *RUNTIME* DECISION */
template<typename MeasurementPoint>
class Measurer{
  void init(/** something configure'y if we need it*/);
  MeasurementPoint GetInitialMeasurement(Caliper* c); // chrono::high_resolution_clock::now, mob_get_point
  MeasurementPoint GetSnapshotMeasurement(Caliper* c); // chrono::high_resolution_clock::now, mob_get_point
  void compareToRecent(Caliper* c, SnapshotBufferOffsetHandle* s, MeasurementPoint then, MeasurementPoint now); // this is for comparison to the most recent measurement point, "time since measurement epoch" in timestamp terms
  void handlePhaseBegin(Caliper* c, Snapshotbuffer* s, MeasurementPoint then, MeasurementPoint now);
  void handlePhaseSet(Caliper* c, Snapshotbuffer* s, MeasurementPoint then, MeasurementPoint now);
  void handlePhaseEnd(Caliper* c, Snapshotbuffer* s, MeasurementPoint then, MeasurementPoint now);
  //void phaseBegin(): DZP: phase begin is really just inserting the value of the start into the offset attribute
};

template<typename MeasurementUtility>
class MeasurementDriver{
   decltype(MeasurementUtility::GetMeasurement()) starting_point;
   /** Maintain
    *  1) A starting point
    *  2) The most recent point
    *  3) The most recent point per "phase-like attribute" per level
    */
   /** static? */
   void init( /** typical caliper stuff */){
     /** all :: could be moved to -> if we go with instances rather than statics (which we should) */
      MeasurementUtility::initialize();
      starting_point = MeasurementUtility::GetMeasurement();
   }
    void snapshot_cb(Caliper* c, int scope, const SnapshotRecord* trigger_info, SnapshotRecord* sbuf){
      auto now = MeasurementUtility::GetMeasurement();
      
    if ((record_duration || record_phases || record_offset) && scope & CALI_SCOPE_THREAD) {
        MeasurementUtility::compareToInitial(starting_point, now);
        //uint64_t  usec = chrono::duration_cast<chrono::microseconds>(now - tstart).count(); //DZP compare to original. "How long has it been since the start"
        //Variant v_usec = Variant(usec);
        auto recent = c->exchange(offset_attr, now);
        //Variant v_offs = c->abstract_exchange(timeoffs_attr, v_usec); // DZP "Exchange with some global 'last' the value I record here"
        // DZP v_usec now has the value of *the most recent* timestamp. Not the one we just took

        if (record_duration && !v_offs.empty()) {
            //uint64_t duration = usec - v_offs.to_uint();

            //sbuf->append(snapshot_duration_attr.id(), Variant(duration)); // DZP interact with sbuf using *most recent* and new
            MeasurementUtility::compareToRecent(now, recent);
        }

        if (record_phases && trigger_info) {
            Entry event = trigger_info->get(begin_evt_attr);

            cali_id_t evt_attr_id;
            Variant   v_level;

            if (event.is_empty()) // DZPQ: why not at least set "this is a begin or end or set" here, rather than look up the attribute() later?
                event = trigger_info->get(set_evt_attr);
            if (event.is_empty())
                event = trigger_info->get(end_evt_attr);
            if (event.is_empty())
                goto record_phases_exit;
            
            evt_attr_id = event.value().to_id();
            v_level     = trigger_info->get(lvl_attr).value();

            if (evt_attr_id == CALI_INV_ID || v_level.empty())
                goto record_phases_exit;

            if (event.attribute() == begin_evt_attr.id()) { // DZP: are begins sets? See comment below
                // begin/set event: save time for current entry
                
                c->abstract_set(make_offset_attribute(c, evt_attr_id, v_level.to_uint()), v_usec); //DZP set offset local to some other measurement
            } else if (event.attribute() == set_evt_attr.id())   {
                // set event: get saved time for current entry and calculate duration

                Variant v_p_usec    = 
                    c->abstract_exchange(make_offset_attribute(c, evt_attr_id, v_level.to_uint()), v_usec); //DZP calculate duration relative to some other measurement

                if (!v_p_usec.empty())
                    sbuf->append(phase_duration_attr.id(), Variant(usec - v_p_usec.to_uint())); // DZP: Append my duration observation to the snapshot buffer
            } else if (event.attribute() == end_evt_attr.id())   {
                // end event: get saved time for current entry and calculate duration

                Attribute offs_attr = 
                    find_offset_attribute(c, evt_attr_id, v_level.to_uint());

                if (offs_attr == Attribute::invalid)
                    goto record_phases_exit;

                Variant v_p_usec    = 
                    c->abstract_exchange(offs_attr, Variant()); // DZP pull my begin value from the snapshot

                if (!v_p_usec.empty())
                    sbuf->append(phase_duration_attr.id(), Variant(usec - v_p_usec.to_uint())); // DZP append to the buffer "my value is blah for duration"
            }
record_phases_exit:
            ;
        }
    }

    if (record_timestamp && (scope & CALI_SCOPE_PROCESS)) // DZP "if I am recording some global value at each snapshot, here append it to the snapshot buffer"
        sbuf->append(timestamp_attr.id(),
                     Variant(static_cast<int>(chrono::system_clock::to_time_t(chrono::system_clock::now()))));
      
   }
};
