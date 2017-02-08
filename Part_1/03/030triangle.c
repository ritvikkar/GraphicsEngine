/*
 * 030triangle.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "000pixel.h"

/*
 * Getting a 
 * warning: implicit declaration of function 'mat221Multiply' 
 * is invalid in C99 [-Wimplicit-function-declaration]
 * without these signatures
 */

void vecSubtract(int dim, double v[], double w[], double vMinusW[]);
void vecAdd(int dim, double v[], double w[], double vPlusW[]);
void mat22Columns(double col0[2], double col1[2], double m[2][2]);
void mat221Multiply(double m[2][2], double v[2], double mTimesV[2]);
double mat22Invert(double m[2][2], double mInv[2][2]);
void vecScale(int dim, double c, double w[], double cTimesW[]);
void vecPrint(int dim, double v[]);
void mat22Print(double m[2][2]);

/*function to rasterize a triangle*/
void triRender(double a[2], double b[2], double c[2], double rgb[3], 
               double alpha[3], double beta[3], double gamma[3]) {
    int i, j;
    
    pixClearRGB(0.0,0.0,0.0);
    
    if(b[1] < c[1] && b[1] < a[1])
    {
        for(i = ceil(a[0]); i<= floor(b[0]); i++)
        {
            double xHigh = a[1] + ((c[1] - a[1])/(c[0] - a[0]))*(i-a[0]); //lower bound
            double xLow = a[1] + ((b[1] - a[1])/(b[0] - a[0]))*(i-a[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++)
            {
                //calculate khi for each value
                double ba[2];
                vecSubtract(2,b,a,ba);
                
                double ca[2];
                vecSubtract(2,c,a,ca);

                double x[2] = {i, j};
                double xa[2];
                vecSubtract(2,x,a,xa);
                
                double baca[2][2];
                mat22Columns(ba,ca,baca);
                
                double bacaInv[2][2];
                mat22Invert(baca,bacaInv);

                double pq[2];
                mat221Multiply(bacaInv,xa,pq);
                
                double betaAlpha[3];
                vecSubtract(3,beta,alpha,betaAlpha);
                
                double gammaAlpha[3];
                vecSubtract(3,gamma,alpha,gammaAlpha);
                
                double scaleP[3];
                vecScale(3,pq[0],betaAlpha,scaleP);
                
                double scaleQ[3];
                vecScale(3,pq[1],gammaAlpha,scaleQ);
                
                double addPQ[3];
                vecAdd(3,scaleP,scaleQ,addPQ);
                
                double khi[3];
                vecAdd(3,alpha,addPQ,khi);
                
                pixSetRGB(x[0], x[1], khi[0], khi[1], khi[2]);
            }
        }

        //second half
        for(i = ceil(b[0]); i<= floor(c[0]); i++)
        {
            double xHigh = a[1] + ((c[1] - a[1])/(c[0] - a[0]))*(i-a[0]); //lower bound
            double xLow = b[1] + ((c[1] - b[1])/(c[0] - b[0]))*(i-b[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++)
            {
                //calculate khi for each value
                double ba[2];
                vecSubtract(2,b,a,ba);
                
                double ca[2];
                vecSubtract(2,c,a,ca);

                double x[2] = {i, j};
                double xa[2];
                vecSubtract(2,x,a,xa);
                
                double baca[2][2];
                mat22Columns(ba,ca,baca);
                
                double bacaInv[2][2];
                mat22Invert(baca,bacaInv);

                double pq[2];
                mat221Multiply(bacaInv,xa,pq);
                
                double betaAlpha[3];
                vecSubtract(3,beta,alpha,betaAlpha);
                
                double gammaAlpha[3];
                vecSubtract(3,gamma,alpha,gammaAlpha);
                
                double scaleP[3];
                vecScale(3,pq[0],betaAlpha,scaleP);
                
                double scaleQ[3];
                vecScale(3,pq[1],gammaAlpha,scaleQ);
                
                double addPQ[3];
                vecAdd(3,scaleP,scaleQ,addPQ);
                
                double khi[3];
                vecAdd(3,alpha,addPQ,khi);
                
                pixSetRGB(x[0], x[1], khi[0], khi[1], khi[2]);
            }
        }

    }
    
    //first half
    else{
        for(i = ceil(a[0]); i<= floor(c[0]); i++)
        {
            double xLow = a[1] + ((b[1] - a[1])/(b[0] - a[0]))*(i-a[0]); //lower bound
            double xHigh = a[1] + ((c[1] - a[1])/(c[0] - a[0]))*(i-a[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++)
            {
                //calculate khi for each value
                double ba[2];
                vecSubtract(2,b,a,ba);
                
                double ca[2];
                vecSubtract(2,c,a,ca);

                double x[2] = {i, j};
                double xa[2];
                vecSubtract(2,x,a,xa);
                
                double baca[2][2];
                mat22Columns(ba,ca,baca);
                
                double bacaInv[2][2];
                mat22Invert(baca,bacaInv);

                double pq[2];
                mat221Multiply(bacaInv,xa,pq);
                
                double betaAlpha[3];
                vecSubtract(3,beta,alpha,betaAlpha);
                
                double gammaAlpha[3];
                vecSubtract(3,gamma,alpha,gammaAlpha);
                
                double scaleP[3];
                vecScale(3,pq[0],betaAlpha,scaleP);
                
                double scaleQ[3];
                vecScale(3,pq[1],gammaAlpha,scaleQ);
                
                double addPQ[3];
                vecAdd(3,scaleP,scaleQ,addPQ);
                
                double khi[3];
                vecAdd(3,alpha,addPQ,khi);
                
                pixSetRGB(x[0], x[1], khi[0], khi[1], khi[2]);
            }
        }

        //second half
        for(i = ceil(c[0]); i<= floor(b[0]); i++)
        {
            double xLow = a[1] + ((b[1] - a[1])/(b[0] - a[0]))*(i-a[0]); //lower bound
            double xHigh = c[1] + ((b[1] - c[1])/(b[0] - c[0]))*(i-c[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++)
            {
                //calculate khi for each value
                double ba[2];
                vecSubtract(2,b,a,ba);
                
                double ca[2];
                vecSubtract(2,c,a,ca);

                double x[2] = {i, j};
                double xa[2];
                vecSubtract(2,x,a,xa);
                
                double baca[2][2];
                mat22Columns(ba,ca,baca);
                
                double bacaInv[2][2];
                mat22Invert(baca,bacaInv);

                double pq[2];
                mat221Multiply(bacaInv,xa,pq);
                
                double betaAlpha[3];
                vecSubtract(3,beta,alpha,betaAlpha);
                
                double gammaAlpha[3];
                vecSubtract(3,gamma,alpha,gammaAlpha);
                
                double scaleP[3];
                vecScale(3,pq[0],betaAlpha,scaleP);
                
                double scaleQ[3];
                vecScale(3,pq[1],gammaAlpha,scaleQ);
                
                double addPQ[3];
                vecAdd(3,scaleP,scaleQ,addPQ);
                
                double khi[3];
                vecAdd(3,alpha,addPQ,khi);
                
                pixSetRGB(x[0], x[1], khi[0], khi[1], khi[2]);
            }
        }
    }
}