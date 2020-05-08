/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#include "magnien_graph_utils.h"
#include "magnien_utils.h"

/******** GRAPH MANAGEMENT functions - begin *********/

void free_graph(graph *g){
    if (g!=NULL) {
        if (g->links!=NULL) {
            if (g->links[0]!=NULL)
                free(g->links[0]);
            free(g->links);
        }
        if (g->capacities!=NULL)
            free(g->capacities);
        if (g->degrees!=NULL)
            free(g->degrees);
        free(g);
    }
}

graph *graph_from_file(FILE *f){
    char line[MAX_LINE_LENGTH];
    int i, u, v;
    graph *g;

    if( (g=(graph *)malloc(sizeof(graph))) == NULL )
        report_error("graph_from_file: malloc() error 1");

    /* read n */
    if( fgets(line,MAX_LINE_LENGTH,f) == NULL )
        report_error("graph_from_file: read error (fgets) 1");
    if( sscanf(line, "%d\n", &(g->n)) != 1 )
        report_error("graph_from_file: read error (sscanf) 2");

    /* read the degree sequence */
    if( (g->capacities=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("graph_from_file: malloc() error 2");
    if( (g->degrees=(int *)calloc(g->n,sizeof(int))) == NULL )
        report_error("graph_from_file: calloc() error");
    for(i=0;i<g->n;i++){
        if( fgets(line,MAX_LINE_LENGTH,f) == NULL )
            report_error("graph_from_file; read error (fgets) 2");
        if( sscanf(line, "%d %d\n", &v, &(g->capacities[i])) != 2 )
            report_error("graph_from_file; read error (sscanf) 2");
        if( v != i ){
            fprintf(stderr,"Line just read : %s\n i = %d; v = %d\n",line,i,v);
            report_error("graph_from_file: error while reading degrees");
        }
    }

    /* compute the number of links */
    g->m=0;
    for(i=0;i<g->n;i++)
        g->m += g->capacities[i];
    g->m /= 2;

    /* create contiguous space for links */
    if (g->n==0){
        g->links = NULL; g->degrees = NULL; g->capacities = NULL;
    }
    else {
        if( (g->links=(int **)malloc(g->n*sizeof(int*))) == NULL )
            report_error("graph_from_file: malloc() error 3");
        if( (g->links[0]=(int *)malloc(2*g->m*sizeof(int))) == NULL )
            report_error("graph_from_file: malloc() error 4");
        // Every links are stored in only one array, the 0 index one
        // Then each links[i] => i: node number
        // This loop sets all the pointers to allow the good capacity of links to
        // be written for each nodes
        for(i=1;i<g->n;i++)
            g->links[i] = g->links[i-1] + g->capacities[i-1];
    }

    /* read the links */
    for(i=0;i<g->m;i++) {
        if( fgets(line,MAX_LINE_LENGTH,f) == NULL )
            report_error("graph_from_file; read error (fgets) 3");
        if( sscanf(line, "%d %d\n", &u, &v) != 2 ){
            fprintf(stderr,"Attempt to scan link #%d failed. Line read:%s\n", i, line);
            report_error("graph_from_file; read error (sscanf) 3");
        }
        if ( (u>=g->n) || (v>=g->n) || (u<0) || (v<0) ) {
            fprintf(stderr,"Line just read: %s",line);
            report_error("graph_from_file: bad node number");
        }
        if ( (g->degrees[u]>=g->capacities[u]) ||
             (g->degrees[v]>=g->capacities[v]) ){
            fprintf(stderr, "reading link %s\n", line);
            report_error("graph_from_file: too many links for a node");
        }
        // For node u write the linked node in the next available int place
        g->links[u][g->degrees[u]] = v;
        g->degrees[u]++;
        g->links[v][g->degrees[v]] = u;
        g->degrees[v]++;
    }
    for(i=0;i<g->n;i++)
        if (g->degrees[i]!=g->capacities[i])
            report_error("graph_from_file: capacities <> degrees");
    if( fgets(line,MAX_LINE_LENGTH,f) != NULL )
        report_error("graph_from_file; too many lines");

    return(g);
}

/* Graph sorting and renumbering */

void sort_graph(graph *g){
    int i;
    for(i=0;i<g->n;i++)
        quicksort(g->links[i],g->degrees[i]);
}

int *sort_nodes_by_degrees(graph *g){ /* in O(m) time and O(n) space */
    int *distrib, *resu;
    int **tmp, *tmpi;
    int v, i, j, x;

    if( (distrib=(int *)calloc(g->n,sizeof(int))) == NULL )
        report_error("sort_nodes_by_degrees: calloc() error");

    for (v=g->n-1;v>=0;v--)
        distrib[g->degrees[v]]++;

    if( (tmpi=(int *)calloc(g->n,sizeof(int))) == NULL )
        report_error("sort_nodes_by_degrees: calloc() error");
    if( (tmp=(int **)malloc(g->n*sizeof(int *))) == NULL )
        report_error("sort_nodes_by_degrees: malloc() error");
    if( (tmp[0]=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("sort_nodes_by_degrees: malloc() error");
    for (i=1;i<g->n;i++)
        tmp[i] = tmp[i-1] + distrib[i-1];

    for (v=g->n-1;v>=0;v--) {
        tmp[g->degrees[v]][tmpi[g->degrees[v]]] = v;
        tmpi[g->degrees[v]]++;
    }

    if( (resu=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("sort_nodes_by_degrees: malloc() error");

    x = 0;
    for (i=g->n-1;i>=0;i--)
        for (j=tmpi[i]-1;j>=0;j--)
            resu[x++] = tmp[i][j];

    free(tmpi);
    free(tmp[0]);
    free(tmp);
    free(distrib);
    return(resu);
}

void renumbering(graph *g, int *perm){
    int *tmpp, **tmppp;
    int i, j;

    for (i=g->n-1;i>=0;i--)
        for (j=g->degrees[i]-1;j>=0;j--)
            g->links[i][j] = perm[g->links[i][j]];

    if( (tmpp=(int *)malloc(g->n*sizeof(int))) == NULL )
        report_error("renumbering: malloc() error");
    if( (tmppp=(int **)malloc(g->n*sizeof(int *))) == NULL )
        report_error("renumbering: malloc() error");

    memcpy(tmppp,g->links,g->n*sizeof(int *));
    for (i=g->n-1;i>=0;i--)
        g->links[perm[i]] = tmppp[i];

    memcpy(tmpp,g->degrees,g->n*sizeof(int));
    for (i=g->n-1;i>=0;i--)
        g->degrees[perm[i]] = tmpp[i];

    memcpy(tmpp,g->capacities,g->n*sizeof(int));
    for (i=g->n-1;i>=0;i--)
        g->capacities[perm[i]] = tmpp[i];

    free(tmpp);
    free(tmppp);
}

void random_renumbering(graph *g){
    int *perm;
    perm = random_perm(g->n);
    renumbering(g,perm);
    free(perm);
}

/******** GRAPH MANAGEMENT functions - end *********/