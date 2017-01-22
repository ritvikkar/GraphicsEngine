/*
 * 020mainRasterizing.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

#include "020triangle.c"
#include "000pixel.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
	/* Make a 512 x 512 window with the title 'Pixel Graphics'. This function 
	returns 0 if no error occurred. */
	
    int i,j;
    
    double ax,ay,bx,by,cx,cy;
    //accept the initial values of the verices
    printf("Enter vertex 1, x: ");
    scanf("%lf", &ax);
    
    printf("Enter vertex 1, y: ");
    scanf("%lf", &ay);

    printf("Enter vertex 2, x: ");
    scanf("%lf", &bx);

    printf("Enter vertex 2, y: ");
    scanf("%lf", &by);

    printf("Enter vertex 3, x: ");
    scanf("%lf", &cx);

    printf("Enter vertex 3, y: ");
    scanf("%lf", &cy);
    
    double rgb[3];
    printf("Enter the color you want (0.0 - 1.0):\n");
    scanf("%lf", &rgb[0]);
    scanf("%lf", &rgb[1]);
    scanf("%lf", &rgb[2]);

    double a[2], b[2], c[2];
    a[1] = a[0] = b[0] = b[1] = c[1] = c[0] = 0;    
    
    
    //begin sorting the vertices
    if((ax == bx && ax == cx) || (ay == by && ay == cy)) //a b and c are a line in either orientation
    {
        a[0] = ax;
        a[1] = ay;
        b[0] = bx;
        b[1] = by;
        c[0] = cx;
        c[1] = cy;
        ax = ay = cx = cy = bx = by = 0;
    }
    
    //figures out the first element
    else if(ax <= bx && ax <= cx){ //if a is the smallest or 2nd
        
        if(ax == bx){
            if(ay < by){ //a is the least and b 2nd least, and c 3rd
                a[0] = ax;
                a[1] = ay;
                b[0] = cx;
                b[1] = cy;
                c[0] = bx;
                c[1] = by;
                
                ax = ay = cx = cy = bx = by = 0; 
                // since element is sorted, the place holders are marked zero
            }
            else{ //b is the least, a 2nd least and c 3rd
                a[0] = bx;
                a[1] = by;
                b[0] = cx;
                b[1] = cy;
                c[0] = ax;
                c[1] = ay;
                
                ax = ay = cx = cy = bx = by = 0;
            }
        }
        
        else if(ax == cx)
        {
            if(ay < cy) //a is least, c is 2nd least and b 3rd
            {
                a[0] = ax;
                a[1] = ay;
                b[0] = bx;
                b[1] = by;
                c[0] = cx;
                c[1] = cy;
                
                ax = ay = cx = cy = bx = by = 0;
            }
            
            else{ //c is least, a 2nd, and b 3rd
                a[0] = cx;
                a[1] = cy;
                b[0] = bx;
                b[1] = by;
                c[0] = ax;
                c[1] = ay;
                
                ax = ay = cx = cy = bx = by = 0;
            }
        }
        
        else//if a is the smallest
        {
            a[0] = ax;
            a[1] = ay;
            ax = ay = 0;
        }
    }
    
    else if(bx <= ax && bx <= cx){ //if b is the smallest or 2nd
        
        if(bx == ax){
            if(by < ay){ //b is the least and a 2nd least, and c 3rd
                a[0] = bx;
                a[1] = by;
                b[0] = cx;
                b[1] = cy;
                c[0] = ax;
                c[1] = ay;
                ax = ay = cx = cy = bx = by = 0;
            }
            else{ //a is the least, b 2nd least and c 3rd
                a[0] = ax;
                a[1] = ay;
                b[0] = cx;
                b[1] = cy;
                c[0] = bx;
                c[1] = by;
                ax = ay = cx = cy = bx = by = 0;
            }
        }
        
        else if(bx == cx)
        {
            if(by < cy) //b is least, c is 2nd least and a 3rd
            {
                a[0] = bx;
                a[1] = by;
                b[0] = ax;
                b[1] = ay;
                c[0] = cx;
                c[1] = cy;
                ax = ay = cx = cy = bx = by = 0;
            }
            
            else{ //c is least, b 2nd, and a 3rd
                a[0] = cx;
                a[1] = cy;
                b[0] = ax;
                b[1] = ay;
                c[0] = bx;
                c[1] = by;
                ax = ay = cx = cy = bx = by = 0;
            }
        }
        
        else//if b is the smallest
        {
            a[0] = bx;
            a[1] = by;
            bx = by = 0;
        }
    }
    
    else if(cx <= ax && cx <= bx){ //if c is the smallest or 2nd

        if(cx == ax){
            if(cy < ay){ //c is the least and a 2nd least, and b 3rd
                a[0] = cx;
                a[1] = cy;
                b[0] = bx;
                b[1] = by;
                c[0] = ax;
                c[1] = ay;
                ax = ay = cx = cy = bx = by = 0;
            }
            else{ //a is the least, c 2nd least and b 3rd
                a[0] = ax;
                a[1] = ay;
                b[0] = bx;
                b[1] = by;
                c[0] = cx;
                c[1] = cy;
                ax = ay = cx = cy = bx = by = 0;
            }
        }

        else if(cx == bx)
        {
            if(cy < by) //c is least, b is 2nd least and a 3rd
            {
                a[0] = cx;
                a[1] = cy;
                b[0] = ax;
                b[1] = ay;
                c[0] = bx;
                c[1] = by;
                ax = ay = cx = cy = bx = by = 0;
            }

            else{ //c is least, b 2nd, and a 3rd
                a[0] = cx;
                a[1] = cy;
                b[0] = ax;
                b[1] = ay;
                c[0] = bx;
                c[1] = by;
                ax = ay = cx = cy = bx = by = 0;
            }
        }

        else//if c is the smallest
        {
            a[0] = cx;
            a[1] = cy;
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
                        c[0] = cx;
                        c[1] = cy;

                        cx = cy = bx = by = 0;
                    }
                    else{ //c 2nd least and b 3rd
                        b[0] = cx;
                        b[1] = cy;
                        c[0] = bx;
                        c[1] = by;

                        cx = cy = bx = by = 0;
                    }
                }

                else//if b is the smallest
                {
                    b[0] = bx;
                    b[1] = by;
                    c[0] = cx;
                    c[1] = cy;
                    cx = cy = bx = by = 0;
                }
            }
            
            else{//c is 2nd
                c[0] = bx;
                c[1] = by;
                b[0] = cx;
                b[1] = cy;
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
                        c[0] = cx;
                        c[1] = cy;

                        cx = cy = ax = ay = 0;
                    }
                    else{ //c 2nd least and a 3rd
                        b[0] = cx;
                        b[1] = cy;
                        c[0] = ax;
                        c[1] = ay;

                        cx = cy = ax = ay = 0;
                    }
                }

                else//if a is the smallest
                {
                    b[0] = ax;
                    b[1] = ay;
                    c[0] = cx;
                    c[1] = cy;
                    cx = cy = ax = ay = 0;
                }
            }
            
            else{//c is 2nd
                c[0] = ax;
                c[1] = ay;
                b[0] = cx;
                b[1] = cy;
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
                        c[0] = bx;
                        c[1] = by;

                        bx = by = ax = ay = 0;
                    }
                    else{ //b 2nd least and a 3rd
                        b[0] = bx;
                        b[1] = by;
                        c[0] = ax;
                        c[1] = by;

                        bx = by = ax = ay = 0;
                    }
                }

                else//if a is the smallest
                {
                    b[0] = ax;
                    b[1] = ay;
                    c[0] = bx;
                    c[1] = by;
                    bx = by = ax = ay = 0;
                }
            }
            
            else{//b is 2nd
                c[0] = ax;
                c[1] = ay;
                b[0] = bx;
                b[1] = by;
                bx = by = ax = ay = 0;
            }
        }
    }

    printf("(%f,%f) (%f,%f) (%f,%f) \n", a[0], a[1], b[0], b[1], c[0], c[1]); //print out final orientation

    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
		return 1;
    
	else {
        triRender(a,b,c,rgb);
        pixRun();
		return 0;
	}
}
