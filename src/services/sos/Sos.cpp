/// \file  TextLog.cpp
/// \brief Caliper text log service

#include "../CaliperService.h"

#include "caliper/Caliper.h"
#include "caliper/SnapshotRecord.h"

#include "caliper/common/Log.h"
#include "caliper/common/RuntimeConfig.h"
#include "caliper/common/SnapshotTextFormatter.h"

#include "caliper/common/util/split.hpp"

#include <algorithm>
#include <fstream>
#include <functional>
#include <iterator>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <sstream>
#include <vector>

#include<sos.h>

using namespace cali;
using namespace std;

namespace
{

const ConfigSet::Entry   configdata[] = {
    { "trigger", CALI_TYPE_STRING, "",
      "List of attributes for which to write text log entries",
      "Colon-separated list of attributes for which to write text log entries."
    },
    { "formatstring", CALI_TYPE_STRING, "",
      "Format of the text log output",
      "Description of the text log format output. If empty, a default one will be created."
    },
    { "filename", CALI_TYPE_STRING, "stdout",
      "File name for event record stream. Auto-generated by default.",
      "File name for event record stream. Either one of\n"
      "   stdout: Standard output stream,\n"
      "   stderr: Standard error stream,\n"
      "   none:   No output,\n"
      " or a file name. The default is stdout\n"
    },
    ConfigSet::Terminator
};

class SosService
{
    ConfigSet                   config;
    std::mutex                  trigger_attr_mutex;
    typedef std::map<cali_id_t, Attribute> TriggerAttributeMap;
    TriggerAttributeMap         trigger_attr_map;
    std::vector<std::string>    trigger_attr_names;
    // TODO: determine if this is necessary
    int                         pub_count; 
    SnapshotTextFormatter       formatter;

    enum class Stream { None, File, StdErr, StdOut };

    Stream                      m_stream;
    ofstream                    m_ofstream;

    Attribute                   set_event_attr;   
    Attribute                   end_event_attr;

    std::mutex                  stream_mutex;
    
    static unique_ptr<SosService> 
                                s_sos;
    SOS_runtime *sos_runtime;
    SOS_pub *sos_publication_handle;
    typedef std::map<cali_id_t, SOS_val_type> AttrIdPackTypeMap;
    AttrIdPackTypeMap attr_to_sos_type;

    static std::pair<bool,SOS_val_type> sos_type_for_cali_type(cali::Attribute attr){
      switch( attr.type() ){
        case CALI_TYPE_INV: return std::make_pair(false,SOS_VAL_TYPE_BYTES); 
        case CALI_TYPE_USR: return std::make_pair(false,SOS_VAL_TYPE_BYTES); 
        case CALI_TYPE_TYPE: return std::make_pair(false,SOS_VAL_TYPE_BYTES); 
        case CALI_TYPE_INT: return std::make_pair(true,SOS_VAL_TYPE_INT); 
        case CALI_TYPE_UINT: return std::make_pair(true,SOS_VAL_TYPE_INT); 
        case CALI_TYPE_STRING: return std::make_pair(true,SOS_VAL_TYPE_STRING); 
        case CALI_TYPE_ADDR: return std::make_pair(true,SOS_VAL_TYPE_INT); 
        case CALI_TYPE_DOUBLE: return std::make_pair(true,SOS_VAL_TYPE_INT); 
        case CALI_TYPE_BOOL: return std::make_pair(true,SOS_VAL_TYPE_INT); 
      }
    }

    void create_attribute(Caliper* c, const Attribute& attr) {
        if (attr.skip_events())
            return;
        std::pair<bool,SOS_val_type> validAndType = sos_type_for_cali_type(attr);
        if(validAndType.first){
          attr_to_sos_type.insert(std::make_pair(attr.id(), validAndType.second));
        }
    }

    const void* variantValue(Variant in){
      cali_variant_t inner_variant = in.c_variant();
      return cali_variant_get_data(&inner_variant);
    }
    void process_snapshot(Caliper* c, const SnapshotRecord* trigger_info, const SnapshotRecord* snapshot) {
        //// operate only on cali.snapshot.event.end attributes for now
        //if (!trigger_info)
        //    return;

        //Entry event = trigger_info->get(end_event_attr);

        //if (event.is_empty())
        //    event = trigger_info->get(set_event_attr);
        //if (event.is_empty())
        //    return;

        //Attribute trigger_attr { Attribute::invalid };

        //{
        //    std::lock_guard<std::mutex> lock(trigger_attr_mutex);

        //    TriggerAttributeMap::const_iterator it = 
        //        trigger_attr_map.find(event.value().to_id());

        //    if (it != trigger_attr_map.end())
        //        trigger_attr = it->second;
        //}
        //if (trigger_attr == Attribute::invalid || snapshot->get(trigger_attr).is_empty())
        //    return;

        std::cout<<"FLUSHING\n";
        auto unpacked = snapshot->unpack(*c);
        for(std::pair<const Attribute,std::vector<Variant>> & iter : unpacked){
          auto search = attr_to_sos_type.find(iter.first.id());
          if(search != attr_to_sos_type.end()){
            for(auto item : iter.second){
              const char* stringData = item.to_string().c_str();
              std::cout<<stringData;
              SOS_pack(sos_publication_handle,iter.first.name().c_str(),SOS_VAL_TYPE_STRING,(void*)stringData);
            }
          }
        }
        SOS_publish(sos_publication_handle);
        //SnapshotRecord::Sizes size = snapshot->size();
        //SnapshotRecord::Data  data = snapshot->data();

        //for (size_t n = 0; n < size.n_nodes; ++n)
        //    entrylist.push_back(Entry(data.node_entries[n]));
        //for (size_t n = 0; n < size.n_immediate; ++n)
        //    entrylist.push_back(Entry(data.immediate_attr[n], data.immediate_data[n]));

        //ostringstream os;
        //
        //formatter.print(os, c, entrylist) << std::endl;

        //std::lock_guard<std::mutex>
        //    g(stream_mutex);

        //get_stream() << os.str();
    }

    void post_init(Caliper* c) {
      sos_runtime = NULL;
      SOS_init(NULL, NULL, &sos_runtime, SOS_ROLE_CLIENT, SOS_RECEIVES_NO_FEEDBACK, NULL);
      SOS_pub_create(sos_runtime, &sos_publication_handle, (char *)"caliper.data", SOS_NATURE_CREATE_OUTPUT);
    }

    // static callbacks

    static void create_attr_cb(Caliper* c, const Attribute& attr) {
        s_sos->create_attribute(c, attr);
    }

    static void process_snapshot_cb(Caliper* c, const SnapshotRecord* trigger_info, const SnapshotRecord* snapshot) {
        s_sos->process_snapshot(c, trigger_info, snapshot);
    }

    static void post_init_cb(Caliper* c) { 
        s_sos->post_init(c);
    }

    SosService(Caliper* c)
        : config(RuntimeConfig::init("sos", configdata)),
          set_event_attr(Attribute::invalid),
          end_event_attr(Attribute::invalid)
        { 

            util::split(config.get("trigger").to_string(), ':', 
                        std::back_inserter(trigger_attr_names));

            c->events().create_attr_evt.connect(&SosService::create_attr_cb);
            c->events().post_init_evt.connect(&SosService::post_init_cb);
            c->events().process_snapshot.connect(&SosService::process_snapshot_cb);

            Log(1).stream() << "Registered text log service" << std::endl;
        }

public:

    static void sos_register(Caliper* c) {
        s_sos.reset(new SosService(c));
    }

}; // SosService

unique_ptr<SosService> SosService::s_sos { nullptr };

} // namespace

namespace cali
{
    CaliperService sos_service = { "sos", ::SosService::sos_register };
} // namespace cali
