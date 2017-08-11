//Generated by translate.py
#include "gotcha_mpi_io.c"
#include <gotcha/gotcha.h>
#include <mpi.h>
#include <stdio.h>
#include "dissectio.h"
#include <caliper/Annotation.h>

#define NFUNCS 53

static int (*WRAP_MPI_File_close) (MPI_File * fh);
static int dissectio_MPI_File_close (MPI_File * fh){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_close");
		io_annot.end();
	return WRAP_MPI_File_close(fh);
}

static int (*WRAP_MPI_File_set_size) (MPI_File fh, MPI_Offset size);
static int dissectio_MPI_File_set_size (MPI_File fh, MPI_Offset size){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_set_size");
		io_annot.end();
	return WRAP_MPI_File_set_size(fh, size);
}

static int (*WRAP_MPI_File_iread_at) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
static int dissectio_MPI_File_iread_at (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_iread_at");
		io_annot.end();
	return WRAP_MPI_File_iread_at(fh, offset, buf, count, datatype, request);
}

static int (*WRAP_MPI_File_iread) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
static int dissectio_MPI_File_iread (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_iread");
		io_annot.end();
	return WRAP_MPI_File_iread(fh, buf, count, datatype, request);
}

static int (*WRAP_MPI_File_iread_shared) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
static int dissectio_MPI_File_iread_shared (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_iread_shared");
		io_annot.end();
	return WRAP_MPI_File_iread_shared(fh, buf, count, datatype, request);
}

static int (*WRAP_MPI_File_iwrite_at) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
static int dissectio_MPI_File_iwrite_at (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Request *request){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_iwrite_at");
		io_annot.end();
	return WRAP_MPI_File_iwrite_at(fh, offset, buf, count, datatype, request);
}

static int (*WRAP_MPI_File_iwrite) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
static int dissectio_MPI_File_iwrite (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_iwrite");
		io_annot.end();
	return WRAP_MPI_File_iwrite(fh, buf, count, datatype, request);
}

static int (*WRAP_MPI_File_iwrite_shared) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request);
static int dissectio_MPI_File_iwrite_shared (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_iwrite_shared");
		io_annot.end();
	return WRAP_MPI_File_iwrite_shared(fh, buf, count, datatype, request);
}

static int (*WRAP_MPI_File_open) (MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh);
static int dissectio_MPI_File_open (MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_open");
		io_annot.end();
	return WRAP_MPI_File_open(comm, filename, amode, info, fh);
}

static int (*WRAP_MPI_File_read_all_begin) (MPI_File fh, void *buf, int count,  MPI_Datatype datatype);
static int dissectio_MPI_File_read_all_begin (MPI_File fh, void *buf, int count,  MPI_Datatype datatype){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_read_all_begin");
		io_annot.end();
	return WRAP_MPI_File_read_all_begin(fh, buf, count, datatype);
}

static int (*WRAP_MPI_File_read_all) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_read_all (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_read_all");
		io_annot.end();
	return WRAP_MPI_File_read_all(fh, buf, count, datatype, status);
}

static int (*WRAP_MPI_File_read_at_all) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_read_at_all (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_read_at_all");
		io_annot.end();
	return WRAP_MPI_File_read_at_all(fh, offset, buf, count, datatype, status);
}

static int (*WRAP_MPI_File_read_at_all_begin) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype);
static int dissectio_MPI_File_read_at_all_begin (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_read_at_all_begin");
		io_annot.end();
	return WRAP_MPI_File_read_at_all_begin(fh, offset, buf, count, datatype);
}

static int (*WRAP_MPI_File_read_at) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_read_at (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_read_at");
		io_annot.end();
	return WRAP_MPI_File_read_at(fh, offset, buf, count, datatype, status);
}

static int (*WRAP_MPI_File_read) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_read (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_read");
		io_annot.end();
	return WRAP_MPI_File_read(fh, buf, count, datatype, status);
}

static int (*WRAP_MPI_File_read_ordered_begin) (MPI_File fh, void *buf, int count, MPI_Datatype datatype);
static int dissectio_MPI_File_read_ordered_begin (MPI_File fh, void *buf, int count, MPI_Datatype datatype){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_read_ordered_begin");
		io_annot.end();
	return WRAP_MPI_File_read_ordered_begin(fh, buf, count, datatype);
}

static int (*WRAP_MPI_File_read_ordered) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_read_ordered (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_read_ordered");
		io_annot.end();
	return WRAP_MPI_File_read_ordered(fh, buf, count, datatype, status);
}

static int (*WRAP_MPI_File_read_shared) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_read_shared (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_read_shared");
		io_annot.end();
	return WRAP_MPI_File_read_shared(fh, buf, count, datatype, status);
}

static int (*WRAP_MPI_File_set_view) (MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info);
static int dissectio_MPI_File_set_view (MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_set_view");
		io_annot.end();
	return WRAP_MPI_File_set_view(fh, disp, etype, filetype, datarep, info);
}

static int (*WRAP_MPI_File_sync) (MPI_File fh);
static int dissectio_MPI_File_sync (MPI_File fh){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_sync");
		io_annot.end();
	return WRAP_MPI_File_sync(fh);
}

static int (*WRAP_MPI_File_write_all_begin) (MPI_File fh, void *buf, int count, MPI_Datatype datatype);
static int dissectio_MPI_File_write_all_begin (MPI_File fh, void *buf, int count, MPI_Datatype datatype){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_write_all_begin");
		io_annot.end();
	return WRAP_MPI_File_write_all_begin(fh, buf, count, datatype);
}

static int (*WRAP_MPI_File_write_all) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_write_all (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_write_all");
		io_annot.end();
	return WRAP_MPI_File_write_all(fh, buf, count, datatype, status);
}

static int (*WRAP_MPI_File_write_at_all_begin) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype);
static int dissectio_MPI_File_write_at_all_begin (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_write_at_all_begin");
		io_annot.end();
	return WRAP_MPI_File_write_at_all_begin(fh, offset, buf, count, datatype);
}

static int (*WRAP_MPI_File_write_at_all) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_write_at_all (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_write_at_all");
		io_annot.end();
	return WRAP_MPI_File_write_at_all(fh, offset, buf, count, datatype, status);
}

static int (*WRAP_MPI_File_write_at) (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_write_at (MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_write_at");
		io_annot.end();
	return WRAP_MPI_File_write_at(fh, offset, buf, count, datatype, status);
}

static int (*WRAP_MPI_File_write) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_write (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_write");
		io_annot.end();
	return WRAP_MPI_File_write(fh, buf, count, datatype, status);
}

static int (*WRAP_MPI_File_write_ordered_begin) (MPI_File fh, void *buf, int count, MPI_Datatype datatype);
static int dissectio_MPI_File_write_ordered_begin (MPI_File fh, void *buf, int count, MPI_Datatype datatype){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_write_ordered_begin");
		io_annot.end();
	return WRAP_MPI_File_write_ordered_begin(fh, buf, count, datatype);
}

static int (*WRAP_MPI_File_write_ordered) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_write_ordered (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_write_ordered");
		io_annot.end();
	return WRAP_MPI_File_write_ordered(fh, buf, count, datatype, status);
}

static int (*WRAP_MPI_File_write_shared) (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status);
static int dissectio_MPI_File_write_shared (MPI_File fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_File_write_shared");
		io_annot.end();
	return WRAP_MPI_File_write_shared(fh, buf, count, datatype, status);
}

static int (*WRAP_MPI_Init_thread) (int *argc, char ***argv, int required, int *provided);
static int dissectio_MPI_Init_thread (int *argc, char ***argv, int required, int *provided){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Init_thread");
		io_annot.end();
	return WRAP_MPI_Init_thread(argc, argv, required, provided);
}

static double (*WRAP_MPI_Wtime) ();
static double dissectio_MPI_Wtime (){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Wtime");
		io_annot.end();
	return WRAP_MPI_Wtime();
}

static int (*WRAP_MPI_Barrier) (MPI_Comm comm);
static int dissectio_MPI_Barrier (MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Barrier");
		io_annot.end();
	return WRAP_MPI_Barrier(comm);
}

static int (*WRAP_MPI_Bcast) (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm);
static int dissectio_MPI_Bcast (void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Bcast");
		io_annot.end();
	return WRAP_MPI_Bcast(buffer, count, datatype, root, comm);
}

static int (*WRAP_MPI_Gather) (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm);
static int dissectio_MPI_Gather (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Gather");
		io_annot.end();
	return WRAP_MPI_Gather(sbuf, scount, stype, rbuf, rcount, rtype, root, comm);
}

static int (*WRAP_MPI_Scatter) (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm);
static int dissectio_MPI_Scatter (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Scatter");
		io_annot.end();
	return WRAP_MPI_Scatter(sbuf, scount, stype, rbuf, rcount, rtype, root, comm);
}

static int (*WRAP_MPI_Gatherv) (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int *rcount, int *displs, MPI_Datatype rtype, int root, MPI_Comm comm);
static int dissectio_MPI_Gatherv (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int *rcount, int *displs, MPI_Datatype rtype, int root, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Gatherv");
		io_annot.end();
	return WRAP_MPI_Gatherv(sbuf, scount, stype, rbuf, rcount, displs, rtype, root, comm);
}

static int (*WRAP_MPI_Scatterv) (void *sbuf, int *scount, int *displa, MPI_Datatype stype, void *rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm);
static int dissectio_MPI_Scatterv (void *sbuf, int *scount, int *displa, MPI_Datatype stype, void *rbuf, int rcount, MPI_Datatype rtype, int root, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Scatterv");
		io_annot.end();
	return WRAP_MPI_Scatterv(sbuf, scount, displa, stype, rbuf, rcount, rtype, root, comm);
}

static int (*WRAP_MPI_Allgather) (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int rcount, MPI_Datatype rtype, MPI_Comm comm);
static int dissectio_MPI_Allgather (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int rcount, MPI_Datatype rtype, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Allgather");
		io_annot.end();
	return WRAP_MPI_Allgather(sbuf, scount, stype, rbuf, rcount, rtype, comm);
}

static int (*WRAP_MPI_Allgatherv) (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int *rcount, int *displs, MPI_Datatype rtype, MPI_Comm comm);
static int dissectio_MPI_Allgatherv (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int *rcount, int *displs, MPI_Datatype rtype, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Allgatherv");
		io_annot.end();
	return WRAP_MPI_Allgatherv(sbuf, scount, stype, rbuf, rcount, displs, rtype, comm);
}

static int (*WRAP_MPI_Alltoall) (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int rcount, MPI_Datatype rtype, MPI_Comm comm);
static int dissectio_MPI_Alltoall (void *sbuf, int scount, MPI_Datatype stype, void *rbuf, int rcount, MPI_Datatype rtype, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Alltoall");
		io_annot.end();
	return WRAP_MPI_Alltoall(sbuf, scount, stype, rbuf, rcount, rtype, comm);
}

static int (*WRAP_MPI_Reduce) (void *sbuf, void *rbuf, int count, MPI_Datatype stype, MPI_Op op, int root, MPI_Comm comm);
static int dissectio_MPI_Reduce (void *sbuf, void *rbuf, int count, MPI_Datatype stype, MPI_Op op, int root, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Reduce");
		io_annot.end();
	return WRAP_MPI_Reduce(sbuf, rbuf, count, stype, op, root, comm);
}

static int (*WRAP_MPI_Allreduce) (void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
static int dissectio_MPI_Allreduce (void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Allreduce");
		io_annot.end();
	return WRAP_MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
}

static int (*WRAP_MPI_Reduce_scatter) (void *sbuf, void *rbuf, int *rcounts, MPI_Datatype stype, MPI_Op op, MPI_Comm comm);
static int dissectio_MPI_Reduce_scatter (void *sbuf, void *rbuf, int *rcounts, MPI_Datatype stype, MPI_Op op, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Reduce_scatter");
		io_annot.end();
	return WRAP_MPI_Reduce_scatter(sbuf, rbuf, rcounts, stype, op, comm);
}

static int (*WRAP_MPI_Scan) (void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
static int dissectio_MPI_Scan (void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Scan");
		io_annot.end();
	return WRAP_MPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
}

static int (*WRAP_MPI_Type_commit) (MPI_Datatype * datatype);
static int dissectio_MPI_Type_commit (MPI_Datatype * datatype){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Type_commit");
		io_annot.end();
	return WRAP_MPI_Type_commit(datatype);
}

static int (*WRAP_MPI_Type_contiguous) (int count, MPI_Datatype oldtype, MPI_Datatype *newtype);
static int dissectio_MPI_Type_contiguous (int count, MPI_Datatype oldtype, MPI_Datatype *newtype){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Type_contiguous");
		io_annot.end();
	return WRAP_MPI_Type_contiguous(count, oldtype, newtype);
}

static int (*WRAP_MPI_Type_extent) (MPI_Datatype datatype, MPI_Aint * extent);
static int dissectio_MPI_Type_extent (MPI_Datatype datatype, MPI_Aint * extent){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Type_extent");
		io_annot.end();
	return WRAP_MPI_Type_extent(datatype, extent);
}

static int (*WRAP_MPI_Type_free) (MPI_Datatype * datatype);
static int dissectio_MPI_Type_free (MPI_Datatype * datatype){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Type_free");
		io_annot.end();
	return WRAP_MPI_Type_free(datatype);
}

static int (*WRAP_MPI_Type_hindexed) (int count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype);
static int dissectio_MPI_Type_hindexed (int count, int *array_of_blocklengths, MPI_Aint *array_of_displacements, MPI_Datatype oldtype, MPI_Datatype *newtype){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Type_hindexed");
		io_annot.end();
	return WRAP_MPI_Type_hindexed(count, array_of_blocklengths, array_of_displacements, oldtype, newtype);
}

static int (*WRAP_MPI_Op_create) (MPI_User_function * function, int commute, MPI_Op *op);
static int dissectio_MPI_Op_create (MPI_User_function * function, int commute, MPI_Op *op){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Op_create");
		io_annot.end();
	return WRAP_MPI_Op_create(function, commute, op);
}

static int (*WRAP_MPI_Op_free) (MPI_Op * op);
static int dissectio_MPI_Op_free (MPI_Op * op){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Op_free");
		io_annot.end();
	return WRAP_MPI_Op_free(op);
}

static int (*WRAP_MPI_Type_get_envelope) (MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner);
static int dissectio_MPI_Type_get_envelope (MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Type_get_envelope");
		io_annot.end();
	return WRAP_MPI_Type_get_envelope(datatype, num_integers, num_addresses, num_datatypes, combiner);
}

static int (*WRAP_MPI_Type_size) (MPI_Datatype datatype, int *size);
static int dissectio_MPI_Type_size (MPI_Datatype datatype, int *size){
		cali::Annotation io_annot("io");
		io_annot.set("MPI_Type_size");
		io_annot.end();
	return WRAP_MPI_Type_size(datatype, size);
}


struct gotcha_binding_t wrap_mpi_io [] = {
	{ "MPI_File_close", dissectio_MPI_File_close, &WRAP_MPI_File_close },
	{ "MPI_File_set_size", dissectio_MPI_File_set_size, &WRAP_MPI_File_set_size },
	{ "MPI_File_iread_at", dissectio_MPI_File_iread_at, &WRAP_MPI_File_iread_at },
	{ "MPI_File_iread", dissectio_MPI_File_iread, &WRAP_MPI_File_iread },
	{ "MPI_File_iread_shared", dissectio_MPI_File_iread_shared, &WRAP_MPI_File_iread_shared },
	{ "MPI_File_iwrite_at", dissectio_MPI_File_iwrite_at, &WRAP_MPI_File_iwrite_at },
	{ "MPI_File_iwrite", dissectio_MPI_File_iwrite, &WRAP_MPI_File_iwrite },
	{ "MPI_File_iwrite_shared", dissectio_MPI_File_iwrite_shared, &WRAP_MPI_File_iwrite_shared },
	{ "MPI_File_open", dissectio_MPI_File_open, &WRAP_MPI_File_open },
	{ "MPI_File_read_all_begin", dissectio_MPI_File_read_all_begin, &WRAP_MPI_File_read_all_begin },
	{ "MPI_File_read_all", dissectio_MPI_File_read_all, &WRAP_MPI_File_read_all },
	{ "MPI_File_read_at_all", dissectio_MPI_File_read_at_all, &WRAP_MPI_File_read_at_all },
	{ "MPI_File_read_at_all_begin", dissectio_MPI_File_read_at_all_begin, &WRAP_MPI_File_read_at_all_begin },
	{ "MPI_File_read_at", dissectio_MPI_File_read_at, &WRAP_MPI_File_read_at },
	{ "MPI_File_read", dissectio_MPI_File_read, &WRAP_MPI_File_read },
	{ "MPI_File_read_ordered_begin", dissectio_MPI_File_read_ordered_begin, &WRAP_MPI_File_read_ordered_begin },
	{ "MPI_File_read_ordered", dissectio_MPI_File_read_ordered, &WRAP_MPI_File_read_ordered },
	{ "MPI_File_read_shared", dissectio_MPI_File_read_shared, &WRAP_MPI_File_read_shared },
	{ "MPI_File_set_view", dissectio_MPI_File_set_view, &WRAP_MPI_File_set_view },
	{ "MPI_File_sync", dissectio_MPI_File_sync, &WRAP_MPI_File_sync },
	{ "MPI_File_write_all_begin", dissectio_MPI_File_write_all_begin, &WRAP_MPI_File_write_all_begin },
	{ "MPI_File_write_all", dissectio_MPI_File_write_all, &WRAP_MPI_File_write_all },
	{ "MPI_File_write_at_all_begin", dissectio_MPI_File_write_at_all_begin, &WRAP_MPI_File_write_at_all_begin },
	{ "MPI_File_write_at_all", dissectio_MPI_File_write_at_all, &WRAP_MPI_File_write_at_all },
	{ "MPI_File_write_at", dissectio_MPI_File_write_at, &WRAP_MPI_File_write_at },
	{ "MPI_File_write", dissectio_MPI_File_write, &WRAP_MPI_File_write },
	{ "MPI_File_write_ordered_begin", dissectio_MPI_File_write_ordered_begin, &WRAP_MPI_File_write_ordered_begin },
	{ "MPI_File_write_ordered", dissectio_MPI_File_write_ordered, &WRAP_MPI_File_write_ordered },
	{ "MPI_File_write_shared", dissectio_MPI_File_write_shared, &WRAP_MPI_File_write_shared },
	{ "MPI_Init_thread", dissectio_MPI_Init_thread, &WRAP_MPI_Init_thread },
	{ "MPI_Wtime", dissectio_MPI_Wtime, &WRAP_MPI_Wtime },
	{ "MPI_Barrier", dissectio_MPI_Barrier, &WRAP_MPI_Barrier },
	{ "MPI_Bcast", dissectio_MPI_Bcast, &WRAP_MPI_Bcast },
	{ "MPI_Gather", dissectio_MPI_Gather, &WRAP_MPI_Gather },
	{ "MPI_Scatter", dissectio_MPI_Scatter, &WRAP_MPI_Scatter },
	{ "MPI_Gatherv", dissectio_MPI_Gatherv, &WRAP_MPI_Gatherv },
	{ "MPI_Scatterv", dissectio_MPI_Scatterv, &WRAP_MPI_Scatterv },
	{ "MPI_Allgather", dissectio_MPI_Allgather, &WRAP_MPI_Allgather },
	{ "MPI_Allgatherv", dissectio_MPI_Allgatherv, &WRAP_MPI_Allgatherv },
	{ "MPI_Alltoall", dissectio_MPI_Alltoall, &WRAP_MPI_Alltoall },
	{ "MPI_Reduce", dissectio_MPI_Reduce, &WRAP_MPI_Reduce },
	{ "MPI_Allreduce", dissectio_MPI_Allreduce, &WRAP_MPI_Allreduce },
	{ "MPI_Reduce_scatter", dissectio_MPI_Reduce_scatter, &WRAP_MPI_Reduce_scatter },
	{ "MPI_Scan", dissectio_MPI_Scan, &WRAP_MPI_Scan },
	{ "MPI_Type_commit", dissectio_MPI_Type_commit, &WRAP_MPI_Type_commit },
	{ "MPI_Type_contiguous", dissectio_MPI_Type_contiguous, &WRAP_MPI_Type_contiguous },
	{ "MPI_Type_extent", dissectio_MPI_Type_extent, &WRAP_MPI_Type_extent },
	{ "MPI_Type_free", dissectio_MPI_Type_free, &WRAP_MPI_Type_free },
	{ "MPI_Type_hindexed", dissectio_MPI_Type_hindexed, &WRAP_MPI_Type_hindexed },
	{ "MPI_Op_create", dissectio_MPI_Op_create, &WRAP_MPI_Op_create },
	{ "MPI_Op_free", dissectio_MPI_Op_free, &WRAP_MPI_Op_free },
	{ "MPI_Type_get_envelope", dissectio_MPI_Type_get_envelope, &WRAP_MPI_Type_get_envelope },
	{ "MPI_Type_size", dissectio_MPI_Type_size, &WRAP_MPI_Type_size },
};

void init_gotcha_mpi_io () {
	gotcha_wrap(wrap_mpi_io, NFUNCS, "dissectio");
}
