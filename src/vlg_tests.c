//
// Created by geoffrey on 28/05/2020.
//

#include "vlg_tests.h"

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
    struct leaf_node *leafs;
    int nb_leafs = 0;
    int max_dist;
    int i;
    if((tree = (int *)calloc(g->n + 1, sizeof(int))) == NULL)
        report_error("malloc error: tree array creation");
    if((are_leafs = (bool *)calloc(g->n, sizeof(bool))) == NULL)
        report_error("malloc error: are_leafs nodes array creation");
    if((leafs = (struct leaf_node *)calloc(g->n, sizeof(struct leaf_node))) == NULL)
        report_error("malloc error: leaf nodes array creation");
    fprintf(stderr, "starting graph leafs detection from node %d\n", v);
    depth_tree = depth_bfs_tree(g, v, &max_dist, &tree, leafs, &nb_leafs);
    fprintf(stderr, "found %d leafs\n", nb_leafs);
    for (i = 0; i < nb_leafs; ++i) {
      are_leafs[leafs[i].id] = 1;
      //fprintf(stderr, "id:%d dist:%d\n", leafs[i].id, leafs[i].dist);
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
    struct leaf_node *leafs;
    int nb_leafs = 0;
    int max_dist;
    int i;
    int size_after_rm;
    if((tree = (int *)calloc(g->n + 1, sizeof(int))) == NULL)
        report_error("malloc error: tree array creation");
    if((leafs = (struct leaf_node *)calloc(g->n, sizeof(struct leaf_node))) == NULL)
        report_error("malloc error: leaf nodes array creation");
    fprintf(stderr, "starting graph leafs detection from node %d\n", v);
    depth_tree = depth_bfs_tree(g, v, &max_dist, &tree, leafs, &nb_leafs);
    min_dist = max_dist;
    fprintf(stderr, "%d leafs found ! Removing leafs < %d\n", nb_leafs, min_dist);

    size_after_rm = nb_leafs;
    for (i = 0; i < nb_leafs; ++i) {
        fprintf(stderr, "id:%d dist:%d\n", leafs[i].id, leafs[i].dist);
        if (leafs[i].dist < min_dist) {
            size_after_rm--;
            if (size_after_rm == 0)
                break;
        }
    }
    bool is_empty = remove_leafs_closer_than(leafs, &nb_leafs, min_dist);
    fprintf(stderr, "%d, %d, %d\n", is_empty, nb_leafs, size_after_rm);
    assert((is_empty && size_after_rm == 0) || (!is_empty && size_after_rm > 0));
    assert(size_after_rm == nb_leafs);

    fprintf(stderr, "After removal lower than %d, we have %d nb leafs: \n", min_dist, nb_leafs);
    for (i = 0; i < nb_leafs; ++i) {
        fprintf(stderr, "id:%d dist:%d\n", leafs[i].id, leafs[i].dist);
    }

    free(leafs);
    free(depth_tree);
    free(tree);
}

static inline int leaf_compare(const void *l1, const void *l2)
{
  struct leaf_node *e1 = (struct leaf_node *)l1;
  struct leaf_node *e2 = (struct leaf_node *)l2;
  return e1->dist - e2->dist;
}

void test_pop_farthest(graph *g, int v)
{
    int *depth_tree, *tree;
    struct leaf_node *leafs, *sorted_leafs;
    struct leaf_node *max_leaf;
    int nb_leafs = 0;
    int max_dist;
    int i;
    if((tree = (int *)calloc(g->n + 1, sizeof(int))) == NULL)
        report_error("malloc error: tree array creation");
    if((leafs = (struct leaf_node *)calloc(g->n, sizeof(struct leaf_node))) == NULL)
        report_error("malloc error: leaf nodes array creation");
    fprintf(stderr, "starting graph leafs detection from node %d\n", v);
    depth_tree = depth_bfs_tree(g, v, &max_dist, &tree, leafs, &nb_leafs);

    swap_leafs(leafs, leafs+nb_leafs - 1);
    swap_leafs(leafs+(nb_leafs/2), leafs+nb_leafs - 1);
    sorted_leafs = malloc(nb_leafs * sizeof(struct leaf_node));
    memcpy(sorted_leafs, leafs, nb_leafs * sizeof(struct leaf_node));
    qsort(sorted_leafs, nb_leafs, sizeof(struct leaf_node), leaf_compare);
    for (i = 0; i < nb_leafs; ++i)
        fprintf(stderr, "id:%d; dist:%d\n", leafs[i].id, leafs[i].dist);

    fprintf(stderr, "copied leaf nodes array sorted!\n");
    fprintf(stderr, "checking pop_farthest_leaf returns the good array\n");
    for (i = nb_leafs - 1; i >= 0; --i) {
        // sadly no memcmp since same order for same dist elements is not a prerequisite
        //assert(memcmp(sorted_leafs[i], pop_farthest_leaf(leafs, &nb_leafs)) == 0);
        max_leaf = pop_farthest_leaf(leafs, &nb_leafs);
        fprintf(stderr, "@id:%d, dist:%d\n", sorted_leafs[i].id, sorted_leafs[i].dist);
        fprintf(stderr, "#id:%d, dist:%d\n", max_leaf->id, max_leaf->dist);
        assert(sorted_leafs[i].dist == max_leaf->dist);
    }

    free(leafs);
    free(depth_tree);
    free(tree);
}