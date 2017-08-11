//Generated by translate.py

#include <gotcha/gotcha.h>
#include <hdf5.h>
#include <stdio.h>
#include "dissectio.h"
#include <caliper/Annotation.h>

#define NFUNCS 71

static hid_t (*WRAP_H5Fcreate) (const char *name, unsigned flags, hid_t fcpl_id, hid_t fapl_id);
static hid_t dissectio_H5Fcreate (const char *name, unsigned flags, hid_t fcpl_id, hid_t fapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Fcreate");
		io_annot.end();
	return WRAP_H5Fcreate(name, flags, fcpl_id, fapl_id);
}

static hid_t (*WRAP_H5Fopen) (const char *name, unsigned flags, hid_t fapl_id);
static hid_t dissectio_H5Fopen (const char *name, unsigned flags, hid_t fapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Fopen");
		io_annot.end();
	return WRAP_H5Fopen(name, flags, fapl_id);
}

static herr_t (*WRAP_H5Fclose) (hid_t file_id);
static herr_t dissectio_H5Fclose (hid_t file_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Fclose");
		io_annot.end();
	return WRAP_H5Fclose(file_id);
}

static herr_t (*WRAP_H5Fflush) (hid_t object_id, H5F_scope_t scope);
static herr_t dissectio_H5Fflush (hid_t object_id, H5F_scope_t scope){
		cali::Annotation io_annot("io");
		io_annot.set("H5Fflush");
		io_annot.end();
	return WRAP_H5Fflush(object_id, scope);
}

static hid_t (*WRAP_H5Gcreate1) (hid_t loc_id, const char *name, size_t size_hint);
static hid_t dissectio_H5Gcreate1 (hid_t loc_id, const char *name, size_t size_hint){
		cali::Annotation io_annot("io");
		io_annot.set("H5Gcreate1");
		io_annot.end();
	return WRAP_H5Gcreate1(loc_id, name, size_hint);
}

static hid_t (*WRAP_H5Gcreate2) (hid_t loc_id, const char *name, hid_t lcpl_id, hid_t gcpl_id, hid_t gapl_id);
static hid_t dissectio_H5Gcreate2 (hid_t loc_id, const char *name, hid_t lcpl_id, hid_t gcpl_id, hid_t gapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Gcreate2");
		io_annot.end();
	return WRAP_H5Gcreate2(loc_id, name, lcpl_id, gcpl_id, gapl_id);
}

static herr_t (*WRAP_H5Gget_objinfo) (hid_t loc_id, const char *name, hbool_t follow_link, H5G_stat_t *statbuf);
static herr_t dissectio_H5Gget_objinfo (hid_t loc_id, const char *name, hbool_t follow_link, H5G_stat_t *statbuf){
		cali::Annotation io_annot("io");
		io_annot.set("H5Gget_objinfo");
		io_annot.end();
	return WRAP_H5Gget_objinfo(loc_id, name, follow_link, statbuf);
}

static int (*WRAP_H5Giterate) (hid_t loc_id, const char *name, int *idx, H5G_iterate_t operator, void *operator_data);
static int dissectio_H5Giterate (hid_t loc_id, const char *name, int *idx, H5G_iterate_t operator, void *operator_data){
		cali::Annotation io_annot("io");
		io_annot.set("H5Giterate");
		io_annot.end();
	return WRAP_H5Giterate(loc_id, name, idx, operator, operator_data);
}

static hid_t (*WRAP_H5Gopen1) (hid_t loc_id, const char *name);
static hid_t dissectio_H5Gopen1 (hid_t loc_id, const char *name){
		cali::Annotation io_annot("io");
		io_annot.set("H5Gopen1");
		io_annot.end();
	return WRAP_H5Gopen1(loc_id, name);
}

static hid_t (*WRAP_H5Gopen2) (hid_t loc_id, const char * name, hid_t gapl_id);
static hid_t dissectio_H5Gopen2 (hid_t loc_id, const char * name, hid_t gapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Gopen2");
		io_annot.end();
	return WRAP_H5Gopen2(loc_id, name, gapl_id);
}

static herr_t (*WRAP_H5Gclose) (hid_t group_id);
static herr_t dissectio_H5Gclose (hid_t group_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Gclose");
		io_annot.end();
	return WRAP_H5Gclose(group_id);
}

static herr_t (*WRAP_H5Dclose) (hid_t dataset_id);
static herr_t dissectio_H5Dclose (hid_t dataset_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Dclose");
		io_annot.end();
	return WRAP_H5Dclose(dataset_id);
}

static hid_t (*WRAP_H5Dcreate1) (hid_t loc_id, const char *name, hid_t type_id, hid_t space_id, hid_t dcpl_id);
static hid_t dissectio_H5Dcreate1 (hid_t loc_id, const char *name, hid_t type_id, hid_t space_id, hid_t dcpl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Dcreate1");
		io_annot.end();
	return WRAP_H5Dcreate1(loc_id, name, type_id, space_id, dcpl_id);
}

static hid_t (*WRAP_H5Dcreate2) (hid_t loc_id, const char *name, hid_t dtype_id, hid_t space_id, hid_t lcpl_id, hid_t dcpl_id, hid_t dapl_id);
static hid_t dissectio_H5Dcreate2 (hid_t loc_id, const char *name, hid_t dtype_id, hid_t space_id, hid_t lcpl_id, hid_t dcpl_id, hid_t dapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Dcreate2");
		io_annot.end();
	return WRAP_H5Dcreate2(loc_id, name, dtype_id, space_id, lcpl_id, dcpl_id, dapl_id);
}

static hid_t (*WRAP_H5Dget_create_plist) (hid_t dataset_id);
static hid_t dissectio_H5Dget_create_plist (hid_t dataset_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Dget_create_plist");
		io_annot.end();
	return WRAP_H5Dget_create_plist(dataset_id);
}

static hid_t (*WRAP_H5Dget_space) (hid_t dataset_id);
static hid_t dissectio_H5Dget_space (hid_t dataset_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Dget_space");
		io_annot.end();
	return WRAP_H5Dget_space(dataset_id);
}

static hid_t (*WRAP_H5Dget_type) (hid_t dataset_id);
static hid_t dissectio_H5Dget_type (hid_t dataset_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Dget_type");
		io_annot.end();
	return WRAP_H5Dget_type(dataset_id);
}

static hid_t (*WRAP_H5Dopen1) (hid_t loc_id, const char *name);
static hid_t dissectio_H5Dopen1 (hid_t loc_id, const char *name){
		cali::Annotation io_annot("io");
		io_annot.set("H5Dopen1");
		io_annot.end();
	return WRAP_H5Dopen1(loc_id, name);
}

static hid_t (*WRAP_H5Dopen2) (hid_t loc_id, const char *name, hid_t dapl_id);
static hid_t dissectio_H5Dopen2 (hid_t loc_id, const char *name, hid_t dapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Dopen2");
		io_annot.end();
	return WRAP_H5Dopen2(loc_id, name, dapl_id);
}

static herr_t (*WRAP_H5Dread) (hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, void * buf);
static herr_t dissectio_H5Dread (hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, void * buf){
		cali::Annotation io_annot("io");
		io_annot.set("H5Dread");
		io_annot.end();
	return WRAP_H5Dread(dataset_id, mem_type_id, mem_space_id, file_space_id, xfer_plist_id, buf);
}

static herr_t (*WRAP_H5Dwrite) (hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, const void * buf);
static herr_t dissectio_H5Dwrite (hid_t dataset_id, hid_t mem_type_id, hid_t mem_space_id, hid_t file_space_id, hid_t xfer_plist_id, const void * buf){
		cali::Annotation io_annot("io");
		io_annot.set("H5Dwrite");
		io_annot.end();
	return WRAP_H5Dwrite(dataset_id, mem_type_id, mem_space_id, file_space_id, xfer_plist_id, buf);
}

static herr_t (*WRAP_H5Sclose) (hid_t space_id);
static herr_t dissectio_H5Sclose (hid_t space_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Sclose");
		io_annot.end();
	return WRAP_H5Sclose(space_id);
}

static hid_t (*WRAP_H5Scopy) (hid_t space_id);
static hid_t dissectio_H5Scopy (hid_t space_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Scopy");
		io_annot.end();
	return WRAP_H5Scopy(space_id);
}

static hid_t (*WRAP_H5Screate) (H5S_class_t type);
static hid_t dissectio_H5Screate (H5S_class_t type){
		cali::Annotation io_annot("io");
		io_annot.set("H5Screate");
		io_annot.end();
	return WRAP_H5Screate(type);
}

static hid_t (*WRAP_H5Screate_simple) (int rank, const hsize_t * current_dims, const hsize_t * maximum_dims);
static hid_t dissectio_H5Screate_simple (int rank, const hsize_t * current_dims, const hsize_t * maximum_dims){
		cali::Annotation io_annot("io");
		io_annot.set("H5Screate_simple");
		io_annot.end();
	return WRAP_H5Screate_simple(rank, current_dims, maximum_dims);
}

static hssize_t (*WRAP_H5Sget_select_npoints) (hid_t space_id);
static hssize_t dissectio_H5Sget_select_npoints (hid_t space_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Sget_select_npoints");
		io_annot.end();
	return WRAP_H5Sget_select_npoints(space_id);
}

static int (*WRAP_H5Sget_simple_extent_dims) (hid_t space_id, hsize_t *dims, hsize_t *maxdims);
static int dissectio_H5Sget_simple_extent_dims (hid_t space_id, hsize_t *dims, hsize_t *maxdims){
		cali::Annotation io_annot("io");
		io_annot.set("H5Sget_simple_extent_dims");
		io_annot.end();
	return WRAP_H5Sget_simple_extent_dims(space_id, dims, maxdims);
}

static hssize_t (*WRAP_H5Sget_select_elem_npoints) (hid_t space_id);
static hssize_t dissectio_H5Sget_select_elem_npoints (hid_t space_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Sget_select_elem_npoints");
		io_annot.end();
	return WRAP_H5Sget_select_elem_npoints(space_id);
}

static herr_t (*WRAP_H5Sselect_elements) (hid_t space_id, H5S_seloper_t op, size_t num_elements, const hsize_t *coord);
static herr_t dissectio_H5Sselect_elements (hid_t space_id, H5S_seloper_t op, size_t num_elements, const hsize_t *coord){
		cali::Annotation io_annot("io");
		io_annot.set("H5Sselect_elements");
		io_annot.end();
	return WRAP_H5Sselect_elements(space_id, op, num_elements, coord);
}

static herr_t (*WRAP_H5Sselect_hyperslab) (hid_t space_id, H5S_seloper_t op, const hsize_t *start, const hsize_t *stride, const hsize_t *count, const hsize_t *block);
static herr_t dissectio_H5Sselect_hyperslab (hid_t space_id, H5S_seloper_t op, const hsize_t *start, const hsize_t *stride, const hsize_t *count, const hsize_t *block){
		cali::Annotation io_annot("io");
		io_annot.set("H5Sselect_hyperslab");
		io_annot.end();
	return WRAP_H5Sselect_hyperslab(space_id, op, start, stride, count, block);
}

static herr_t (*WRAP_H5Sselect_none) (hid_t space_id);
static herr_t dissectio_H5Sselect_none (hid_t space_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Sselect_none");
		io_annot.end();
	return WRAP_H5Sselect_none(space_id);
}

static herr_t (*WRAP_H5Tclose) (hid_t type_id);
static herr_t dissectio_H5Tclose (hid_t type_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Tclose");
		io_annot.end();
	return WRAP_H5Tclose(type_id);
}

static hid_t (*WRAP_H5Tcopy) (hid_t type_id);
static hid_t dissectio_H5Tcopy (hid_t type_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Tcopy");
		io_annot.end();
	return WRAP_H5Tcopy(type_id);
}

static H5T_class_t (*WRAP_H5Tget_class) (hid_t type_id);
static H5T_class_t dissectio_H5Tget_class (hid_t type_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Tget_class");
		io_annot.end();
	return WRAP_H5Tget_class(type_id);
}

static size_t (*WRAP_H5Tget_size) (hid_t type_id);
static size_t dissectio_H5Tget_size (hid_t type_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Tget_size");
		io_annot.end();
	return WRAP_H5Tget_size(type_id);
}

static herr_t (*WRAP_H5Tset_size) (hid_t type_id, size_t size);
static herr_t dissectio_H5Tset_size (hid_t type_id, size_t size){
		cali::Annotation io_annot("io");
		io_annot.set("H5Tset_size");
		io_annot.end();
	return WRAP_H5Tset_size(type_id, size);
}

static hid_t (*WRAP_H5Tcreate) (H5T_class_t type, size_t size);
static hid_t dissectio_H5Tcreate (H5T_class_t type, size_t size){
		cali::Annotation io_annot("io");
		io_annot.set("H5Tcreate");
		io_annot.end();
	return WRAP_H5Tcreate(type, size);
}

static herr_t (*WRAP_H5Tinsert) (hid_t dtype_id, const char * name, size_t offset, hid_t field_id);
static herr_t dissectio_H5Tinsert (hid_t dtype_id, const char * name, size_t offset, hid_t field_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Tinsert");
		io_annot.end();
	return WRAP_H5Tinsert(dtype_id, name, offset, field_id);
}

static hid_t (*WRAP_H5Acreate1) (hid_t loc_id, const char *attr_name, hid_t type_id, hid_t space_id, hid_t acpl_id);
static hid_t dissectio_H5Acreate1 (hid_t loc_id, const char *attr_name, hid_t type_id, hid_t space_id, hid_t acpl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Acreate1");
		io_annot.end();
	return WRAP_H5Acreate1(loc_id, attr_name, type_id, space_id, acpl_id);
}

static hid_t (*WRAP_H5Acreate2) (hid_t loc_id, const char *attr_name, hid_t type_id, hid_t space_id, hid_t acpl_id, hid_t aapl_id);
static hid_t dissectio_H5Acreate2 (hid_t loc_id, const char *attr_name, hid_t type_id, hid_t space_id, hid_t acpl_id, hid_t aapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Acreate2");
		io_annot.end();
	return WRAP_H5Acreate2(loc_id, attr_name, type_id, space_id, acpl_id, aapl_id);
}

static herr_t (*WRAP_H5Aclose) (hid_t attr_id);
static herr_t dissectio_H5Aclose (hid_t attr_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Aclose");
		io_annot.end();
	return WRAP_H5Aclose(attr_id);
}

static ssize_t (*WRAP_H5Aget_name) (hid_t attr_id, size_t buf_size, char *buf);
static ssize_t dissectio_H5Aget_name (hid_t attr_id, size_t buf_size, char *buf){
		cali::Annotation io_annot("io");
		io_annot.set("H5Aget_name");
		io_annot.end();
	return WRAP_H5Aget_name(attr_id, buf_size, buf);
}

static int (*WRAP_H5Aget_num_attrs) (hid_t loc_id);
static int dissectio_H5Aget_num_attrs (hid_t loc_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Aget_num_attrs");
		io_annot.end();
	return WRAP_H5Aget_num_attrs(loc_id);
}

static hid_t (*WRAP_H5Aget_space) (hid_t attr_id);
static hid_t dissectio_H5Aget_space (hid_t attr_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Aget_space");
		io_annot.end();
	return WRAP_H5Aget_space(attr_id);
}

static hid_t (*WRAP_H5Aget_type) (hid_t attr_id);
static hid_t dissectio_H5Aget_type (hid_t attr_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Aget_type");
		io_annot.end();
	return WRAP_H5Aget_type(attr_id);
}

static hid_t (*WRAP_H5Aopen) (hid_t obj_id, const char *attr_name, hid_t aapl_id);
static hid_t dissectio_H5Aopen (hid_t obj_id, const char *attr_name, hid_t aapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Aopen");
		io_annot.end();
	return WRAP_H5Aopen(obj_id, attr_name, aapl_id);
}

static hid_t (*WRAP_H5Aopen_by_idx) (hid_t loc_id, const char *obj_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, hid_t aapl_id, hid_t lapl_id);
static hid_t dissectio_H5Aopen_by_idx (hid_t loc_id, const char *obj_name, H5_index_t idx_type, H5_iter_order_t order, hsize_t n, hid_t aapl_id, hid_t lapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Aopen_by_idx");
		io_annot.end();
	return WRAP_H5Aopen_by_idx(loc_id, obj_name, idx_type, order, n, aapl_id, lapl_id);
}

static hid_t (*WRAP_H5Aopen_idx) (hid_t loc_id, unsigned int idx);
static hid_t dissectio_H5Aopen_idx (hid_t loc_id, unsigned int idx){
		cali::Annotation io_annot("io");
		io_annot.set("H5Aopen_idx");
		io_annot.end();
	return WRAP_H5Aopen_idx(loc_id, idx);
}

static hid_t (*WRAP_H5Aopen_by_name) (hid_t loc_id, const char *obj_name, const char *attr_name, hid_t aapl_id, hid_t lapl_id);
static hid_t dissectio_H5Aopen_by_name (hid_t loc_id, const char *obj_name, const char *attr_name, hid_t aapl_id, hid_t lapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Aopen_by_name");
		io_annot.end();
	return WRAP_H5Aopen_by_name(loc_id, obj_name, attr_name, aapl_id, lapl_id);
}

static hid_t (*WRAP_H5Aopen_name) (hid_t loc_id, const char *name);
static hid_t dissectio_H5Aopen_name (hid_t loc_id, const char *name){
		cali::Annotation io_annot("io");
		io_annot.set("H5Aopen_name");
		io_annot.end();
	return WRAP_H5Aopen_name(loc_id, name);
}

static herr_t (*WRAP_H5Aread) (hid_t attr_id, hid_t mem_type_id, void *buf);
static herr_t dissectio_H5Aread (hid_t attr_id, hid_t mem_type_id, void *buf){
		cali::Annotation io_annot("io");
		io_annot.set("H5Aread");
		io_annot.end();
	return WRAP_H5Aread(attr_id, mem_type_id, buf);
}

static herr_t (*WRAP_H5Awrite) (hid_t attr_id, hid_t mem_type_id, const void *buf);
static herr_t dissectio_H5Awrite (hid_t attr_id, hid_t mem_type_id, const void *buf){
		cali::Annotation io_annot("io");
		io_annot.set("H5Awrite");
		io_annot.end();
	return WRAP_H5Awrite(attr_id, mem_type_id, buf);
}

static herr_t (*WRAP_H5Pclose) (hid_t plist);
static herr_t dissectio_H5Pclose (hid_t plist){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pclose");
		io_annot.end();
	return WRAP_H5Pclose(plist);
}

static hid_t (*WRAP_H5Pcreate) (hid_t cls_id);
static hid_t dissectio_H5Pcreate (hid_t cls_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pcreate");
		io_annot.end();
	return WRAP_H5Pcreate(cls_id);
}

static int (*WRAP_H5Pget_chunk) (hid_t plist, int max_ndims, hsize_t * dims);
static int dissectio_H5Pget_chunk (hid_t plist, int max_ndims, hsize_t * dims){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pget_chunk");
		io_annot.end();
	return WRAP_H5Pget_chunk(plist, max_ndims, dims);
}

static herr_t (*WRAP_H5Pget_mdc_config) (hid_t plist_id, H5AC_cache_config_t *config_ptr);
static herr_t dissectio_H5Pget_mdc_config (hid_t plist_id, H5AC_cache_config_t *config_ptr){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pget_mdc_config");
		io_annot.end();
	return WRAP_H5Pget_mdc_config(plist_id, config_ptr);
}

static herr_t (*WRAP_H5Pset_alignment) (hid_t plist, hsize_t threshold, hsize_t alignment);
static herr_t dissectio_H5Pset_alignment (hid_t plist, hsize_t threshold, hsize_t alignment){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pset_alignment");
		io_annot.end();
	return WRAP_H5Pset_alignment(plist, threshold, alignment);
}

static herr_t (*WRAP_H5Pset_chunk) (hid_t plist, int ndims, const hsize_t * dim);
static herr_t dissectio_H5Pset_chunk (hid_t plist, int ndims, const hsize_t * dim){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pset_chunk");
		io_annot.end();
	return WRAP_H5Pset_chunk(plist, ndims, dim);
}

static herr_t (*WRAP_H5Pset_dxpl_mpio) (hid_t dxpl_id, H5FD_mpio_xfer_t xfer_mode);
static herr_t dissectio_H5Pset_dxpl_mpio (hid_t dxpl_id, H5FD_mpio_xfer_t xfer_mode){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pset_dxpl_mpio");
		io_annot.end();
	return WRAP_H5Pset_dxpl_mpio(dxpl_id, xfer_mode);
}

static herr_t (*WRAP_H5Pset_fapl_mpio) (hid_t fapl_id, MPI_Comm comm, MPI_Info info);
static herr_t dissectio_H5Pset_fapl_mpio (hid_t fapl_id, MPI_Comm comm, MPI_Info info){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pset_fapl_mpio");
		io_annot.end();
	return WRAP_H5Pset_fapl_mpio(fapl_id, comm, info);
}

static herr_t (*WRAP_H5Pset_fapl_core) (hid_t fapl_id, size_t increment, hbool_t backing_store);
static herr_t dissectio_H5Pset_fapl_core (hid_t fapl_id, size_t increment, hbool_t backing_store){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pset_fapl_core");
		io_annot.end();
	return WRAP_H5Pset_fapl_core(fapl_id, increment, backing_store);
}

static herr_t (*WRAP_H5Pget_istore_k) (hid_t fcpl_id, unsigned * ik);
static herr_t dissectio_H5Pget_istore_k (hid_t fcpl_id, unsigned * ik){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pget_istore_k");
		io_annot.end();
	return WRAP_H5Pget_istore_k(fcpl_id, ik);
}

static herr_t (*WRAP_H5Pset_mdc_config) (hid_t plist_id, H5AC_cache_config_t *config_ptr);
static herr_t dissectio_H5Pset_mdc_config (hid_t plist_id, H5AC_cache_config_t *config_ptr){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pset_mdc_config");
		io_annot.end();
	return WRAP_H5Pset_mdc_config(plist_id, config_ptr);
}

static herr_t (*WRAP_H5Pset_meta_block_size) (hid_t fapl_id, hsize_t size);
static herr_t dissectio_H5Pset_meta_block_size (hid_t fapl_id, hsize_t size){
		cali::Annotation io_annot("io");
		io_annot.set("H5Pset_meta_block_size");
		io_annot.end();
	return WRAP_H5Pset_meta_block_size(fapl_id, size);
}

static htri_t (*WRAP_H5Lexists) (hid_t loc_id, const char *name, hid_t lapl_id);
static htri_t dissectio_H5Lexists (hid_t loc_id, const char *name, hid_t lapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Lexists");
		io_annot.end();
	return WRAP_H5Lexists(loc_id, name, lapl_id);
}

static herr_t (*WRAP_H5Lget_val) (hid_t link_loc_id, const char *link_name, void *linkval_buff, size_t size, hid_t lapl_id);
static herr_t dissectio_H5Lget_val (hid_t link_loc_id, const char *link_name, void *linkval_buff, size_t size, hid_t lapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Lget_val");
		io_annot.end();
	return WRAP_H5Lget_val(link_loc_id, link_name, linkval_buff, size, lapl_id);
}

static herr_t (*WRAP_H5Literate) (hid_t group_id, H5_index_t index_type, H5_iter_order_t order, hsize_t *idx, H5L_iterate_t op, void *op_data);
static herr_t dissectio_H5Literate (hid_t group_id, H5_index_t index_type, H5_iter_order_t order, hsize_t *idx, H5L_iterate_t op, void *op_data){
		cali::Annotation io_annot("io");
		io_annot.set("H5Literate");
		io_annot.end();
	return WRAP_H5Literate(group_id, index_type, order, idx, op, op_data);
}

static herr_t (*WRAP_H5Oclose) (hid_t object_id);
static herr_t dissectio_H5Oclose (hid_t object_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Oclose");
		io_annot.end();
	return WRAP_H5Oclose(object_id);
}

static herr_t (*WRAP_H5Oget_info) (hid_t object_id, H5O_info_t *object_info);
static herr_t dissectio_H5Oget_info (hid_t object_id, H5O_info_t *object_info){
		cali::Annotation io_annot("io");
		io_annot.set("H5Oget_info");
		io_annot.end();
	return WRAP_H5Oget_info(object_id, object_info);
}

static herr_t (*WRAP_H5Oget_info_by_name) (hid_t loc_id, const char *object_name, H5O_info_t *object_info, hid_t lapl_id);
static herr_t dissectio_H5Oget_info_by_name (hid_t loc_id, const char *object_name, H5O_info_t *object_info, hid_t lapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Oget_info_by_name");
		io_annot.end();
	return WRAP_H5Oget_info_by_name(loc_id, object_name, object_info, lapl_id);
}

static hid_t (*WRAP_H5Oopen) (hid_t loc_id, const char *name, hid_t lapl_id);
static hid_t dissectio_H5Oopen (hid_t loc_id, const char *name, hid_t lapl_id){
		cali::Annotation io_annot("io");
		io_annot.set("H5Oopen");
		io_annot.end();
	return WRAP_H5Oopen(loc_id, name, lapl_id);
}


struct gotcha_binding_t wrap_hdf5 [] = {
	{ "H5Fcreate", dissectio_H5Fcreate, &WRAP_H5Fcreate },
	{ "H5Fopen", dissectio_H5Fopen, &WRAP_H5Fopen },
	{ "H5Fclose", dissectio_H5Fclose, &WRAP_H5Fclose },
	{ "H5Fflush", dissectio_H5Fflush, &WRAP_H5Fflush },
	{ "H5Gcreate1", dissectio_H5Gcreate1, &WRAP_H5Gcreate1 },
	{ "H5Gcreate2", dissectio_H5Gcreate2, &WRAP_H5Gcreate2 },
	{ "H5Gget_objinfo", dissectio_H5Gget_objinfo, &WRAP_H5Gget_objinfo },
	{ "H5Giterate", dissectio_H5Giterate, &WRAP_H5Giterate },
	{ "H5Gopen1", dissectio_H5Gopen1, &WRAP_H5Gopen1 },
	{ "H5Gopen2", dissectio_H5Gopen2, &WRAP_H5Gopen2 },
	{ "H5Gclose", dissectio_H5Gclose, &WRAP_H5Gclose },
	{ "H5Dclose", dissectio_H5Dclose, &WRAP_H5Dclose },
	{ "H5Dcreate1", dissectio_H5Dcreate1, &WRAP_H5Dcreate1 },
	{ "H5Dcreate2", dissectio_H5Dcreate2, &WRAP_H5Dcreate2 },
	{ "H5Dget_create_plist", dissectio_H5Dget_create_plist, &WRAP_H5Dget_create_plist },
	{ "H5Dget_space", dissectio_H5Dget_space, &WRAP_H5Dget_space },
	{ "H5Dget_type", dissectio_H5Dget_type, &WRAP_H5Dget_type },
	{ "H5Dopen1", dissectio_H5Dopen1, &WRAP_H5Dopen1 },
	{ "H5Dopen2", dissectio_H5Dopen2, &WRAP_H5Dopen2 },
	{ "H5Dread", dissectio_H5Dread, &WRAP_H5Dread },
	{ "H5Dwrite", dissectio_H5Dwrite, &WRAP_H5Dwrite },
	{ "H5Sclose", dissectio_H5Sclose, &WRAP_H5Sclose },
	{ "H5Scopy", dissectio_H5Scopy, &WRAP_H5Scopy },
	{ "H5Screate", dissectio_H5Screate, &WRAP_H5Screate },
	{ "H5Screate_simple", dissectio_H5Screate_simple, &WRAP_H5Screate_simple },
	{ "H5Sget_select_npoints", dissectio_H5Sget_select_npoints, &WRAP_H5Sget_select_npoints },
	{ "H5Sget_simple_extent_dims", dissectio_H5Sget_simple_extent_dims, &WRAP_H5Sget_simple_extent_dims },
	{ "H5Sget_select_elem_npoints", dissectio_H5Sget_select_elem_npoints, &WRAP_H5Sget_select_elem_npoints },
	{ "H5Sselect_elements", dissectio_H5Sselect_elements, &WRAP_H5Sselect_elements },
	{ "H5Sselect_hyperslab", dissectio_H5Sselect_hyperslab, &WRAP_H5Sselect_hyperslab },
	{ "H5Sselect_none", dissectio_H5Sselect_none, &WRAP_H5Sselect_none },
	{ "H5Tclose", dissectio_H5Tclose, &WRAP_H5Tclose },
	{ "H5Tcopy", dissectio_H5Tcopy, &WRAP_H5Tcopy },
	{ "H5Tget_class", dissectio_H5Tget_class, &WRAP_H5Tget_class },
	{ "H5Tget_size", dissectio_H5Tget_size, &WRAP_H5Tget_size },
	{ "H5Tset_size", dissectio_H5Tset_size, &WRAP_H5Tset_size },
	{ "H5Tcreate", dissectio_H5Tcreate, &WRAP_H5Tcreate },
	{ "H5Tinsert", dissectio_H5Tinsert, &WRAP_H5Tinsert },
	{ "H5Acreate1", dissectio_H5Acreate1, &WRAP_H5Acreate1 },
	{ "H5Acreate2", dissectio_H5Acreate2, &WRAP_H5Acreate2 },
	{ "H5Aclose", dissectio_H5Aclose, &WRAP_H5Aclose },
	{ "H5Aget_name", dissectio_H5Aget_name, &WRAP_H5Aget_name },
	{ "H5Aget_num_attrs", dissectio_H5Aget_num_attrs, &WRAP_H5Aget_num_attrs },
	{ "H5Aget_space", dissectio_H5Aget_space, &WRAP_H5Aget_space },
	{ "H5Aget_type", dissectio_H5Aget_type, &WRAP_H5Aget_type },
	{ "H5Aopen", dissectio_H5Aopen, &WRAP_H5Aopen },
	{ "H5Aopen_by_idx", dissectio_H5Aopen_by_idx, &WRAP_H5Aopen_by_idx },
	{ "H5Aopen_idx", dissectio_H5Aopen_idx, &WRAP_H5Aopen_idx },
	{ "H5Aopen_by_name", dissectio_H5Aopen_by_name, &WRAP_H5Aopen_by_name },
	{ "H5Aopen_name", dissectio_H5Aopen_name, &WRAP_H5Aopen_name },
	{ "H5Aread", dissectio_H5Aread, &WRAP_H5Aread },
	{ "H5Awrite", dissectio_H5Awrite, &WRAP_H5Awrite },
	{ "H5Pclose", dissectio_H5Pclose, &WRAP_H5Pclose },
	{ "H5Pcreate", dissectio_H5Pcreate, &WRAP_H5Pcreate },
	{ "H5Pget_chunk", dissectio_H5Pget_chunk, &WRAP_H5Pget_chunk },
	{ "H5Pget_mdc_config", dissectio_H5Pget_mdc_config, &WRAP_H5Pget_mdc_config },
	{ "H5Pset_alignment", dissectio_H5Pset_alignment, &WRAP_H5Pset_alignment },
	{ "H5Pset_chunk", dissectio_H5Pset_chunk, &WRAP_H5Pset_chunk },
	{ "H5Pset_dxpl_mpio", dissectio_H5Pset_dxpl_mpio, &WRAP_H5Pset_dxpl_mpio },
	{ "H5Pset_fapl_mpio", dissectio_H5Pset_fapl_mpio, &WRAP_H5Pset_fapl_mpio },
	{ "H5Pset_fapl_core", dissectio_H5Pset_fapl_core, &WRAP_H5Pset_fapl_core },
	{ "H5Pget_istore_k", dissectio_H5Pget_istore_k, &WRAP_H5Pget_istore_k },
	{ "H5Pset_mdc_config", dissectio_H5Pset_mdc_config, &WRAP_H5Pset_mdc_config },
	{ "H5Pset_meta_block_size", dissectio_H5Pset_meta_block_size, &WRAP_H5Pset_meta_block_size },
	{ "H5Lexists", dissectio_H5Lexists, &WRAP_H5Lexists },
	{ "H5Lget_val", dissectio_H5Lget_val, &WRAP_H5Lget_val },
	{ "H5Literate", dissectio_H5Literate, &WRAP_H5Literate },
	{ "H5Oclose", dissectio_H5Oclose, &WRAP_H5Oclose },
	{ "H5Oget_info", dissectio_H5Oget_info, &WRAP_H5Oget_info },
	{ "H5Oget_info_by_name", dissectio_H5Oget_info_by_name, &WRAP_H5Oget_info_by_name },
	{ "H5Oopen", dissectio_H5Oopen, &WRAP_H5Oopen },
};

void init_gotcha_hdf5 () {
	gotcha_wrap(wrap_hdf5, NFUNCS, "dissectio");
}
