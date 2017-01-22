/*
 * 080mainPosable.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <GLFW/glfw3.h>
#include "000pixel.h"

#include "030matrix.c"
#include "071vector.c"
#include "040texture.c"
#include "080renderer.c"

#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 1000

#define renATTRX 0
#define renATTRY 1
#define renATTRS 2
#define renATTRT 3
#define renATTRR 4
#define renATTRG 5
#define renATTRB 6

#define renVARYX 0
#define renVARYY 1
#define renVARYS 2
#define renVARYT 3
#define renVARYR 4
#define renVARYG 5
#define renVARYB 6

#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2

#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

renRenderer *ren;
texTexture *tex[2];
double unif[3] = {0.0,256.0,256.0};

/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double vary[], double rgb[]) {

    texSample(tex[0], vary[renVARYS], vary[renVARYT]);
    rgb[0] = tex[0]->sample[renTEXR] * unif[renUNIFR] * vary[renVARYR];
    rgb[1] = tex[0]->sample[renTEXG] * unif[renUNIFG] * vary[renVARYG];
    rgb[2] = tex[0]->sample[renTEXB] * unif[renUNIFB] * vary[renVARYB];
}
/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], 
        double vary[]) {
    /* For now, just copy attr to varying. Baby steps. 
    vary[renVARYX] = attr[renATTRX];
    vary[renVARYY] = attr[renATTRY];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];*/

    double r[2][2];
    double xy[2];
    double rTimesxy[2];
    double t[2];

    double r1[2] = { cos(unif[0]) , (-1)*sin(unif[0])};
    double r2[2] = { sin(unif[0]) , cos(unif[0])};

    mat22Columns(r1,r2,r);

    xy[0] = attr[renATTRX];
    xy[1] = attr[renATTRY];

    mat221Multiply(r,xy,rTimesxy);

    t[0] = unif[1];
    t[1] = unif[2];
    vecAdd(2,t,rTimesxy,vary);

    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];

}

#include "080triangle.c"
#include "080mesh.c"
meshMesh mesh;
void draw(void){
    pixClearRGB(0.0,0.0,0.0);
    //meshRender that that will call triRender
    meshRender(&mesh,ren,unif,tex);
  
    //triRender(ren, unif, tex, a,b,c);

}

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
    unif[0] += (newTime - oldTime);
    draw();
}


int main(void) {
    /* Make a 512 x 512 window with the title 'Pixel Graphics'. This function 
    returns 0 if no error occurred. */ 

    ren = (renRenderer *)malloc(sizeof(renRenderer));
    tex[0] = (texTexture *)malloc(sizeof(texTexture));
    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
        return 1;
    
    else {
        texInitializeFile(tex[0], "pic1.jpg");
        if (texInitializeFile(tex[0], "pic1.jpg") !=0)
        {
            return 1;
        }

        ren->attrDim = 4; ren->unifDim = 7; ren->texNum = 1; ren->varyDim = 4;
        //initilize all the values
        meshInitializeEllipse(&mesh,0,0,50,50,1000);
        //meshInitializeRectangle(&mesh,0,256,0,256);
        draw();

        pixSetTimeStepHandler(handleTimeStep);
        pixSetKeyDownHandler(handleKeyDown);
        texDestroy(tex[0]);
        pixRun();
        meshDestroy(&mesh);
        return 0;
    }
}
