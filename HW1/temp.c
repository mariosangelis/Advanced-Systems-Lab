//#error Please comment out the next two lines under linux, then comment this error
//#include "stdafx.h"  //Visual studio expects this line to be the first one, comment out if different compiler
//#include <windows.h> // Include if under windows

#ifndef WIN32
#include <sys/time.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef __x86_64__
#include "tsc_x86.h"
#endif

#define NUM_RUNS 1
#define CYCLES_REQUIRED 1e8
#define FREQUENCY 2.7e9
#define CALIBRATE
#define alpha 0.2

/*
 *	Initialize the input
 */

void fill_matrix(double * A, int n) {
    for(int i=0; i < n; i++) {
        for(int j=0; j < n; j++) {
            A[n*i+j] = (double) i / (j+1) + 1.0;
            printf("%.2lf ",A[n*i+j]);
        }
        printf("\n");
    }
    printf("\n");
}

void fill_vector(double * x, int n) {
    for(int i=0; i < n; i++) {
        x[i] = (double) i + 1.0;
    }
}

/*
 * matrix-matrix multiplication
 */

void compute(double A[], double B[], int n) {
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            double sum = alpha*B[n*i+j];
            printf("sum=%.2lf \n",B[n*i+j]);
            for(int k = 0; k < n; k++) {
                sum = sum + A[n*i+k]*A[n*j+k];

                printf("A[n*i+k]=%.2lf, A[n*j+k]=%.2lf ",A[n*i+k],A[n*j+k]);
            }
            B[n*i+j] = sum;
            printf("%.2lf ",B[n*i+j]);
        }
        printf("\n");
    }
}

/*
 * Timing function based on the TimeStep Counter of the CPU.
 */
#ifdef __x86_64__
double rdtsc(double A[], double B[], int n) {
    int i, num_runs;
    myInt64 cycles;
    myInt64 start;
    num_runs = NUM_RUNS;

    /*
     * The CPUID instruction serializes the pipeline.
     * Using it, we can create execution barriers around the code we want to time.
     * The calibrate section is used to make the computation large enough so as to
     * avoid measurements bias due to the timing overhead.
     */
#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            compute(A, B, n);
        }
        cycles = stop_tsc(start);

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }
#endif

    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
        compute(A, B, n);
    }

    cycles = stop_tsc(start)/num_runs;
    return (double) cycles;
}
#endif

double c_clock(double A[], double B[], int n) {
    int i, num_runs;
    double cycles;
    clock_t start, end;

    num_runs = NUM_RUNS;
#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        start = clock();
        for (i = 0; i < num_runs; ++i) {
            compute(A, B, n);
        }
        end = clock();

        cycles = (double)(end-start);

        // Same as in c_clock: CYCLES_REQUIRED should be expressed accordingly to the order of magnitude of CLOCKS_PER_SEC
        if(cycles >= CYCLES_REQUIRED/(FREQUENCY/CLOCKS_PER_SEC)) break;

        num_runs *= 2;
    }
#endif

    start = clock();
    for(i=0; i<num_runs; ++i) {
        compute(A, B, n);
    }
    end = clock();

    return (double)(end-start)/num_runs;
}

#ifndef WIN32
double timeofday(double A[], double B[], int n) {
    int i, num_runs;
    double cycles;
    struct timeval start, end;

    num_runs = NUM_RUNS;
#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        gettimeofday(&start, NULL);
        for (i = 0; i < num_runs; ++i) {
            compute(A, B, n);
        }
        gettimeofday(&end, NULL);

        cycles = (double)((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6)*FREQUENCY;

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }
#endif

    gettimeofday(&start, NULL);
    for(i=0; i < num_runs; ++i) {
        compute(A, B, n);
    }
    gettimeofday(&end, NULL);

    return (double)((end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6)/ num_runs;
}

#else

double gettickcount(double A[], double B[], int n) {
    int i, num_runs;
    double cycles, start, end;

    num_runs = NUM_RUNS;
#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        start = (double)GetTickCount();
        for (i = 0; i < num_runs; ++i) {
            compute(A, B, n);
        }
        end = (double)GetTickCount();

        cycles = (end-start)*FREQUENCY/1e3; // end-start provides a measurement in the order of milliseconds

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }
#endif

    start = (double)GetTickCount();
    for(i=0; i < num_runs; ++i) {
        compute(A, B, n);
    }
    end = (double)GetTickCount();

    return (end-start)/num_runs;
}

double queryperfcounter(double A[], double B[], int n, LARGE_INTEGER f) {
    int i, num_runs;
    double cycles;
    LARGE_INTEGER start, end;

    num_runs = NUM_RUNS;
#ifdef CALIBRATE
    while(num_runs < (1 << 14)) {
        QueryPerformanceCounter(&start);
        for (i = 0; i < num_runs; ++i) {
            compute(A, B, n);
        }
        QueryPerformanceCounter(&end);

        cycles = (double)(end.QuadPart - start.QuadPart);

        // Same as in c_clock: CYCLES_REQUIRED should be expressed accordingly to the order of magnitude of f
        if(cycles >= CYCLES_REQUIRED/(FREQUENCY/f.QuadPart)) break;

        num_runs *= 2;
    }
#endif

    QueryPerformanceCounter(&start);
    for(i=0; i < num_runs; ++i) {
        compute(A, B, n);
    }
    QueryPerformanceCounter(&end);

    return (double)(end.QuadPart - start.QuadPart)/num_runs;
}

#endif

int main(int argc, char **argv) {
    if (argc!=2) {printf("usage: FW <n>\n"); return -1;}
    int n = atoi(argv[1]);
    printf("n=%d \n",n);
    double* A = (double *)malloc(n*n*sizeof(double));
    double* B = (double *)malloc(n*n*sizeof(double));

    fill_matrix(A, n);
    fill_matrix(B, n);

    compute(A, B, n);
    return 0;
}

