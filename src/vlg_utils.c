//
// Created by geoffrey on 08/05/2020.
//

#include "vlg_utils.h"
#include "magnien_utils.h"

/******** UTILITY functions - begin *********/

void swap_leafs(struct leaf_node *a, struct leaf_node *b) {
    struct leaf_node temp = *a;
    *a = *b;
    *b = temp;
}

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
 ** remove_leafs_closer_than: remove leafs that are closer than the given dist
 **
 ** struct leaf_node *leafs:    leafs array
 ** int *nb_leafs:              pointer to the leafs array size
 ** int min_dist:               minimal distance criteria for leafs
 **
 ** returns bool:               returns true if there is no more leafs in the array
 **/
bool remove_leafs_closer_than(struct leaf_node *leafs, int *nb_leafs, int min_dist)
{
    if (*nb_leafs < 0)
        report_error("remove_leafs_closer_than: leaf list size negative");
    if (leafs == NULL || nb_leafs == 0)
        return true;
    int i;
    for (i = 0; i < *nb_leafs;) {
        if (leafs[i].dist < min_dist) {
            // remove leafs[i]; leafs[i] will be last element, size is decreased
            // MAN: memmove(dest, src, nbbytes);
            // TODO: consider using memcpy since there should not be overlapping
            memmove((leafs + i), (leafs + *nb_leafs - 1), sizeof(struct leaf_node));
            *nb_leafs -= 1;
            if (*nb_leafs == 0)
                return true;
        } else {
            i++;
        }
    }
    return *nb_leafs == 0;
}

/**
 ** pop_farthest_leaf: returns the maximum distance leaf from leafs array, it is removed
 ** 
 ** struct leaf_node *leafs:        the leafs array
 ** int *nb_leafs:                  the number of leafs
 **
 ** returns struct leaf_node *:     the farthest leaf node, or NULL if no more leaf
 **/
struct leaf_node *pop_farthest_leaf(struct leaf_node *leafs, int *nb_leafs)
{
    if (leafs == NULL || *nb_leafs < 0)
        report_error("pop_farthest_leaf: leafs list is NULL");
    if (nb_leafs == 0)
        return NULL;
    int i;
    struct leaf_node *max_leaf = leafs;
    int max = max_leaf->dist;
    for (i = 1; i < *nb_leafs; ++i) {
        if (leafs[i].dist > max) {
            max_leaf = leafs + i;
            max = max_leaf->dist;
        }
    }
    // now swap last element with max_leaf return max_leaf
    // (which is now at end of array)
    swap_leafs(max_leaf, leafs + *nb_leafs - 1);
    *nb_leafs -= 1; // *nb_leafs is decreased
    return leafs + *nb_leafs; // allready decreased
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