/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#include "magnien_queue.h"
#include "magnien_utils.h"

/******** QUEUE functions - begin *********/

queue *empty_queue(int size){
    queue *q;
    if( (q=(queue *)malloc(sizeof(queue))) == NULL )
        report_error("empty_queue: Pb with malloc");
    q->size = size+1;
    if( (q->elts = (int *)malloc((size+1)*sizeof(int))) == NULL )
        report_error("empty_queue: Pb with malloc");
    q->begin = q->end = 0;
    return(q);
}

void free_queue(queue *q){
    if ((q==NULL) || (q->elts==NULL))
        report_error("free_queue: NULL pointer");
    free(q->elts);
    free(q);
}

int is_empty_queue(queue *q){
    return(q->begin == q->end);
}

int is_full_queue(queue *q){
    return(q->begin == (q->end+1)%q->size);
}

void queue_add(queue *q, int e){
    if (is_full_queue(q))
        report_error("queue_add: queue is full");
    q->elts[q->end] = e;
    q->end++;
    q->end %= q->size;
}

int queue_get(queue *q){
    int r;
    if (is_empty_queue(q))
        report_error("queue_get: queue is empty");
    r = q->elts[q->begin];
    q->begin++;
    q->begin %= q->size;
    return(r);
}

/******** QUEUE functions - end *********/
