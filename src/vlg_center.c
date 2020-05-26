//
// Created by geoffrey on 08/05/2020.
//

#include "vlg_center.h"
#include "magnien_utils.h"
#include "magnien_queue.h"

/******** GRAPH CENTER functions - begin *********/

/******** begin statics ********/

/**
 ** get_multisweep_node: returns a random maximum tree level BFS node
 ** (wrapper for calling depth_bfs_tree then random_node_depthtree)
 ** 
 ** graph *g:       the graph itself
 ** int start:      the starting vertice to perform the BFS from
 ** int *max_ecc:   the vertice eccentricity value
 **
 ** returns int:    a random maximum tree level BFS node
 **/
static inline int get_multisweep_node(graph *g, int start, int *max_ecc)
{
    int max = 0;
    int *tree = depth_bfs_tree(g, start, &max, NULL);
    if (max == -1 || max == 0){
        report_error("get_multisweep_node: depth computation error");
        return -1;
    }
    int job_node = random_node_depthtree(tree, g->n, max);
    free(tree); // No need of the first initial depth tree anymore
    *max_ecc = max;
    return job_node;
}

/**
 ** get_vertice_eccentricity: returns the vertice eccentricity (costs 1 BFS)
 **
 ** graph *g:       the graph itself
 ** int start:      the starting vertice to perform the BFS from
 ** 
 ** returns int:    the given vertice eccentricity
 **/
static inline int get_vertice_eccentricity(graph *g, int start) {
    int max;
    int *depth_tree = depth_bfs_tree(g, start, &max, NULL);
    if (max == -1 || max == 0)
        report_error("get_eccentricity: depth computation error");
    free(depth_tree);
    return max;
}

/******** end statics ********/

/** MOD: ADDED
 ** Return the depth of each node
 ** And compute magnien tree at the same time
 ** magnien_tree: node links with bfs computation
 **/
int *depth_bfs_tree(graph *g, int v, int *max, int **magnien_tree)
{
    int u, i;
    int *depth_tree; // depth tree
    int *tree; // magnien tree
    int curr_depth = 0;
    queue *q;
    q = empty_queue(g->n + 1);
    if( (depth_tree = (int*)malloc((g->n + 1) * sizeof(int))) == NULL )
        report_error("bfs_tree: malloc() error");
    if( (tree=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("bfs_tree: malloc() error");
    for (i=0;i<g->n;++i){
        depth_tree[i] = -1;
        tree[i] = -1;
    }
    queue_add(q,v);
    queue_add(q, -1); // -1 special value acts as level seperator
    depth_tree[v] = curr_depth++;
    tree[v] = v;
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
            if (depth_tree[u]==-1){
                queue_add(q,u);
                depth_tree[u] = curr_depth;
                tree[u] = v;
            }
        }
    }
    *max = curr_depth - 1;
    free_queue(q);

    if (magnien_tree != NULL)
        *magnien_tree = tree;
    else
        free(tree);
    return(depth_tree);
}

/** MOD: Added in order to get the list of vertices located in the middle level(s) of the bfs tree
 ** TODO: depth_bfs_tree could return the by level vertices list for quicker computation
 **/
int* compute_central_vertices(graph *g, int start, int *resulting_size, int* next_node, int *diameter, int *diam_upper)
{
    int i = 0;
    int max = -1;
    int *magnien_tree = NULL;
    int *depth_tree = depth_bfs_tree(g, start, &max, &magnien_tree);
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
    *diam_upper = tree_max_dist(magnien_tree, g->n);
    free(magnien_tree);

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

/**
 ** update_histogram: updates the middle nodes histogram with 
 ** the given new middle nodes list
 **
 ** int* histo:         the histogram
 ** int* middle_nodes:  the middle nodes list
 ** int size:           the size of the middle nodes list
 ** 
 ** returns void
 **/
void update_histogram(int* histo, int* middle_nodes, int size)
{
    if (histo == NULL || middle_nodes == NULL)
        return;
    for (int i = 0; i < size; ++i)
        histo[middle_nodes[i]]++;
}

/**
 ** ratio_histo: returns nodes in the middle nodes histogram
 ** that have a frequency higher than the given ratio
 **
 ** int* histo:             the middle nodes histogram
 ** int size:               the size of the histogram
 ** int *result_size:       the number of higher middle nodes returned
 ** float ratio_retention:  the ratio determining higher frequency nodes
 **
 ** returns int*: the list of higher frequency middle nodes from histo
 **/
int *ratio_histo(int *histo, int size, int *result_size, float ratio_retention)
{
    int max = find_maximum(histo, size);
    float ratio;
    int counter = 0;
    for (int i = 0; i < size; ++i){
        ratio = (float)histo[i] / (float)max;
        if (ratio >= ratio_retention) // ratio >= 80% is nice
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
        if (ratio >= ratio_retention){
            nodes_result[hh++] = i;
        }
    }
    *result_size = counter;
    return nodes_result;
}

/**
 ** random_node_depthtree: return random maximum eccentricity node from
 ** a BFS level tree array (each value is the level in the BFS tree)
 **
 ** int *tree:  the BFS level tree array
 ** int size:   the size of the BFS level tree array
 ** int max:    the maximum BFS level
 **
 ** returns int: a random last BFS tree level node
 **/
int random_node_depthtree(int *tree, int size, int max)
{
    int counter = 0;
    for (int i = 0; i < size; ++i) {
        if (tree[i] == max)
            counter++;
    }

    int index_ref = random()%counter;
    for (int i = 0; i < size; ++i) {
        if (tree[i] == max) {
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
 ** TODO:
 ** - entire graph loop costs (same with center rayon's comment):
 ** - depth_bfs_tree could return the by level vertices list for quicker computation
 ** - paramètre pour une meilleure approximation du rayon
 **        (attention *2 plus de BFS)
 **        maybe add a parameter, parcours des nouveaux centres pour
 **        maj de la variable contenant le meilleur centre (nouvelle variable),
 **        pour ensuite effectuer un BFS a partir de celui ci
 ** - meilleure approximation du rayon grâce a un BFS
 **        sur le plus centre le probable
 **/
void calculate_center(graph *g, int start, int num_iterations, int* c, int c_giant)
{
    // MultiSweep technique
    int copy_node = 0;
    int middle_nodes_size = 0;
    int *middle_nodes = NULL;
    int temp_middle_size = 0;
    int *temp_middle_nodes = NULL;
    int max_dist = 0;
    int *histo_center_nodes;
    if ((histo_center_nodes = calloc(g->n + 1, sizeof(int)))== NULL){
        report_error("calculate_center: malloc error histo");
        return;
    }
    int upper_diam = -1, lower_diam, rayon;
    int temp_upper_diam = 0;
    float cur_rayon_approx;

    fprintf(stderr, "Starting bfs with node %d\n", start);
    int *multisweep_check;
    if ((multisweep_check = calloc(g->n + 1, sizeof(int)))== NULL){
        report_error("calculate_center: multisweep_check: error malloc()");
    }
    int counter_tries = 0;
    int counter_limit = 100; // FIXME use a parameter instead

    int *results = NULL;
    int job_node = get_multisweep_node(g, start, &max_dist);
    cur_rayon_approx = max_dist; // this is a not diametral vertice we do not /= 2
    lower_diam = max_dist;

    if (job_node == -1){ // error case
        report_error("calculate_center: get_multisweep_node: error finding node");
        return;
    }

    printf("#1:i=iteration_number #2:best_lower_diam_bound #3:best_upper_diam_bound #4:best_rayon #5:current_bfs_rayon_approx\n");
    for (int i = 0; i < num_iterations; ++i) {
        fprintf(stderr, "Processing bfs with node %d\n", job_node);
        multisweep_check[job_node] = 1; // set to already done
        temp_middle_nodes = compute_central_vertices(g, job_node, &temp_middle_size, &job_node, &max_dist, &temp_upper_diam);
        if (temp_middle_nodes == NULL)
            return; // non allocated array/ error happened

        lower_diam = max(lower_diam, max_dist);
        cur_rayon_approx = min(cur_rayon_approx, max_dist/2); // this is an approximation is current middle node BFS rayon

        if (upper_diam == -1)
            upper_diam = temp_upper_diam;
        if (temp_upper_diam < upper_diam)
            upper_diam = temp_upper_diam;
        // taking the min, since slight chance cur_rayon_approx will be better
        rayon = min(rayon,(upper_diam + lower_diam) / 4);
        rayon = min(rayon, cur_rayon_approx); // (diametral node eccentricity)/2 can be a better rayon

        update_histogram(histo_center_nodes, temp_middle_nodes, temp_middle_size);
        free(temp_middle_nodes);
        fprintf(stdout, "%dth iteration %d %d %d %0.2f\n", i, lower_diam, upper_diam, rayon, cur_rayon_approx);

        // check if node has already been used, if so, pick a new node
        copy_node = job_node;
        while (multisweep_check[job_node]){
            job_node = random()%g->n;
            while (c[job_node] != c_giant) {
                fprintf(stderr, "Wrong component %d for random node %d \n", c[job_node], job_node);
	            job_node = random()%g->n;
            }
            counter_tries++;
            if (counter_tries >= counter_limit){
                memset(multisweep_check, 0, g->n * sizeof(*multisweep_check)); // reset on too many iterations
                counter_tries = 0;
            }
        }
        if (copy_node != job_node){ // check if loop changed the current node, if so, bfs needed
            job_node = get_multisweep_node(g, job_node, &max_dist);
            if (job_node == -1){ // error case
                report_error("calculate_center: get_multisweep_node: error finding node");
                return;
            }
        }

    }
    fprintf(stdout, "Center nodes found:\n");
    middle_nodes = ratio_histo(histo_center_nodes, g->n, &middle_nodes_size, 1); // FIXME: remove hardcoded and use parameter
    for (int i = 0; i < middle_nodes_size; ++i)
        fprintf(stdout, "%d ", middle_nodes[i]);
    fprintf(stdout, "\n");
    fprintf(stdout, "%d BFS done\n", num_iterations + 1);
    fprintf(stdout, "Final values: %d %d %d\n", lower_diam, upper_diam, rayon);
    free(middle_nodes);
    free(multisweep_check);
}

/******** GRAPH CENTER functions - end *********/