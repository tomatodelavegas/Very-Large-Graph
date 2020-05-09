//
// Created by geoffrey on 08/05/2020.
//

#ifndef VLG_VLG_CENTER_H
#define VLG_VLG_CENTER_H

#include "magnien_prelim.h"

/******** GRAPH CENTER functions - begin *********/

int *depth_bfs_tree(graph *g, int v, int *max);

int* compute_central_vertices(graph *g, int start, int *resulting_size);

int *intersection_lists(int *list1, int *list2, int size1, int size2, int *resulting_size);

int* get_center_rayon(graph *g, int start, int *resulting_size);

/******** GRAPH CENTER functions - end *********/

#endif //VLG_VLG_CENTER_H