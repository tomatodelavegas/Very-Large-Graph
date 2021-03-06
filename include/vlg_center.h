//
// Created by geoffrey on 08/05/2020.
//

#ifndef VLG_VLG_CENTER_H
#define VLG_VLG_CENTER_H

#include "magnien_prelim.h"
#include "magnien_utils.h"
#include "magnien_distances.h"
#include "vlg_utils.h"
#include <math.h>

/******** GRAPH CENTER functions - begin *********/

int *depth_bfs_tree(graph *g, int v, int *max, int **magnien_tree, int *leafs, int *nb_leafs);

int* compute_central_vertices(graph *g, int start, int *resulting_size, int* next_node, int *diameter, int *diam_upper);

int random_node_depthtree(int *tree, int size, int max);

// higher functions:

void compute_center_convergence(graph *g, int num_iterations, int* c, int c_giant);

void calculate_center(graph *g, int start, int num_iterations, int* c, int c_giant, bool check_centers);

/******** GRAPH CENTER functions - end *********/

#endif //VLG_VLG_CENTER_H