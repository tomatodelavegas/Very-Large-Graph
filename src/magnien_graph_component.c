/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#include "magnien_graph_component.h"
#include "magnien_utils.h"
#include "magnien_queue.h"

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

/******** CONNECTED COMPONENTS functions - end *********/