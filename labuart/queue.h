#ifndef _G01_QUEUE_H_
#define _G01_QUEUE_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Queue structure using a linked list.
 */
struct queue;
/**
 * @brief Queue structure using a linked list.
 */
typedef struct queue queue_t;

/**
 * @brief Creates a queue structure.
 * @return The pointer to the queue structure created.
 */
queue_t * (queue_init)();

/**
 * @brief Adds an element to the back of the given queue.
 * @param queue The given queue.
 * @param byte The element to be added.
 */
void (queue_push)(queue_t * queue, uint8_t byte);

/**
 * @brief Gets the element at the front of the given queue.
 * @param queue The given queue.
 * @return The element at the front.
 */
uint8_t (queue_front)(queue_t * queue);

/**
 * @brief Removes the element at the front of the given queue.
 * @param queue The given queue.
 */
void (queue_pop)(queue_t * queue);

/**
 * @brief Checks if the given queue is empty.
 * @param queue The given queue.
 * @return True if the queue is empty, false otherwise.
 */
bool (queue_empty)(queue_t * queue);

/**
 * @brief Frees memory allocated by the given queue's items, and the queue itself.
 * @param queue The given queue.
 */
void (queue_delete)(queue_t * queue);


void (queue_clean)(queue_t * queue);
#endif
