//#error Please comment out the next two lines under linux, then comment this error
//#include "stdafx.h"  //Visual studio expects this line to be the first one, comment out if different compiler
//#include <windows.h> // Include if under windows

#ifndef WIN32
#include <sys/time.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#ifdef __x86_64__
#include "tsc_x86.h"
#endif

#define NUM_RUNS 1
#define CYCLES_REQUIRED 1e8
#define FREQUENCY 2.2e9
#define CALIBRATE
#define alpha 0.2

/*
 *	Initialize the input
 */


void fill_vector(double * x, int n) {
    for(int i=0; i < n; i++) {
        x[i] = (double) i + 1.0;
    }
}


/*void comp(double *x,double *y,int n){
    double s=0.0;
    for (int i=0;i<n;i++) {
        s=(s+x[i]* x[i])+y[i]*y[i]*y[i];
    }
    x[0]=s;

}*/

void comp(double *x,double *y,int n){
    double s0=0.0,s1=0.0,s2=0.0,s3=0.0,s4=0.0,s5=0.0,s6=0.0,s7=0.0;
    int i;

    for (i=0;i<n;i+=8) {
        s0=(s0+x[i]*x[i])+y[i]*y[i]*y[i];
        s1=(s1+x[i+1]* x[i+1])+y[i+1]*y[i+1]*y[i+1];
        s2=(s2+x[i+2]* x[i+2])+y[i+2]*y[i+2]*y[i+2];
        s3=(s3+x[i+3]* x[i+3])+y[i+3]*y[i+3]*y[i+3];
        s4=(s4+x[i+4]* x[i+4])+y[i+4]*y[i+4]*y[i+4];
        s5=(s5+x[i+5]* x[i+5])+y[i+5]*y[i+5]*y[i+5];
        s6=(s6+x[i+6]* x[i+6])+y[i+6]*y[i+6]*y[i+6];
        s7=(s7+x[i+7]* x[i+7])+y[i+7]*y[i+7]*y[i+7];
    }

    for (;i<n;i+=1) {
        s0=(s0+x[i]*x[i])+y[i]*y[i]*y[i];
    }

    x[0]=s0+s1+s2+s3+s4+s5+s6+s7;
}


/*
 * Timing function based on the TimeStep Counter of the CPU.
 */
#ifdef __x86_64__
double rdtsc(double *x, double *y, int n) {
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
            comp(x, y, n);
        }
        cycles = stop_tsc(start);

        if(cycles >= CYCLES_REQUIRED) break;

        num_runs *= 2;
    }
#endif

    start = start_tsc();
    for (i = 0; i < num_runs; ++i) {
        comp(x, y, n);
    }

    cycles = stop_tsc(start)/num_runs;
    return (double) cycles;
}
#endif


int main(int argc, char **argv) {
    int n;
    double* x;
    double* y;


    int i,k;
    double r=0.0;

    for(k=4;k<=23;k++){
        n=2;

        for(i=1;i<k;i++){
           n*=2;
        }

        x = (double *)malloc(n*sizeof(double));
        y = (double *)malloc(n*sizeof(double));
        fill_vector(x, n);
        fill_vector(y, n);
        r=0.0;

        for(i=0;i<30;i++){

        #ifdef __x86_64__
            r += rdtsc(x,y,n);
            //printf("RDTSC instruction:\n %lf cycles measured => %lf seconds, assuming frequency is %lf MHz. (change in source file if different)\n\n", r, r/(FREQUENCY), (FREQUENCY)/1e6);
        #endif

        }
        r=r/30.0;
        free(x);
        free(y);
        printf("k=%d and performance=%lf\n",k,(5.0*n)/r);
    }

    return 0;
}

