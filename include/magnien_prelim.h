/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#ifndef VLG_MAGNIEN_PRELIM_H
#define VLG_MAGNIEN_PRELIM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <stdbool.h> // MOD: Added

#define MAX_LINE_LENGTH 1000

typedef struct graph{
    int n; // number of nodes
    int m; // number of links
    int **links;
    int *degrees; // capacities <=> degrees
    int *capacities;
} graph;

#endif // VLG_MAGNIEN_PRELIM_H