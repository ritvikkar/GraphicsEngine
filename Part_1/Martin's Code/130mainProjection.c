/* Martin Green (greenm2), CS 311 Graphics */
/* January 2017 */

#define KEY_ENTER 257
#define KEY_SPACE 32

#define KEY_LEFT 263
#define KEY_RIGHT 262
#define KEY_UP 265
#define KEY_DOWN 264

#define KEY_M 77

#define KEY_W 87
#define KEY_A 65
#define KEY_S 83
#define KEY_D 68
#define KEY_Q 81
#define KEY_E 69

#define KEY_I 73
#define KEY_J 74
#define KEY_K 75
#define KEY_L 76

#define renVARYDIMBOUND 48
#define renVERTNUMBOUND 1000
#define depthLimit -10000.0
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
#define renVARYS 3
#define renVARYT 4
#define renVARYR 5
#define renVARYG 6
#define renVARYB 7

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
#include "100mesh.c"
#include "090scene.c"

texTexture *tex[3];
renRenderer *ren;
sceneNode *scene;
depthBuffer *depth;
int texFiltering;
int projection;

double position[3] = {0.0, 0.0, 0.0};
double phi = pi;
double theta = pi/2;
double angle = 0.0;

double fov = pi/6.0;
double focal = 10.0;
double ratio = 10.0;


/* Sets rgb, based on the other parameters, which are unaltered. vary is an 
interpolated varried vector from attribute. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], 
                double vary[], double rgbz[]) {

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

    

    // Camera
    double transfer2[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFT]), transfer, transfer2);

    // Viewport
    // printf("transfer2 %g %g %g %g\n", transfer2[0], transfer2[1], transfer2[2], transfer2[3]);
    vecScale(4, 1 / transfer2[3], transfer2, transfer2);
    double transfer3[4];
    mat441Multiply(ren->viewport, transfer2, transfer3);
    // vecCopy(4, transfer2, transfer3);

    // printf("transfer2 %g %g %g %g\n", transfer2[0], transfer2[1], transfer2[2], transfer2[3]);
    // printf("transfer3 %g %g %g %g\n", transfer3[0], transfer3[1], transfer3[2], transfer3[3]);

    // printf("unifT looks like this:\n");
    // int i = 0, j;
    // for (j = 0; j < 4; j += 1) {
    //     int lim = i;
    //     for (i = i; i < lim + 4; i += 1) {
    //         printf("%f\t", unif[renUNIFT + i]);
    //     }
    //     printf("\n");
    // }



    vary[renVARYX] = transfer3[0];
    vary[renVARYY] = transfer3[1];
    vary[renVARYZ] = transfer3[2];
    vary[renVARYS] = attr[renATTRS];
    vary[renVARYT] = attr[renATTRT];

    // printf("transformVertex %g %g\n", vary[renVARYX], vary[renVARYY]);
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
    for (i = 0; i < 16; i += 1)
        unif[renUNIFT+i] = ren->viewing[i];

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

    // angle -= 0.05;
    // setOneUniform(scene, renUNIFA, angle);
    // draw();

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

        // Camera
        case KEY_D:
            position[0] += 0.05;
            renLookFrom(ren, position, phi, theta);
            draw();
            break;
        case KEY_A:
            position[0] -= 0.05;
            renLookFrom(ren, position, phi, theta);
            draw();
            break;
        case KEY_W:
            position[1] += 0.05;
            renLookFrom(ren, position, phi, theta);
            draw();
            break;
        case KEY_S:
            position[1] -= 0.05;
            renLookFrom(ren, position, phi, theta);
            draw();
            break;
        case KEY_E:
            position[2] += 0.05;
            renLookFrom(ren, position, phi, theta);
            draw();
            break;
        case KEY_Q:
            position[2] -= 0.05;
            renLookFrom(ren, position, phi, theta);
            draw();
            break;
        case KEY_I:
            phi += pi/64;
            renLookFrom(ren, position, phi, theta);
            draw();
            break;
        case KEY_K:
            phi -= pi/64;
            renLookFrom(ren, position, phi, theta);
            draw();
            break;
        case KEY_L:
            theta += pi/64;
            renLookFrom(ren, position, phi, theta);
            draw();
            break;
        case KEY_J:
            theta -= pi/64;
            renLookFrom(ren, position, phi, theta);
            draw();
            break;
        case KEY_SPACE:
            printf("Camera position: %g %g %g\n", position[0], position[1], position[2]);
            printf("Angle: phi %g (pi/%g), theta %g (pi/%g)\n", phi, pi/phi, theta, pi/theta);
            printf("Projection %d, FOV %g (pi/%g), focal distance %g\n", 
                projection, fov, pi/fov, focal);
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
                                         0.0, 1.0, 0.0, 0.0,    // A, P, W, O
                                         0.0, 0.0, -10.0,         // X, Y, Z

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
                                         0.8, 0.0, -20.0,

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
                                         -100.0, 0.0, -500.0,

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
        ren->varyDim = 5;
        ren->depth = depth;
        ren->transformVertex = transformVertex;
        ren->colorPixel = colorPixel;
        ren->updateUniform = updateUniform;

        int error = 0;

        error += texInitializeFile(tex[0], "Puppy.jpeg");
        error += texInitializeFile(tex[1], "360.jpg");
        error += texInitializeFile(tex[2], "Rainbow.jpg");


        error += meshInitializeBox(&mesh1, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
        // error += meshInitializeBox(&mesh1, 0, 0.2, 0, 0.2, 0, 0.2);
        error += meshInitializeSphere(&mesh2, 1.5, 10, 20);
        // error += meshInitializeBox(&mesh3, -500.0, 500.0, -200.0, 200.0, 0.0, 100.0);

        error += sceneInitialize(&node1, ren, unif1, &tex[0], &mesh1, NULL, &node2);
        // error += sceneInitialize(&node1, ren, unif1, &tex[0], &mesh1, &node2, &node3);
        error += sceneInitialize(&node2, ren, unif2, &tex[1], &mesh2, NULL, NULL);
        // error += sceneInitialize(&node3, ren, unif3, &tex[2], &mesh3, NULL, NULL);

        error += depthInitialize(depth, 512, 512);

        if (error != 0) {
            printf("Error creating scene.\n");
            return 1;
        }

        // renSetFrustum(ren, renORTHOGRAPHIC, pi/6.0, 10.0, 10.0);
        int projection = renORTHOGRAPHIC;
        renSetFrustum(ren, projection, fov, focal, ratio);
        // renSetFrustum(ren, renPERSPECTIVE, pi/6.0, 10.0, 10.0);

        texSetFiltering(tex[0], texQUADRATIC);
        texSetFiltering(tex[1], texQUADRATIC);
        texSetFiltering(tex[2], texQUADRATIC);
        texFiltering = texQUADRATIC;

        renLookFrom(ren, position, phi, theta);
        draw();
        pixRun();

        meshDestroy(&mesh1);
        // meshDestroy(&mesh2);
        // meshDestroy(&mesh3);

        texDestroy(tex[0]);
        texDestroy(tex[1]);
        texDestroy(tex[2]);

        depthDestroy(depth);

        sceneDestroyRecursively(scene);
        return 0;
    }
}
