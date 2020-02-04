
#include <slurm/slurm.h>
#include <slurm/slurm_wrapper.h>
#include <stdlib.h>
#include <string.h>

#define SLURM_NODE_IDLE 0
#define SLURM_NODE_USED 1
#define SLURM_NODE_UNHEALTH 2

// 返回操作失败的原因
char* error_info() {
    int err;

    err = slurm_get_errno();
    return slurm_strerror(err);
}

// 获取和解析SLURM集群的信息
int get_node_list_and_partition_list(node_list_t* node_list,
                                     partition_list_t* partition_list) {
    time_t now;
    node_info_msg_t* nodes;
    partition_info_msg_t* partitions;
    int ret;
    int i;
    int state;

    now = 0;
    ret = slurm_load_node(now, &nodes, SHOW_LOCAL);
    if (ret != 0) {
        return ret;
    }
    ret = slurm_load_partitions(now, &partitions, SHOW_LOCAL);
    if (ret != 0) {
        return ret;
    }
    slurm_populate_node_partitions(nodes, partitions);
    ret = slurm_get_errno();
    if (ret != 0) {
        return ret;
    }
    partition_list->partitions = NULL;
    if (partitions->record_count > 0) {
        partition_list->partitions = (partition_t*)malloc(
            partitions->record_count * sizeof(partition_t));
        if (NULL != partition_list->partitions) {
            partition_list->count = partitions->record_count;
            for (i = 0; i < partition_list->count; i++) {
                if (NULL != partitions->partition_array[i].name) {
                    partition_list->partitions[i].name = (char*)malloc(
                        strlen(partitions->partition_array[i].name) + 1);
                    if (NULL != partition_list->partitions[i].name) {
                        strcpy(partition_list->partitions[i].name,
                               partitions->partition_array[i].name);
                    }
                } else {
                    partition_list->partitions[i].name = NULL;
                }
                partition_list->partitions[i].state =
                    partitions->partition_array[i].state_up == PARTITION_UP;
            }
        } else {
            partition_list->count = 0;
        }
    }
    node_list->nodes = NULL;
    if (nodes->record_count > 0) {
        node_list->nodes =
            (node_t*)malloc(nodes->record_count * sizeof(node_t));
        if (NULL != node_list->nodes) {
            node_list->count = nodes->record_count;
            for (i = 0; i < node_list->count; i++) {
                if (NULL != nodes->node_array[i].name) {
                    node_list->nodes[i].name =
                        (char*)malloc(strlen(nodes->node_array[i].name) + 1);
                    if (NULL != node_list->nodes[i].name) {
                        strcpy(node_list->nodes[i].name,
                               nodes->node_array[i].name);
                    }
                } else {
                    node_list->nodes[i].name = NULL;
                }
                if (NULL != nodes->node_array[i].partitions) {
                    node_list->nodes[i].partitions = (char*)malloc(
                        strlen(nodes->node_array[i].partitions) + 1);
                    if (NULL != node_list->nodes[i].partitions) {
                        strcpy(node_list->nodes[i].partitions,
                               nodes->node_array[i].partitions);
                    }
                } else {
                    node_list->nodes[i].partitions = NULL;
                }
                node_list->nodes[i].reason = NULL;
                node_list->nodes[i].drain =
                    (nodes->node_array[i].node_state & NODE_STATE_DRAIN) > 0;
                if (node_list->nodes[i].drain) {
                    if (NULL != nodes->node_array[i].reason) {
                        node_list->nodes[i].reason = (char*)malloc(
                            strlen(nodes->node_array[i].reason) + 1);
                        if (NULL != node_list->nodes[i].reason) {
                            strcpy(node_list->nodes[i].reason,
                                   nodes->node_array[i].reason);
                        }
                    }
                }
                state = nodes->node_array[i].node_state & NODE_STATE_BASE;
                if (state == NODE_STATE_ALLOCATED ||
                    (nodes->node_array[i].node_state & NODE_STATE_COMPLETING)) {
                    node_list->nodes[i].state = SLURM_NODE_USED;
                } else if (state == NODE_STATE_IDLE) {
                    node_list->nodes[i].state = SLURM_NODE_IDLE;
                } else {
                    node_list->nodes[i].state = SLURM_NODE_UNHEALTH;
                }
                if (nodes->node_array[i].node_state &
                    (NODE_STATE_FAIL | NODE_STATE_MAINT |
                     NODE_STATE_NO_RESPOND | NODE_STATE_POWER_SAVE |
                     NODE_STATE_POWERING_DOWN | NODE_STATE_RES)) {
                    node_list->nodes[i].state = SLURM_NODE_UNHEALTH;
                }
            }
        } else {
            node_list->count = 0;
        }
    }
    slurm_free_node_info_msg(nodes);
    slurm_free_partition_info_msg(partitions);
    return 0;
}

// 释放集群信息占用的内存
void free_node_list_and_partition_list(node_list_t* node_list,
                                       partition_list_t* partition_list) {
    int i;

    for (i = 0; i < node_list->count; i++) {
        free(node_list->nodes[i].name);
        free(node_list->nodes[i].partitions);
    }
    free(node_list->nodes);
    for (i = 0; i < partition_list->count; i++) {
        free(partition_list->partitions[i].name);
    }
    free(partition_list->partitions);
}

// 获取集群排队作业的信息
int get_job_list(job_list_t* job_list) {
    time_t now;
    job_info_msg_t* jobs;
    int ret;
    int i;

    now = 0;
    ret = slurm_load_jobs(now, &jobs, SHOW_LOCAL);
    if (ret != 0) {
        return ret;
    }
    job_list->count = 0;
    job_list->jobs = NULL;
    if (jobs->record_count > 0) {
        job_list->jobs = (job_t*)malloc(jobs->record_count * sizeof(job_t));
        if (NULL != job_list->jobs) {
            for (i = 0; i < jobs->record_count; i++) {
                if (jobs->job_array[i].job_state == JOB_PENDING) {
                    if (NULL != jobs->job_array[i].partition) {
                        job_list->jobs[job_list->count].partition =
                            (char*)malloc(strlen(jobs->job_array[i].partition) +
                                          1);
                        if (NULL != job_list->jobs[job_list->count].partition) {
                            strcpy(job_list->jobs[job_list->count].partition,
                                   jobs->job_array[i].partition);
                            job_list->jobs[job_list->count].nodes =
                                jobs->job_array[i].num_nodes;
                            job_list->count++;
                        }
                    }
                }
            }
        }
    }
    return 0;
}

// 释放作业信息占用的内存
void free_job_list(job_list_t* job_list) {
    int i;

    for (i = 0; i < job_list->count; i++) {
        free(job_list->jobs[i].partition);
    }
    free(job_list->jobs);
}

// 禁用节点
int drain_nodes(char* node_names, char* reason) {
    update_node_msg_t node;

    memset(&node, 0, sizeof(node));
    node.node_names = node_names;
    node.node_state = NODE_STATE_DRAIN;
    node.reason = reason;

    return slurm_update_node(&node);
}

// 启用节点
int undrain_nodes(char* node_names) {
    update_node_msg_t node;

    memset(&node, 0, sizeof(node));
    node.node_names = node_names;
    node.node_state = NODE_STATE_UNDRAIN;

    return slurm_update_node(&node);
}
