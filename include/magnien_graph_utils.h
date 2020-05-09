/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#ifndef VLG_MAGNIEN_GRAPH_UTILS_H
#define VLG_MAGNIEN_GRAPH_UTILS_H

#include "magnien_prelim.h"

/******** GRAPH MANAGEMENT functions - begin *********/

void free_graph(graph *g);

graph *graph_from_file(FILE *f);

/* Graph sorting and renumbering */

void sort_graph(graph *g);

int *sort_nodes_by_degrees(graph *g);

void renumbering(graph *g, int *perm);

void random_renumbering(graph *g);

/******** GRAPH MANAGEMENT functions - end *********/

#endif //VLG_MAGNIEN_GRAPH_UTILS_H