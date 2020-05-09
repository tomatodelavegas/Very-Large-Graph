/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#include "magnien_distances.h"
#include "magnien_utils.h"
#include "magnien_queue.h"

/******** DISTANCE functions - begin *********/

/*
  Resulting tree is links between nodes from bfs process
*/
int *bfs_tree(graph *g, int v){
    int u, i;
    int *tree;
    queue *q;
    q = empty_queue(g->n);
    if( (tree=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("bfs_tree: malloc() error");
    for (i=0;i<g->n;i++)
        tree[i] = -1;
    queue_add(q,v);
    tree[v] = v;
    while (!is_empty_queue(q)) {
        v = queue_get(q);
        for (i=0;i<g->degrees[v];i++) {
            u = g->links[v][i];
            if (tree[u]==-1){
                queue_add(q,u);
                tree[u] = v;
            }
        }
    }
    free_queue(q);
    return(tree);
}

/* degrees in a tree
*  Compute degrees according to links between the nodes
*/
int *tree_degrees(int *t, int n){
    int i;
    int *r;
    if( (r=(int *)calloc(n,sizeof(int))) == NULL )
        report_error("tree_degrees: calloc() error");
    for (i=0;i<n;i++)
        if (t[i]==-1)
            r[i] = -1;
        else if (i!=t[i])
            r[t[i]]++;
    return(r);
}

/* maximal distance in a tree */
int tree_max_dist(int *t, int n){
    int i, v;
    int *degrees;
    int *subtree_height, *subtree_max_dist;
    int *max1_subsubtree_height;
    int *max2_subsubtree_height;
    queue *q;
    q = empty_queue(n);
    if( (subtree_height=(int *)calloc(n,sizeof(int))) == NULL )
        report_error("tree_max_dist: malloc() error");
    if( (subtree_max_dist=(int *)calloc(n,sizeof(int))) == NULL )
        report_error("tree_max_dist: malloc() error");
    if( (max1_subsubtree_height=(int *)calloc(n,sizeof(int))) == NULL )
        report_error("tree_max_dist: malloc() error");
    if( (max2_subsubtree_height=(int *)calloc(n,sizeof(int))) == NULL )
        report_error("tree_max_dist: malloc() error");
    degrees = tree_degrees(t,n);
    for (i=0;i<n;i++)
        if (degrees[i]==0)
            queue_add(q,i);

    while (1) {
        v = queue_get(q);
        if (t[v]==v) {
            int r;
            if (!is_empty_queue(q))
                report_error("tree_max_dist: queue should be empty");
            r = subtree_max_dist[v];
            free_queue(q);
            free(degrees);
            free(subtree_height);
            free(subtree_max_dist);
            free(max1_subsubtree_height);
            free(max2_subsubtree_height);
            return(r);
        }

        subtree_height[t[v]] = max(subtree_height[v]+1,subtree_height[t[v]]);

        if (subtree_height[v] + 1 > max1_subsubtree_height[t[v]]){
            max2_subsubtree_height[t[v]] = max1_subsubtree_height[t[v]];
            max1_subsubtree_height[t[v]] = subtree_height[v] + 1;
        }
        else if (subtree_height[v] + 1 > max2_subsubtree_height[t[v]])
            max2_subsubtree_height[t[v]] = subtree_height[v] + 1;

        subtree_max_dist[t[v]] = max(subtree_max_dist[t[v]],subtree_max_dist[v]);
        subtree_max_dist[t[v]] = max(subtree_max_dist[t[v]],max1_subsubtree_height[t[v]]+max2_subsubtree_height[t[v]]);

        degrees[t[v]]--;
        if (degrees[t[v]]==0)
            queue_add(q,t[v]);
    }
}

/* compute all distances from v */
void distances(graph *g, int v, int *dist){
    int u, i;
    queue *q;
    q = empty_queue(g->n);
    queue_add(q,v);
    dist[v] = 0;
    while (!is_empty_queue(q)) {
        v = queue_get(q);
        for (i=0;i<g->degrees[v];i++) {
            u = g->links[v][i];
            if (dist[u]==-1){
                queue_add(q,u);
                dist[u] = dist[v]+1;
            }
        }
    }
    free_queue(q);
}

/* clem : enlever le (dist[i] != -1 ) dans le if ? */
float max_distance(graph *g, int v){
    int *dist;
    int i, max=0;
    if( (dist=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("avg_distance: malloc() error");
    for (i=0;i<g->n;i++)
        dist[i] = -1;
    distances(g,v,dist);
    for (i=0;i<g->n;i++)
        if ((dist[i]!=-1) && (dist[i]>max))
            max = dist[i];
    free(dist);
    return(max);
}

/******** DISTANCE functions - end *********/