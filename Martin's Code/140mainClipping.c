/* Martin Green (greenm2), CS 311 Graphics */
/* February 2017 */

#define KEY_ENTER 257   // Texture Filtering
#define KEY_SPACE 32    // Info Printout

#define KEY_LEFT  263   // Frustum Controls
#define KEY_RIGHT 262
#define KEY_UP    265
#define KEY_DOWN  264

#define KEY_M 77        // Switch Projection

#define KEY_W 87        // Camera Position Controls
#define KEY_A 65
#define KEY_S 83
#define KEY_D 68
#define KEY_Q 81
#define KEY_E 69

#define KEY_I 73        // Camera Angle Controls
#define KEY_J 74
#define KEY_K 75
#define KEY_L 76

#define renVARYDIMBOUND 48
#define renVERTNUMBOUND 1000
#define depthLimit -1.0
#define pi M_PI

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
#define renVARYW 3
#define renVARYS 4
#define renVARYT 5

#define renUNIFR 0
#define renUNIFG 1
#define renUNIFB 2
#define renUNIFA 3  // Angle
#define renUNIFP 4  // Angle Pitch
#define renUNIFW 5  // Angle yaW
#define renUNIFO 6  // Angle rOll
#define renUNIFX 7  // Trans X
#define renUNIFY 8  // Trans Y
#define renUNIFZ 9  // Trans Z
#define renUNIFI 10 // Isometry, 10 - 25
#define renUNIFT 26 // Inverse Isometry, 26 - 41

#define renTEXR 0
#define renTEXG 1
#define renTEXB 2

#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include "000pixel.h"
#include "100vector.c"
#include "130matrix.c"
#include "040texture.c"
#include "110depth.c"
#include "130renderer.c"
#include "110triangle.c"
#include "140clipping.c"
#include "140mesh.c"
#include "090scene.c"

texTexture *tex[3];
renRenderer *ren;
sceneNode *scene;
depthBuffer *depth;
int texFiltering;
int projection;

double target[3] = {0.0, 0.0, 0.0};
double rho = 10.0;
double phi = pi/4;
double theta = pi/-4;

double fov = pi/6.0;
double focal = 10.0;
double ratio = 10.0;


/* Sets rgb, based on the other parameters, which are unaltered. vary is an 
interpolated varried vector from attribute. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], double vary[], double rgbz[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);

    rgbz[0] = tex[0]->sample[renTEXR] * unif[renUNIFR];
    rgbz[1] = tex[0]->sample[renTEXG] * unif[renUNIFG];
    rgbz[2] = tex[0]->sample[renTEXB] * unif[renUNIFB];
    rgbz[3] = vary[renVARYZ];
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], double vary[]) {
    double transfer[4];
    double translateAttr[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 1.0};
    mat441Multiply((double(*)[4])(&unif[renUNIFI]), translateAttr, transfer);

    // Apply Camera & Projection
    double transfer2[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFT]), transfer, transfer2);

    vary[renVARYX] = transfer2[0];
    vary[renVARYY] = transfer2[1];
    vary[renVARYZ] = transfer2[2];
    vary[renVARYW] = transfer2[3];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];
}

/* If unifParent is NULL, then sets the uniform matrix to the 
rotation-translation M described by the other uniforms. If unifParent is not 
NULL, but instead contains a rotation-translation P, then sets the uniform 
matrix to the matrix product P * M. */
void updateUniform(renRenderer *ren, double unif[], double unifParent[]) {
    // Rotation, Pitch - X, Yaw - Y, Roll - Z
    double rotate[3][3];
    double axis[3] = {unif[renUNIFP], unif[renUNIFW], unif[renUNIFO]};
    vecUnit(3, axis, axis);
    mat33AngleAxisRotation(unif[renUNIFA], axis, rotate);

    // Translation
    double translate[3] = {unif[renUNIFX], unif[renUNIFY], unif[renUNIFZ]};

    // Camera & Projection
    int i;
    for (i = 0; i < 4; i += 1)
        vecCopy(4, ren->viewing[i], &unif[renUNIFT + (i*4)]);

    // Application
    if (unifParent == NULL) {
        mat44Isometry(rotate, translate, (double(*)[4])(&unif[renUNIFI]));
    } else {
        double m[4][4];
        mat44Isometry(rotate, translate, m);
        mat444Multiply((double(*)[4])(&unifParent[renUNIFI]), m, (double(*)[4])(&unif[renUNIFI]));
    }
}

void setOneUniform(sceneNode *node, int index, double unifData) {
    node->unif[index] = unifData;
}

void draw() {
    pixClearRGB(0.0, 0.0, 0.0);
    depthClearZs(ren->depth, depthLimit);
    renUpdateViewing(ren);
    sceneRender(scene, ren, NULL);
}

void handleTimeStep(double oldTime, double newTime) {
    // if (floor(newTime) - floor(oldTime) >= 1.0) {
    //     printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
    // }
}

void handleKeyDown(int key, int shift, int control, int alt, int command) {
    switch (key) {
        // Texture Filtering
        case KEY_ENTER:
            if (texFiltering == texNEAREST) {
                int i;
                for (i = 0; i < ren->texNum; i += 1)
                    texSetFiltering(tex[i], texQUADRATIC);
                texFiltering = texQUADRATIC;
            } else {
                int i;
                for (i = 0; i < ren->texNum; i += 1)
                    texSetFiltering(tex[i], texNEAREST);
                texFiltering = texNEAREST;
            }
            draw();
            break;

        // Projection
        case KEY_M:
            if (projection == renORTHOGRAPHIC) {
                projection = renPERSPECTIVE;
                renSetFrustum(ren, projection, fov, focal, ratio);
            } else {
                projection = renORTHOGRAPHIC;
                renSetFrustum(ren, projection, fov, focal, ratio);
            }
            draw();
            break;
        case KEY_LEFT:
            focal += 1.0;
            renSetFrustum(ren, projection, fov, focal, ratio);
            draw();
            break;
        case KEY_RIGHT:
            focal -= 1.0;
            renSetFrustum(ren, projection, fov, focal, ratio);
            draw();
            break;
        case KEY_UP:
            fov += pi/8;
            renSetFrustum(ren, projection, fov, focal, ratio);
            draw();
            break;
        case KEY_DOWN:
            fov -= pi/8;
            renSetFrustum(ren, projection, fov, focal, ratio);
            draw();
            break;

        // Camera Target
        case KEY_D:
            target[0] += 0.05;
            renLookAt(ren, target, rho, phi, theta);
            draw();
            break;
        case KEY_A:
            target[0] -= 0.05;
            renLookAt(ren, target, rho, phi, theta);
            draw();
            break;
        case KEY_W:
            target[1] += 0.05;
            renLookAt(ren, target, rho, phi, theta);
            draw();
            break;
        case KEY_S:
            target[1] -= 0.05;
            renLookAt(ren, target, rho, phi, theta);
            draw();
            break;
        case KEY_E:
            rho += 0.05;
            renLookAt(ren, target, rho, phi, theta);
            draw();
            break;
        case KEY_Q:
            rho -= 0.05;
            renLookAt(ren, target, rho, phi, theta);
            draw();
            break;

        // Camera Angle
        case KEY_K:
            phi += pi/64;
            renLookAt(ren, target, rho, phi, theta);
            draw();
            break;
        case KEY_I:
            phi -= pi/64;
            renLookAt(ren, target, rho, phi, theta);
            draw();
            break;
        case KEY_L:
            theta += pi/64;
            renLookAt(ren, target, rho, phi, theta);
            draw();
            break;
        case KEY_J:
            theta -= pi/64;
            renLookAt(ren, target, rho, phi, theta);
            draw();
            break;
        case KEY_SPACE:
            printf("Camera target: %g %g %g\n", target[0], target[1], target[2]);
            printf("Angle: phi %g (pi/%g), theta %g (pi/%g), rho %g\n", phi, pi/phi, theta, pi/theta, rho);
            printf("Projection %d, FOV %g (pi/%g), focal distance %g\n", projection, fov, pi/fov, focal);
            break;
        default:
            break;
    }
}

int main() {
    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
        return 1;
    else {
        pixSetTimeStepHandler(handleTimeStep);
        pixSetKeyDownHandler(handleKeyDown);
        pixClearRGB(0.0, 0.0, 0.0);

        double unif1[renVARYDIMBOUND] = {1.0, 1.0, 1.0,         // RGB
                                         pi/2, 1.0, 0.0, 0.0,   // A, P, W, O
                                         0.0, 0.0, 0.0,         // X, Y, Z

                                         1.0, 0.0, 0.0, 0.0,    // Isometry
                                         0.0, 1.0, 0.0, 0.0,
                                         0.0, 0.0, 1.0, 0.0,
                                         0.0, 0.0, 0.0, 1.0,

                                         1.0, 0.0, 0.0, 0.0,    // Inverse Isometry
                                         0.0, 1.0, 0.0, 0.0,
                                         0.0, 0.0, 1.0, 0.0,
                                         0.0, 0.0, 0.0, 1.0};


        // Upper Ball
        double unif2[renVARYDIMBOUND] = {1.0, 1.0, 1.0,
                                         -pi/2, 0.0, 0.0, 1.0,
                                         1.0, 1.0, -1.0,

                                         1.0, 0.0, 0.0, 0.0,
                                         0.0, 1.0, 0.0, 0.0,
                                         0.0, 0.0, 1.0, 0.0,
                                         0.0, 0.0, 0.0, 1.0,

                                         1.0, 0.0, 0.0, 0.0,
                                         0.0, 1.0, 0.0, 0.0,
                                         0.0, 0.0, 1.0, 0.0,
                                         0.0, 0.0, 0.0, 1.0};

        // Back Wall
        double unif3[renVARYDIMBOUND] = {1.0, 1.0, 1.0,
                                         0.0, 1.0, 0.0, 0.0,
                                         0.0, 0.0, -5.0,

                                         1.0, 0.0, 0.0, 0.0,
                                         0.0, 1.0, 0.0, 0.0,
                                         0.0, 0.0, 1.0, 0.0,
                                         0.0, 0.0, 0.0, 1.0,

                                         1.0, 0.0, 0.0, 0.0,
                                         0.0, 1.0, 0.0, 0.0,
                                         0.0, 0.0, 1.0, 0.0,
                                         0.0, 0.0, 0.0, 1.0};

        texTexture texture1;
        texTexture texture2;
        texTexture texture3;

        tex[0] = &texture1;
        tex[1] = &texture2;
        tex[2] = &texture3;

        meshMesh mesh1;
        meshMesh mesh2;
        meshMesh mesh3;

        sceneNode node1;
        sceneNode node2;
        sceneNode node3;
        scene = &node1;

        depthBuffer dBuffer;
        depth = &dBuffer;

        renRenderer renderer;
        ren = &renderer;

        ren->unifDim = 42;
        ren->texNum = 3;
        ren->attrDim = 8;
        ren->varyDim = 6;
        ren->depth = depth;
        ren->transformVertex = transformVertex;
        ren->colorPixel = colorPixel;
        ren->updateUniform = updateUniform;

        int error = 0;

        error += texInitializeFile(tex[0], "Kitten.jpg");
        error += texInitializeFile(tex[1], "360.jpg");
        error += texInitializeFile(tex[2], "Rainbow.jpg");

        error += meshInitializeBox(&mesh1, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
        error += meshInitializeSphere(&mesh2, 0.7, 10, 20);
        error += meshInitializeBox(&mesh3, -5.0, 5.0, -2.0, 2.0, 0.0, 1.0);

        error += sceneInitialize(&node1, ren, unif1, &tex[0], &mesh1, &node2, NULL);
        error += sceneInitialize(&node2, ren, unif2, &tex[1], &mesh2, NULL, NULL);
        error += sceneInitialize(&node3, ren, unif3, &tex[2], &mesh3, NULL, NULL);

        sceneAddChild(&node1, &node3);

        error += depthInitialize(depth, 512, 512);

        if (error != 0) {
            printf("Error creating scene.\n");
            return 1;
        }

        projection = renPERSPECTIVE;
        renSetFrustum(ren, projection, fov, focal, ratio);

        texFiltering = texQUADRATIC;
        texSetFiltering(tex[0], texFiltering);
        texSetFiltering(tex[1], texFiltering);
        texSetFiltering(tex[2], texFiltering);
        
        renLookAt(ren, target, rho, phi, theta);
        draw();
        pixRun();

        meshDestroy(&mesh1);
        meshDestroy(&mesh2);
        meshDestroy(&mesh3);

        texDestroy(tex[0]);
        texDestroy(tex[1]);
        texDestroy(tex[2]);

        depthDestroy(depth);

        sceneDestroyRecursively(scene);
        return 0;
    }
}
