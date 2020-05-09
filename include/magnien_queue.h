/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#ifndef VLG_MAGNIEN_QUEUE_H
#define VLG_MAGNIEN_QUEUE_H

#include "magnien_prelim.h"

/******** QUEUE functions - begin *********/

typedef struct queue{
    int size;
    int *elts;
    int begin;
    int end;
} queue;

queue *empty_queue(int size);

void free_queue(queue *q);

int is_empty_queue(queue *q);

int is_full_queue(queue *q);

void queue_add(queue *q, int e);

int queue_get(queue *q);

/******** QUEUE functions - end *********/

#endif //VLG_MAGNIEN_QUEUE_H
