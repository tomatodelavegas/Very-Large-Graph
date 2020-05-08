/* Clemence Magnien and Matthieu Latapy */
/* September 2007 */
/* http://www-rp.lip6.fr/~magnien/Diameter */
/* clemence.magnien@lip6.fr */

#include "magnien_utils.h"

/******** UTILITY functions - begin *********/

void report_error(char *s){
    fprintf(stderr,"%s\n",s);
    exit(-1);
}

int is_in_array(int *a, int e, int left, int right){
    int mid;
    while (right>left) {
        mid = (left+right)/2;
        if (e < a[mid])
            right = mid-1;
        else if (e > a[mid])
            left = mid+1;
        else return(1);
    }
    return(0);
}

int max_in_array(int *a, int n){
    int i, m;
    if (n<=0)
        report_error("max_in_array: empty array");
    m = a[0];
    for (i=1;i<n;i++)
        if (m<a[i])
            m = a[i];
    return(m);
}

int max(int i, int j){
    if (i>j)
        return(i);
    return(j);
}

int index_max_array(int *a, int n){
    int i, index_max, max;
    if (n<=0)
        report_error("index_max_array: empty array");
    max = a[0];
    index_max = 0;
    for (i=1;i<n;i++)
        if (a[i]>max) {
            max = a[i];
            index_max = i;
        }
    return(index_max);
}

int rand_index_max_array(int *a, int n){
    int *tmp;
    int i, max, index, index_r;

    if (n<=0)
        report_error("index_max_array: empty array");
    if( (tmp=malloc(n*sizeof(int))) == NULL )
        report_error("rand_index_max_array: Pb with malloc.");
    max=a[0];
    for (i=1;i<n;i++)
        if (a[i]>max) {
            max = a[i];
        }
    index=0;
    for(i=0;i<n;i++)
        if( a[i] == max ){
            tmp[index]=i;
            index++;
        }
    index_r=tmp[random()%index];
    free(tmp);
    return(index_r);
}

/* in-place quicksort from Fabien Viger */
/* Median of three int */
inline int med3(int a, int b, int c) {
    if(a<b) {
        if(c<b) return (a<c) ? c : a;
        else return b;
    }
    else {
        if(c<a) return (b<c) ? c : b;
        else return a;
    }
}

/* Sort integer arrays in ASCENDING order */
inline void isort(int *v, int t) {
    int i;
    if(t<2) return;
    for(i=1; i<t; i++) {
        register int *w = v+i;
        int tmp = *w;
        while(w!=v && *(w-1)>tmp) { *w = *(w-1); w--; }
        *w = tmp;
    }
}

/* Sort integer arrays in ASCENDING order */
void quicksort(int *v, int t) {
    if(t<15) isort(v,t);
    else {
        int p = med3(v[t>>1], v[(t>>2)+2], v[t-(t>>1)-2]);
        int i=0;
        int j=t-1;
        while(i<j) {
            while(i<=j && v[i]<p) i++;
            while(i<=j && v[j]>p) j--;
            if(i<j) {
                int tmp=v[i];
                v[i++]=v[j];
                v[j--]=tmp;
            }
        }
        if(i==j && v[i]<p) i++;
        quicksort(v,i);
        quicksort(v+i,t-i);
    }
}

int *random_perm(int n){
    int *perm;
    int i, tmp, j;
    if( (perm=(int *)malloc(n*sizeof(int))) == NULL )
        report_error("random_perm: malloc() error");
    for (i=n-1;i>=0;i--)
        perm[i] = i;
    for (i=n-1;i>=0;i--){
        j = random()%n;
        tmp = perm[i];
        perm[i] = perm[j];
        perm[j] = tmp;
    }
    return(perm);
}

int *inverse_perm(int *p, int n){
    int *perm;
    int i;
    if( (perm=(int *)malloc(n*sizeof(int))) == NULL )
        report_error("random_perm: malloc() error");
    for (i=n-1;i>=0;i--)
        perm[p[i]]=i;
    return(perm);
}

/******** UTILITY functions - end *********/