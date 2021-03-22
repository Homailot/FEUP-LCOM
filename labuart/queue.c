#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct queue_item {
    uint8_t byte;
    struct queue_item * next;
} queue_item_t;

struct queue {
    queue_item_t * front;
    queue_item_t * back;
};

queue_t * (queue_init)() {
    queue_t * new_queue = malloc(sizeof(queue_t));

    new_queue->front = NULL;
    new_queue->back = NULL;

    return new_queue;
}

void (queue_push)(queue_t * queue, uint8_t byte) {
    queue_item_t * queue_item = (queue_item_t *) malloc(sizeof(queue_item_t));

    queue_item->byte = byte;
    queue_item->next = NULL;

    if(queue->front == NULL) {
        queue->front = queue_item;
    } else {
        queue->back->next = queue_item;
    }

    queue->back = queue_item;
}

uint8_t (queue_front)(queue_t * queue) {
    if(queue->front == NULL) return 0;
    return queue->front->byte;
}

void (queue_pop)(queue_t * queue) {
    if(queue->front == NULL) return;

    queue_item_t * item = queue->front;
    queue->front = item->next;
    free(item);
}

bool (queue_empty)(queue_t * queue) {
    return queue->front == NULL;
}

void (queue_delete)(queue_t * queue) {
    while(!queue_empty(queue)) {
        queue_pop(queue);
    }

    free(queue);
}

void (queue_clean)(queue_t * queue) {
    while(!queue_empty(queue)) {
        queue_pop(queue);
    }
}
