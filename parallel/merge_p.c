/*
    --  Paralelna implementacija --
    Prvi zadatak iz Multiprocesorskih sistema 

    gcc -o merge_p merge_p.c -fopenmp
    ./merge_p 2 10000000
    
    Student : Milin Ivan E1-79/2023
*/ 

#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TASK_SIZE 100000

void merge(int *X, int n, int *tmp) 
{
    int i = 0;
    int j = n / 2;
    int ti = 0;

    while (i < n / 2 && j < n) 
    {
        if (X[i] < X[j]) 
        {
            tmp[ti] = X[i];
            ti++;
            i++;
        } 
        else 
        {
            tmp[ti] = X[j];
            ti++;
            j++;
        }
    }
    while (i < n / 2) 
    {
        tmp[ti] = X[i];
        ti++;
        i++;
    }
    while (j < n) 
    {
        tmp[ti] = X[j];
        ti++;
        j++;
    }
    memcpy(X, tmp, n * sizeof(int));
}
//=================================================================================
void mergeSort(int *X, int n, int *tmp) 
{
    if (n < 2)
        return;

    #pragma omp task shared(X) if (n > TASK_SIZE)
    mergeSort(X, n / 2, tmp);

    #pragma omp task shared(X) if (n > TASK_SIZE)
    mergeSort(X + (n / 2), n - (n / 2), tmp + n / 2);

    #pragma omp taskwait
    merge(X, n, tmp);
}
//=================================================================================
void printArray(int *a, int size) 
{
    for (int i = 0; i < size; i++)
        printf("%d ", a[i]);
    printf("\n");
}
//=================================================================================
int isSorted(int *a, int size) 
{
    for (int i = 0; i < size - 1; i++)
        if (a[i] > a[i + 1])
            return 0;
    return 1;
}
//=================================================================================
void saveArrayToFile(const char *filename, int *data, int size) 
{
    FILE *file = fopen(filename, "w");
    if (file == NULL) 
    {
        printf("Error: Could not open file %s for writing.\n", filename);
        return;
    }

    for (int i = 0; i < size; i++) 
    {
        fprintf(file, "%d\n", data[i]);
    }

    fclose(file);
    printf("Data successfully written to %s\n", filename);
}
//=================================================================================
int main(int argc, char *argv[]) 
{
    int numThreads = atoi(argv[1]);
    int N = (argc > 1) ? atoi(argv[2]) : 10;
    int *X = malloc(N * sizeof(int));
    int *tmp = malloc(N * sizeof(int));

    int print = 0;

    omp_set_dynamic(1);             
    omp_set_num_threads(numThreads);

    for (int i = 0; i < N; i++) {
        X[i] = random() % 1000;
    }
    saveArrayToFile("before_mergeSort.txt", X, N);

    //PRINT ARRAY BEFORE SORT
    if (print) {
        printf("Array before sort : ");
        printArray(X, N);
    }

    double begin = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        mergeSort(X, N, tmp);
    }
    double end = omp_get_wtime();
    saveArrayToFile("after_mergeSort.txt", X, N);
    

    //PRINT ARRAY AFTER SORT
    if (print) {
        printf("Array after sort  : ");
        printArray(X, N);
    }

    printf("\nTime: %f (s)", end - begin);

    if(isSorted(X, N) == 0)
    {
        printf("\nArray is NOT properly sorted! \n\n");
    }
    else
    {
        printf("\nArray is properly sorted! \n\n");
    }

    free(X);
    free(tmp);
    return (EXIT_SUCCESS);
}

