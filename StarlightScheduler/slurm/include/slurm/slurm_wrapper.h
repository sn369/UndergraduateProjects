
#ifndef _SLURM_WRAPPER_H
#define _SLURM_WRAPPER_H

typedef struct {
    char* name;
    char* partitions;
    int state;
    int drain;
    char* reason;
} node_t;

typedef struct {
    char* name;
    int state;
} partition_t;

typedef struct {
    node_t* nodes;
    int count;
} node_list_t;

typedef struct {
    partition_t* partitions;
    int count;
} partition_list_t;

typedef struct {
    char* partition;
    int nodes;
} job_t;

typedef struct {
    job_t* jobs;
    int count;
} job_list_t;

extern char* error_info();
extern int get_node_list_and_partition_list(node_list_t*, partition_list_t*);
extern void free_node_list_and_partition_list(node_list_t*, partition_list_t*);
extern int get_job_list(job_list_t*);
extern void free_job_list(job_list_t*);
extern int drain_nodes(char*, char*);
extern int undrain_nodes(char*);

#endif  //_SLURM_WRAPPER_H
