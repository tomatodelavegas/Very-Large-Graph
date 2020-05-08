/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#ifndef VLG_MAGNIEN_GRAPH_COMPONENT_H
#define VLG_MAGNIEN_GRAPH_COMPONENT_H

#include "magnien_prelim.h"

/******** CONNECTED COMPONENTS functions - begin *********/

int aux_connected_components(graph *g, int *comp, int v, int num_comp);

int connected_components(graph *g, int *components, int *sizes);

int giant(int *sizes, int n);

/******** CONNECTED COMPONENTS functions - end *********/

#endif //VLG_MAGNIEN_GRAPH_COMPONENT_H
