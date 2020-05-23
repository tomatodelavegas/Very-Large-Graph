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
int* compute_central_vertices(graph *g, int start, int *resulting_size, int* next_node, int *diameter)
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
    *diameter = max;

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

void update_histogram(int* histo, int* middle_nodes, int size)
{
    if (histo == NULL || middle_nodes == NULL)
        return;
    for (int i = 0; i < size; ++i)
        histo[middle_nodes[i]]++;
}

int *ratio_histo(int *histo, int size, int *result_size, float ratio_retention)
{
    int max = find_maximum(histo, size);
    float ratio;
    int counter = 0;
    for (int i = 0; i < size; ++i){
        ratio = (float)histo[i] / (float)max;
        if (ratio > ratio_retention) // ratio of 80% is nice
            counter++;
    }

    int *nodes_result;
    if ((nodes_result = malloc((counter+1) * sizeof(int))) == NULL){
        report_error("ratio histo: error malloc nodes_result");
        return NULL;
    }

    int hh = 0;
    for (int i = 0; i < size; ++i){
        ratio = (float)histo[i] / (float)max;
        if (ratio > ratio_retention){
            nodes_result[hh++] = i;
        }
    }
    *result_size = counter;
    return nodes_result;
}
/**
 * MOD: return random maximum eccentricity node
 **/
int random_node_depthtree(int *tree, int size, int max)
{
    int counter = 0;
    for (int i = 0; i < size; ++i){
        if (tree[i] == max)
            counter++;
    }

    int index_ref = random()%counter;
    for (int i = 0; i < size; ++i){
        if (tree[i] == max){
            if (index_ref <= 0)
                return i;
            else
                index_ref--;
        }
    }
    report_error("random_node_depthtree: couldn't find a random index");
    return -1;
}

int get_multisweep_node(graph *g, int start)
{
    int max = 0;
    int *tree = depth_bfs_tree(g, start, &max);
    if (max == -1 || max == 0){
        report_error("get_multisweep_node: depth computation error");
        return -1;
    }
    int job_node = random_node_depthtree(tree, g->n, max);
    free(tree); // No need of the first initial depth tree anymore
    return job_node;
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
    int copy_node = 0;
    int middle_nodes_size = 0;
    int *middle_nodes = NULL;
    int temp_middle_size = 0;
    int *temp_middle_nodes = NULL;
    int max = 0;
    int *histo_center_nodes;
    if ((histo_center_nodes = calloc(g->n + 1, sizeof(int)))== NULL){
        report_error("calculate_center: malloc error histo");
        return;
    }
    int upper_diam = -1, lower_diam = 0, rayon = -1;

    fprintf(stderr, "Starting bfs with node %d\n", start);
    int *multisweep_check;
    if ((multisweep_check = calloc(g->n + 1, sizeof(int)))== NULL){
        report_error("calculate_center: multisweep_check: error malloc()");
    }
    int counter_tries = 0;
    int counter_limit = 100; // FIXME use a parameter instead

    int *results = NULL;
    int job_node = get_multisweep_node(g, start);
    if (job_node == -1){ // error case
        report_error("calculate_center: get_multisweep_node: error finding node");
        return;
    }

    for (int i = 0; i < num_iterations; ++i){
        fprintf(stderr, "Processing bfs with node %d\n", job_node);
        multisweep_check[job_node] = 1; // set to already done
        temp_middle_nodes = compute_central_vertices(g, job_node, &temp_middle_size, &job_node, &max);
        if (temp_middle_nodes == NULL)
            return; // non allocated array/ error happened

        if (max > upper_diam || upper_diam == -1)
            upper_diam = 2*max; // ecc(u) <= D(G) <= 2ecc(u)
        else if (max > lower_diam || lower_diam == 0)
            lower_diam = max;
        if (rayon == -1)
            rayon = upper_diam / 2;
        
        update_histogram(histo_center_nodes, temp_middle_nodes, temp_middle_size);
        free(temp_middle_nodes);
        fprintf(stdout, "%dth iteration %d %d %d\n", i, upper_diam, lower_diam, rayon);

        // check if node has already been used, if so, pick a new node
        copy_node = job_node;
        while (multisweep_check[job_node]){
            job_node = random()%g->n;
            counter_tries++;
            if (counter_tries >= counter_limit){
                memset(multisweep_check, 0, g->n * sizeof(*multisweep_check)); // reset on too many iterations
                counter_tries = 0;
            }
        }
        if (copy_node != job_node){ // check if loop changed the current node, if so, bfs needed
            job_node = get_multisweep_node(g, job_node);
            if (job_node == -1){ // error case
                report_error("calculate_center: get_multisweep_node: error finding node");
                return;
            }
        }

    }
    fprintf(stdout, "Center nodes found:\n");
    middle_nodes = ratio_histo(histo_center_nodes, g->n, &middle_nodes_size, 0.8); // FIXME: remove hardcoded and use parameter
    for (int i = 0; i < middle_nodes_size; ++i)
        fprintf(stdout, "%d ", middle_nodes[i]);
    fprintf(stdout, "\n");
    fprintf(stdout, "%d BFS done\n", num_iterations + 1);
    fprintf(stdout, "Final values: %d %d %d\n", upper_diam, lower_diam, rayon);
    free(middle_nodes);
    free(multisweep_check);
}

/******** GRAPH CENTER functions - end *********/