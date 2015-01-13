/*
* @@name:	async_target.2c
* @@type:	C
* @@compilable:	yes
* @@linkable:	no
* @@expect:	success
*/
#include <stdlib.h>
extern void init(float *, float *, int);
extern void output(float *, int);
void vec_mult(float *p, float *v1, float *v2, int N, int dev)
{
   int i;
   init(p, N);
   #pragma omp task depend(out: v1, v2)
   #pragma omp target device(dev) map(v1, v2)
   {
       // check whether on device dev
       if (omp_is_initial_device())
	  abort();
       v1 = malloc(N*sizeof(float));
       v2 = malloc(N*sizeof(float));
       init(v1,v2);
   }
   foo(); // execute other work asychronously
   #pragma omp task depend(in: v1, v2)
   #pragma omp target device(dev) map(to: v1, v2) map(from: p[0:N])
   {
       // check whether on device dev
       if (omp_is_initial_device())
	  abort();
       #pragma omp parallel for
       for (i=0; i<N; i++)
	 p[i] = v1[i] * v2[i];
       output(p, N);
       free(v1);
       free(v2);
   }
}