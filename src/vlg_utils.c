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


/******** UTILITY functions - end *********/