/*
 * 090mainScene.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#include <GLFW/glfw3.h>
#include "000pixel.h"

#include "100vector.c"
#include "100matrix.c"
#include "040texture.c"
#include "090renderer.c"

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
#define renUNIFTHETA 0
#define renUNIFTRANSX 1
#define renUNIFTRANSY 2
#define renUNIFISOMETRY 3

#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

renRenderer ren;

texTexture *tex[1];
double unif[12] = {0.0,0.0,0.0};

/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double vary[], double rgb[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);
    rgb[0] = tex[0]->sample[renTEXR];
    rgb[1] = tex[0]->sample[renTEXG];
    rgb[2] = tex[0]->sample[renTEXB];
}
/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], 
        double vary[]) {
    double xy [3] = {attr[renATTRX],attr[renATTRY],1};
    double rxy[3];
    mat331Multiply((double(*)[3])(&unif[renUNIFISOMETRY]),xy,rxy);
    vary[renVARYX] = rxy[0];
    vary[renVARYY] = rxy[1];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}

/* If unifParent is NULL, then sets the uniform matrix to the 
rotation-translation M described by the other uniforms. If unifParent is not 
NULL, but instead contains a rotation-translation P, then sets the uniform 
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    if (unifParent == NULL)
        /* The nine uniforms for storing the matrix start at index 
        renUNIFISOMETRY. So &unif[renUNIFISOMETRY] is an array containing those 
        nine numbers. We use '(double(*)[3])' to cast it to a 3x3 matrix. */
        mat33Isometry(unif[renUNIFTHETA], unif[renUNIFTRANSX], 
            unif[renUNIFTRANSY], (double(*)[3])(&unif[renUNIFISOMETRY]));
    else {
        double m[3][3];
        mat33Isometry(unif[renUNIFTHETA], unif[renUNIFTRANSX], 
            unif[renUNIFTRANSY], m);
        mat333Multiply((double(*)[3])(&unifParent[renUNIFISOMETRY]), m, 
            (double(*)[3])(&unif[renUNIFISOMETRY]));
    }
}

#include "092triangle.c"
#include "100mesh.c"
#include "090scene.c"
sceneNode scene0;
sceneNode scene1;
sceneNode scene2;

meshMesh mesh0;
meshMesh mesh1;
meshMesh mesh2;

//meshMesh mesh;
void draw(void){
    pixClearRGB(0.0,0.0,0.0);
    //meshRender that that will call triRender
    //meshRender(&mesh,ren,unif,tex);
      
    //sceneRender that will call meshRender
    sceneRender(&scene0,&ren,NULL);

    //triRender(ren, unif, tex, a,b,c);

}

void handleKeyDown(int key, int shiftIsDown, int controlIsDown,
        int altOptionIsDown, int superCommandIsDown){
    if(key == GLFW_KEY_ENTER)
    {
        if(tex[0]->filtering == texQUADRATIC)
        {
            texSetFiltering(tex[0], texNEAREST);
            draw();
        }
            
        else{
            texSetFiltering(tex[0], texQUADRATIC);
            draw();
        }
    }

    printf("key down %d, shift %d, control %d, altOpt %d, supComm %d\n",
        key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
}

void handleTimeStep(double oldTime, double newTime) {
  if (floor(newTime) - floor(oldTime) >= 1.0)
    printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
    unif[0] = unif[0] + 0.01;
    unif[1] = 256;
    unif[2] = 256;

    sceneSetUniform(&scene0,&ren,unif);
    draw();
}

int main(void) {
    /* Make a 512 x 512 window with the title 'Pixel Graphics'. This function 
    returns 0 if no error occurred. */ 
    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
        return 1;
    
    else {
        texTexture texture0, texture1, texture2;
        texInitializeFile(&texture0, "pic.jpg");
        //texInitializeFile(&texture1, "pic2.jpg");
        //texInitializeFile(&texture2, "pic3.png");
        tex[0] = &texture0;

        if (texInitializeFile(&texture0, "pic.jpg")!=0 /*||
            texInitializeFile(tex[1], "pic2.jpg") !=0 ||
            texInitializeFile(tex[2], "pic3.png") !=0*/)
        {
            return 1;
        }

        ren.attrDim = 4; ren.varyDim = 4; 
        ren.unifDim = 12; ren.texNum = 1; 
        ren.colorPixel = colorPixel;
        ren.transformVertex = transformVertex;
        ren.updateUniform = updateUniform;

        //initilize all the renderer values        
        meshInitializeEllipse(&mesh0, 0, 0, 100.0, 100.0, 100);
        meshInitializeRectangle(&mesh1,-50.0,50.0,-50.0,50.0);
        //meshInitializeEllipse(&mesh2, 0.0, 0.0, 50.0, 50.0, 100);

        double unif1[12] ={0.0,256.0,256.0};
        double unif2[12] ={0.0,-100.0,-100.0};
        //double unif3[12] ={0.0,50.0,-50.0};

        sceneInitialize(&scene0,&ren,unif1,tex,&mesh0,NULL,NULL);
        sceneInitialize(&scene1,&ren,unif2,tex,&mesh1,NULL,NULL);
        //sceneInitialize(&scene2,&ren,unif3,tex,&mesh2,NULL,NULL);
        
        sceneSetTexture(&scene1,&ren,0,tex[0]);
        //sceneSetTexture(&scene2,&ren,0,tex[0]);

        sceneAddChild(&scene0,&scene1);
        //sceneAddChild(&scene1,&scene2);
        //meshInitializeRectangle(&mesh,0,256,0,256);
        draw();

        pixSetTimeStepHandler(handleTimeStep);
        pixSetKeyDownHandler(handleKeyDown);
        pixRun();
        texDestroy(tex[0]);
        meshDestroy(&mesh0);
        meshDestroy(&mesh1);
        //meshDestroy(&mesh2);
        sceneDestroyRecursively(&scene0);
        return 0;
    }
}
