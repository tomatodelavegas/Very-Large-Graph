//
// Created by geoffrey on 28/05/2020.
//

#include "vlg_tests.h"

#ifdef TEST

/**
 ** test function for depth_bfs_tree leafs detection
 **
 ** graph *g:       the graph itself
 ** int v:          starting BFS node
 **
 ** check leafs detection using magnien tree_degrees function,
 ** beware: (v should not be a leaf itself for leaf detections
 **         to get all graph leafs)
 **
 ** this test function has a high memory cost, watch out !
 **/
void test_leafs_detection(graph *g, int v)
{
    int *depth_tree, *tree;
    bool *are_leafs; // are_leafs is 1 at i when i is a leaf node
    int *leafs;
    int nb_leafs = 0;
    int max_dist;
    int i;
    if((tree = (int *)calloc(g->n + 1, sizeof(int))) == NULL)
        report_error("malloc error: tree array creation");
    if((are_leafs = (bool *)calloc(g->n, sizeof(bool))) == NULL)
        report_error("malloc error: are_leafs nodes array creation");
    if((leafs = (int *)calloc(g->n, sizeof(int))) == NULL)
        report_error("malloc error: leaf nodes array creation");
    fprintf(stderr, "starting graph leafs detection from node %d\n", v);
    depth_tree = depth_bfs_tree(g, v, &max_dist, &tree, leafs, &nb_leafs);
    fprintf(stderr, "found %d leafs\n", nb_leafs);
    for (i = 0; i < nb_leafs; ++i) {
      are_leafs[leafs[i]] = 1;
    }
    /** magnien backprop leaf detection **/
    fprintf(stderr, "searching leafs with magnien tree_degrees\n");
    int *degrees;
    bool good_detection;
    int degrees0nb = 0;
    degrees = tree_degrees(tree,g->n);
    for (i = 0; i < g->n; i++) {
        if (degrees[i] == 0) {
            //fprintf(stderr, "%d should be a leaf", i);
            degrees0nb++;
            assert(are_leafs[i] == 1);
            //fprintf(stderr, ", leaf found !\n");
        }
    }
    fprintf(stderr, "magnien tree_degrees found %d leafs\n", degrees0nb);
    assert(degrees0nb == nb_leafs);
    free(leafs);
    free(depth_tree);
    free(tree);
    free(degrees);
    free(are_leafs);
}

/**
 ** testing leafs removal with min_dit
 **
 ** graph *g:       the graph itself
 ** int v:          starting BFS node
 **
 ** check leafs removal using min_dist
 **/
void test_leafs_rm_lw_than(graph *g, int v)
{
    int min_dist; // leafs removal criteria
    int *depth_tree, *tree;
    int *leafs;
    int nb_leafs = 0;
    int max_dist;
    int i;
    int size_after_rm;
    if((tree = (int *)calloc(g->n + 1, sizeof(int))) == NULL)
        report_error("malloc error: tree array creation");
    if((leafs = (int *)calloc(g->n, sizeof(int))) == NULL)
        report_error("malloc error: leaf nodes array creation");
    fprintf(stderr, "starting graph leafs detection from node %d\n", v);
    depth_tree = depth_bfs_tree(g, v, &max_dist, &tree, leafs, &nb_leafs);
    min_dist = max_dist;
    fprintf(stderr, "%d leafs found ! Removing leafs < %d\n", nb_leafs, min_dist);

    size_after_rm = nb_leafs;
    for (i = 0; i < nb_leafs; ++i) {
        //fprintf(stderr, "id:%d dist:%d\n", leafs[i], depth_tree[leafs[i]]);
        if (depth_tree[leafs[i]] < min_dist) {
            size_after_rm--;
            if (size_after_rm == 0)
                break;
        }
    }
    bool is_empty = remove_leafs_closer_than(depth_tree, leafs, &nb_leafs, min_dist);
    //fprintf(stderr, "%d, %d, %d\n", is_empty, nb_leafs, size_after_rm);
    assert((is_empty && size_after_rm == 0) || (!is_empty && size_after_rm > 0));
    assert(size_after_rm == nb_leafs);

    fprintf(stderr, "After removal lower than %d, we have %d nb leafs: \n", min_dist, nb_leafs);
    /**
    for (i = 0; i < nb_leafs; ++i) {
        fprintf(stderr, "id:%d dist:%d\n", leafs[i], depth_tree[leafs[i]]);
    }**/

    free(leafs);
    free(depth_tree);
    free(tree);
}

static inline int leaf_compare(const void *l1, const void *l2, void *dists)
{
  int *depth_tree = (int *)dists;
  int e1 = *((int *)l1);
  int e2 = *((int *)l2);
  return depth_tree[e1] - depth_tree[e2];
}

void test_pop_farthest(graph *g, int v)
{
    int *depth_tree, *tree;
    int *leafs, *sorted_leafs;
    int max_leaf;
    int nb_leafs = 0;
    int max_dist;
    int i;
    if((tree = (int *)calloc(g->n + 1, sizeof(int))) == NULL)
        report_error("malloc error: tree array creation");
    if((leafs = (int *)calloc(g->n, sizeof(int))) == NULL)
        report_error("malloc error: leaf nodes array creation");
    fprintf(stderr, "starting graph leafs detection from node %d\n", v);
    depth_tree = depth_bfs_tree(g, v, &max_dist, &tree, leafs, &nb_leafs);

    swap_leafs(leafs, leafs+nb_leafs - 1);
    swap_leafs(leafs+(nb_leafs/2), leafs+nb_leafs - 1);
    sorted_leafs = malloc(nb_leafs * sizeof(int));
    memcpy(sorted_leafs, leafs, nb_leafs * sizeof(int));
    /**GNU_SOURCE provides: void qsort_r(void *base, size_t nmemb, size_t size,
             int (*compar)(const void *, const void *, void *),
             void *arg);**/
    qsort_r(sorted_leafs, nb_leafs, sizeof(int), leaf_compare, depth_tree);
    /**
    for (i = 0; i < nb_leafs; ++i)
        fprintf(stderr, "id:%d; dist:%d\n", leafs[i], depth_tree[leafs[i]]);
    **/

    fprintf(stderr, "copied leaf nodes array sorted!\n");
    fprintf(stderr, "checking pop_farthest_leaf returns the good array\n");
    for (i = nb_leafs - 1; i >= 0; --i) {
        max_leaf = pop_farthest_leaf(depth_tree, leafs, &nb_leafs); // takes quite a while... O(n)
        assert(max_leaf >= 0);
        //fprintf(stderr, "@id:%d, dist:%d\n", sorted_leafs[i], depth_tree[sorted_leafs[i]]);
        //fprintf(stderr, "#id:%d, dist:%d\n", max_leaf, depth_tree[max_leaf]);
        assert(depth_tree[sorted_leafs[i]] == depth_tree[max_leaf]);
    }
    fprintf(stderr, "OK !\n");

    free(leafs);
    free(sorted_leafs);
    free(depth_tree);
    free(tree);
}

#else

void test_leafs_detection(graph *g, int v)
{
    fprintf(stderr, "TEST macro not defined !\n");
}

void test_leafs_rm_lw_than(graph *g, int v)
{
    fprintf(stderr, "TEST macro not defined !\n");
}

void test_pop_farthest(graph *g, int v)
{
    fprintf(stderr, "TEST macro not defined !\n");
}

#endif