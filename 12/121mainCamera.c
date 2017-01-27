/*
 * 120mainCamera.c
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
#include "110depth.c"
#include "040texture.c"
#include "120renderer.c"

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
#define renVARYZ 2
#define renVARYS 3
#define renVARYT 4
#define renVARYR 5
#define renVARYG 6
#define renVARYB 7

#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renUNIFTRANSX 3
#define renUNIFTRANSY 4
#define renUNIFTRANSZ 5
#define renUNIFTHETA 6
#define renUNIFISOMETRY 7

#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

renRenderer ren;
texTexture *tex[1];
depthBuffer dep;

double unif[39] = {1.0,1.0,1.0,
                   0.0,0.0,0.0,
                   0.0, 
                   1.0,0.0,0.0,0.0, //ISOMETRY
                   0.0,1.0,0.0,0.0,
                   0.0,0.0,1.0,0.0,
                   0.0,0.0,0.0,1.0,
                   1.0,0.0,0.0,0.0, //CAMERA
                   0.0,1.0,0.0,0.0,
                   0.0,0.0,1.0,0.0,
                   0.0,0.0,0.0,1.0};

double unif1[39] = {0.0,0.0,0.0,
                    250.0,250.0,0.0,
                    0.0,  
                    1.0,0.0,0.0,0.0, //ISOMETRY
                    0.0,1.0,0.0,0.0,
                    0.0,0.0,1.0,0.0,
                    0.0,0.0,0.0,1.0,
                    1.0,0.0,0.0,0.0, //CAMERA
                    0.0,1.0,0.0,0.0,
                    0.0,0.0,1.0,0.0,
                    0.0,0.0,0.0,1.0};

/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
        double vary[], double rgbz[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);
    rgbz[0] = tex[0]->sample[renTEXR];
    rgbz[1] = tex[0]->sample[renTEXG];
    rgbz[2] = tex[0]->sample[renTEXB];
    rgbz[3] = depthGetZ(ren->depth, vary[renVARYX], vary[renVARYY]);
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[],
        double vary[]) {
    /* For now, just copy attr to varying. Baby steps. */
    double xyz1[4] = {attr[renATTRX],attr[renATTRY],attr[renATTRZ],1};
    double RtimesXYZ[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]),xyz1,RtimesXYZ);
    vary[renVARYX] = RtimesXYZ[0];
    vary[renVARYY] = RtimesXYZ[1];
    vary[renVARYZ] = RtimesXYZ[2];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}

/* If unifParent is NULL, then sets the uniform matrix to the 
rotation-translation M described by the other uniforms. If unifParent is not 
NULL, but instead contains a rotation-translation P, then sets the uniform 
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    double rot[3][3];
    int i,j;
    for(i = 0; i < 4; i++){
        for (j = 0; j < 4; j++)
        {
            unif[23+i*4+j] = ren->viewing[i][j];
        }
    }
    double temp[3]={1,1,1};
    double u[3];
    vecUnit(3,temp,u);
    //vec3Spherical(1.0,unif[renUNIFPHI],unif[renUNIFTHETA],u);
    mat33AngleAxisRotation(unif[renUNIFTHETA],u,rot);

    double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};

    if (unifParent == NULL){
        /* The nine uniforms for storing the matrix start at index 
        renUNIFISOMETRY. So &unif[renUNIFISOMETRY] is an array containing those 
        nine numbers. We use '(double(*)[4])' to cast it to a 4x4 matrix. */
        mat44Isometry(rot, trans, (double(*)[4])(&unif[renUNIFISOMETRY]));
        //renUpdateViewing(ren)
    }

    else {
        double isometry[4][4];
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

void draw(void){
    depthClearZs(&dep,-1000);
    renUpdateViewing(&ren);
    pixClearRGB(0.0,0.0,0.0);      
    //sceneRender that will call meshRender
    sceneRender(&scene0,&ren,NULL);
}

void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
    int altOptionIsDown, int superCommandIsDown) {
    if (key == GLFW_KEY_ENTER) {
        if (tex[0]->filtering == texQUADRATIC) {
            texSetFiltering(tex[0], texNEAREST);
        }else {
            texSetFiltering(tex[0], texQUADRATIC);
        }
    }

    printf("key up %d, shift %d, control %d, altOpt %d, supComm %d\n",
    key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);

}

void handleTimeStep(double oldTime, double newTime) {
    if (floor(newTime) - floor(oldTime) >= 1.0)
        printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));

    scene0.unif[renUNIFTHETA] = scene0.unif[renUNIFTHETA] + (newTime - oldTime);
    draw();
}

int main(void) {
    /* Make a 512 x 512 window with the title 'Pixel Graphics'. This function 
    returns 0 if no error occurred. */ 
    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
        return 1;
    
    else {
        pixSetTimeStepHandler(handleTimeStep);
        pixSetKeyUpHandler(handleKeyUp);

        texTexture texture0, texture1, texture2;
        texInitializeFile(&texture0, "pic.jpg");
        tex[0] = &texture0;

        if (texInitializeFile(&texture0, "pic.jpg")!=0)
        {
            return 1;
        }

        //initilize all the renderer values        
        ren.attrDim = 8; ren.varyDim = 4; 
        ren.unifDim = 39; ren.texNum = 1; 
        ren.colorPixel = colorPixel;
        ren.transformVertex = transformVertex;
        ren.updateUniform = updateUniform;
        ren.depth = &dep;
        

        double unif2[39] = {0.0,0.0,0.0,
                            250.0,250.0,0.0,
                            0.0,  
                            1.0,0.0,0.0,0.0, //ISOMETRY
                            0.0,1.0,0.0,0.0,
                            0.0,0.0,1.0,0.0,
                            0.0,0.0,0.0,1.0,
                            1.0,0.0,0.0,0.0, //CAMERA
                            0.0,1.0,0.0,0.0,
                            0.0,0.0,1.0,0.0,
                            0.0,0.0,0.0,1.0};

        depthInitialize(&dep,512,512);

        meshInitializeBox(&mesh0, 150.0, 250.0, 150.0, 250.0, 150.0, 250.0);
        //meshInitializeSphere(&mesh1, 30, 20, 20);
        sceneInitialize(&scene0,&ren,unif,tex,&mesh0,NULL,NULL);
        //sceneInitialize(&scene1,&ren,unif,tex,&mesh1,NULL,NULL);

        //double unif1[22] ={0.0,256.0,256.0};
        
        sceneSetUniform(&scene0,&ren,unif2);
        //sceneAddChild(&scene0,&scene1);

        sceneSetTexture(&scene0,&ren,0,tex[0]);

        draw();
        pixRun();
        texDestroy(tex[0]);
        meshDestroy(&mesh0);
        //meshDestroy(&mesh1);
        //meshDestroy(&mesh2);
        sceneDestroyRecursively(&scene0);
        return 0;
    }
}