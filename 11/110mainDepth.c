/*
 * 100main3D.c
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
#include "110depth.c"
#include "110renderer.c"

#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 1000

#define renATTRX 0
#define renATTRY 1
#define renATTRZ 3
#define renATTRS 4
#define renATTRT 5
#define renATTRN 6
#define renATTRO 7
#define renATTRP 8

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

renRenderer ren;

texTexture *tex[1];
double unif[3+3+16] = {0.1,0.1,0.1,     //RHO PHI THETA
                       256.0,256.0,0.0, //XYZ
                       1.0,0.0,0.0,0.0, //ISOMETRY [4x4 Matrix]
                       0.0,1.0,0.0,0.0,
                       0.0,0.0,1.0,0.0,
                       0.0,0.0,0.0,1.0};

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
    /* For now, just copy attr to varying. Baby steps. */
    double attrXYZ[4] = {attr[renATTRX],attr[renATTRY],attr[renATTRZ],1};
    double RtimexXYZ[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]),attrXYZ,RtimexXYZ);
    vary[renVARYX] = RtimexXYZ[0];
    vary[renVARYY] = RtimexXYZ[1];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}

/* If unifParent is NULL, then sets the uniform matrix to the 
rotation-translation M described by the other uniforms. If unifParent is not 
NULL, but instead contains a rotation-translation P, then sets the uniform 
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    double u[3];
    double rot[3][3];

    vec3Spherical(unif[renUNIFRHO],unif[renUNIFPHI],unif[renUNIFTHETA],u);
    mat33AngleAxisRotation(unif[renUNIFRHO],u,rot);

    if (unifParent == NULL){
        /* The nine uniforms for storing the matrix start at index 
        renUNIFISOMETRY. So &unif[renUNIFISOMETRY] is an array containing those 
        nine numbers. We use '(double(*)[3])' to cast it to a 3x3 matrix. */
        double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};
        mat44Isometry(rot, trans, (double(*)[4])(&unif[renUNIFISOMETRY]));
    }

    else {
        double isometry[4][4];
        double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};
        mat44Isometry(rot, trans, isometry);
        mat444Multiply((double(*)[4])(&unifParent[renUNIFISOMETRY]), isometry,
        (double(*)[4])(&unif[renUNIFISOMETRY]));
    }
}

#include "110triangle.c"
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

        ren.attrDim = 8; ren.varyDim = 8; 
        ren.unifDim = 22; ren.texNum = 1; 
        ren.colorPixel = colorPixel;
        ren.transformVertex = transformVertex;
        ren.updateUniform = updateUniform;
        //initilize all the renderer values        
        meshInitializeBox(&mesh0, -50.0,50.0,-50.0,50.0,-50,50);
        //meshInitializeRectangle(&mesh1,-50.0,50.0,-50.0,50.0);
        //meshInitializeEllipse(&mesh2, 0.0, 0.0, 50.0, 50.0, 100);

        double unif1[12] ={0.0,256.0,256.0};
        //double unif2[12] ={0.0,-100.0,-100.0};
        //double unif3[12] ={0.0,50.0,-50.0};

        sceneInitialize(&scene0,&ren,unif1,tex,&mesh0,NULL,NULL);
        //sceneInitialize(&scene1,&ren,unif2,tex,&mesh1,NULL,NULL);
        //sceneInitialize(&scene2,&ren,unif3,tex,&mesh2,NULL,NULL);

        sceneSetTexture(&scene0,&ren,0,tex[0]);
        //sceneSetTexture(&scene2,&ren,0,tex[0]);
        //sceneAddChild(&scene0,&scene1);
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
