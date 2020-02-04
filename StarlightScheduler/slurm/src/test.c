
#include <slurm/slurm_wrapper.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int ret;
    char* err;
    node_list_t node_list;
    partition_list_t partition_list;
    job_list_t job_list;
    ret = get_node_list_and_partition_list(&node_list, &partition_list);
    if (ret != 0) {
        err = error_info();
        printf("%s\n", err);
        free(err);
        return 1;
    }
    ret = drain_nodes(node_list.nodes[0].name, "test");
    if (ret != 0) {
        err = error_info();
        printf("%s\n", err);
        free(err);
        return 1;
    }
    ret = undrain_nodes(node_list.nodes[0].name);
    if (ret != 0) {
        err = error_info();
        printf("%s\n", err);
        free(err);
        return 1;
    }
    ret = get_job_list(&job_list);
    if (ret != 0) {
        err = error_info();
        printf("%s\n", err);
        free(err);
        return 1;
    }
    free_job_list(&job_list);
    free_node_list_and_partition_list(&node_list, &partition_list);
    return 0;
}