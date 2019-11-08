/*
-------------------------------------------------------------------------
   GATEWAY ROUTINE FOR CALLING REORTH FROM MATLAB.

   REORTH   Reorthogonalize a vector using iterated Gram-Schmidt

   [R_NEW,NORMR_NEW,NRE] = reorth(Q,R,NORMR,INDEX,ALPHA,METHOD)
   reorthogonalizes R against the subset of columns of Q given by INDEX. 
   If INDEX==[] then R is reorthogonalized all columns of Q.
   If the result R_NEW has a small norm, i.e. if norm(R_NEW) < ALPHA*NORMR,
   then a second reorthogonalization is performed. If the norm of R_NEW
   is once more decreased by  more than a factor of ALPHA then R is 
   numerically in span(Q(:,INDEX)) and a zero-vector is returned for R_NEW.

   If method==0 then iterated modified Gram-Schmidt is used.
   If method==1 then iterated classical Gram-Schmidt is used.

   The default value for ALPHA is 0.5. 
   NRE is the number of reorthogonalizations performed (1 or 2).

 References: 
  Aake Bjorck, "Numerical Methods for Least Squares Problems",
  SIAM, Philadelphia, 1996, pp. 68-69.

  J.~W. Daniel, W.~B. Gragg, L. Kaufman and G.~W. Stewart, 
  ``Reorthogonalization and Stable Algorithms Updating the
  Gram-Schmidt QR Factorization'', Math. Comp.,  30 (1976), no.
  136, pp. 772-795.

  B. N. Parlett, ``The Symmetric Eigenvalue Problem'', 
  Prentice-Hall, Englewood Cliffs, NJ, 1980. pp. 105-109

  Rasmus Munk Larsen, DAIMI, 1998.
-------------------------------------------------------------------------
  */

/* Modifications by Stephen Becker, srbecker@caltech.edu
 * Update, 3/7/09
 * Re-wrote reorth.f to reorth.c (in C) so that it's easier to compile
 * on Windows (since windows fortran compilers are not that common).
 *
 * When install on Windows, define the pre-processor definition "WINDOWS"
 * Then it will call the appropriate functions
 *
 * 11/9/09
 * Fixed bugs in reorth.c; it is now preferable to use this, as opposed
 * to reorth.f.  Should work with 64-bit systems.
 * */

#include <string.h>
#include "mex.h"

/* keep this defined; no longer use fortran version */
#define REORTH_IN_C

/* I use the "WINDOWS" symbol to change between underscores and no underscores
 * _WIN32 *should* be automatically defined on WINDOWS machines, for most compilers */
#if defined(_WIN32)
    #define WINDOWS
#endif

/* Template for reorth: */
#ifdef WINDOWS
void reorth(int *n, int *k, double *V, int *ldv, double *vnew,
	    double *normvnew, double *index, double *alpha, double *work,
	    int *iflag, int *nre);
#else
  #ifdef REORTH_IN_C
void reorth(int *n, int *k, double *V, int *ldv, double *vnew,
	    double *normvnew, double *index, double *alpha, double *work,
	    int *iflag, int *nre);
  #else
/* if reorth.f is compiled, then use "reorth_", but if
 *  reorth.c is compiled, then use "reorth" */
void reorth_(int *n, int *k, double *V, int *ldv, double *vnew,
	    double *normvnew, double *index, double *alpha, double *work,
	    int *iflag, int *nre);
  #endif
#endif

/* Here comes the gateway function to be called by Matlab: */
void mexFunction(int nlhs, mxArray *plhs[], 
		 int nrhs, const mxArray *prhs[])
{
  int n, k1, k, imethod, inre, i;
  double *work;
  double *columnIndex;
  int LDV;
  int DEFAULT_INDEX = 0;

  if (nrhs != 6)
     mexErrMsgTxt("reorth requires 6 input arguments");
  else if  (nlhs < 2)
     mexErrMsgTxt("reorth requires at least 2 output arguments");

  n = mxGetM(prhs[0]); /* get the dimensions of the input */
  k1 = mxGetN(prhs[0]); /* SRB: total possible number of columns */
  k = mxGetM(prhs[3]) * mxGetN(prhs[3]);  /* SRB: this is index */
  /* SRB, Nov 9 2009, adding support for the empty matrix input */
  if ( ( k == 0 ) || mxIsEmpty( prhs[3] ) ) {
      columnIndex = (double *)mxCalloc(k1,sizeof(double));
      k = k1;
      DEFAULT_INDEX = 1;
      for ( i=0; i < k1 ; i++ )
          columnIndex[i] = i + 1;  /* MATLAB is 1-based */
  } else {
      columnIndex = mxGetPr( prhs[3] );
  }
    
  /* Create/allocate return argument, a 1x1 real-valued Matrix */
  plhs[0]=mxCreateDoubleMatrix(n,1,mxREAL); 
  plhs[1]=mxCreateDoubleMatrix(1,1,mxREAL); 
  if (nlhs>2) 
    plhs[2]=mxCreateDoubleMatrix(1,1,mxREAL); 
  
  work = mxCalloc(k,sizeof(double));
  
  memcpy(mxGetPr(plhs[0]),mxGetPr(prhs[1]), n*sizeof(double));
  memcpy(mxGetPr(plhs[1]),mxGetPr(prhs[2]), sizeof(double));
  imethod = (int) mxGetScalar(prhs[5]);
   
  LDV = n;
  inre = 0;

#ifdef WINDOWS
  reorth(&n, &k, mxGetPr(prhs[0]), &LDV, mxGetPr(plhs[0]), 
	  mxGetPr(plhs[1]), columnIndex, mxGetPr(prhs[4]), 
	  work,&imethod,&inre);
#else
  #ifdef REORTH_IN_C
  reorth(&n, &k, mxGetPr(prhs[0]), &LDV, mxGetPr(plhs[0]), 
	  mxGetPr(plhs[1]), columnIndex, mxGetPr(prhs[4]), 
	  work,&imethod,&inre);
  #else
  reorth_(&n, &k, mxGetPr(prhs[0]), &LDV, mxGetPr(plhs[0]), 
	  mxGetPr(plhs[1]), columnIndex, mxGetPr(prhs[4]), 
	  work,&imethod,&inre);
  #endif
#endif
  if (nlhs>2) 
    *(mxGetPr(plhs[2])) = (double) inre*k;

  mxFree(work);
  if ( DEFAULT_INDEX )
      mxFree( columnIndex );

}



