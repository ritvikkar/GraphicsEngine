/*
 * 122mainCamera.c
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

#define GLFW_KEY_ENTER 257
#define GLFW_KEY_RIGHT 262
#define GLFW_KEY_LEFT 263
#define GLFW_KEY_DOWN 264
#define GLFW_KEY_UP 265

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
#define renVARYZ 2
#define renVARYS 3
#define renVARYT 4
#define renVARYR 5
#define renVARYG 6
#define renVARYB 7

#define renUNIFRHO 0
#define renUNIFPHI 1
#define renUNIFTHETA 2
#define renUNIFTRANSX 3
#define renUNIFTRANSY 4
#define renUNIFTRANSZ 5
#define renUNIFISOMETRY 6
#define renUNIFVIEWING 22


#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

double camera[2] = {M_PI/2,-1*M_PI/2};

double target[3] = {-100.0, -100.0, -100.0};

double unif[38] =  {0.0,1.0,2.0,
                    200.0,300.0,0.0,     
                    1.0,0.0,0.0,0.0,
                    0.0,1.0,0.0,0.0,
                    0.0,0.0,1.0,0.0,
                    0.0,0.0,0.0,1.0, 
                    0.0,0.0,0.0,0.0,
                    0.0,0.0,0.0,0.0,
                    0.0,0.0,0.0,0.0,
                    0.0,0.0,0.0,0.0};

double unif2[38] = {0.0,0.0,0.0,
                    0.0,0.0,60.0, 
                    1.0,0.0,0.0,0.0,
                    0.0,1.0,0.0,0.0,
                    0.0,0.0,1.0,0.0,
                    0.0,0.0,0.0,1.0, 
                    0.0,0.0,0.0,0.0,
                    0.0,0.0,0.0,0.0,
                    0.0,0.0,0.0,0.0,
                    0.0,0.0,0.0,0.0};


/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], double vary[]) {
    /* For now, just copy attr to varying. Baby steps. */
    double ATTRXYZ1[4] = {attr[renATTRX],attr[renATTRY],attr[renATTRZ],1};
    double RtimesXYZ[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]),ATTRXYZ1,RtimesXYZ);

    double MtimesRXYZ[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFVIEWING]),RtimesXYZ,MtimesRXYZ);

    vary[renVARYX] = MtimesRXYZ[0];
    vary[renVARYY] = MtimesRXYZ[1];
    vary[renVARYZ] = MtimesRXYZ[2];
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

    vec3Spherical(1.0,unif[renUNIFPHI],unif[renUNIFTHETA],u);
    mat33AngleAxisRotation(unif[renUNIFRHO],u,rot);    
    
    mat44Copy(ren->viewing, (double(*)[4])(&unif[renUNIFVIEWING]));
    double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};

    if (unifParent == NULL){
        /* The nine uniforms for storing the matrix start at index 
        renUNIFISOMETRY. So &unif[renUNIFISOMETRY] is an array containing those 
        nine numbers. We use '(double(*)[3])' to cast it to a 3x3 matrix. */
        mat44Isometry(rot, trans, (double(*)[4])(&unif[renUNIFISOMETRY]));
    }

    else {
        double isometry[4][4];
        mat44Isometry(rot, trans, isometry);
        mat444Multiply((double(*)[4])(&unifParent[renUNIFISOMETRY]), isometry,(double(*)[4])(&unif[renUNIFISOMETRY]));
    }
}

/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], double vary[], double rgbz[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);
    rgbz[0] = tex[0]->sample[renTEXR];
    rgbz[1] = tex[0]->sample[renTEXG];
    rgbz[2] = tex[0]->sample[renTEXB];
    //rgbz[3] = depthGetZ(ren->depth, vary[renVARYX], vary[renVARYY]);
    rgbz[3] = depthGetZ(ren->depth, vary[renVARYX], vary[renVARYY]);
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

renRenderer ren;
texTexture *tex[3];
depthBuffer dep;

void handleKeyUp(int key, int shiftIsDown, int controlIsDown, int altOptionIsDown, int superCommandIsDown) {
    if (key == GLFW_KEY_ENTER) {
        if (tex[0]->filtering == texNEAREST) {
            texSetFiltering(tex[0], texQUADRATIC);
        }
        else {
            texSetFiltering(tex[0], texNEAREST);
        }
    } 

    else if (key == GLFW_KEY_UP) {
        if (camera[0] + 0.05 > M_PI) {
            camera[0] = 0.01;
        }
        else {
            camera[0] = camera[0] + 0.05;
        }
    } 

    else if (key == GLFW_KEY_DOWN) {
        if (camera[0] - 0.05 < 0.0) {
            camera[0] = M_PI - 0.05;
        }
        else {
            camera[0] = camera[0] - 0.05;
        }
    } 

    else if (key == GLFW_KEY_LEFT) {
        if (camera[1] - 0.05 < (-1*M_PI)) {
            camera[1] = M_PI - 0.05;
        }
        else {
            camera[1] = camera[1] - 0.05;
        }
    } 

    else if (key == GLFW_KEY_RIGHT) {
        if (camera[1] + 0.05 > M_PI) {
            camera[1] = (-1*M_PI) + 0.05;
        }
        else {
            camera[1] = camera[1] + 0.05;
        }
    }

    printf("key up %d, shift %d, control %d, altOpt %d, supComm %d\n",
    key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
}

void draw(void){
    renUpdateViewing(&ren);
    depthClearZs(&dep,-2000);
    pixClearRGB(0.0,0.0,0.0);      
    
    //sceneRender that will call meshRender
    sceneRender(&scene0,&ren,NULL);
}

void handleTimeStep(double oldTime, double newTime) {
    if (floor(newTime) - floor(oldTime) >= 1.0)
        printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));

    scene0.unif[renUNIFRHO] = scene0.unif[renUNIFRHO] + 0.02;
    renLookAt(&ren, target, 0.0, camera[0], camera[1]);

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
        texInitializeFile(&texture0, "pic2.jpg");
        texInitializeFile(&texture1, "pic1.jpg");
        tex[0] = &texture0;
        tex[1] = &texture1;
        texSetLeftRight(&texture0, texREPEAT);
        texSetTopBottom(&texture0, texREPEAT);

        depthInitialize(&dep,512,512);
        ren.attrDim = 8; ren.varyDim = 5; 
        ren.unifDim = 38; ren.texNum = 1; 
        ren.colorPixel = colorPixel;
        ren.transformVertex = transformVertex;
        ren.updateUniform = updateUniform;
        ren.depth = &dep;
        //initilize all the renderer values 

        meshInitializeSphere(&mesh0, 100, 20, 20);
        meshInitializeSphere(&mesh1, 50, 40, 40);

        sceneInitialize(&scene0,&ren,unif,tex,&mesh0,NULL,NULL);
        sceneInitialize(&scene1,&ren,unif,tex,&mesh1,NULL,NULL);        

        sceneSetTexture(&scene1,&ren,0,&texture1);

        sceneSetUniform(&scene1,&ren,unif2);
        sceneAddChild(&scene0,&scene1);

        renLookAt(&ren, target, 0.0, camera[0], camera[1]);

        draw();
        pixRun();

        texDestroy(tex[0]);
        meshDestroy(&mesh0);
        meshDestroy(&mesh1);
        depthDestroy(&dep);
        sceneDestroyRecursively(&scene0);
        return 0;
    }
}
