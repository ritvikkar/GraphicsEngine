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
#define renATTRZ 2
#define renATTRS 3
#define renATTRT 4
#define renATTRR 5
#define renATTRG 6
#define renATTRB 7

#define renVARYX 0
#define renVARYY 1
#define renVARYS 2
#define renVARYT 3
#define renVARYR 4
#define renVARYG 5
#define renVARYB 6

#define renUNIFRHO 0
#define renUNIFPHI 1
#define renUNIFTHETA 2
#define renUNIFTRANSX 3
#define renUNIFTRANSY 4
#define renUNIFTRANSZ 5
#define renUNIFISOMETRY 6

#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

renRenderer *ren;

texTexture *tex[3];
double unif[3+3+1+16] = 
                {1.0,1.0,1.0,    //RHO PHI THETA
                 0.0,0.0,0.0,    //XYZ
                 0.0,            //ISOMETRY
                 1.0,0.0,0.0,0.0,//4x4 rotational matrix
                 0.0,1.0,0.0,0.0,
                 0.0,0.0,1.0,0.0,
                 0.0,0.0,0.0,1.0};

/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double vary[], double rgb[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);
    rgb[0] = tex[0]->sample[renTEXR] * vary[renVARYR];
    rgb[1] = tex[0]->sample[renTEXG] * vary[renVARYG];
    rgb[2] = tex[0]->sample[renTEXB] * vary[renVARYB];
}
/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], 
        double vary[]) {
    double attrXYZ[4] = {attr[renATTRX],attr[renATTRY],attr[renATTRZ],1};
    double RXYZ[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]),attrXYZ,RXYZ);
    vary[renVARYX] = RXYZ[0];
    vary[renVARYY] = RXYZ[1];
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

#include "090triangle.c"
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
    sceneRender(&scene2,ren,NULL);

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
    unif[0] = unif[0] + 0.1;
    unif[1] = 256;
    unif[2] = 256;

    sceneSetUniform(&scene2,ren,unif);
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
        //texInitializeFile(tex[1], "pic2.jpg");
        //texInitializeFile(tex[2], "pic3.png");

        if (texInitializeFile(tex[0], "pic1.jpg") !=0 /*||
            texInitializeFile(tex[1], "pic2.jpg") !=0 ||
            texInitializeFile(tex[2], "pic3.png") !=0*/)
        {
            return 1;
        }

        ren->attrDim = 4; ren->varyDim = 4; 
        ren->unifDim = 12; ren->texNum = 1; 
        ren->colorPixel = colorPixel;
        ren->transformVertex = transformVertex;
        ren->updateUniform = updateUniform;

        //initilize all the renderer values
        meshInitializeRectangle(&mesh0,0.0,512.0,0.0,512.0);
        meshInitializeEllipse(&mesh1, 300.0, 300.0, 30.0, 30.0, 50);
        meshInitializeEllipse(&mesh2, 0.0, 0.0, 50.0, 50.0, 50);

        sceneInitialize(&scene0,ren,unif,tex,&mesh0,NULL,NULL);
        sceneInitialize(&scene1,ren,unif,tex,&mesh1,NULL,NULL);
        sceneInitialize(&scene2,ren,unif,tex,&mesh2,NULL,NULL);
        
        sceneSetTexture(&scene1,ren,0,tex[0]);
        sceneSetTexture(&scene2,ren,0,tex[0]);

        sceneAddChild(&scene0,&scene1);
        sceneAddChild(&scene1,&scene2);
        //meshInitializeRectangle(&mesh,0,256,0,256);
        draw();

        pixSetTimeStepHandler(handleTimeStep);
        pixSetKeyDownHandler(handleKeyDown);
        pixRun();
        texDestroy(tex[0]);
        meshDestroy(&mesh0);
        meshDestroy(&mesh1);
        meshDestroy(&mesh2);
        sceneDestroyRecursively(&scene0);
        return 0;
    }
}
