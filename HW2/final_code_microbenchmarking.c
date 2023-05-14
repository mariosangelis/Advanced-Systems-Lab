#include "include/microbenchmark.h"
#include "include/tsc_x86.h"
#include "include/foo.h"
#include <stdio.h>
#include <unistd.h>
#define CYCLES_REQUIRED 1e8
#define ARRAY_SIZE 1000

double *array;
double check_var,check_var2,result1=0.0,result2=0.0,result3=0.0,result4=0.0,result5=0.0,result6=0.0,result7=0.0,result8=0.0;
int DIV_MIN=0,FOO_MIN=0,DIV_MIN_GAP=0,FOO_MIN_GAP=0;

double generate_random_double(){
    double min=-10000, max=10000;
    double range = max - min, div = RAND_MAX / range;
    return min + (rand() / div);
}


void initialize_array(){
    int i;

    array=(double *)malloc(ARRAY_SIZE*sizeof(double));
    for(i=0;i<ARRAY_SIZE;i++){
      array[i]=generate_random_double();
    }
}

void initialize_microbenchmark_data (microbenchmark_mode_t mode) {

  initialize_array();
  /* You can use to initialize some data if needed */
    switch (mode) {
        case MAX_LAT:
          break;
        case MAX_GAP:
          break;
        case DIV_LAT:
          break;
        case DIV_GAP:
          break;
        case DIV_LAT_MIN:
          DIV_MIN=1;
          break;
        case DIV_GAP_MIN:
          DIV_MIN_GAP=1;
          break;
        case FOO_LAT:
          break;
        case FOO_GAP:
          break;
        case FOO_LAT_MIN:
          FOO_MIN=1;
          break;
        case FOO_GAP_MIN:
          FOO_MIN_GAP=1;
          break;
        default: break;
    }
}

double microbenchmark_get_max_latency() {
    /* Implement your microbenchmark benchmark here */

    double cycles;
    myInt64 start;
    int num_runs=1,i,k;
    check_var=generate_random_double();

    //Warming up
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            for(k=0;k<ARRAY_SIZE;k++){
                check_var = (check_var < array[k]) ? array[k] : check_var;
            }
        }

        cycles = stop_tsc(start);
        if(cycles >= CYCLES_REQUIRED){break;}
        num_runs *= 2;
    }

    start = start_tsc();
    for (i = 0; i < num_runs; i++) {
        for(k=0;k<ARRAY_SIZE;k++){
            check_var = (check_var < array[k]) ? array[k] : check_var;
        }
    }

    cycles = (double) stop_tsc(start);

    free(array);
    return cycles/(num_runs*ARRAY_SIZE);
}

double microbenchmark_get_max_gap() {
    /* Implement your microbenchmark benchmark here */
    double cycles;
    myInt64 start;
    int num_runs=1,i,k,iterations=10;
    check_var=generate_random_double();

    //Warming up
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            for(k=0;k<ARRAY_SIZE;k++){
                check_var = (check_var < array[k]) ? array[k] : check_var;
            }
        }

        cycles = stop_tsc(start);
        if(cycles >= CYCLES_REQUIRED){break;}
        num_runs *= 2;
    }

    //check_var=0.0;
    start = start_tsc();
    for (i = 0; i < num_runs; i++) {
        for(k=0;k<iterations;k+=2){
            array[k] = (array[k] > check_var) ? array[k] : check_var;
            array[k+1] = (array[k+1] > check_var) ? array[k+1] : check_var;
        }
    }

    cycles = (double) stop_tsc(start);
    free(array);
    return cycles/(num_runs*iterations);
}

double microbenchmark_get_div_latency() {
    /* Implement your microbenchmark benchmark here */
    double cycles=0;
    myInt64 start;
    int num_runs=1,i,k;

    //Warming up
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            for(k=0;k<ARRAY_SIZE;k++){
                check_var= check_var/array[k];
            }
        }

        cycles = stop_tsc(start);
        if(cycles >= CYCLES_REQUIRED){break;}
        num_runs *= 2;
    }

    if(DIV_MIN==1){
        check_var=0.0;
        start = start_tsc();
        for (i = 0; i < num_runs; i++) {
            for(k=0;k<ARRAY_SIZE;k++){
                check_var= check_var/array[k];
            }
        }
        cycles = (double) stop_tsc(start);
    }
    else{
        check_var=generate_random_double();
        start = start_tsc();
        for (i = 0; i < num_runs; i++) {
            for(k=0;k<ARRAY_SIZE;k++){
                check_var= check_var/array[k];
            }
            check_var=99945.122;
        }
        cycles = (double) stop_tsc(start);
    }
    free(array);
    return cycles/(num_runs*ARRAY_SIZE);
}

double microbenchmark_get_div_gap() {
    /* Implement your microbenchmark benchmark here */
    double cycles=0;
    myInt64 start;
    int num_runs=1,i,k;

    //Warming up
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            for(k=0;k<ARRAY_SIZE;k++){
                check_var= 100.0/array[k];
            }
        }

        cycles = stop_tsc(start);
        if(cycles >= CYCLES_REQUIRED){break;}
        num_runs *= 2;
    }

    if(DIV_MIN_GAP==1){
        start = start_tsc();
        for (i = 0; i < num_runs; i++) {
            for(k=0;k<ARRAY_SIZE;k+=2){
                result1=0.0/array[k];
                result2=0.0/array[k+1];
            }
        }
        cycles = (double) stop_tsc(start);
    }
    else{
        start = start_tsc();
        for (i = 0; i < num_runs; i++) {
            for(k=0;k<ARRAY_SIZE;k+=2){
                result1=100.0/array[k];
                result2=100.0/array[k+1];
            }
        }
        cycles = (double) stop_tsc(start);
    }
    free(array);
    return cycles/(num_runs*ARRAY_SIZE);
}

double microbenchmark_get_foo_latency() {
    /* Implement your microbenchmark benchmark here */
    double cycles=0;
    myInt64 start;
    int num_runs=1,i,k;

    //Warming up
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            for(k=0;k<ARRAY_SIZE;k++){
                check_var=foo(check_var);
            }
        }

        cycles = stop_tsc(start);
        if(cycles >= CYCLES_REQUIRED){break;}
        num_runs *= 2;
    }

    if(FOO_MIN==1){
        check_var=generate_random_double();
        start = start_tsc();
        for (i = 0; i < num_runs; i++) {
            for(k=0;k<ARRAY_SIZE;k++){
                check_var=foo(check_var);
            }
        }
        cycles = (double) stop_tsc(start);
        return cycles/(num_runs*ARRAY_SIZE);
    }
    else{
        check_var2=1.272019649514069;

        start = start_tsc();
        for (i = 0; i < num_runs; i++) {
            for(k=0;k<ARRAY_SIZE;k++){
                check_var2=foo(check_var2);
            }
        }
        cycles = (double) stop_tsc(start);
    }
    free(array);
    return cycles/(num_runs*ARRAY_SIZE);
}

double microbenchmark_get_foo_gap() {
    /* Implement your microbenchmark benchmark here */
    double cycles=0;
    myInt64 start,stop;
    int num_runs=1,i,k;

    //Warming up
    while(num_runs < (1 << 14)) {
        start = start_tsc();
        for (i = 0; i < num_runs; ++i) {
            for(k=0;k<ARRAY_SIZE;k+=1){
                check_var2=foo(array[k]);
            }
        }

        cycles = stop_tsc(start);
        if(cycles >= CYCLES_REQUIRED){break;}
        num_runs *= 2;
    }

    if(FOO_MIN_GAP==1){
        start = start_tsc();
        for (i = 0; i < num_runs; i++) {
            for(k=0;k<ARRAY_SIZE;k+=4){
                array[k]=foo(array[k]);
                array[k+1]=foo(array[k+1]);
                array[k+2]=foo(array[k+2]);
                array[k+3]=foo(array[k+3]);
            }
        }
        cycles = (double) stop_tsc(start);
    }
    else{
        check_var2=1.272019649514069;
        start = start_tsc();
        for (i = 0; i < num_runs; i++) {
            for(k=0;k<ARRAY_SIZE;k+=4){
              array[k]=foo(check_var2);
              array[k+1]=foo(check_var2);
              array[k+2]=foo(check_var2);
              array[k+3]=foo(check_var2);
            }
        }
        cycles = (double) stop_tsc(start);
    }

    free(array);
    return cycles/(num_runs*ARRAY_SIZE);
}
