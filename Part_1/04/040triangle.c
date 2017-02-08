/*
 * 040triangle.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "000pixel.h"


/*function to rasterize a triangle*/
void triRender(double a[2], double b[2], double c[2], double rgb[3], 
               texTexture *tex, double alpha[3], 
               double beta[3], double gamma[3]) {
    int i, j;
        
    if(b[1] < c[1] && b[1] < a[1])
    {
        for(i = ceil(a[0]); i<= floor(b[0]); i++)
        {
            double xHigh = a[1] + ((c[1] - a[1])/(c[0] - a[0]))*(i-a[0]); //lower bound
            double xLow = a[1] + ((b[1] - a[1])/(b[0] - a[0]))*(i-a[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++)
            {
                double x[2] = {i, j};

                /*
                 * Implementing the following formula
                 * khi = alpha + p (beta - alpha) + q(gamma - alpha)
                 *        [1]            [2]               [3]
                 *                       [4]               [5]
                 *                               [6]
                 *
                 * [b-a | c-a]^-1 [x-a] = [p q]
                 *  [1*]  [2*]     [3*]
                 *     [4*]
                 *           [5*]   
                 */
                double ba[2]; //[1*]
                vecSubtract(2,b,a,ba);
                
                double ca[2];//[2*]
                vecSubtract(2,c,a,ca);

                double xa[2];//[3*]
                vecSubtract(2,x,a,xa);
                
                double baca[2][2];//[4*]
                mat22Columns(ba,ca,baca);
                
                double bacaInv[2][2];//[5*]
                mat22Invert(baca,bacaInv);

                double pq[2];//pq
                mat221Multiply(bacaInv,xa,pq);
                
                //calculate khi
                double betaAlpha[3];//[2]
                vecSubtract(3,beta,alpha,betaAlpha);
                
                double gammaAlpha[3];//[3]
                vecSubtract(3,gamma,alpha,gammaAlpha);
                
                double scaleP[3];//[4]
                vecScale(3,pq[0],betaAlpha,scaleP);
                
                double scaleQ[3];//[5]
                vecScale(3,pq[1],gammaAlpha,scaleQ);
                
                double addPQ[3];//[6]
                vecAdd(3,scaleP,scaleQ,addPQ);
                
                double khi[3];//khi
                vecAdd(3,alpha,addPQ,khi);
                
                texSample(tex, khi[0], khi[1]);
                //getting the texture color at Khi

                pixSetRGB(x[0], x[1], tex->sample[0], tex->sample[1], tex->sample[2]);
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
                
                texSample(tex, khi[0], khi[1]);
                pixSetRGB(x[0], x[1], tex->sample[0], tex->sample[1], tex->sample[2]);
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
                
                texSample(tex, khi[0], khi[1]);
                pixSetRGB(x[0], x[1], tex->sample[0], tex->sample[1], tex->sample[2]);

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
                
                texSample(tex, khi[0], khi[1]);
                                
                pixSetRGB(x[0], x[1], tex->sample[0], tex->sample[1], tex->sample[2]);

            }
        }
    }
}