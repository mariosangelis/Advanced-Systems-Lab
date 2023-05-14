#include <immintrin.h>
#include "common.h"

using namespace std;

// Precondition: 0 <= y[i] < 1.0
void slow_performance1(complex_t *x, double *y, int n) {
    for (int i = 0; i < n; i++) {
        unsigned int k = floor(4.0*y[i]);
        
        switch (k) {
            case 0:  y[i] += fmin(re(sqr(x[i])), im(sqr(x[i]))); break;
            case 1:  y[i] += fmax(re(sqr(x[i])), im(sqr(x[i]))); break;
            default: y[i] += pow(abs(x[i]), 2.0); break;
        }
    }
}



void slow_performance2(complex_t* x, double* y, int n) {
  /* Add your final implementation here */
      __m256d x_re,x_im,y_vector,y_vector_temp,k_vector,four,ones,zeros,vmask0,vmask1,vmask2,fmin_vector,fmax_vector,pow_vector,res1,res2,res3;
      __m256d x_im_square,x_re_x_im;
      
      four=_mm256_set1_pd(4.0);
      ones=_mm256_set1_pd(1.0);
      zeros=_mm256_set1_pd(0.0);
      
      for (int i = 0; i < n; i+=4) {
        
        x_re=_mm256_set_pd(x[i+3].re,x[i+2].re,x[i+1].re,x[i].re);
        x_im=_mm256_set_pd(x[i+3].im,x[i+2].im,x[i+1].im,x[i].im);
        

        x_im_square=_mm256_mul_pd(x_im,x_im);
        x_re_x_im=_mm256_mul_pd(x_re,x_im);
          
        res1=_mm256_fmsub_pd(x_re,x_re,x_im_square);
        res2=_mm256_fmadd_pd(x_re,x_im,x_re_x_im);
        res3=_mm256_fmadd_pd(x_re,x_re,x_im_square);
        res3=_mm256_sqrt_pd(res3);
        
        vmask1=_mm256_cmp_pd(res1,res2,_CMP_LE_OQ);
        fmin_vector=_mm256_blendv_pd(res2,res1,vmask1);
        fmax_vector=_mm256_blendv_pd(res1,res2,vmask1);
        pow_vector=_mm256_mul_pd(res3,res3);

        y_vector=_mm256_load_pd(y+i);
        y_vector_temp=_mm256_mul_pd(y_vector,four);
        k_vector=_mm256_floor_pd(y_vector_temp);
        
        vmask0=_mm256_cmp_pd(k_vector,zeros,_CMP_EQ_OQ);
        vmask1=_mm256_cmp_pd(k_vector,ones,_CMP_EQ_OQ);
        vmask2=_mm256_or_pd(vmask0,vmask1);
        
        res1=_mm256_blendv_pd(zeros,fmin_vector,vmask0);
        res2=_mm256_blendv_pd(zeros,fmax_vector,vmask1);
        res3=_mm256_blendv_pd(pow_vector,zeros,vmask2);
        
        y_vector=_mm256_add_pd(y_vector,res1);
        y_vector=_mm256_add_pd(y_vector,res2);
        y_vector=_mm256_add_pd(y_vector,res3);
       
        _mm256_store_pd(y+i,y_vector);
    }
  
}

void slow3(complex_t* x, double* y, int n) {
  /* Add your final implementation here */
      __m256d x_re,x_im,y_vector,y_vector_temp,k_vector,four,ones,zeros,vmask0,vmask1,vmask2,fmin_vector,fmax_vector,pow_vector,res1,res2,res3;
      __m256d x_im_square,x_re_square,x_re_x_im,temp1,temp2,temp3,x_01,x_23;
      
      four=_mm256_set1_pd(4.0);
      ones=_mm256_set1_pd(1.0);
      zeros=_mm256_set1_pd(0.0);
      
      for (int i = 0; i < n; i+=4) {
        
        x_01=_mm256_load_pd(&x[i].re);
        x_23=_mm256_load_pd(&x[i+2].re);
        
        x_re=_mm256_unpacklo_pd(x_01,x_23);
        x_im=_mm256_unpackhi_pd(x_01,x_23);
        
        x_re=_mm256_permute4x64_pd(x_re,0b11011000);
        x_im=_mm256_permute4x64_pd(x_im,0b11011000);
        
        x_im_square=_mm256_mul_pd(x_im,x_im);
        x_re_x_im=_mm256_mul_pd(x_re,x_im);

        res1=_mm256_fmsub_pd(x_re,x_re,x_im_square);
        res2=_mm256_add_pd(x_re_x_im,x_re_x_im);
        
        fmin_vector=_mm256_min_pd(res1,res2);
        fmax_vector=_mm256_max_pd(res1,res2);
        pow_vector=_mm256_fmadd_pd(x_re,x_re,x_im_square);
        
        y_vector=_mm256_load_pd(y+i);

        y_vector_temp=_mm256_mul_pd(y_vector,four);
        k_vector=_mm256_floor_pd(y_vector_temp);
        
        vmask0=_mm256_cmp_pd(k_vector,zeros,_CMP_EQ_OQ);
        vmask1=_mm256_cmp_pd(k_vector,ones,_CMP_EQ_OQ);
        vmask2=_mm256_or_pd(vmask0,vmask1);
        
        res1=_mm256_and_pd(fmin_vector,vmask0);
        res2=_mm256_and_pd(fmax_vector,vmask1);
        res3=_mm256_blendv_pd(pow_vector,zeros,vmask2);
        
        temp1=_mm256_add_pd(res1,res2);
        temp2=_mm256_add_pd(y_vector,res3);
        y_vector=_mm256_add_pd(temp1,temp2);
       
        _mm256_store_pd(y+i,y_vector);
    }
  
}

void slow4(complex_t* x, double* y, int n) {
  /* Add your final implementation here */
      __m256d temp1,temp2,x_re,x_im,y_vector,two,_025,_05,vmask0,vmask2,fmin_vector,fmax_vector,pow_vector,res1,res2,x_im_square,x_re_x_im,x_01,x_23;
      
      _025=_mm256_set1_pd(0.25);
      _05=_mm256_set1_pd(0.5);
      two=_mm256_set1_pd(2.0);
      
      for (int i = 0; i < n; i+=4) {
        
        //load
        x_01=_mm256_load_pd(&x[i].re);
        x_23=_mm256_load_pd(&x[i+2].re);
        y_vector=_mm256_load_pd(y+i);
        
        x_re=_mm256_unpacklo_pd(x_01,x_23);
        x_im=_mm256_unpackhi_pd(x_01,x_23);
        
        x_re=_mm256_permute4x64_pd(x_re,0b11011000);
        x_im=_mm256_permute4x64_pd(x_im,0b11011000);
        
        x_im_square=_mm256_mul_pd(x_im,x_im);
          
        temp1=_mm256_fmadd_pd(x_re,x_re,y_vector);
        res1=_mm256_sub_pd(temp1,x_im_square);
        temp2=_mm256_mul_pd(x_re,two);
        res2=_mm256_fmadd_pd(temp2,x_im,y_vector);
        
        fmin_vector=_mm256_min_pd(res1,res2);
        fmax_vector=_mm256_max_pd(res1,res2);
        pow_vector=_mm256_add_pd(temp1,x_im_square);
        
        vmask0=_mm256_cmp_pd(y_vector,_025,_CMP_LT_OQ);
        vmask2=_mm256_cmp_pd(y_vector,_05,_CMP_GE_OQ);
        
        res1=_mm256_blendv_pd(fmax_vector,fmin_vector,vmask0);
        res2=_mm256_blendv_pd(res1,pow_vector,vmask2);
       
        _mm256_store_pd(y+i,res2);
    }
}


void maxperformance(complex_t* x, double* y, int n) {
  /* Add your final implementation here */
      __m256d temp1,temp2,two,x_re,x_im,y_vector,_025,_05,vmask0,vmask2,fmin_vector,fmax_vector,pow_vector,res1,res2,x_im_square,x_01,x_23;
      __m256d temp3,temp4,x_rei2,x_imi2,y_vectori2,vmask1,vmask3,fmin_vectori2,fmax_vectori2,pow_vectori2,res3,res4,x_im_squarei2,x_45,x_67;
      
      _025=_mm256_set1_pd(0.25);
      _05=_mm256_set1_pd(0.5);
      two=_mm256_set1_pd(2.0);
      
      for (int i = 0; i < n; i+=8) {
        
        //load
        x_01=_mm256_load_pd(&x[i].re);
        x_23=_mm256_load_pd(&x[i+2].re);
        y_vector=_mm256_load_pd(y+i);
        
        x_45=_mm256_load_pd(&x[i+4].re);
        x_67=_mm256_load_pd(&x[i+6].re);
        y_vectori2=_mm256_load_pd(y+i+4);
        
        x_re=_mm256_unpacklo_pd(x_01,x_23);
        x_im=_mm256_unpackhi_pd(x_01,x_23);
        
        x_re=_mm256_permute4x64_pd(x_re,0b11011000);
        x_im=_mm256_permute4x64_pd(x_im,0b11011000);
  
        x_im_square=_mm256_mul_pd(x_im,x_im);
        
        //---------------------------------------------
        x_rei2=_mm256_unpacklo_pd(x_45,x_67);
        x_imi2=_mm256_unpackhi_pd(x_45,x_67);
        
        x_rei2=_mm256_permute4x64_pd(x_rei2,0b11011000);
        x_imi2=_mm256_permute4x64_pd(x_imi2,0b11011000);
        
        x_im_squarei2=_mm256_mul_pd(x_imi2,x_imi2);
          
        //---------------------------------------------------
          
        temp1=_mm256_fmadd_pd(x_re,x_re,y_vector);
        res1=_mm256_sub_pd(temp1,x_im_square);
        temp2=_mm256_mul_pd(x_re,two);
        res2=_mm256_fmadd_pd(temp2,x_im,y_vector);
        
        fmin_vector=_mm256_min_pd(res1,res2);
        fmax_vector=_mm256_max_pd(res1,res2);
        pow_vector=_mm256_add_pd(temp1,x_im_square);
        
        vmask0=_mm256_cmp_pd(y_vector,_025,_CMP_LT_OQ);
        vmask2=_mm256_cmp_pd(y_vector,_05,_CMP_GE_OQ);
        
        res1=_mm256_blendv_pd(fmax_vector,fmin_vector,vmask0);
        res2=_mm256_blendv_pd(res1,pow_vector,vmask2);
        
        //---------------------------------------------------
        
        temp3=_mm256_fmadd_pd(x_rei2,x_rei2,y_vectori2);
        res3=_mm256_sub_pd(temp3,x_im_squarei2);
        temp4=_mm256_mul_pd(x_rei2,two);
        res4=_mm256_fmadd_pd(temp4,x_imi2,y_vectori2);
        
        fmin_vectori2=_mm256_min_pd(res3,res4);
        fmax_vectori2=_mm256_max_pd(res3,res4);
        pow_vectori2=_mm256_add_pd(temp3,x_im_squarei2);
        
        vmask1=_mm256_cmp_pd(y_vectori2,_025,_CMP_LT_OQ);
        vmask3=_mm256_cmp_pd(y_vectori2,_05,_CMP_GE_OQ);
        
        res3=_mm256_blendv_pd(fmax_vectori2,fmin_vectori2,vmask1);
        res4=_mm256_blendv_pd(res3,pow_vectori2,vmask3);
       
        //store
        _mm256_store_pd(y+i,res2);
        _mm256_store_pd(y+i+4,res4);
    }
}


/*
* Called by the driver to register your functions
* Use add_function(func, description) to add your own functions
*/
void register_functions() {
  add_function(&slow_performance1, "slow_performance1",1);
  add_function(&slow_performance2, "slow_performance2",1);
  add_function(&slow3, "slow3",1);
  add_function(&slow4, "slow4",1);
  add_function(&maxperformance, "maxperformance",1);
}