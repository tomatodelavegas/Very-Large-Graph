/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#ifndef VLG_MAGNIEN_DISTANCES_H
#define VLG_MAGNIEN_DISTANCES_H

#include "magnien_prelim.h"

/******** DISTANCE functions - begin *********/

int *bfs_tree(graph *g, int v);

int *tree_degrees(int *t, int n);

int tree_max_dist(int *t, int n);

void distances(graph *g, int v, int *dist);

float max_distance(graph *g, int v);

/******** DISTANCE functions - end *********/

#endif //VLG_MAGNIEN_DISTANCES_H