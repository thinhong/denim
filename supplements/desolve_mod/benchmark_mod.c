/* file benchmark_mod.c */
#include <R.h>
static double parms[6];
#define R0 parms[0]
#define scale_I parms[1]
#define shape_I parms[2]
#define scale_R parms[3]
#define shape_R parms[4]
#define N parms[5]


/* initializer  */
void initmod(void (* odeparms)(int *, double *)) {
  int len=6;
  odeparms(&len, parms);
}

/* Derivatives */
void derivs (int *neq, double *t, double *y, double *ydot,
             double *yout, int *ip) {
  
  double tr = scale_R * shape_R;
  
  double rate_I = 1/scale_I;
  double rate_R = 1/scale_R;
  
  ydot[0] = - (R0/tr) * y[0] * y[6]/N; // deriv for S 
  ydot[1] = (R0/tr) * y[0] * y[6]/N - rate_I*y[1]; // deriv for E1
  ydot[2] = rate_I*y[1] - rate_I*y[2]; // deriv for E2
  ydot[3] = ydot[1] + ydot[2]; // deriv for E
  ydot[4] = rate_I*y[2] - rate_R*y[4]; // deriv for I1
  ydot[5] = rate_R*y[4] - rate_R*y[5]; // deriv for I2
  ydot[6] = ydot[4] + ydot[5]; // deriv for I
  ydot[7] = rate_R*y[5]; // deriv for R
} 