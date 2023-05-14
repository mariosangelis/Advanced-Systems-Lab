void comp(double* x, double *y, int n) {
  /* Insert here your optimized implementation. Test your implementation by 
     clicking on the "flask" symbol next to the console. Once your function 
     runs without errors, click on "Create new submission". */
     
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