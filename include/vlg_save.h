//
// Created by geoffrey on 08/05/2020.
//

#ifndef VLG_VLG_SAVE_H
#define VLG_VLG_SAVE_H

#include "magnien_prelim.h"

/******** GRAPH SAVING functions - begin *********/

/** MOD: Added to make a BFS either to append degrees to begining of given file or to append links to the end of f
 ** bool links:     wether to append links or degrees (and graph size) to f
 ** FILE *f:        opened FILE
 ** bool *seen:     allocated g->n sized seen array for BFS node seen tracking
 ** int *corresp:   allocated g->n correspondence array for in place renumbering
 ** graph *g:       the graph to save the giant component
 ** int *c:         the component indexes array
 ** int size_giant: the size of the giant component
 ** int c_giant:    the giant component index
 **
 ** improve?:       do not recompute the corresp array on second call ? not such a big deal
 **/
void save_giant_degrees_or_links(bool links, FILE *f, bool *seen, int *corresp, graph *g, int *c, int size_giant, int c_giant);

/** MOD: Added in place graph saving using BFS and edges correspondence array
 ** graph *g:       the graph to save the giant component
 ** int *c:         the component indexes array
 ** int c_giant:    the giant component index
 ** int size_giant: the size of the giant component
 ** char *path:     the path string to open the file
 **/
void save_giant_bfs(graph *g, int *c, int c_giant, int size_giant, char *path);

/** MOD: Added to save graph up to a limit (the new graph size if previously correctly renumbered) **/
void graph_to_file(graph *g, FILE *f, int limit);

/** MOD: Added to save giant graph without a BFS but using permutations **/
void save_giant(graph *g, int *c, int c_giant, int size_giant, char *path);

/******** GRAPH SAVING functions - end *********/

#endif //VLG_VLG_SAVE_H
