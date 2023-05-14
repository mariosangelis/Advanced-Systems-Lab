#include "common.h"
#include "mat.h"

#define C1 0.1
#define C2 (2.0/3.0)

void slow_performance1(mat* x, mat* y, mat*z) {
    double t1;
    for (int i = 0; i < z->n1; i++) {
        for (int j = 1; j < z->n2 - 1; j++) {
            if (i % 2) {
                t1 = mat_get(z,i,j)/sqrt(mat_get(y,0,i%2)) + (mat_get(x,i,j) + C1)*(mat_get(x,i,j) - C1);
            } else {
                t1 = mat_get(z,i,j)/sqrt(mat_get(y,0,i%2)) + C1*mat_get(x,i,j);
            }
            mat_set(z,i,j-1, mat_get(z,i,j-1)*cos(C2*M_PI*j));
            mat_set(z,i,j,t1);
            mat_set(z,i,j+1,fmax(mat_get(z,i,j+1), mat_get(x,i,j+1)));
        }
    }
}

//remove all mat_set and mat_get calls
void fun1(mat* x, mat* y, mat* z) {
    double t1;
    int n1=z->n1, n2= z->n2-1,imod,i,j,n=z->n2;

    double matget_xij,matget_zij;

    for (i = 0; i < n1; i+=1) {
        imod=i%2;
        for (j = 1; j < n2; j++) {
            matget_zij= z->data[i*n+j];
            matget_xij= x->data[i*n+j];

            if (i%2) {
                t1 = matget_zij/sqrt(y->data[imod]) + (matget_xij + C1)*(matget_xij - C1);
            } else {
                t1 = matget_zij/sqrt(y->data[imod]) + C1*matget_xij;
            }

            z->data[i*n+j-1]=z->data[i*n+j-1]*cos(C2*M_PI*j);
            z->data[i*n+j]=t1;
            z->data[i*n+j+1]=fmax(z->data[i*n+j+1], x->data[i*n+j+1]);
        }
    }
}

//get rid of imod%2 by applying loop unrolling to the outer loop
void fun2(mat* x, mat* y, mat* z) {
    double t1;
    int n1=z->n1, n2= z->n2-1,in_j,i1n_j,i,j,n=z->n2;

    double x_ij,x_i1j,z_ij,z_i1j,z_ij_min_1,z_i1j_min_1,z_ij_plus_1,z_i1j_plus_1,x_ij_plus_1,x_i1j_plus_1,cos_val;

    for (i = 0; i < n1; i+=2) {
        for (j = 1; j < n2; j+=1) {
            in_j=i*n + j;
            i1n_j=(i+1)*n + j;

            cos_val=cos(C2*M_PI*j);

            z_ij=z->data[in_j];
            x_ij=x->data[in_j];
            z_ij_min_1=z->data[in_j-1];
            z_ij_plus_1=z->data[in_j+1];
            x_ij_plus_1=x->data[in_j+1];

            z_i1j=z->data[i1n_j];
            x_i1j=x->data[i1n_j];
            z_i1j_min_1=z->data[i1n_j-1];
            z_i1j_plus_1=z->data[i1n_j+1];
            x_i1j_plus_1=x->data[i1n_j+1];


            z_ij_min_1=z_ij_min_1*cos_val;
            z_ij=z_ij/sqrt(y->data[0]) + C1*x_ij;
            z_ij_plus_1=fmax(z_ij_plus_1,x_ij_plus_1);

            z_i1j_min_1=z_i1j_min_1*cos_val;
            z_i1j=z_i1j/sqrt(y->data[1]) + (x_i1j + C1)*(x_i1j - C1);
            z_i1j_plus_1=fmax(z_i1j_plus_1,x_i1j_plus_1);


            z->data[in_j-1]=z_ij_min_1;
            z->data[in_j]=z_ij;
            z->data[in_j+1]=z_ij_plus_1;

            z->data[i1n_j-1]=z_i1j_min_1;
            z->data[i1n_j]=z_i1j;
            z->data[i1n_j+1]=z_i1j_plus_1;


        }
    }
}

//get rid of cos value by applying loop unrolling in the inner loop
void fun3(mat* x, mat* y, mat* z) {

    int n1=z->n1,i,j,n=z->n2,n2=n-1,in_j,in_j1,in_j_1;
    double y_data_0,y_data_1,c_mul=C1*C1,matget_zij,matget_xij,x_val,z_val;

    y_data_0=1/sqrt(y->data[0]);
    y_data_1=1/sqrt(y->data[1]);

    for (i = 0; i < n1; i+=2) {
        for (j = 1; j < n2; j+=3) {

          //-------------i=0,j=1-------------
          in_j=i*n + j;
          in_j1=in_j+1;
          in_j_1=in_j-1;

          matget_zij= z->data[in_j];
          matget_xij= x->data[in_j];
          z_val=z->data[in_j1];
          x_val=x->data[in_j1];

          //cos_val=-0.5;

          z->data[in_j_1]*=-0.5;
          z->data[in_j]=matget_zij*y_data_0 + C1*matget_xij;
          z->data[in_j1]= (z_val < x_val) ? x_val : z_val;

          //-------------i=0,j=2-------------
          in_j=in_j+1;
          in_j1=in_j+1;
          in_j_1=in_j-1;

          matget_zij= z->data[in_j];
          matget_xij= x->data[in_j];
          z_val=z->data[in_j1];
          x_val=x->data[in_j1];


          z->data[in_j_1]*=-0.5;
          z->data[in_j]=matget_zij*y_data_0 + C1*matget_xij;
          z->data[in_j1]=(z_val < x_val) ? x_val : z_val;


          //-------------i=0,j=3-------------
          in_j=in_j+1;
          in_j1=in_j+1;

          matget_zij= z->data[in_j];
          matget_xij= x->data[in_j];
          z_val=z->data[in_j1];
          x_val=x->data[in_j1];


          z->data[in_j]=matget_zij*y_data_0 + C1*matget_xij;
          z->data[in_j1]=(z_val < x_val) ? x_val : z_val;

          //-------------i=1,j=1-------------
          in_j=(i+1)*n+j;
          in_j1=in_j+1;
          in_j_1=in_j-1;

          matget_zij= z->data[in_j];
          matget_xij= x->data[in_j];
          z_val=z->data[in_j1];
          x_val=x->data[in_j1];

          z->data[in_j_1]*=-0.5;
          z->data[in_j]=matget_zij*y_data_1 + matget_xij*matget_xij - c_mul;
          z->data[in_j1]=(z_val < x_val) ? x_val : z_val;



          //-------------i=1,j=2-------------

          in_j=in_j+1;
          in_j1=in_j+1;
          in_j_1=in_j-1;

          matget_zij= z->data[in_j];
          matget_xij= x->data[in_j];
          z_val=z->data[in_j1];
          x_val=x->data[in_j1];

          z->data[in_j_1]*=-0.5;
          z->data[in_j]=matget_zij*y_data_1 + matget_xij*matget_xij - c_mul;
          z->data[in_j1]=(z_val < x_val) ? x_val : z_val;


          //-------------i=1,j=3-------------

          in_j=in_j+1;
          in_j1=in_j+1;

          matget_zij= z->data[in_j];
          matget_xij= x->data[in_j];
          z_val=z->data[in_j1];
          x_val=x->data[in_j1];

          //z->data[in1_j-1]=z->data[in1_j-1]*cos_val;
          z->data[in_j]=matget_zij*y_data_1 + matget_xij*matget_xij - c_mul;
          z->data[in_j1]=(z_val < x_val) ? x_val : z_val;
          }
        }
    }
}

//refactor the code, remove fmax operator using ternary expressions, apply load/compute/store pattern
void maxperformance(mat* x, mat* y, mat* z) {

    int n1=z->n1,i,j,n=z->n2,n2=n-1,in_j,in_j1,in_j_1,in_j2,in_j3;
    double y_data_0,y_data_1,c_mul=C1*C1,res0,res1,res2,res3,res4,x_ij,z_ij,x_ij1,x_ij2,x_ij3,z_ij1,z_ij2,z_ij3,z_ij_1,res;

    y_data_0=1/sqrt(y->data[0]);
    y_data_1=1/sqrt(y->data[1]);

    for (j = 1; j < n2; j+=3) {
      for (i = 0; i < n1; i+=2) {

          //-------------i=0,j=0-------------
          //load
          in_j=i*n+j;
          in_j1=in_j+1;
          in_j_1=in_j-1;
          in_j2=in_j+2;
          in_j3=in_j+3;

          z_ij_1 = z->data[in_j_1];
          z_ij= z->data[in_j];
          x_ij= x->data[in_j];

          z_ij1=z->data[in_j1];
          x_ij1=x->data[in_j1];

          z_ij2=z->data[in_j2];
          x_ij2=x->data[in_j2];

          z_ij3=z->data[in_j3];
          x_ij3=x->data[in_j3];

          //compute
          //z00
          res0=-0.5 * z_ij_1;
          //z01
          res1=-0.5*(z_ij*y_data_0 + C1*x_ij);
          //z02
          res=(z_ij1 < x_ij1) ? x_ij1 : z_ij1;
          res2=res*y_data_0 + x_ij1*C1;

          //z03
          res=(z_ij2 < x_ij2) ? x_ij2 : z_ij2;
          res3=res*y_data_0 + x_ij2*C1;

          //z04
          res4=(z_ij3 < x_ij3) ? x_ij3 : z_ij3;

          //store
          z->data[in_j_1]=res0;
          z->data[in_j]=res1;
          z->data[in_j1]=res2;
          z->data[in_j2]=res3;
          z->data[in_j3]=res4;

          //-------------i=1,j=0-------------

          //load
          in_j=(i+1)*n+j;
          in_j1=in_j+1;
          in_j_1=in_j-1;
          in_j2=in_j+2;
          in_j3=in_j+3;

          z_ij_1=z->data[in_j_1];
          z_ij= z->data[in_j];
          x_ij= x->data[in_j];

          z_ij1=z->data[in_j1];
          x_ij1=x->data[in_j1];

          z_ij2=z->data[in_j2];
          x_ij2=x->data[in_j2];

          z_ij3=z->data[in_j3];
          x_ij3=x->data[in_j3];

          //compute
          //z10
          res0=-0.5 * z_ij_1;
          //z11
          res1= -0.5*(z_ij*y_data_1 + x_ij*x_ij - c_mul);

          //z12
          res2=((z_ij1 < x_ij1) ? x_ij1 : z_ij1)*y_data_1 + x_ij1*x_ij1 - c_mul;

          //z13
          res3=((z_ij2 < x_ij2) ? x_ij2 : z_ij2)*y_data_1 + x_ij2*x_ij2 - c_mul;

          //z14
          res4=(z_ij3 < x_ij3) ? x_ij3 : z_ij3;

          //store
          z->data[in_j_1]=res0;
          z->data[in_j]=res1;
          z->data[in_j1]=res2;
          z->data[in_j2]=res3;
          z->data[in_j3]=res4;

        }
    }
}
/*
* Called by the driver to register your functions
* Use add_function(func, description) to add your own functions
*/
void register_functions() {
  add_function(&slow_performance1, "slow_performance1",1);
  add_function(&fun1, "performance_improvement_1",1);
  add_function(&fun2, "performance_improvement_2",1);
  add_function(&fun3, "performance_improvement_3",1);
 // add_function(&fun4, "performance_improvement_4",1);
  add_function(&maxperformance, "maxperformance",1);
}
