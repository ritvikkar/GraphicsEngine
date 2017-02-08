/*
 * 020triangle.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "000pixel.h"

/*function to rasterize a triangle*/
void triRender(double a[], double b[], double c[], double rgb[]) {
    int i, j;
    
    pixClearRGB(1.0,1.0,1.0);
    
    if(b[1] < c[1] && b[1] < a[1])
    {
        for(i = ceil(a[0]); i<= floor(b[0]); i++)
        {
            double xHigh = a[1] + ((c[1] - a[1])/(c[0] - a[0]))*(i-a[0]); //lower bound
            double xLow = a[1] + ((b[1] - a[1])/(b[0] - a[0]))*(i-a[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++)
            {
                pixSetRGB(i, j, rgb[0], rgb[1], rgb[2]);
            }
        }

        //second half
        for(i = ceil(b[0]); i<= floor(c[0]); i++)
        {
            double xHigh = a[1] + ((c[1] - a[1])/(c[0] - a[0]))*(i-a[0]); //lower bound
            double xLow = b[1] + ((c[1] - b[1])/(c[0] - b[0]))*(i-b[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++)
            {
                pixSetRGB(i, j, rgb[0], rgb[1], rgb[2]);
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
                pixSetRGB(i, j, rgb[0], rgb[1], rgb[2]);
            }
        }

        //second half
        for(i = ceil(c[0]); i<= floor(b[0]); i++)
        {
            double xLow = a[1] + ((b[1] - a[1])/(b[0] - a[0]))*(i-a[0]); //lower bound
            double xHigh = c[1] + ((b[1] - c[1])/(b[0] - c[0]))*(i-c[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++)
            {
                pixSetRGB(i, j, rgb[0], rgb[1], rgb[2]);
            }
        }
    }
}