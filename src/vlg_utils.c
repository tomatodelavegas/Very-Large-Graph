//
// Created by geoffrey on 08/05/2020.
//

#include "vlg_utils.h"
#include "magnien_utils.h"

/******** UTILITY functions - begin *********/

/** MOD: Added giant component renumbering (not giant components renumbered to end) **/
int *giant_perm(graph *g, int *c, int size_giant, int c_giant) {
    int *perm;
    int i;
    int lw, up; // lower bound, upper bound
    if( (perm=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("giant_perm: malloc() error");
    lw = 0;
    up = g->n - 1;
    for (i = 0; i < g->n ; ++i) {
        if (c[i] == c_giant) {
            perm[i] = lw; lw++;
        } else {
            perm[i] = up; up--;
        }
    }
    return (perm);
}

/** MOD: Added **/
int min(int a, int b) {
    return a < b ? a : b;
}

/** MOD: Max of an array of int */
int find_maximum(int *a, int n) {
  int c, max, index;
 
  max = a[0];
  index = 0;
 
  for (c = 1; c < n; c++) {
    if (a[c] > max) {
       index = c;
       max = a[c];
    }
  }
 
  return max;
}

/**
 ** intersection_lists: compute intersection between two lists
 ** 
 ** int *list1:             the first list
 ** int *list2:             the second list
 ** int size1:              the first list length
 ** int size2:              the second list length
 ** int *resulting_size:    the resulting intersected list length
 **
 ** returns int*:           list1 ∩ list2
 **/
int *intersection_lists(int *list1, int *list2, int size1, int size2, int *resulting_size)
{
    int final_size = size1 > size2 ? size1 : size2;
    int *new_list;
    if ((new_list = (int*) malloc((final_size + 1) * sizeof(int))) == NULL)
        report_error("intersections_lists: error malloc()");
    int k = 0;
    for (int i = 0; i < size1; ++i){
        for (int j = 0; j < size2; ++j){
            if (list1[i] == list2[j]){
                new_list[k++] = list1[i];
            }
        }
    }
    *resulting_size = k;
    return new_list;
}

/******** UTILITY functions - end *********/