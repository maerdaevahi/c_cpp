#include "cycle_queue.h"
void init_cycle_queue(cycle_queue * cq) {
    cq->start = 0;
    cq->end = 0;
}

int test_cycle_queue_full(cycle_queue * cq) {
    return (cq->end + 1) % CAPACITY == cq->start;
}

int test_cycle_queue_empty(cycle_queue * cq) {
    return cq->start == cq->end;
}

void add_into_cycle_queue(cycle_queue * cq, int e) {
    cq->container[cq->end++] = e;
    cq->end %= CAPACITY;
}

int remove_from_cycle_queue(cycle_queue * cq) {
    int e = cq->container[cq->start++];
    cq->start %= CAPACITY;
    return e;
}