#ifndef __CYCLE_QUEUE__
#define __CYCLE_QUEUE__
#define CAPACITY 32
typedef struct cycle_queue {
    int container[CAPACITY];
    int start, end;
} cycle_queue;
void init_cycle_queue(cycle_queue * cq);
int test_cycle_queue_full(cycle_queue * cq);
int test_cycle_queue_empty(cycle_queue * cq);
void add_into_cycle_queue(cycle_queue * cq, int e);
int remove_from_cycle_queue(cycle_queue * cq);
#endif