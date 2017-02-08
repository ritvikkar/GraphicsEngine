/*
 * 050mainRasterizing.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

#include "000pixel.h"
#include "030matrix.c"
#include "030vector.c"
#include "040texture.c"

#define renATTRDIMBOUND 16

#include "050renderer.c"

#define renATTRX 0
#define renATTRY 1
#define renATTRS 2
#define renATTRT 3
#define renATTRR 4
#define renATTRG 5
#define renATTRB 6
#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

renRenderer *ren;
texTexture *tex[1];

/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double attr[], double rgb[]) {
    texSample(tex[0], attr[renATTRS], attr[renATTRT]);
    rgb[0] = tex[0]->sample[renTEXR] * unif[renUNIFR] * attr[renATTRR];
    rgb[1] = tex[0]->sample[renTEXG] * unif[renUNIFG] * attr[renATTRG];
    rgb[2] = tex[0]->sample[renTEXB] * unif[renUNIFB] * attr[renATTRB];
}


#include "051triangle.c"
#include "000pixel.h"
#include <stdio.h>
#include <stdlib.h>

#define GLFW_KEY_ENTER 0


void handleKeyDown(int key, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown){
    if(key == GLFW_KEY_ENTER)
    {
        if(tex[0]->filtering == texQUADRATIC)
            texSetFiltering(tex[0], texNEAREST);
            
        else
            texSetFiltering(tex[0], texQUADRATIC);
    }

    printf("key down %d, shift %d, control %d, altOpt %d, supComm %d\n",
        key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
}

void handleTimeStep(double oldTime, double newTime) {
	if (floor(newTime) - floor(oldTime) >= 1.0)
		printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
}

void sort(double a[2], double b[2], double c[2], double alpha[3], double beta[3], double gamma[3])
{
    double ax,ay,a0,a1,a2,bx,by,b0,b1,b2,cx,cy,c0,c1,c2;
    ax = a[0];
    ay = a[1];
    a0 = alpha[0];
    a1 = alpha[1];
    a2 = alpha[2];
    bx = b[0];
    by = b[1];
    b0 = beta[0];
    b1 = beta[1];
    b2 = beta[2];
    cx = c[0];
    cy = c[1];
    c0 = gamma[0];
    c1 = gamma[1];
    c2 = gamma[2];
    
    //to be refilled with sorted quantities
    a[1] = a[0] = b[0] = b[1] = c[1] = c[0] = 0;
    alpha[0] = alpha[1] = alpha[2] = 0;
    beta[0] = beta[1] = beta[2] = 0;
    gamma[0] = gamma[1] = gamma[2] = 0;    
    
    //begin sorting the vertices
    if((ax == bx && ax == cx) || (ay == by && ay == cy)) //a b and c are a line in either orientation
    {
        a[0] = ax;
        a[1] = ay;
        alpha[0] = a0;
        alpha[1] = a1;
        alpha[2] = a2;
        b[0] = bx;
        b[1] = by;
        beta[0] = b0;
        beta[1] = b1;
        beta[2] = b2;
        c[0] = cx;
        c[1] = cy;
        gamma[0] = c0;
        gamma[1] = c1;
        gamma[2] = c2;
        ax = ay = cx = cy = bx = by = 0;
    }
    
    //figures out the first element
    else if(ax <= bx && ax <= cx){ //if a is the smallest or 2nd
        
        if(ax == bx){
            if(ay < by){ //a is the least and b 2nd least, and c 3rd
                a[0] = ax;
                a[1] = ay;
                alpha[0] = a0;
                alpha[1] = a1;
                alpha[2] = a2;
                b[0] = cx;
                b[1] = cy;
                beta[0] = c0;
                beta[1] = c1;
                beta[2] = c2;
                c[0] = bx;
                c[1] = by;
                gamma[0] = b0;
                gamma[1] = b1;
                gamma[2] = b2;

                
                ax = ay = cx = cy = bx = by = 0; 
                // since element is sorted, the place holders are marked zero
            }
            else{ //b is the least, a 2nd least and c 3rd
                a[0] = bx;
                a[1] = by;
                alpha[0] = b0;
                alpha[1] = b1;
                alpha[2] = b2;
                b[0] = cx;
                b[1] = cy;
                beta[0] = c0;
                beta[1] = c1;
                beta[2] = c2;
                c[0] = ax;
                c[1] = ay;
                gamma[0] = a0;
                gamma[1] = a1;
                gamma[2] = a2;
                
                ax = ay = cx = cy = bx = by = 0;
            }
        }
        
        else if(ax == cx)
        {
            if(ay < cy) //a is least, c is 2nd least and b 3rd
            {
                a[0] = ax;
                a[1] = ay;
                alpha[0] = a0;
                alpha[1] = a1;
                alpha[2] = a2;
                b[0] = bx;
                b[1] = by;
                beta[0] = b0;
                beta[1] = b1;
                beta[2] = b2;
                c[0] = cx;
                c[1] = cy;
                gamma[0] = c0;
                gamma[1] = c1;
                gamma[2] = c2; 
                ax = ay = cx = cy = bx = by = 0;
            }
            
            else{ //c is least, a 2nd, and b 3rd
                a[0] = cx;
                a[1] = cy;
                alpha[0] = c0;
                alpha[1] = c1;
                alpha[2] = c2;
                b[0] = bx;
                b[1] = by;
                beta[0] = b0;
                beta[1] = b1;
                beta[2] = b2;
                c[0] = ax;
                c[1] = ay;
                gamma[0] = a0;
                gamma[1] = a1;
                gamma[2] = a2; 
                ax = ay = cx = cy = bx = by = 0;
            }
        }
        
        else//if a is the smallest
        {
            a[0] = ax;
            a[1] = ay;
            alpha[0] = a0;
            alpha[1] = a1;
            alpha[2] = a2;
            ax = ay = 0;
        }
    }
    
    else if(bx <= ax && bx <= cx){ //if b is the smallest or 2nd
        
        if(bx == ax){
            if(by < ay){ //b is the least and a 2nd least, and c 3rd
                a[0] = bx;
                a[1] = by;
                alpha[0] = b0;
                alpha[1] = b1;
                alpha[2] = b2;
                b[0] = cx;
                b[1] = cy;
                beta[0] = c0;
                beta[1] = c1;
                beta[2] = c2;
                c[0] = ax;
                c[1] = ay;
                gamma[0] = a0;
                gamma[1] = a1;
                gamma[2] = a2; 
                ax = ay = cx = cy = bx = by = 0;
            }
            else{ //a is the least, b 2nd least and c 3rd
                a[0] = ax;
                a[1] = ay;
                alpha[0] = a0;
                alpha[1] = a1;
                alpha[2] = a2;
                b[0] = cx;
                b[1] = cy;
                beta[0] = c0;
                beta[1] = c1;
                beta[2] = c2;
                c[0] = bx;
                c[1] = by;
                gamma[0] = b0;
                gamma[1] = b1;
                gamma[2] = b2;
                ax = ay = cx = cy = bx = by = 0;
            }
        }
        
        else if(bx == cx)
        {
            if(by < cy) //b is least, c is 2nd least and a 3rd
            {
                a[0] = bx;
                a[1] = by;
                alpha[0] = b0;
                alpha[1] = b1;
                alpha[2] = b2;
                b[0] = ax;
                b[1] = ay;
                beta[0] = a0;
                beta[1] = a1;
                beta[2] = a2;
                c[0] = cx;
                c[1] = cy;
                gamma[0] = c0;
                gamma[1] = c1;
                gamma[2] = c2;
                ax = ay = cx = cy = bx = by = 0;
            }
            
            else{ //c is least, b 2nd, and a 3rd
                a[0] = cx;
                a[1] = cy;
                alpha[0] = c0;
                alpha[1] = c1;
                alpha[2] = c2;
                b[0] = ax;
                b[1] = ay;
                beta[0] = a0;
                beta[1] = a1;
                beta[2] = a2;
                c[0] = bx;
                c[1] = by;
                gamma[0] = b0;
                gamma[1] = b1;
                gamma[2] = b2;
                ax = ay = cx = cy = bx = by = 0;
            }
        }
        
        else//if b is the smallest
        {
            a[0] = bx;
            a[1] = by;
            alpha[0] = b0;
            alpha[1] = b1;
            alpha[2] = b2;
            bx = by = 0;
        }
    }
    
    else if(cx <= ax && cx <= bx){ //if c is the smallest or 2nd

        if(cx == ax){
            if(cy < ay){ //c is the least and a 2nd least, and b 3rd
                a[0] = cx;
                a[1] = cy;
                alpha[0] = c0;
                alpha[1] = c1;
                alpha[2] = c2;
                b[0] = bx;
                b[1] = by;
                beta[0] = b0;
                beta[1] = b1;
                beta[2] = b2;
                c[0] = ax;
                c[1] = ay;
                gamma[0] = a0;
                gamma[1] = a1;
                gamma[2] = a2;
                ax = ay = cx = cy = bx = by = 0;
            }
            else{ //a is the least, c 2nd least and b 3rd
                a[0] = ax;
                a[1] = ay;
                alpha[0] = a0;
                alpha[1] = a1;
                alpha[2] = a2;
                b[0] = bx;
                b[1] = by;
                beta[0] = b0;
                beta[1] = b1;
                beta[2] = b2;
                c[0] = cx;
                c[1] = cy;
                gamma[0] = c0;
                gamma[1] = c1;
                gamma[2] = c2;
                ax = ay = cx = cy = bx = by = 0;
            }
        }

        else if(cx == bx)
        {
            if(cy < by) //c is least, b is 2nd least and a 3rd
            {
                a[0] = cx;
                a[1] = cy;
                alpha[0] = c0;
                alpha[1] = c1;
                alpha[2] = c2;
                b[0] = ax;
                b[1] = ay;
                beta[0] = a0;
                beta[1] = a1;
                beta[2] = a2;
                c[0] = bx;
                c[1] = by;
                gamma[0] = b0;
                gamma[1] = b1;
                gamma[2] = b2;
                ax = ay = cx = cy = bx = by = 0;
            }

            else{ //b is least, c 2nd, and a 3rd
                a[0] = bx;
                a[1] = by;
                alpha[0] = b0;
                alpha[1] = b1;
                alpha[2] = b2;
                b[0] = ax;
                b[1] = ay;
                beta[0] = a0;
                beta[1] = a1;
                beta[2] = a2;
                c[0] = cx;
                c[1] = cy;
                gamma[0] = c0;
                gamma[1] = c1;
                gamma[2] = c2;
                ax = ay = cx = cy = bx = by = 0;
            }
        }

        else//if c is the smallest
        {
            a[0] = cx;
            a[1] = cy;
            alpha[0] = c0;
            alpha[1] = c1;
            alpha[2] = c2;
            cx = cy = 0;
        }
    }
    
    //any elements sorted have been marked 0
    if((b[0] == 0.0 && b[1] == 0.0) || (c[0] == 0.0 && c[1] == 0.0)) // if only the first element was caught
    {
        if(ax == 0 && ay == 0)//bxby cxcy is left
        {
            if(cx <= bx){ //if b is 2nd or 3rd 

                if(by == cy){
                    if(bx > cx){ //b is the 2nd least and c 3rd
                        b[0] = bx;
                        b[1] = by;
                        beta[0] = b0;
                        beta[1] = b1;
                        beta[2] = b2;
                        c[0] = cx;
                        c[1] = cy;
                        gamma[0] = c0;
                        gamma[1] = c1;
                        gamma[2] = c2;
                        cx = cy = bx = by = 0;
                    }
                    else{ //c 2nd least and b 3rd
                        b[0] = cx;
                        b[1] = cy;
                        beta[0] = c0;
                        beta[1] = c1;
                        beta[2] = c2;
                        c[0] = bx;
                        c[1] = by;
                        gamma[0] = b0;
                        gamma[1] = b1;
                        gamma[2] = b2;
                        cx = cy = bx = by = 0;
                    }
                }

                else//if b is the smallest
                {
                    b[0] = bx;
                    b[1] = by;
                    beta[0] = b0;
                    beta[1] = b1;
                    beta[2] = b2;
                    c[0] = cx;
                    c[1] = cy;
                    gamma[0] = c0;
                    gamma[1] = c1;
                    gamma[2] = c2;
                    cx = cy = bx = by = 0;
                }
            }
            
            else{//c is 2nd
                c[0] = bx;
                c[1] = by;
                gamma[0] = b0;
                gamma[1] = b1;
                gamma[2] = b2;
                b[0] = cx;
                b[1] = cy;
                beta[0] = c0;
                beta[1] = c1;
                beta[2] = c2;
                cx = cy = bx = by = 0;
            }
        }
        
        else if(bx == 0 && by == 0)//axay cxcy is left
        {
            if(ax <= cx){ //if a is 2nd or 3rd 

                if(ax == cx){
                    if(ay < cy){ //a is the 2nd least and c 3rd
                        b[0] = ax;
                        b[1] = ay;
                        beta[0] = a0;
                        beta[1] = a1;
                        beta[2] = a2;
                        c[0] = cx;
                        c[1] = cy;
                        gamma[0] = c0;
                        gamma[1] = c1;
                        gamma[2] = c2;
                        cx = cy = ax = ay = 0;
                    }
                    else{ //c 2nd least and a 3rd
                        b[0] = cx;
                        b[1] = cy;
                        beta[0] = c0;
                        beta[1] = c1;
                        beta[2] = c2;
                        c[0] = ax;
                        c[1] = ay;
                        gamma[0] = a0;
                        gamma[1] = a1;
                        gamma[2] = a2;
                        cx = cy = ax = ay = 0;
                    }
                }

                else//if a is the smallest
                {
                    b[0] = cx;
                    b[1] = cy;
                    beta[0] = c0;
                    beta[1] = c1;
                    beta[2] = c2;
                    c[0] = ax;
                    c[1] = ay;
                    gamma[0] = a0;
                    gamma[1] = a1;
                    gamma[2] = a2;
                    cx = cy = ax = ay = 0;
                }
            }
            
            else{//c is 2nd
                c[0] = cx;
                c[1] = cy;
                gamma[0] = c0;
                gamma[1] = c1;
                gamma[2] = c2;
                b[0] = ax;
                b[1] = ay;
                beta[0] = a0;
                beta[1] = a1;
                beta[2] = a2;
                cx = cy = ax = ay = 0;
            }
        }
        
        else if(cx == 0 && cy == 0)//bxby axay is left
        {
            if(ax <= bx){ //if a is 2nd or 3rd 

                if(ax == bx){
                    if(ay < by){ //a is the 2nd least and b 3rd
                        b[0] = ax;
                        b[1] = ay;
                        beta[0] = a0;
                        beta[1] = a1;
                        beta[2] = a2;
                        c[0] = bx;
                        c[1] = by;
                        gamma[0] = b0;
                        gamma[1] = b1;
                        gamma[2] = b2;
                        bx = by = ax = ay = 0;
                    }
                    else{ //b 2nd least and a 3rd
                        b[0] = bx;
                        b[1] = by;
                        beta[0] = b0;
                        beta[1] = b1;
                        beta[2] = b2;
                        c[0] = ax;
                        c[1] = ay;
                        gamma[0] = a0;
                        gamma[1] = a1;
                        gamma[2] = a2;
                        bx = by = ax = ay = 0;
                    }
                }

                else//if a is the smallest
                {
                    b[0] = ax;
                    b[1] = ay;
                    beta[0] = a0;
                    beta[1] = a1;
                    beta[2] = a2;
                    c[0] = bx;
                    c[1] = by;
                    gamma[0] = b0;
                    gamma[1] = b1;
                    gamma[2] = b2;
                    bx = by = ax = ay = 0;
                }
            }
            
            else{//b is 2nd
                c[0] = ax;
                c[1] = ay;
                gamma[0] = a0;
                gamma[1] = a1;
                gamma[2] = a2;
                b[0] = bx;
                b[1] = by;
                beta[0] = b0;
                beta[1] = b1;
                beta[2] = b2;

                bx = by = ax = ay = 0;
            }
        }
    }
}

void draw(void){
    pixClearRGB(0.0,0.0,0.0);
    ren = (renRenderer *)malloc(sizeof(renRenderer));

    ren->attrDim = 4;
    ren->unifDim = 3;
    tex[0] = (texTexture *)malloc(sizeof(texTexture));
    texInitializeFile(tex[0], "pic.jpg");
    double a[ren->attrDim] = {0,512,0,1};
    double b[ren->attrDim] = {512,0,0.5,0.5};
    double c[ren->attrDim] = {512,512,1,1};
    double unif[ren->unifDim] ={0,0,0};    
    //sort(a,b,c,alpha,beta,gamma);//also keeps the alpha beta gamma
    texSetFiltering(tex[0],texQUADRATIC);
    triRender(ren,unif,tex,a,b,c);
    texDestroy(tex[0]);

}

int main(void) {
	/* Make a 512 x 512 window with the title 'Pixel Graphics'. This function 
	returns 0 if no error occurred. */
        
    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
		return 1;
    
	else {
        pixSetTimeStepHandler(handleTimeStep);
        pixSetKeyDownHandler(handleKeyDown);
        
        draw();
        
        pixRun();
		return 0;
	}
}
