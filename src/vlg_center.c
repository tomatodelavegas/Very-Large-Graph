//
// Created by geoffrey on 08/05/2020.
//

#include "vlg_center.h"
#include "magnien_utils.h"
#include "magnien_queue.h"

/******** GRAPH CENTER functions - begin *********/

/** MOD: ADDED
 ** Return the depth of each node
 **/
int *depth_bfs_tree(graph *g, int v, int *max)
{
    int u, i;
    int *tree = NULL;
    int curr_depth = 0;
    queue *q;
    q = empty_queue(g->n + 1);
    if( (tree = (int*)malloc((g->n + 1) * sizeof(int))) == NULL )
        report_error("bfs_tree: malloc() error");
    for (i=0;i<g->n + 1;++i) // -1 terminated array
        tree[i] = -1;
    queue_add(q,v);
    queue_add(q, -1); // -1 special value acts as level seperator
    tree[v] = curr_depth++;
    while (!is_empty_queue(q)) {
        v = queue_get(q);
        if (v == -1){
            if (is_empty_queue(q))
                break;
            curr_depth++;
            queue_add(q, -1);
            continue;
        }
        for (i=0;i<g->degrees[v];++i) {
            u = g->links[v][i];
            if (tree[u]==-1){
                queue_add(q,u);
                tree[u] = curr_depth;
            }
        }
    }
    *max = curr_depth - 1;
    free_queue(q);
    return(tree);
}

/** MOD: Added in order to get the list of vertices located in the middle level(s) of the bfs tree
 ** TODO: depth_bfs_tree could return the by level vertices list for quicker computation
 **/
int* compute_central_vertices(graph *g, int start, int *resulting_size, int* next_node)
{
    int i = 0;
    int max = -1;
    int *depth_tree = depth_bfs_tree(g, start, &max);
    if (max == -1 || max == 0)
        report_error("compute_central_vertices: depth computation error");
    // On calcul le milieu, si nombre impaire il va falloir prendre
    // deux milieux: middle et middle + 1
    int middle = max/2;
    int is_odd = max%2;
    int counter = 0;
    int *middle_nodes;

    for (i = 0; i < g->n; ++i){
        if (depth_tree[i] == middle || (is_odd && depth_tree[i] == middle + 1))
            counter++;
    }
    if (counter == 0) {
        free(depth_tree);
        fprintf(stderr, "compute_central_vertices: no middle vertices could be found !"); // ? report_error
        *resulting_size = 0;
        return NULL;
    }
    *next_node = random_node_depthtree(depth_tree, g->n, max); // next node to use for multisweep

    if ((middle_nodes = (int*) malloc((counter + 1) * sizeof(int))) == NULL)
        report_error("compute_central_vertices: middle_nodes: malloc() error");
    int j = 0;
    for (i = 0 ; i < g->n; ++i){
        if (depth_tree[i] == middle || (is_odd && depth_tree[i] == middle + 1))
            middle_nodes[j++] = i;
    }
    *resulting_size = j;
    free(depth_tree);
    return middle_nodes;
}

/** MOD: Added to compute intersection between two lists
 **/
int *intersection_lists(int *list1, int *list2, int size1, int size2, int *resulting_size)
{
    int final_size = size1 > size2 ? size1 : size2;
    int *new_list;
    if ((new_list = (int*) malloc((final_size + 1) * sizeof(int))) == NULL)
        report_error("intersections_lists: error malloc()");
    int k = 0;
    for (int i = 0; i < size1; ++i){
        for (int j = 0; j < size2; ++j){
            if (list1[i] == list2[j]){
                new_list[k++] = list1[i];
            }
        }
    }
    *resulting_size = k;
    return new_list;
}

/**
 * MOD: return random maximum eccentricity node
 **/
int random_node_depthtree(int *tree, int size, int max)
{
    int counter = 0;
    for (int i = 0; i < size; ++i)
    {
        if (tree[i] == max)
            counter++;
    }

    int index_ref = random()%counter;
    for (int i = 0; i < size; ++i)
    {
        if (tree[i] == max)
        {
            if (index_ref <= 0)
                return i;
            else
                index_ref--;
        }
    }
    report_error("random_node_depthtree: couldn't find a random index");
    return -1;
}

/** MOD: Added
 ** Steps:
 ** - depth_bfs_tree() gets the farthest points
 ** - Do bfs from these nodes
 ** - Store the bfs somehow (tree or list) (depth list)
 ** - We get a center approximation and a rayon approximation
 ** - (First version TODO) make intersection between lists found
 ** TODO: entire graph loop costs (same with center rayon's comment):
 ** depth_bfs_tree could return the by level vertices list for quicker computation
 **/
void calculate_center(graph *g, int start, int num_iterations)
{
    // MultiSweep technique
    int middle_nodes_size = 0;
    int *middle_nodes = NULL;
    int temp_middle_size = 0;
    int *temp_middle_nodes = NULL;
    int max = 0;

    fprintf(stderr, "Starting bfs with node %d\n", start);
    int *tree = depth_bfs_tree(g, start, &max);
    int *results = NULL;
    int job_node = random_node_depthtree(tree, g->n, max);
    free(tree); // No need of the first initial depth tree anymore

    for (int i = 0; i < num_iterations; ++i)
    {
        fprintf(stderr, "Processing bfs with node %d\n", job_node);
        temp_middle_nodes = compute_central_vertices(g, job_node, &temp_middle_size, &job_node);
        if (temp_middle_nodes == NULL)
            return; // we do not intersect non allocated arrays/ error happened
        if (middle_nodes == NULL)
        {
            middle_nodes = temp_middle_nodes;
            middle_nodes_size = temp_middle_size;
        }
        int *inter = intersection_lists(middle_nodes, temp_middle_nodes,
                                        middle_nodes_size, temp_middle_size, &middle_nodes_size);
        free(middle_nodes);
        if (middle_nodes != temp_middle_nodes) // Avoid double free
            free(temp_middle_nodes);
        middle_nodes = inter;
        for (int i = 0; i < middle_nodes_size; ++i)
            printf("%d ", middle_nodes[i]);
        printf("\n");
    }
    free(middle_nodes);
}

/******** GRAPH CENTER functions - end *********/