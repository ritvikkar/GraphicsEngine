/*
 * 030vector.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

#include <stdio.h>
#include <stdlib.h>

/* Prints the dim-dimensional vector v*/
void vecPrint(int dim, double v[]) {
	int i;
	for (i = 0; i < dim; i += 1)
		printf("%lf\n", v[i]);
}

/* Copies the dim-dimensional vector v to the dim-dimensional vector copy. */
void vecCopy(int dim, double v[], double copy[]) {    
    int i;

    for(i = 0; i < dim; i++)
    {
        copy[i] = v [i];
    }
}

/* Adds the dim-dimensional vectors v and w. */
void vecAdd(int dim, double v[], double w[], double vPlusW[]) {    
    int i;

    for (i = 0; i < dim; i++) {
        vPlusW[i] = v[i] + w[i];
    }
}

/* Subtracts the dim-dimensional vectors v and w. */
void vecSubtract(int dim, double v[], double w[], double vMinusW[]) {    
    int i;

    for (i = 0; i < dim; i++) {
        vMinusW[i] = v[i] - w[i];
    }
}

/* Scales the dim-dimensional vector w by the number c. */
void vecScale(int dim, double c, double w[], double cTimesW[]) {
    int i;

    for (i = 0; i < dim; i++) {
        cTimesW[i] = w[i]*c;
    }	
}