//
// Created by geoffrey on 08/05/2020.
//

#include "vlg_utils.h"
#include "magnien_utils.h"

/******** UTILITY functions - begin *********/

void swap_leafs(int *a, int *b) {
    int temp = *a;
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

float min_float(float a, float b){
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
 ** Will remove all leafs <= given min_dist (lower moves number usage).
 ** Additionnaly, return -1 (if no more leafs), or the poped farthest leaf
 **
 ** int *depth_tree:    the depth_tree array for leafs distances access
 ** int *leafs:         the leafs array
 ** int *nb_leafs:      the number of leafs in the array
 ** int min_dist:       the minimal leaf distances
 **
 ** returns int:        the farthest leaf node (popped)
 ** NB: *nb_leafs can be 0 after function call, since we pop the farthest at the end of function (!= -1)
 **/
int remove_leafs_closer_than_and_pop_farthest(int *depth_tree, int *leafs, int *nb_leafs, int min_dist)
{
    if (*nb_leafs < 0)
        report_error("remove_leafs_closer_than_and_pop_farthest: leafs list is NULL");
    if (leafs == NULL || *nb_leafs == 0)
        return -1;
    int i;
    int max_l_id = 0;
    int max_d = depth_tree[leafs[max_l_id]];
    for (i = *nb_leafs - 1; i >= 0 && *nb_leafs > 0; --i) {
        // remove_leafs_close_than
        if (depth_tree[leafs[i]] < min_dist) {
            if (i < *nb_leafs - 1) { // we swap only if elements after us have been kept
                memmove((leafs + i), (leafs + *nb_leafs - 1), sizeof(int));
                if (*nb_leafs -1 == max_l_id) // we need to swap indexes
                    max_l_id = i;
            }
            *nb_leafs -= 1;
        } else {
            // get the farthest, a farthest cannot be one that's lower than min_dist
            if (depth_tree[leafs[i]] > max_d) {
                max_l_id = i;
                max_d = depth_tree[leafs[max_l_id]];
            }
        }
    }
    if (*nb_leafs == 0)// all we did was remove, mo possible maximum
        return -1;
    // now swap last element with max_leaf return max_leaf
    // (which is now at end of array)
    swap_leafs(leafs + max_l_id, leafs + *nb_leafs - 1);
    *nb_leafs -= 1; // *nb_leafs is decreased since we pop...
    return leafs[*nb_leafs]; // allready decreased
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
bool remove_leafs_closer_than(int *depth_tree, int *leafs, int *nb_leafs, int min_dist)
{
    if (*nb_leafs < 0)
        report_error("remove_leafs_closer_than: leaf list size negative");
    if (leafs == NULL || nb_leafs == 0)
        return true;
    int i;
    for (i = 0; i < *nb_leafs;) {
        if (depth_tree[leafs[i]] < min_dist) {
            // remove leafs[i]; leafs[i] will be last element, size is decreased
            // MAN: memmove(dest, src, nbbytes);
            // TODO: consider using memcpy since there should not be overlapping
            memmove((leafs + i), (leafs + *nb_leafs - 1), sizeof(int));
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
int pop_farthest_leaf(int *depth_tree, int *leafs, int *nb_leafs)
{
    if (leafs == NULL || *nb_leafs < 0)
        report_error("pop_farthest_leaf: leafs list is NULL");
    if (*nb_leafs == 0)
        return -1;
    int i;
    int *max_l = leafs;
    int max_d = depth_tree[*max_l];
    for (i = 1; i < *nb_leafs; ++i) {
        if (depth_tree[leafs[i]] > max_d) {
            max_l = leafs + i;
            max_d = depth_tree[*max_l];
        }
    }
    // now swap last element with max_leaf return max_leaf
    // (which is now at end of array)
    swap_leafs(max_l, leafs + *nb_leafs - 1);
    *nb_leafs -= 1; // *nb_leafs is decreased
    return leafs[*nb_leafs]; // allready decreased
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