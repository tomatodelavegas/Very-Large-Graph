
/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "prelim.c"

/******** QUEUE functions - begin *********/

typedef struct queue{
  int size;
  int *elts;
  int begin;
  int end;
} queue;

queue *empty_queue(int size){
  queue *q;
  if( (q=(queue *)malloc(sizeof(queue))) == NULL )
    report_error("empty_queue: Pb with malloc");
  q->size = size+1;
  if( (q->elts = (int *)malloc((size+1)*sizeof(int))) == NULL )
    report_error("empty_queue: Pb with malloc");
  q->begin = q->end = 0;
  return(q);
}

void free_queue(queue *q){
  if ((q==NULL) || (q->elts==NULL))
    report_error("free_queue: NULL pointer");
  free(q->elts);
  free(q);
}

int is_empty_queue(queue *q){
  return(q->begin == q->end);
}

int is_full_queue(queue *q){
  return(q->begin == (q->end+1)%q->size);
}

void queue_add(queue *q, int e){
  if (is_full_queue(q))
    report_error("queue_add: queue is full");
  q->elts[q->end] = e;
  q->end++;
  q->end %= q->size;
}

int queue_get(queue *q){
  int r;
  if (is_empty_queue(q))
    report_error("queue_get: queue is empty");
  r = q->elts[q->begin];
  q->begin++;
  q->begin %= q->size;
  return(r);
 }

/******** QUEUE functions - end *********/

/******** DISTANCE functions - begin *********/

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

/* degrees in a tree */
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

/******** CONNECTED COMPONENTS functions - begin *********/

int aux_connected_components(graph *g, int *comp, int v, int num_comp){
  int u, size=0, i;
  queue *q;
  q = empty_queue(g->n);
  queue_add(q,v);
  comp[v] = num_comp;
  size++;
  while (!is_empty_queue(q)) {
    v = queue_get(q);
    for (i=0;i<g->degrees[v];i++) {
      u = g->links[v][i];
      if (comp[u]==-1){
	queue_add(q,u);
	comp[u] = num_comp;
	size++;
      }
      else if (comp[u]!=num_comp)
	report_error("aux_connected_components: Pb");
    }
  }
  free_queue(q);
  return(size);
}

int connected_components(graph *g, int *components, int *sizes){
  int nb_components = 0, i;
  for (i=0;i<g->n;i++)
    if (components[i]==-1) {
      sizes[nb_components] = aux_connected_components(g,components,i,nb_components);
      nb_components++;
    }
  return(nb_components);
}

int giant(int *sizes, int n){
  return(index_max_array(sizes, n));
}


/* Output functions */

void usage(char *c){
  fprintf(stderr,"Usage: %s -diam nb_max difference\n",c);
  fprintf(stderr,"Usage: %s -prec nb_max precision\n",c);
  fprintf(stderr,"Usage: %s -tlb|dslb|tub|rtub|hdtub nb [deg_begin]\n",c);
  fprintf(stderr, "\n");
  fprintf(stderr," -diam nb_max difference: compute bounds for the diameter until the difference between the best bounds is at most 'difference', or until nb_max iterations have been done.\n");
  fprintf(stderr," -prec nb_max precision: compute bounds for the diameter until it is evaluated with a relative error of at most 'precision', or until nb_max iterations have been done.\n");
  fprintf(stderr, "\n");
  fprintf(stderr, " -tlb nb: computes trivial lower bounds, from nb randomly chosen nodes.\n");
  fprintf(stderr," -dslb nb: computes double-sweep lower bounds, from nb randomly chosen nodes.\n");
  fprintf(stderr, " -tub nb: computes trivial upper bounds, from nb randomly chosen nodes.\n");
  fprintf(stderr, " -rtub nb: computes random tree upper bounds, from nb randomly chosen nodes.\n");
  fprintf(stderr," -hdtub nb deg_begin: computes highest degree tree upper bounds, from nb nodes starting from first node with degree lesser than or equal to deg_begin. 0 means start from the highest degree node.\n");
  exit(-1);
}


/* MAIN */
int main(int argc, char **argv){
  graph *g;
  int i;
  int *sorted_nodes, *dist;
  int tlb, diam, rtub, dslb, tub,  hdtub, nb_max, prec_option;
  int deg_begin=0;
  float precision;
  int *c, *c_s, nb_c, c_giant, size_giant;


  srandom(time(NULL));

  /* parse the command line */
  tlb=0; diam = 0; prec_option=0; dslb=0; tub=0; rtub=0; 
  hdtub=0;
  for (i=1; i<argc; i++){
    if (strcmp(argv[i],"-tlb")==0) {
      tlb = 1;
      if( i == argc-1 )
 	usage(argv[0]); 
      nb_max = atoi(argv[++i]);
    }
    else if (strcmp(argv[i],"-diam")==0){
      diam = 1;
      if ((i==argc-2) || (i==argc-1))
	usage(argv[0]);
      nb_max = atoi(argv[++i]);
      precision = atof(argv[++i]);
    }
    else if (strcmp(argv[i],"-prec")==0){
      prec_option = 1;
      if ((i==argc-2) || (i==argc-1))
	usage(argv[0]);
      nb_max = atoi(argv[++i]);
      precision = atof(argv[++i]);
    }
    else if (strcmp(argv[i],"-hdtub")==0){
      hdtub = 1;
      if ((i==argc-2) || (i==argc-1))
	usage(argv[0]);
      nb_max = atoi(argv[++i]);
      deg_begin = atoi(argv[++i]);
    }
    else if( strcmp(argv[i], "-dslb") == 0 ){
      dslb=1;
      if( (i==argc-1) )
	usage(argv[0]);
      nb_max=atoi(argv[++i]);
    }
    else if( strcmp(argv[i], "-tub") == 0 ){
      tub=1;
      if( (i==argc-1) )
	usage(argv[0]);
      nb_max=atoi(argv[++i]);
    }
    else if( strcmp(argv[i], "-rtub") == 0 ){
      rtub=1;
      if( (i==argc-1) )
	usage(argv[0]);
      nb_max=atoi(argv[++i]);
    }
    else
      usage(argv[0]);
  }
  if (tlb+diam+prec_option+rtub+dslb+tub+hdtub != 1){
    usage(argv[0]);
  }
  
  fprintf(stderr,"Preprocessing the graph...\n");
  fprintf(stderr," reading...\n");
  g = graph_from_file(stdin);
  fprintf(stderr," random renumbering...\n");
  random_renumbering(g);
  fprintf(stderr," %d nodes, %d links.\n",g->n,g->m);
  fflush(stderr);

  /* compute connected components */
  fprintf(stderr," computing connected components...\n");
  fflush(stderr);
  if( (c=(int *)malloc(g->n*sizeof(int))) == NULL )
    report_error("main: malloc() error");
  if( (c_s=(int *)malloc(g->n*sizeof(int))) == NULL )
    report_error("main: malloc() error");
  for (i=0;i<g->n;i++)
    c[i] = c_s[i] = -1;
  nb_c = connected_components(g,c,c_s);
  c_giant = giant(c_s,nb_c);
  size_giant = c_s[c_giant];
  fprintf(stderr," %d components; giant component: %d nodes\n",nb_c,size_giant);
  fflush(stderr);

  if( (dist=(int *)malloc(g->n*sizeof(int))) == NULL )
    report_error("main, dist: malloc() error");
  
  /* trivial lower bound */
  if(tlb) {
    int v,  step=0;
    int new_lower;
    printf("#1:iteration_number #2:i-th_node #3:degree_of_ith_node #4:i-th_lower_bound\n");
    while ( step < nb_max ) {
      /* choose v randomly in the giant component */
      v = random()%g->n;
      while (c[v] != c_giant)
	v = random()%g->n;
      /* lower bound */
      for (i=0;i<g->n;i++)
	dist[i] = -1;
      distances(g,v,dist);
      new_lower = max_in_array(dist,g->n);
      printf("%d %d %d %d\n",step++,v, g->degrees[v], new_lower);
      fflush(stdout);
    }
  }

  /* double-sweep lower bound and highest degree tree upper bound for the diameter */
  else if (diam) {
    int v, upper_step=0, step=0;
    int lower_bound = 0, new_lower;
    int upper_node, new_upper, upper_bound = g->n, *t;
    sorted_nodes = sort_nodes_by_degrees(g);
    printf("#1:i=iteration_number #2:i-th_lower_node #3:i-th_upper_node #4:degree_of_i-th_lower_node #5:degree_of_i-th_upper_node #6:i-th_lower_bound #7:i-th_upper_bound #8:i-th_best_lower_bound #9:i-th_best_upper_bound\n");
    while ((step<nb_max) && (upper_bound-lower_bound > precision)) {
      /* choose v randomly in the giant component */
      v = random()%g->n;
      while (c[v] != c_giant)
	v = random()%g->n;
      /* lower_bound */
      for (i=0;i<g->n;i++)
	dist[i] = -1;
      distances(g,v,dist);
      new_lower = max_distance(g,rand_index_max_array(dist,g->n));
      if (new_lower > lower_bound)
	lower_bound = new_lower;
      /* highest degree tree upper bound */
      upper_node = sorted_nodes[upper_step++];
      while ((upper_step<g->n) && (c[upper_node] != c_giant))
	upper_node = sorted_nodes[upper_step++];
      if (upper_step==g->n)
	report_error("main: too many iterations");
      t = bfs_tree(g,upper_node);
      new_upper = tree_max_dist(t,g->n);
      if (new_upper < upper_bound)
	upper_bound = new_upper;
      printf("%d %d %d %d %d %d %d %d %d\n",step++,v,upper_node, g->degrees[v], g->degrees[upper_node], new_lower, new_upper, lower_bound, upper_bound);
      fflush(stdout);
      free(t);
    }
    free(sorted_nodes);
  }

  else if (prec_option) {
    int v, upper_step=0, step=0;
    int lower_bound = 0, new_lower;
    int upper_node, new_upper, upper_bound = g->n, *t;
    sorted_nodes = sort_nodes_by_degrees(g);

    printf("#1:i=iteration_number #2:i-th_lower_node #3:i-th_upper_node #4:degree_of_i-th_lower_node #5:degree_of_i-th_upper_node #6:i-th_lower_bound #7:i-th_upper_bound #8:i-th_best_lower_bound #9:i-th_best_upper_bound\n");
    while ( (step<nb_max) && ( (float)(upper_bound-lower_bound)/lower_bound > precision) ) {
      /* choose v randomly in the giant component */
      v = random()%g->n;
      while (c[v] != c_giant)
	v = random()%g->n;
      /* lower_bound */
      for (i=0;i<g->n;i++)
	dist[i] = -1;
      distances(g,v,dist);
      new_lower = max_distance(g,rand_index_max_array(dist,g->n));
      if (new_lower > lower_bound)
	lower_bound = new_lower;
      /* highest degree tree upper bound */
      upper_node = sorted_nodes[upper_step++];
      while ((upper_step<g->n) && (c[upper_node] != c_giant))
	upper_node = sorted_nodes[upper_step++];
      if (upper_step==g->n)
	report_error("main: too many iterations");
      t = bfs_tree(g,upper_node);
      new_upper = tree_max_dist(t,g->n);
      if (new_upper < upper_bound)
	upper_bound = new_upper;
      printf("%d %d %d %d %d %d %d %d %d\n",step++,v,upper_node, g->degrees[v], g->degrees[upper_node], new_lower, new_upper, lower_bound, upper_bound);
      fflush(stdout);
      free(t);
    }
    free(sorted_nodes);
  }

  /* highest degree tree upper bound */
  else if (hdtub) {
    int upper_step=0, step=0;
    int upper_node, new_upper, *t;
    sorted_nodes = sort_nodes_by_degrees(g);
    printf("#1:i=iteration_number #2:i-th_node #3:degree_of_i-th_node #4:i-th_upper_bound\n");
    /* Switch to first node with degree lesser than or equal to deg_begin */
    if( deg_begin != 0 ){
      upper_node = sorted_nodes[upper_step];
      while( g->degrees[upper_node] > deg_begin ){
	upper_step++;
	upper_node=sorted_nodes[upper_step];
      }
    }
    while ( step < nb_max ) {
      /* upper bound */
      upper_node = sorted_nodes[upper_step++];
      while ((upper_step<g->n) && (c[upper_node] != c_giant))
	upper_node = sorted_nodes[upper_step++];
      if (upper_step==g->n+1)
	report_error("main: too many iterations");
      t = bfs_tree(g,upper_node);
      new_upper = tree_max_dist(t,g->n);
      printf("%d %d %d %d\n",step++,upper_node, g->degrees[upper_node], new_upper);
      fflush(stdout);
      free(t);
    }
    free(sorted_nodes);
  }

  /* double-sweep lower bound */
  if(dslb) {
    int v,  step=0;
    int new_lower;
    printf("#1:iteration_number #2:i-th_node #3:degree_of_ith_node #4:i-th_lower_bound\n");
    while ( step < nb_max ) {
      /* choose v randomly in the giant component */
      v = random()%g->n;
      while (c[v] != c_giant)
	v = random()%g->n;
      /* lower bound */
      for (i=0;i<g->n;i++)
	dist[i] = -1;
      distances(g,v,dist);
      new_lower = max_distance(g,rand_index_max_array(dist,g->n));
      printf("%d %d %d %d\n",step++,v, g->degrees[v], new_lower);
      fflush(stdout);
    }
  }

  /* trivial upper bound */
  else if(tub){
    int step=0;
    int v, new_upper;
    printf("#1:i=iteration_number #2:i-th_node #3:degree_of_i-th_node #4:i-th_upper_bound\n");
    while ( step < nb_max ){
      /* choose v randomly in the giant component */
      v = random()%g->n;
      while (c[v] != c_giant)
	v = random()%g->n;
      /* upper bound */
      for (i=0;i<g->n;i++)
	dist[i] = -1;
      distances(g,v,dist);
      new_upper = 2*max_in_array(dist,g->n);
      printf("%d %d %d %d\n",step++, v, g->degrees[v], new_upper);
      fflush(stdout);
    }
  }

  /* random tree upper bound */
  else if(rtub){
    int step=0;
    int upper_node, new_upper, *t;
    sorted_nodes = sort_nodes_by_degrees(g);
    printf("#1:i=iteration_number #2:i-th_node #3:degree_of_i-th_node #4:i-th_upper_bound\n");
    while ( step < nb_max ){
      /* upper bound, from a randomly chosen node */
      upper_node = random()%g->n;
      while (c[upper_node] != c_giant)
	upper_node = random()%g->n;
      /* upper bound */
      t = bfs_tree(g,upper_node);
      new_upper = tree_max_dist(t,g->n);
      printf("%d %d %d %d\n",step++,upper_node, g->degrees[upper_node], new_upper);
      fflush(stdout);
      free(t);
    }
    free(sorted_nodes);
  }
  
  /* cannot be used because of renumbering... */
  /* free_graph(g); */
  free(dist);
  free(c);
  free(c_s);
  return(0);
}


