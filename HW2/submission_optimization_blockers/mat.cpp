#include "mat.h"
#include "utils.h"

void mat_build(mat* m, int n1, int n2) {
    build(&m->data, n1, n2);
    m->n1 = n1;
    m->n2 = n2;
}

void mat_destroy(mat* m) {
    destroy(m->data);
}

int mat_len(mat* m) {
    return m->n1 * m->n2;
}

/* retrieve vector element and store at val */
double mat_get(mat *m, int i, int j) {
    int ij = i * m->n2 + j;
    if (ij < 0 || ij >= mat_len(m))
        return NAN;
    return m->data[ij];
}

void mat_set(mat *m, int i, int j, double val) {
    int ij = i * m->n2 + j;
    if (ij < 0 || ij >= mat_len(m))
        return;
    m->data[ij] = val;
}