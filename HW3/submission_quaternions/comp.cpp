#include <immintrin.h>
#include "common.h"
#include "complex.h"
#include <unistd.h>

void slow_performance1(quaternion_t x[N], quaternion_t y[N],quaternion_t A[N][N]) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
            A[i][j] = mul(x[i], y[j]);
        }
    }
}

void slow2(quaternion_t x[N], quaternion_t y[N],quaternion_t A[N][N]) {
  /* Add your final implementation here */
    __m256d vector_y,vector_x_r,vector_x_i,vector_x_j,vector_x_k,res1,res2,res3,res4,vector_y1,vector_y2;
    
    for (int i = 0; i < N; i++) {
        //vector_x=_mm256_load_pd(&(x+i)->r);
        vector_x_r=_mm256_set1_pd((x+i)->r);
        vector_x_i=_mm256_set1_pd((x+i)->i);
        vector_x_j=_mm256_set1_pd((x+i)->j);
        vector_x_k=_mm256_set1_pd((x+i)->k);
        for (int j = 0; j < N; j++) {
          vector_y=_mm256_load_pd(&(y+j)->r);
          vector_y1=_mm256_permute4x64_pd(vector_y,0b00011011);
          vector_y2=_mm256_permute4x64_pd(vector_y,0b01001110);

          
          res1=_mm256_mul_pd(vector_x_r,vector_y);
          res2=_mm256_mul_pd(vector_x_i,vector_y);
          res2=_mm256_shuffle_pd(res2, res2,0b0101);
          res1=_mm256_addsub_pd(res1,res2);
          res1=_mm256_shuffle_pd(res1, res1,0b0110);
          
          res3=_mm256_mul_pd(vector_x_j,vector_y1);
          res4=_mm256_mul_pd(vector_x_k,vector_y2);
          res3=_mm256_addsub_pd(res3,res4);
          
          res3=_mm256_shuffle_pd(res3, res3,0b1001);
          
          res4=_mm256_addsub_pd(res1,res3);
          res4=_mm256_shuffle_pd(res4, res4,0b0110);
          _mm256_store_pd(&A[i][j].r,res4);
        }
    }
}

void slow3(quaternion_t x[N], quaternion_t y[N],quaternion_t A[N][N]) {
  /* Add your final implementation here */
    
    __m256d vector_y,vector_y1,vector_y2,vector_x_r,vector_x_i,vector_x_j,vector_x_k,res1,res2,res3,res4;
    
    for (int j = 0; j < N; j++) {
        vector_y=_mm256_load_pd(&(y+j)->r);
        vector_y1=_mm256_permute4x64_pd(vector_y,0b00011011);
        vector_y2=_mm256_permute4x64_pd(vector_y,0b01001110);
        
        for (int i = 0; i < N; i++) {
            
            vector_x_r=_mm256_broadcast_sd (&(x+i)->r);
            vector_x_i=_mm256_broadcast_sd (&(x+i)->i);
            vector_x_j=_mm256_broadcast_sd (&(x+i)->j);
            vector_x_k=_mm256_broadcast_sd (&(x+i)->k);
          
            res1=_mm256_mul_pd(vector_x_r,vector_y);
            res2=_mm256_mul_pd(vector_x_i,vector_y);
            res2=_mm256_shuffle_pd(res2, res2,0b0101);
            res1=_mm256_addsub_pd(res1,res2);
            res1=_mm256_shuffle_pd(res1, res1,0b0110);
            
            res3=_mm256_mul_pd(vector_x_j,vector_y1);
            res4=_mm256_mul_pd(vector_x_k,vector_y2);
            res3=_mm256_addsub_pd(res3,res4);
            res3=_mm256_shuffle_pd(res3, res3,0b1001);
            
            res4=_mm256_addsub_pd(res1,res3);
            res4=_mm256_shuffle_pd(res4, res4,0b0110);
            _mm256_store_pd(&A[i][j].r,res4);
        }
    }
}

void maxperformance(quaternion_t x[N], quaternion_t y[N],quaternion_t A[N][N]) {
  /* Add your final implementation here */
    
    __m256d temp1,temp2,temp3,vector_y,vector_y1,vector_y2,vector_x_r,vector_x_i,vector_x_j,vector_x_k,res1,res2,res3,res4;
    double a[4]={-1,1,-1,1};
    double b[4]={1,-1,1,-1};
    double c[4]={-1,1,1,-1};
    
    temp1=_mm256_load_pd(a);
    temp2=_mm256_load_pd(b);
    temp3=_mm256_load_pd(c);
    
    for (int j = 0; j < N; j++) {
        vector_y=_mm256_load_pd(&(y+j)->r);
        vector_y1=_mm256_permute4x64_pd(vector_y,0b01001110);
        vector_y2=_mm256_permute4x64_pd(vector_y,0b00011011);
        
        for (int i = 0; i < N; i++) {
            
            vector_x_r=_mm256_broadcast_sd (&(x+i)->r);
            vector_x_i=_mm256_broadcast_sd (&(x+i)->i);
            vector_x_j=_mm256_broadcast_sd (&(x+i)->j);
            vector_x_k=_mm256_broadcast_sd (&(x+i)->k);
          
            res1=_mm256_mul_pd(vector_x_r,vector_y);
            res2=_mm256_mul_pd(vector_x_i,vector_y);
            res2=_mm256_shuffle_pd(res2, res2,0b0101);
            res1=_mm256_fmadd_pd(temp1,res2,res1);
            
            res3=_mm256_mul_pd(vector_x_j,vector_y1);
            res4=_mm256_mul_pd(vector_x_k,vector_y2);
            
            res3=_mm256_fmadd_pd(temp2,res4,res3);
            
            res4=_mm256_fmadd_pd(temp3,res3,res1);
            _mm256_store_pd(&A[i][j].r,res4);
        }
    }
}

/*
* Called by the driver to register your functions
* Use add_function(func, description) to add your own functions
*/
void register_functions() {
  add_function(&slow_performance1, "slow_performance1",1);
  add_function(&slow2, "slow2",1);
  add_function(&slow3, "slow3",1);
  add_function(&maxperformance, "maxperformance",1);
}