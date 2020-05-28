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
    int *tree = depth_bfs_tree(g, start, &max, NULL, NULL, NULL);
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
    int *depth_tree = depth_bfs_tree(g, start, &max, NULL, NULL, NULL);
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
 ** struct leaf_node *leafs:       optionnal pre-allocated leaf_nodes list
 ** int *nb_leafs:                 if leafs != NULL && *nb_leafs == 0, will compute the number of leafs, otherwise use it
 **                                beware: (v should not be a leaf itself for leaf detections
 **                                to get all graph leafs)
 **/
int *depth_bfs_tree(graph *g, int v, int *max, int **magnien_tree, struct leaf_node *leafs, int *nb_leafs)
{
    bool compute_leafs = leafs != NULL && *nb_leafs == 0; // watch out with order here
    int u, i, is_leaf;
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
        is_leaf = true;
        for (i=0;i<g->degrees[v];++i) {
            u = g->links[v][i];
            if (depth_tree[u]==-1){
                queue_add(q,u);
                is_leaf = false;
                depth_tree[u] = curr_depth;
                tree[u] = v;
            }
        }
        /** leafs computation **/
        if (leafs != NULL && is_leaf) {
            if (compute_leafs) { // recompute all leafs
                leafs[*nb_leafs].id = v;
                leafs[*nb_leafs].dist = curr_depth - 1;
                *nb_leafs += 1; // watch out not ++; !
            }
        }
    }
    if (leafs != NULL && !compute_leafs) { // updating leafs
        // if leafs are supposed to be updated, go through them and take there depth as
        for (i = 0; i < *nb_leafs; ++i) {
            if (depth_tree[leafs[i].id] != -1) {
                leafs[i].dist = depth_tree[leafs[i].id];
            } else {
                report_error("Something went really bad, computed leaf not in BFS");
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
 ** TODO: depth_bfs_tree could return the by level vertices list for quicker computation, using dynamic sized array and realloc...
 **/
int* compute_central_vertices(graph *g, int start, int *resulting_size, int* next_node, int *diameter, int *diam_upper)
{
    int i = 0;
    int max = -1;
    int *magnien_tree = NULL;
    int *depth_tree = depth_bfs_tree(g, start, &max, &magnien_tree, NULL, NULL);
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



/**
 ** Steps:
 ** Init:
 **     - u = random node
 **     - v = get_multi_sweep_node(u) // diametral node
 **     - middles = compute_central_vertices(v, ...)
 **     - u = bestmiddle
 **     - goto[1]
 ** DO:
 **     - v = pop_farthest_leaf(leafs, &nb_leafs)->id; // v is diametral leaf
 **     - 
 **     -[1] depth_bfs_tree(g, u, &max_dist, NULL, leafs, &nb_leafs) // BFS from best middle u
 **     - // compute bounds
 **     - remove_leafs_closer_than(lw)
 **     - 
 **/
void compute_center_convergence(graph *g, int num_iterations, int* c, int c_giant)
{
    struct leaf_node *leafs, *tmp_leaf; // leafs is array, tmp_leaf is ptr
    int *depth_tree, *temp_middle_nodes, *middle_nodes, *histo_center_nodes;
    int max_dist = 0, temp_middle_size, middle_nodes_size, nb_leafs = 0;
    int lower_diam = 0, rayon = ~0U >> 1, upper_diam = -1, temp_upper_diam = 0;
    int u,v, i, iter = 1;
    bool finished = false; // if true, there is no more leafs
    if ((histo_center_nodes = calloc(g->n + 1, sizeof(int)))== NULL)
        report_error("compute_center_convergence: malloc error histo");
    if ((leafs = (struct leaf_node *)calloc(g->n, sizeof(struct leaf_node)))== NULL)
        report_error("compute_center_convergence: malloc error histo");
    printf("#1:i=iteration_number #2:best_lower_diam_bound #3:best_upper_diam_bound #4:best_rayon\n");
    // INIT
    u = random()%g->n;
    while (c[u] != c_giant)
        u = random()%g->n;
    v = get_multisweep_node(g, u, &max_dist); // max_dist_u
    fprintf(stderr, "Starting bfs with node %d, then %d\n", u, v);
    goto DIAMETRALBFS;
    // main loop
    do {
        tmp_leaf = pop_farthest_leaf(leafs, &nb_leafs);
        if (tmp_leaf == NULL)
            break;
        v = tmp_leaf->id;
        free(middle_nodes);
        // * diametral BFS (from v)
    DIAMETRALBFS: fprintf(stderr, "Diametral BFS (from %d)... (%d leafs)\n", v, nb_leafs + 1);
        temp_middle_nodes = compute_central_vertices(g, v, &temp_middle_size, &u, &max_dist, &temp_upper_diam);
        // compute_central_vertices will change u but it doesn't matter since set with middle nodes array
        if (temp_middle_nodes == NULL)
            return;
        // * diametral BFS bounds: lower_diam & upper_diam
        lower_diam = max(lower_diam, max_dist); // diametral nodes are great for diam
        if (upper_diam == -1)
            upper_diam = temp_upper_diam;
        if (temp_upper_diam < upper_diam)
            upper_diam = temp_upper_diam;
        // * Histogram center update
        update_histogram(histo_center_nodes, temp_middle_nodes, temp_middle_size);
        // * Take best center // or could just compute ratio_update
        // * at the end if u the the last middle node
        middle_nodes = ratio_histo(histo_center_nodes, g->n, &middle_nodes_size, 1);
        u = middle_nodes[random()%middle_nodes_size]; // u is one of the overall best centers
        //u = temp_middle_nodes[random()%temp_middle_size]; // u is a random last middle node
        free(temp_middle_nodes);
        if (iter != 1 && nb_leafs == 0) // we just removed the last leaf and do not want to recompute again
        {
            free(leafs);
            leafs = NULL;
        }
    CENTRALBFS: depth_tree = depth_bfs_tree(g, u, &max_dist, NULL, leafs, &nb_leafs);
        fprintf(stderr, "Central BFS (from %d) DONE (%d leafs)\n", u, nb_leafs);
        // * central BFS bounds: rayon
        rayon = min(rayon, max_dist); // middle nodes are great to approximate rayon
        finished = remove_leafs_closer_than(leafs, &nb_leafs, lower_diam / 2);
        // TODO: do we use this ?
        // TODO: -> get rid of struct leaf_node and have g->n sized int array instead
        // TODO: this will save 1 g->n sized int array !
        free(depth_tree);
        fprintf(stdout, "%dth iteration %d %d %d\n", iter, lower_diam, upper_diam, rayon);
        //fprintf(stderr, "%d, %d, %d, %d\n", finished, iter, num_iterations, nb_leafs);
    } while(!finished && ++iter < num_iterations);
    // we still have middle_nodes access
    fprintf(stdout, "Center nodes found:\n");
    for (i = 0; i < middle_nodes_size; ++i)
        fprintf(stdout, "%d ", middle_nodes[i]);
    fprintf(stdout, "\n%d BFS done\n", 2*iter + 1);
    fprintf(stdout, "Final values: %d %d %d\n", lower_diam, upper_diam, rayon);
    fprintf(stdout, "Approximated diameter: %d; Approximated rayon: %d\n", lower_diam, rayon);
    free(histo_center_nodes);
    if (leafs != NULL)
        free(leafs);
    free(middle_nodes);
}

/** MOD: Added
 ** Steps:
 ** - depth_bfs_tree() gets the farthest points
 ** - Do bfs from these nodes
 ** - Store the bfs somehow (tree or list) (depth list)
 ** - We get a center approximation and a rayon approximation
 ** - (First version) make intersection between lists found
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

    int nb_bfs = num_iterations;
    int *histo_center_nodes;
    if ((histo_center_nodes = calloc(g->n + 1, sizeof(int)))== NULL)
        report_error("calculate_center: malloc error histo");
    int upper_diam = -1, lower_diam, rayon = ~0U >> 1; // or use INT_MAX
    int temp_upper_diam = 0;
    float cur_rayon_approx;

    fprintf(stderr, "Starting bfs with node %d\n", start);
    int *multisweep_check;
    if ((multisweep_check = calloc(g->n + 1, sizeof(int)))== NULL)
        report_error("calculate_center: multisweep_check: error malloc()");
    int counter_tries = 0;
    int counter_limit = 100; // FIXME use a parameter instead

    int *results = NULL;
    int job_node = get_multisweep_node(g, start, &max_dist);
    cur_rayon_approx = max_dist; // this is a not diametral vertice we do not /= 2
    lower_diam = max_dist;

    if (job_node == -1)
        report_error("calculate_center: get_multisweep_node: error finding node");

    printf("#1:i=iteration_number #2:best_lower_diam_bound #3:best_upper_diam_bound #4:best_rayon #5:current_bfs_rayon_approx\n");
    for (int i = 0; i < num_iterations; ++i) {
        fprintf(stderr, "Processing bfs with node %d\n", job_node);
        multisweep_check[job_node] = 1; // set to already done
        temp_middle_nodes = compute_central_vertices(g, job_node, &temp_middle_size, &job_node, &max_dist, &temp_upper_diam);
        if (temp_middle_nodes == NULL)
            return; // non allocated array/ error happened

        lower_diam = max(lower_diam, max_dist);
        cur_rayon_approx = min_float(cur_rayon_approx, (float)max_dist/2); // this is an approximation is current middle node BFS rayon

        if (upper_diam == -1)
            upper_diam = temp_upper_diam;
        if (temp_upper_diam < upper_diam)
            upper_diam = temp_upper_diam;
        // taking the min, since slight chance cur_rayon_approx will be better
        rayon = min(rayon, max_dist);//(upper_diam + lower_diam) / 4);
        rayon = min(rayon, ceil(cur_rayon_approx)); // (diametral node eccentricity)/2 can be a better rayon

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
            if (counter_tries >= counter_limit) {
                memset(multisweep_check, 0, g->n * sizeof(*multisweep_check)); // reset on too many iterations
                counter_tries = 0;
            }
        }
        if (copy_node != job_node){ // check if loop changed the current node, if so, bfs needed
            nb_bfs++;
            job_node = get_multisweep_node(g, job_node, &max_dist);
            if (job_node == -1)
                report_error("calculate_center: get_multisweep_node: error finding node");
        }

    }
    fprintf(stdout, "Center nodes found:\n");
    middle_nodes = ratio_histo(histo_center_nodes, g->n, &middle_nodes_size, 1); // FIXME: remove hardcoded and use parameter
    for (int i = 0; i < middle_nodes_size; ++i)
        fprintf(stdout, "%d ", middle_nodes[i]);
    // perform BFS from central nodes to get diametral node to perform BFS
    // at each iteration:
    // - from node u: 1 BFS for better center, rayon, diam approximation
    //   (gives us a list of most probable centers)
    // - from non visited most probable central node c: 1 BFS to compute next u
    //   which is the next non visited diametral starting point node
    //   (gives us a list of diametral nodes)
    for (int i = 0; i < middle_nodes_size; ++i) {
        nb_bfs += 2;
        num_iterations += 2;
        copy_node = middle_nodes[i];
        job_node = get_multisweep_node(g, copy_node, &max_dist); // one random diametral node from middle
        rayon = max_dist; // :'(
        lower_diam = max(lower_diam, get_vertice_eccentricity(g, job_node));
        fprintf(stdout, "\nProbable central node is %d", copy_node);
        fprintf(stdout, "\nMultiple BFS yielded %d as a diametral node", job_node);
        fprintf(stdout, "\ncentral BFS %dth iteration %d %d %d", num_iterations, lower_diam, upper_diam, rayon);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "%d BFS done\n", nb_bfs);
    fprintf(stdout, "Final values: %d %d %d\n", lower_diam, upper_diam, rayon);
    fprintf(stdout, "Approximated diameter: %d; Approximated rayon: %d\n", lower_diam, rayon);
    free(middle_nodes);
    free(multisweep_check);
}

/******** GRAPH CENTER functions - end *********/