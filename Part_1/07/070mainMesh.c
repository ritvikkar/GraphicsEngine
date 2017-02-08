/*
 * 070mainMesh.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <GLFW/glfw3.h>
#include "000pixel.h"

#include "030matrix.c"
#include "071vector.c"
#include "040texture.c"
#include "050renderer.c"

#define renATTRDIMBOUND 16

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
texTexture *tex[2];

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
#include "070mesh.c"
meshMesh mesh;

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

void draw(void){
    pixClearRGB(0.0,0.0,0.0);
    
    /*
    double a[renATTRDIMBOUND] = {0.0,0.0,0.0,0.0,1.0,1.0,1.0,1.0};
    double b[renATTRDIMBOUND] = {256.0,0.0,0.5,0.5,1.0,1.0,1.0};
    double c[renATTRDIMBOUND] = {0.0,450.0,1,1,1.0,1.0,1.0};
    */
    double unif[renATTRDIMBOUND] = {1.0,1.0,0.0,0.0,0.0,0.0,0.0};
    //meshRender that that will call triRender
    meshInitializeEllipse(&mesh,256,256,50,50,1000);
    //meshInitializeRectangle(&mesh,0,256,0,256);
    meshRender(&mesh,ren,unif,tex);
  
    //triRender(ren, unif, tex, a,b,c);

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

        ren->attrDim = 4; ren->unifDim = 7; ren->texNum = 1;
        //initilize all the values

        draw();

        pixSetTimeStepHandler(handleTimeStep);
        pixSetKeyDownHandler(handleKeyDown);
        texDestroy(tex[0]);
        pixRun();
        meshDestroy(&mesh);
        return 0;
    }
}
