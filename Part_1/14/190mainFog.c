/*
 * 190mainFog.c
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
#include "130matrix.c"
#include "110depth.c"
#include "040texture.c"
#include "130renderer.c"

#define GLFW_KEY_ENTER 257
#define GLFW_KEY_RIGHT 262
#define GLFW_KEY_LEFT 263
#define GLFW_KEY_DOWN 264
#define GLFW_KEY_UP 265

#define GLFW_KEY_LIGHT_LEFT 324
#define GLFW_KEY_LIGHT_RIGHT 326
#define GLFW_KEY_LIGHT_UP 328
#define GLFW_KEY_LIGHT_DOWN 325
#define GLFW_KEY_LIGHT_IN 321
#define GLFW_KEY_LIGHT_OUT 323

#define GLFW_KEY_OBJECT_SHINEUP 44
#define GLFW_KEY_OBJECT_SHINEDOWN 46

#define GLFW_KEY_ZOOM_IN 334
#define GLFW_KEY_ZOOM_OUT 333

#define renVARYDIMBOUND 16
#define renVERTNUMBOUND 1000
#define renHUGEDEPTH -10000000.0

#define renATTRX 0
#define renATTRY 1
#define renATTRZ 2
#define renATTRS 3
#define renATTRT 4
#define renATTRN 5
#define renATTRO 6
#define renATTRP 7

#define renVARYX 0
#define renVARYY 1
#define renVARYZ 2
#define renVARYW 3
#define renVARYS 4
#define renVARYT 5
#define renVARYN 6 //world unit normal vector at a vertex
#define renVARYO 7
#define renVARYP 8
#define renVARYWORLDX 9 //world position of a triangle vertex
#define renVARYWORLDY 10
#define renVARYWORLDZ 11

#define renUNIFRHO 0 //angles
#define renUNIFPHI 1
#define renUNIFTHETA 2
#define renUNIFTRANSX 3 //trans XYZ
#define renUNIFTRANSY 4
#define renUNIFTRANSZ 5
#define renUNIFLIGHTX 6 //light XYZ
#define renUNIFLIGHTY 7
#define renUNIFLIGHTZ 8
#define renUNIFLIGHTR 9 //light RGB
#define renUNIFLIGHTG 10
#define renUNIFLIGHTB 11
#define renUNIFSHINE 12  //object shininess factor
#define renUNIFAMBIENCE 13  //ambient light
#define renUNIFFOGR 14
#define renUNIFFOGG 15
#define renUNIFFOGB 16
#define renUNIFWORLDCAMX 17 //camera XYZ
#define renUNIFWORLDCAMY 18
#define renUNIFWORLDCAMZ 19
#define renUNIFREFLECTIONR 20
#define renUNIFREFLECTIONG 21
#define renUNIFREFLECTIONB 22
#define renUNIFISOMETRY 23 //isometry
#define renUNIFVIEWING 39 //viewing

#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

double unif[55] = {0.0,0.0,0.0,         //RHO PHI THETA
                   1.0,0.0,-1.05,       //TRANS X Y Z
                   0.0,0.0,0.0,         //LIGHT X Y Z
                   0.0,0.0,0.0,         //LIGHT R G B
                   0.0,0.0,             //SHINE AND AMBIENCE
                   0.0,0.0,0.0,         //FOG R G B
                   0.0,0.0,0.0,         //CAMERA X Y Z                 
                   1.0,0.0,0.0,0.0,     //ISOMETRY
                   0.0,1.0,0.0,0.0,
                   0.0,0.0,1.0,0.0,
                   0.0,0.0,0.0,1.0,
                   0.0,0.0,0.0,0.0,     //VIEWING
                   0.0,0.0,0.0,0.0,
                   0.0,0.0,0.0,0.0,
                   0.0,0.0,0.0,0.0};

double unif2[55] = {0.0,0.0,0.0,
                    0.0,0.0,0.0,
                    0.0,0.0,0.0,
                    0.0,0.0,0.0,
                    0.0,0.0,
                    0.0,0.0,0.0,
                    0.0,0.0,0.0,
                    1.0,0.0,0.0,0.0,
                    0.0,1.0,0.0,0.0,
                    0.0,0.0,1.0,0.0,
                    0.0,0.0,0.0,1.0,
                    0.0,0.0,0.0,0.0,
                    0.0,0.0,0.0,0.0,
                    0.0,0.0,0.0,0.0,
                    0.0,0.0,0.0,0.0};

double lightSource[3] = {1.0, 2.0, 2.0};
double lightRGB[3] = {1.0, 1.0, 1.0};
double camera[2] = {M_PI/2,0.0};
double zoom = 10;
double viewing[3] = {0.0, 1.0, 0.0};
double ambience = 0.6;
double shine = 4.0;

/* Sets rgb, based on the other parameters, which are unaltered. attr is an 
interpolated attribute vector. can also add effects to the rgbz depending on what
you pass it. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], double vary[], double rgbz[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);

    //diffused lighting
    double l[3] = {unif[renUNIFLIGHTX] - vary[renVARYWORLDX], 
                   unif[renUNIFLIGHTY] - vary[renVARYWORLDY], 
                   unif[renUNIFLIGHTZ] - vary[renVARYWORLDZ]};

    vecUnit(3, l, l);
    double nDotL = vecDot(3, &vary[renVARYN], l);
    double diffInt = fmax(0, nDotL);

    //specular lighting r = 2 ( n - l ) n - l
    vecScale(3, 2 * nDotL, &vary[renVARYN], &unif[renUNIFREFLECTIONR]);
    vecSubtract(3, &unif[renUNIFREFLECTIONR], l, &unif[renUNIFREFLECTIONR]);
    unif[renUNIFSHINE] = vecDot(3, &unif[renUNIFWORLDCAMX],  &unif[renUNIFREFLECTIONR]);
    double specInt = pow(fmax(0,unif[renUNIFSHINE]),1);

    //ambient lighting
    double totalInt = fmax(diffInt, unif[renUNIFAMBIENCE]) + 2 * specInt;

    //final color
    rgbz[0] = tex[0]->sample[renTEXR] * totalInt * unif[renUNIFLIGHTR];
    rgbz[1] = tex[0]->sample[renTEXG] * totalInt * unif[renUNIFLIGHTG];
    rgbz[2] = tex[0]->sample[renTEXB] * totalInt * unif[renUNIFLIGHTB];
    rgbz[3] = depthGetZ(ren->depth, vary[renVARYX], vary[renVARYY]);

    //fog = ( (z+1)/z ) * c + ( 1 - (z+1)/z ) * g
    double z = (rgbz[3] + 1) / z;

    double zg[3];
    vecScale(3,(1-z),&unif[renUNIFFOGR],zg);

    double zc[3];
    vecScale(3,z,rgbz,zc);
    
    double fog[3];
    vecAdd(3,zc,zg,fog);

    vecCopy(3,fog,rgbz);

}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], double vary[]) {
    //world transformations
    double ATTRXYZ1[4] = {attr[renATTRX],attr[renATTRY],attr[renATTRZ],1};
    double RtimesXYZ[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]),ATTRXYZ1,RtimesXYZ);

    double MtimesRXYZ[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFVIEWING]),RtimesXYZ,MtimesRXYZ);

    //light transformations
    double light[4];
    double lightNOP0[4] = {attr[renATTRN], attr[renATTRO], attr[renATTRP], 0.0};
    mat441Multiply((double(*)[4])(&unif[renUNIFISOMETRY]), lightNOP0, light);

    vary[renVARYX] = MtimesRXYZ[0];
    vary[renVARYY] = MtimesRXYZ[1];
    vary[renVARYZ] = MtimesRXYZ[2];
    vary[renVARYW] = MtimesRXYZ[3];

    vary[renVARYWORLDX] = light[0];
    vary[renVARYWORLDY] = light[1];
    vary[renVARYWORLDZ] = light[2];

    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
    vary[renVARYN] = attr[renATTRN];
    vary[renVARYO] = attr[renATTRO];
    vary[renVARYP] = attr[renATTRP];    
}

/* If unifParent is NULL, then sets the uniform matrix to the 
rotation-translation M described by the other uniforms. If unifParent is not 
NULL, but instead contains a rotation-translation P, then sets the uniform 
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    double u[3]; // 3D vector from PHI and THETA
    double rot[3][3]; //Rotational Matrix from RHO
    
    mat44Copy(ren->viewing, (double(*)[4])(&unif[renUNIFVIEWING]));

    vec3Spherical(1.0,unif[renUNIFPHI],unif[renUNIFTHETA],u); //set a axis
    mat33AngleAxisRotation(unif[renUNIFRHO],u,rot);  //rotate around that  
    
    double trans[3] = {unif[renUNIFTRANSX], unif[renUNIFTRANSY], unif[renUNIFTRANSZ]};

    //update the Light Unifs
    vecCopy(3,lightSource,&unif[renUNIFLIGHTX]);
    vecCopy(3,lightRGB,&unif[renUNIFLIGHTR]);

    unif[renUNIFAMBIENCE] = ambience;
    unif[renUNIFSHINE] = shine;

    //update the Camera Unifs
    vecCopy(3, ren->cameraTranslation, &unif[renUNIFWORLDCAMX]);
    //make the camera direction a unit vector    
    vecUnit(3, &unif[renUNIFWORLDCAMX], &unif[renUNIFWORLDCAMX]); 

    //fog
    unif[renUNIFFOGR] = 0.5;
    unif[renUNIFFOGG] = 0.5;
    unif[renUNIFFOGB] = 0.5;

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

#include "110triangle.c"
#include "140clipping.c"
#include "140mesh.c"
#include "090scene.c"

sceneNode scene0; //scene type variable
sceneNode scene1;
sceneNode scene2;

meshMesh mesh0; //mesh type variable
meshMesh mesh1;
meshMesh mesh2;

renRenderer ren;
texTexture *tex[3]; //pointer to textures types
depthBuffer dep;

/* actually gives the draw command */
void draw(void){
    pixClearRGB(0.0,0.0,0.0);
    depthClearZs(&dep,renHUGEDEPTH);
    renLookAt(&ren, viewing, zoom, camera[0], camera[1]);
    renUpdateViewing(&ren);
    sceneRender(&scene0,&ren,NULL);
}

/* handles the key clicks for the program */
void handleKeyUp(int key, int shiftIsDown, int controlIsDown, int altOptionIsDown, int superCommandIsDown) {
    /*if (key == GLFW_KEY_ENTER) {
        if (tex[0]->filtering == texQUADRATIC) {
            texSetFiltering(tex[0], texNEAREST);
        }else {
            texSetFiltering(tex[0], texQUADRATIC);
        }
    }*/ 

    if (key == GLFW_KEY_UP) {
        if (camera[0] + 0.05 > M_PI) {
            camera[0] = 0.01;
        }else{
            camera[0] = camera[0] + 0.05;
        }
    } 

    else if (key == GLFW_KEY_DOWN) {
        if (camera[0] - 0.05 < 0.0) {
            camera[0] = M_PI - 0.05;
        }else{
            camera[0] = camera[0] - 0.05;
        }
    } 

    else if (key == GLFW_KEY_LEFT) {
        if (camera[1] - 0.05 < (-1*M_PI)) {
            camera[1] = M_PI - 0.05;
        }else{
            camera[1] = camera[1] - 0.05;
        }
    } 

    else if (key == GLFW_KEY_RIGHT) {
        if (camera[1] + 0.05 > M_PI) {
            camera[1] = (-1*M_PI) + 0.05;
        }else {
            camera[1] = camera[1] + 0.05;
        }
    }

    else if (key == GLFW_KEY_ZOOM_OUT) {
        zoom = zoom + 1.0;
    } 

    else if (key == GLFW_KEY_ZOOM_IN) {
        if (zoom - 5.0 < 5.0) {
            return;
        } 
        else{
            zoom = zoom - 1.0;
        }
    }

    else if (key == GLFW_KEY_LIGHT_LEFT){
        lightSource[0] = lightSource[0] + 0.5;
    }

    else if (key == GLFW_KEY_LIGHT_RIGHT){
        lightSource[0] = lightSource[0] - 0.5;
    }

    else if (key == GLFW_KEY_LIGHT_UP){
        lightSource[1] = lightSource[1] + 0.5;
    }

    else if (key == GLFW_KEY_LIGHT_DOWN){
        lightSource[1] = lightSource[1] - 0.5;
    }

    else if (key == GLFW_KEY_LIGHT_IN){
        lightSource[2] = lightSource[2] + 0.5;
    }

    else if (key == GLFW_KEY_LIGHT_OUT){
        lightSource[2] = lightSource[2] - 0.5;
    }

    else if (key == GLFW_KEY_OBJECT_SHINEUP){
        shine = shine + 1.0;
    }

    else if (key == GLFW_KEY_OBJECT_SHINEDOWN){
        shine = shine - 1.0;
    }

    draw();

    printf("key up %d, shift %d, control %d, altOpt %d, supComm %d\n",
    key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
}

/* handles the animations with the time difference in frames */
void handleTimeStep(double oldTime, double newTime) {
    if (floor(newTime) - floor(oldTime) >= 1.0)
        printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));

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

        texTexture texture0, texture1, texture2; //actualy texture type
        texInitializeFile(&texture0, "pic.jpg");
        tex[0] = &texture0; //placing the textures in a array of textures
        texSetLeftRight(&texture0, texREPEAT);
        texSetTopBottom(&texture0, texREPEAT);

        depthInitialize(&dep,512,512);
        ren.attrDim = 8; ren.varyDim = 12; 
        ren.unifDim = 55; ren.texNum = 1; 
        ren.colorPixel = colorPixel;
        ren.transformVertex = transformVertex;
        ren.updateUniform = updateUniform;
        ren.depth = &dep;
        //initilize all the renderer values 

        meshInitializeSphere(&mesh0, 1, 20, 20);
        sceneInitialize(&scene0,&ren,unif,tex,&mesh0,NULL,NULL);
        texSetFiltering(tex[0], texQUADRATIC);
        renSetFrustum(&ren, renPERSPECTIVE, M_PI/6.0, 10.0, 10.0);

        draw();
        pixRun();

        texDestroy(tex[0]);
        meshDestroy(&mesh0);
        depthDestroy(&dep);
        sceneDestroyRecursively(&scene0);

        return 0;
    }
}
