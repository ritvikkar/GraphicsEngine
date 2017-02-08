/*
 * 040mainRasterizing.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

#include "030matrix.c"
#include "030vector.c"
#include "040texture.c"
#include "040triangle.c"
#include "000pixel.h"
#include <stdio.h>
#include <stdlib.h>

#define GLFW_KEY_ENTER 257

int key,filter=0;
texTexture *tex;

void handleKeyDown(int key, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown){
    if(key == 257)
    {
        if(filter == 0)
        {
            filter = 1;
            tex->filtering = filter;
            //printf("Filter: %d\n", tex->filtering);
        }

        else{
            filter = 0;
            tex->filtering = filter;        
            //printf("Filter: %d\n", tex->filtering);
        }
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
            if(by <= cy){ //if b is 2nd or 3rd 

                if(by == cy){
                    if(bx < cx){ //b is the 2nd least and c 3rd
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
            if(ay <= cy){ //if a is 2nd or 3rd 

                if(ay == cy){
                    if(ax < cx){ //a is the 2nd least and c 3rd
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
            }
            
            else{//c is 2nd
                c[0] = ax;
                c[1] = ay;
                gamma[0] = a0;
                gamma[1] = a1;
                gamma[2] = a2;
                b[0] = cx;
                b[1] = cy;
                beta[0] = c0;
                beta[1] = c1;
                beta[2] = c2;
                cx = cy = ax = ay = 0;
            }
        }
        
        else if(cx == 0 && cy == 0)//bxby axay is left
        {
            if(ay <= by){ //if a is 2nd or 3rd 

                if(ay == by){
                    if(ax < bx){ //a is the 2nd least and b 3rd
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

int main(void) {
	/* Make a 512 x 512 window with the title 'Pixel Graphics'. This function 
	returns 0 if no error occurred. */
	
    int i,j;
    
    double a[2], b[2], c[2];
    double alpha[3], beta[3], gamma[3];
    double rgb[3];

/*    //accept the initial values of the verices
    printf("Enter vertex 1, x: ");
    scanf("%lf", &a[0]);
    
    printf("Enter vertex 1, y: ");
    scanf("%lf", &a[1]);
    
    printf("Enter α, (RGB, 0.0 - 1.0):\n");
    scanf("%lf", &alpha[0]);
    scanf("%lf", &alpha[1]);
    //scanf("%lf", &alpha[2]);

    printf("Enter vertex 2, x: ");
    scanf("%lf", &b[0]);

    printf("Enter vertex 2, y: ");
    scanf("%lf", &b[1]);
    
    printf("Enter β, (RGB, 0.0 - 1.0):\n");
    scanf("%lf", &beta[0]);
    scanf("%lf", &beta[1]);
    //scanf("%lf", &beta[2]);

    printf("Enter vertex 3, x: ");
    scanf("%lf", &c[0]);

    printf("Enter vertex 3, y: ");
    scanf("%lf", &c[1]);
    
    printf("Enter γ, (RGB, 0.0 - 1.0):\n");
    scanf("%lf", &gamma[0]);
    scanf("%lf", &gamma[1]);
   // scanf("%lf", &gamma[2]);
    
    printf("Enter the μ you want, (RGB, 0.0 - 1.0):\n");
    scanf("%lf", &rgb[0]);
    scanf("%lf", &rgb[1]);
    scanf("%lf", &rgb[2]);*/


    //tester code
    a[0] = 0;
    a[1] = 256;
    b[0] = 256;
    b[1] = 0;
    c[0] = 512;
    c[1] = 256;
    
    rgb[0] = 0;
    rgb[1] = 0;
    rgb[2] = 0;
        
    alpha[0] = 0;
    alpha[1] = 0.5;

    beta[0] = 0.5;
    beta[1] = 0;

    gamma[0] = 1;
    gamma[1] = 0.5;
        
    double a1[2], b1[2], c1[2];
    double alpha1[3], beta1[3], gamma1[3];
    a1[0] = 0;
    a1[1] = 256;
    b1[0] = 512;
    b1[1] = 256;
    c1[0] = 256;
    c1[1] = 512;

    alpha1[0] = 0;
    alpha1[1] = 0.5;

    beta1[0] = 1;
    beta1[1] = 0.5;

    gamma1[0] = 0.5;
    gamma1[1] = 0;

    pixClearRGB(0.0,0.0,0.0);

    sort(a,b,c,alpha,beta,gamma);//also keeps the alpha beta gamma

    tex = (texTexture *)malloc(sizeof(texTexture));
    texInitializeFile(tex, "pic.jpg");
    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
		return 1;
    
	else {
        pixSetKeyDownHandler(handleKeyDown);
        pixSetTimeStepHandler(handleTimeStep);
            
        tex->filtering = 0;
        triRender(a,b,c,rgb,tex,alpha,beta,gamma);
        
        tex->filtering = 1;
        triRender(a1,b1,c1,rgb,tex,alpha1,beta1,gamma1);

        
        texDestroy(tex);
        pixRun();
		return 0;
	}

}
