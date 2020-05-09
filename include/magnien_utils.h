/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#ifndef VLG_MAGNIEN_UTILS_H
#define VLG_MAGNIEN_UTILS_H

#include "magnien_prelim.h"

/******** UTILITY functions - begin *********/

void report_error(char *s);

int is_in_array(int *a, int e, int left, int right);

int max_in_array(int *a, int n);

int max(int i, int j);

int index_max_array(int *a, int n);

int rand_index_max_array(int *a, int n);

void quicksort(int *v, int t);

int *random_perm(int n);

int *inverse_perm(int *p, int n);

/******** UTILITY functions - end *********/

#endif //VLG_MAGNIEN_UTILS_H