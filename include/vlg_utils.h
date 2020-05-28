//
// Created by geoffrey on 08/05/2020.
//

#ifndef VLG_VLG_UTILS_H
#define VLG_VLG_UTILS_H

#include "magnien_prelim.h"

struct leaf_node {
    int id; /** id of the leaf vertice **/
    int dist; /** distance from BFS starting point **/
}; /** leaf_node structure to old vertice id and distance from BFS start **/

/******** UTILITY functions - begin *********/

int *giant_perm(graph *g, int *c, int size_giant, int c_giant);

int min(int a, int b);

int find_maximum(int *a, int n);

bool remove_leafs_closer_than(struct leaf_node *leafs, int *nb_leafs, int min_dist);

struct leaf_node *pop_farthest_leaf(struct leaf_node *leafs, int *nb_leafs);

int *intersection_lists(int *list1, int *list2, int size1, int size2, int *resulting_size);

/******** UTILITY functions - end *********/

#endif //VLG_VLG_UTILS_H