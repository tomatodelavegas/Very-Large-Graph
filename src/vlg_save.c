//
// Created by geoffrey on 08/05/2020.
//

#include "vlg_save.h"
#include "magnien_graph_utils.h"
#include "magnien_utils.h"
#include "magnien_queue.h"
#include "vlg_utils.h"

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
void save_giant_degrees_or_links(bool links, FILE *f, bool *seen, int *corresp, graph *g, int *c, int size_giant, int c_giant) {
    int v, u; // current node, child node
    int n = 0; // current new graph max index
    int i;
    queue *q; // queue for the bfs
    for (v = 0; v < g->n && c[v] != c_giant; ++v)
        continue;
    if (v == g->n) {
        free(corresp); free(seen); free_graph(g); fclose(f);
        report_error("save_giant_degrees_or_links: wrong giant component id");
    }
    memset(corresp, -1, g->n * sizeof(int)); // Setting up corresp array
    memset(seen, 0, g->n * sizeof(bool)); // Setting up seen array
    if (!links)
        fprintf(f, "%d", size_giant); // writing giant component size to file
    /** BFS starting from v **/
    q = empty_queue(g->n);
    queue_add(q,v);
    corresp[v] = n;
    n++;
    while (!is_empty_queue(q)) {
        v = queue_get(q);
        seen[v] = 1;
        if (!links)
            fprintf(f, "\n%d %d", corresp[v], g->degrees[v]); // degrees: here we fprint corresp[v] and g->degrees[v]
        for (i = 0; i < g->degrees[v]; i++) {
            u = g->links[v][i];
            if (corresp[u] == -1) {
                queue_add(q,u);
                corresp[u] = n;
                n++;
            }
            if (links && !seen[u]) { // links: here we fprint corresp[v] corresp[u]
                fprintf(f, "\n%d %d", corresp[v], corresp[u]);
            }
        }
    }
    free_queue(q);
}

/** MOD: Added in place graph saving using BFS and edges correspondence array
 ** graph *g:       the graph to save the giant component
 ** int *c:         the component indexes array
 ** int c_giant:    the giant component index
 ** int size_giant: the size of the giant component
 ** char *path:     the path string to open the file
 **/
void save_giant_bfs(graph *g, int *c, int c_giant, int size_giant, char *path) {
    int *corresp; // correspondence array
    bool *seen; // visited link array
    FILE *f; // output FILE
    int i;
    if ((seen = (bool *)malloc(g->n * sizeof(int))) == NULL)
        report_error("save_giant: seen array: calloc() error");
    if ((corresp=(int *)malloc(g->n * sizeof(int))) == NULL) // we could realloc...
        report_error("save_giant: correspondance array: malloc() error");
    f = fopen(path, "w");
    save_giant_degrees_or_links(0, f, seen, corresp, g, c, size_giant, c_giant);
    save_giant_degrees_or_links(1, f, seen, corresp, g, c, size_giant, c_giant);
    free(seen);
    free(corresp);
    fclose(f);
}

/** MOD: Added to save graph up to a limit (the new graph size if previously correctly renumbered) **/
void graph_to_file(graph *g, FILE *f, int limit) {
    int i, j;
    fprintf(f, "%d", limit);
    for (i = 0; i < limit; ++i)
        fprintf(f, "\n%d %d", i, g->degrees[i]);
    for (i = 0; i < limit; ++i)
        for (j = 0; j < g->degrees[i]; ++j)
            if (g->links[i][j] > i)
                fprintf(f, "\n%d %d", i, g->links[i][j]);
}

/** MOD: Added to save giant graph without a BFS but using permutations **/
void save_giant(graph *g, int *c, int c_giant, int size_giant, char *path) {
    int *perm;
    FILE *f; // output FILE
    f = fopen(path, "w");
    perm = giant_perm(g, c, size_giant, c_giant);
    renumbering(g,perm);
    graph_to_file(g, f, size_giant);
    free(perm);
    fclose(f);
}

/******** GRAPH SAVING functions - end *********/