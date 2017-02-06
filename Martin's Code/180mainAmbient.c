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

#define KEY_T 84        // Light Position Controls
#define KEY_F 70
#define KEY_G 71
#define KEY_H 72
#define KEY_R 82
#define KEY_Y 89

#define KEY_U 85        // Shininess
#define KEY_O 79

#define renVARYDIMBOUND 60
#define renVERTNUMBOUND 1000
#define depthLimit -1.0
#define pi M_PI

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
#define renVARYN 6   // Normal Vector
#define renVARYO 7
#define renVARYP 8
#define renVARYWX 9  // World
#define renVARYWY 10
#define renVARYWZ 11

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
#define renUNIFLX 10  // Light X
#define renUNIFLY 11  // Light Y
#define renUNIFLZ 12  // Light Z
#define renUNIFLR 13  // Light R
#define renUNIFLG 14  // Light G
#define renUNIFLB 15  // Light B
#define renUNIFLA 16  // Ambient light
#define renUNIFOS 17  // Object shininess
#define renUNIFCX 18  // Camera X
#define renUNIFCY 19  // Camera Y
#define renUNIFCZ 20  // Camera Z
#define renUNIFI  21  // Isometry, 21 - 36
#define renUNIFT  37  // Inverse Isometry, 37 - 52

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
double phi = pi/2;
double theta = pi/2.56;

double fov = pi/6.0;
double focal = 10.0;
double ratio = 10.0;

double lightPos[3] = {2.5, 2.0, 2.0};
double lightRGB[3] = {1.0, 1.0, 1.0};
double ambientLight = 0.4;
double shininess = 25.0;


/* Sets rgb, based on the other parameters, which are unaltered. vary is an 
interpolated varried vector from attribute. */
void colorPixel(renRenderer *ren, double unif[], texTexture *tex[], double vary[], double rgbz[]) {
    texSample(tex[0], vary[renVARYS], vary[renVARYT]);

    // Diffuse lighting
    double lightDir[3] = {unif[renUNIFLX] - vary[renVARYWX], 
                          unif[renUNIFLY] - vary[renVARYWY], 
                          unif[renUNIFLZ] - vary[renVARYWZ]};

    vecUnit(3, lightDir, lightDir);
    double lightDot = vecDot(3, &vary[renVARYN], lightDir);
    double diffIntensity = fmax(0, lightDot);

    // Specular lighting
    double reflect[3];
    vecScale(3, 2 * lightDot, &vary[renVARYN], reflect);
    vecSubtract(3, reflect, lightDir, reflect);
    double specDot = vecDot(3, &unif[renUNIFCX], reflect);

    double specIntensity = pow(fmax(0, specDot), unif[renUNIFOS]);
    double totalIntensity = fmax(diffIntensity, unif[renUNIFLA]) + 2 * specIntensity;

    rgbz[0] = tex[0]->sample[renTEXR] * unif[renUNIFR] * totalIntensity * unif[renUNIFLR];
    rgbz[1] = tex[0]->sample[renTEXG] * unif[renUNIFG] * totalIntensity * unif[renUNIFLG];
    rgbz[2] = tex[0]->sample[renTEXB] * unif[renUNIFB] * totalIntensity * unif[renUNIFLB];
    rgbz[3] = vary[renVARYZ];
}

/* Writes the vary vector, based on the other parameters. */
void transformVertex(renRenderer *ren, double unif[], double attr[], double vary[]) {
    // World movement
    double transfer[4];
    double translateAttr[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 1.0};
    mat441Multiply((double(*)[4])(&unif[renUNIFI]), translateAttr, transfer);

    // Lighting angle
    double lightTransfer[4];
    double lightRotation[4] = {attr[renATTRX], attr[renATTRY], attr[renATTRZ], 0.0};
    mat441Multiply((double(*)[4])(&unif[renUNIFI]), lightRotation, lightTransfer);

    // Apply Camera & Projection
    double transfer2[4];
    mat441Multiply((double(*)[4])(&unif[renUNIFT]), transfer, transfer2);

    vary[renVARYX] = transfer2[0];
    vary[renVARYY] = transfer2[1];
    vary[renVARYZ] = transfer2[2];
    vary[renVARYW] = transfer2[3];
    vary[renVARYWX] = lightTransfer[0];
    vary[renVARYWY] = lightTransfer[1];
    vary[renVARYWZ] = lightTransfer[2];
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
    // Rotation, Pitch - X, Yaw - Y, Roll - Z
    double rotate[3][3];
    double axis[3] = {unif[renUNIFP], unif[renUNIFW], unif[renUNIFO]};
    vecUnit(3, axis, axis);
    mat33AngleAxisRotation(unif[renUNIFA], axis, rotate);

    // Translation
    double translate[3] = {unif[renUNIFX], unif[renUNIFY], unif[renUNIFZ]};

    // Light
    vecCopy(3, lightPos, &unif[renUNIFLX]);
    vecCopy(3, lightRGB, &unif[renUNIFLR]);
    unif[renUNIFLA] = ambientLight;
    unif[renUNIFOS] = shininess;

    // Camera vector
    vecCopy(3, ren->cameraTranslation, &unif[renUNIFCX]);
    vecUnit(3, &unif[renUNIFCX], &unif[renUNIFCX]);

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
        mat444Multiply(
            (double(*)[4])(&unifParent[renUNIFI]), m, (double(*)[4])(&unif[renUNIFI]));
    }
}

void setOneUniform(sceneNode *node, int index, double unifData) {
    node->unif[index] = unifData;
}

void draw() {
    pixClearRGB(0.1, 0.1, 0.1);
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

        // Light Position
        case KEY_H:
            lightPos[0] += 0.5;
            draw();
            break;
        case KEY_F:
            lightPos[0] -= 0.5;
            draw();
            break;
        case KEY_T:
            lightPos[1] += 0.5;
            draw();
            break;
        case KEY_G:
            lightPos[1] -= 0.5;
            draw();
            break;
        case KEY_Y:
            lightPos[2] += 0.5;
            draw();
            break;
        case KEY_R:
            lightPos[2] -= 0.5;
            draw();
            break;

        case KEY_O:
            shininess += 1.0;
            draw();
            break;
        case KEY_U:
            shininess -= 1.0;
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
            printf("Light Position: %g %g %g\n", lightPos[0], lightPos[1], lightPos[2]);
            printf("Angle: phi %g (pi/%g), theta %g (pi/%g), rho %g\n", 
                phi, pi/phi, theta, pi/theta, rho);
            printf("Projection %d, FOV %g (pi/%g), focal distance %g\n", 
                projection, fov, pi/fov, focal);
            printf("Shininess: %g\n", shininess);
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

        double unif1[renVARYDIMBOUND] = {1.0, 1.0, 1.0,         // RGB
                                         0.0, 1.0, 0.0, 0.0,   // A, P, W, O
                                         0.0, 0.0, 0.0,         // X, Y, Z
                                         0.0, 0.0, 0.0,         // Light XYZ
                                         0.0, 0.0, 0.0,         // Light RGB
                                         0.0, 0.0,              // Ambient light, Shininess
                                         0.0, 0.0, 0.0,         // Camrea XYZ


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
                                         0.0, 0.0, 0.0,
                                         0.0, 0.0, 0.0,
                                         0.0, 0.0,
                                         0.0, 0.0, 0.0,

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
                                         0.0, 0.0, 0.0,
                                         0.0, 0.0, 0.0,
                                         0.0, 0.0,
                                         0.0, 0.0, 0.0,

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

        ren->unifDim = 53;
        ren->texNum = 3;
        ren->attrDim = 8;
        ren->varyDim = 12;
        ren->depth = depth;
        ren->transformVertex = transformVertex;
        ren->colorPixel = colorPixel;
        ren->updateUniform = updateUniform;

        int error = 0;

        error += texInitializeFile(tex[0], "jupiter.jpg");
        // error += texInitializeFile(tex[0], "Kitten.jpg");
        // error += texInitializeFile(tex[1], "360.jpg");
        // error += texInitializeFile(tex[2], "Rainbow.jpg");

        error += meshInitializeSphere(&mesh1, 1.2, 20, 40);
        // error += meshInitializeBox(&mesh1, -1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
        // error += meshInitializeSphere(&mesh2, 0.7, 10, 20);
        // error += meshInitializeBox(&mesh3, -5.0, 5.0, -2.0, 2.0, 0.0, 1.0);

        error += sceneInitialize(&node1, ren, unif1, &tex[0], &mesh1, NULL, NULL);
        // error += sceneInitialize(&node1, ren, unif1, &tex[0], &mesh1, &node2, NULL);
        // error += sceneInitialize(&node2, ren, unif2, &tex[1], &mesh2, NULL, NULL);
        // error += sceneInitialize(&node3, ren, unif3, &tex[2], &mesh3, NULL, NULL);

        // sceneAddChild(&node1, &node3);

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
        // meshDestroy(&mesh2);
        // meshDestroy(&mesh3);

        texDestroy(tex[0]);
        // texDestroy(tex[1]);
        // texDestroy(tex[2]);

        depthDestroy(depth);

        sceneDestroyRecursively(scene);
        return 0;
    }
}
