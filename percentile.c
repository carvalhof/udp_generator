#include <stdio.h>
#include <stdlib.h>

int cmp_func(const void * a, const void * b) {
    int da = (*(int*)a);
    int db = (*(int*)b);

    return da - db;
}

int main(int argc, char **argv) {
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <percentile> <file_name>\n", argv[0]);
        exit(-1);
    }

    FILE *fp = fopen(argv[2], "r");
    if(!fp) {
        exit(-1);
    }

    int n;
    int __attribute__((unused)) ret = fscanf(fp, "%d\n", &n);

    int *arr = (int*) malloc(n * sizeof(int));
    if(!arr) {
        exit(-1);
    }

    int val;
    for(int i = 0; i < n; i++) {
        ret = fscanf(fp, "%d\n", &val);
        arr[i] = val;
    }

    double p = strtod(argv[1], NULL);
    int percentile = (p/100.0) * n;

    qsort(arr, n, sizeof(int), cmp_func);
    printf("%d\n", arr[percentile]);

    free(arr);

    return 0;
}