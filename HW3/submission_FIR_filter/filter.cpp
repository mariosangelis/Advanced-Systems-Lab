#include "common.h"
#include <immintrin.h>
#include <math.h>
#include <unistd.h>

void slow_performance1(double *x, double* h, double* y, int N, int M) {
  
  //printf("N=%d,M=%d\n",N,M);
  for (int i = 0; i < N - (M - 1); i++) {
    y[i] = 0.0;
    for (int k = 0; k < M; k++) {
      y[i] += (i + k + 1) * h[k] * fabs(x[i + (M - 1) - k]);
    }
  }
}

void slow2(double *x, double* h, double* y, int N, int M) {
  /* This is the most optimized version. */
  int limit=N-(M-1);
  __m256d h0_mul4,h1_mul4,h2_mul4,h3_mul4,four,x_vector_i,x_vector_i1,x_vector_i2,x_vector_i3,res1,res2,res,mask,h0,h1,h2,h3,i_vector1,i_vector2,i_vector3;
  
  mask = _mm256_castsi256_pd(_mm256_set1_epi64x(0x7FFFFFFFFFFFFFFF));
 
  h0=_mm256_set1_pd(h[0]);
  h1=_mm256_set1_pd(h[1]);
  h2=_mm256_set1_pd(h[2]);
  h3=_mm256_set1_pd(h[3]);
  
  four=_mm256_set1_pd(4.0);
  
  i_vector1=_mm256_set_pd(4,3,2,1);
  i_vector2=_mm256_set_pd(5,4,3,2);
  i_vector3=_mm256_set_pd(6,5,4,3);
  
  h0_mul4=_mm256_mul_pd(h0,four);
  h1_mul4=_mm256_mul_pd(h1,four);
  h2_mul4=_mm256_mul_pd(h2,four);
  h3_mul4=_mm256_mul_pd(h3,four);
  
  h0=_mm256_mul_pd(h0,i_vector1);
  h1=_mm256_fmadd_pd(h1,i_vector1,h1);
  h2=_mm256_fmadd_pd(h2,i_vector2,h2);
  h3=_mm256_fmadd_pd(h3,i_vector3,h3);

  for (int i = 0; i < limit; i+=4) {
    //load
    x_vector_i=_mm256_load_pd(x+i);
    x_vector_i1=_mm256_load_pd(x+i+1);
    x_vector_i2=_mm256_load_pd(x+i+2);
    x_vector_i3=_mm256_load_pd(x+i+3);
    
    x_vector_i=_mm256_and_pd(x_vector_i,mask);
    x_vector_i1=_mm256_and_pd(x_vector_i1,mask);
    x_vector_i2=_mm256_and_pd(x_vector_i2,mask);
    x_vector_i3=_mm256_and_pd(x_vector_i3,mask);
    
    res1=_mm256_mul_pd(x_vector_i,h3);
    res=_mm256_fmadd_pd(x_vector_i1,h2,res1);
    res=_mm256_fmadd_pd(x_vector_i2,h1,res);
    res=_mm256_fmadd_pd(x_vector_i3,h0,res);
    
    h0=_mm256_add_pd(h0_mul4,h0);
    h1=_mm256_add_pd(h1_mul4,h1);
    h2=_mm256_add_pd(h2_mul4,h2);
    h3=_mm256_add_pd(h3_mul4,h3);
    
    _mm256_store_pd(y+i,res);
  }
}

void slow3(double *x, double* h, double* y, int N, int M) {
  /* This is the most optimized version. */
  int limit=N-(M-1);
  __m256d ones,h0_mul4,h1_mul4,h2_mul4,h3_mul4,four,x_vector_i,x_vector_i1,x_vector_i2,x_vector_i3,res1,res,mask,h0,h1,h2,h3,i_vector1,i_vector2,i_vector3;
  mask = _mm256_castsi256_pd(_mm256_set1_epi64x(0x7FFFFFFFFFFFFFFF));
 
  h0=_mm256_set1_pd(h[0]);
  h1=_mm256_set1_pd(h[1]);
  h2=_mm256_set1_pd(h[2]);
  h3=_mm256_set1_pd(h[3]);
  
  four=_mm256_set1_pd(4.0);
  ones=_mm256_set1_pd(1.0);
  
  i_vector1=_mm256_set_pd(4,3,2,1);
  i_vector2=_mm256_add_pd(i_vector1,ones);
  i_vector3=_mm256_add_pd(i_vector2,ones);
  
  h0_mul4=_mm256_mul_pd(h0,four);
  h1_mul4=_mm256_mul_pd(h1,four);
  h2_mul4=_mm256_mul_pd(h2,four);
  h3_mul4=_mm256_mul_pd(h3,four);
  
  h0=_mm256_mul_pd(h0,i_vector1);
  h1=_mm256_fmadd_pd(h1,i_vector1,h1);
  h2=_mm256_fmadd_pd(h2,i_vector2,h2);
  h3=_mm256_fmadd_pd(h3,i_vector3,h3);

  for (int i = 0; i < limit; i+=4) {
    //load
    x_vector_i=_mm256_load_pd(x+i);
    x_vector_i1=_mm256_load_pd(x+i+1);
    x_vector_i2=_mm256_load_pd(x+i+2);
    x_vector_i3=_mm256_load_pd(x+i+3);
    
    x_vector_i=_mm256_and_pd(x_vector_i,mask);
    x_vector_i1=_mm256_and_pd(x_vector_i1,mask);
    x_vector_i2=_mm256_and_pd(x_vector_i2,mask);
    x_vector_i3=_mm256_and_pd(x_vector_i3,mask);
    
    res1=_mm256_mul_pd(x_vector_i,h3);
    res=_mm256_fmadd_pd(x_vector_i1,h2,res1);
    res=_mm256_fmadd_pd(x_vector_i2,h1,res);
    res=_mm256_fmadd_pd(x_vector_i3,h0,res);
    
    h0=_mm256_add_pd(h0_mul4,h0);
    h1=_mm256_add_pd(h1_mul4,h1);
    h2=_mm256_add_pd(h2_mul4,h2);
    h3=_mm256_add_pd(h3_mul4,h3);
    
    _mm256_store_pd(y+i,res);
  }
}

void maxperformance(double *x, double* h, double* y, int N, int M) {
  /* This is the most optimized version. */
  int limit=N-(M-1);
  __m256d ones,h0_mul4,h1_mul4,h2_mul4,h3_mul4,four,x_vector_i,x_vector_i1,x_vector_i2,x_vector_i3,res1,res2,res,mask,h0,h1,h2,h3,i_vector1,i_vector2,i_vector3;
  __m256d res3,x_vector_i4,x_vector_i5,x_vector_i6,x_vector_i7;
  mask = _mm256_castsi256_pd(_mm256_set1_epi64x(0x7FFFFFFFFFFFFFFF));
 
  h0=_mm256_set1_pd(h[0]);
  h1=_mm256_set1_pd(h[1]);
  h2=_mm256_set1_pd(h[2]);
  h3=_mm256_set1_pd(h[3]);
  
  four=_mm256_set1_pd(4.0);
  ones=_mm256_set1_pd(1.0);
  
  i_vector1=_mm256_set_pd(4,3,2,1);
  i_vector2=_mm256_add_pd(i_vector1,ones);
  i_vector3=_mm256_add_pd(i_vector2,ones);
  
  h0_mul4=_mm256_mul_pd(h0,four);
  h1_mul4=_mm256_mul_pd(h1,four);
  h2_mul4=_mm256_mul_pd(h2,four);
  h3_mul4=_mm256_mul_pd(h3,four);
  
  h0=_mm256_mul_pd(h0,i_vector1);
  h1=_mm256_fmadd_pd(h1,i_vector1,h1);
  h2=_mm256_fmadd_pd(h2,i_vector2,h2);
  h3=_mm256_fmadd_pd(h3,i_vector3,h3);

  for (int i = 0; i < limit; i+=8) {
    //load
    x_vector_i=_mm256_load_pd(x+i);
    x_vector_i1=_mm256_load_pd(x+i+1);
    x_vector_i2=_mm256_load_pd(x+i+2);
    x_vector_i3=_mm256_load_pd(x+i+3);
    
    x_vector_i4=_mm256_load_pd(x+i+4);
    x_vector_i5=_mm256_load_pd(x+i+5);
    x_vector_i6=_mm256_load_pd(x+i+6);
    x_vector_i7=_mm256_load_pd(x+i+7);
    
    x_vector_i=_mm256_and_pd(x_vector_i,mask);
    x_vector_i1=_mm256_and_pd(x_vector_i1,mask);
    x_vector_i2=_mm256_and_pd(x_vector_i2,mask);
    x_vector_i3=_mm256_and_pd(x_vector_i3,mask);
    
    x_vector_i4=_mm256_and_pd(x_vector_i4,mask);
    x_vector_i5=_mm256_and_pd(x_vector_i5,mask);
    x_vector_i6=_mm256_and_pd(x_vector_i6,mask);
    x_vector_i7=_mm256_and_pd(x_vector_i7,mask);
    
    res1=_mm256_mul_pd(x_vector_i,h3);
    res=_mm256_fmadd_pd(x_vector_i1,h2,res1);
    res=_mm256_fmadd_pd(x_vector_i2,h1,res);
    res=_mm256_fmadd_pd(x_vector_i3,h0,res);
    
    
    h0=_mm256_add_pd(h0_mul4,h0);
    h1=_mm256_add_pd(h1_mul4,h1);
    h2=_mm256_add_pd(h2_mul4,h2);
    h3=_mm256_add_pd(h3_mul4,h3);
    
    res3=_mm256_mul_pd(x_vector_i4,h3);
    res2=_mm256_fmadd_pd(x_vector_i5,h2,res3);
    res2=_mm256_fmadd_pd(x_vector_i6,h1,res2);
    res2=_mm256_fmadd_pd(x_vector_i7,h0,res2);
    
    h0=_mm256_add_pd(h0_mul4,h0);
    h1=_mm256_add_pd(h1_mul4,h1);
    h2=_mm256_add_pd(h2_mul4,h2);
    h3=_mm256_add_pd(h3_mul4,h3);
    
    _mm256_store_pd(y+i,res);
    _mm256_store_pd(y+i+4,res2);
  }
}


/*
* Called by the driver to register your functions
* Use add_function(func, description) to add your own functions
*/
void register_functions() 
{
  add_function(&slow_performance1, "slow_performance1",1);
  add_function(&slow2, "slow2",1);
  add_function(&slow3, "slow3",1);
  add_function(&maxperformance, "maxperformance",1);
}