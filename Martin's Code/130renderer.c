/* Martin Green (greenm2), CS 311 Graphics */
/* January 2017 */

#define renORTHOGRAPHIC 0
#define renPERSPECTIVE 1
#define renPROJL 0
#define renPROJR 1
#define renPROJB 2
#define renPROJT 3
#define renPROJF 4
#define renPROJN 5

typedef struct renRenderer renRenderer;
struct renRenderer {
    int unifDim, texNum, attrDim, varyDim, projectionType;
    depthBuffer *depth;
    double cameraRotation[3][3];
    double cameraTranslation[3];
    double viewing[16];
    double projection[6];
    double viewport[4][4];
    void (*colorPixel)(renRenderer*, double[], texTexture**, double[], double[]);
    void (*transformVertex)(renRenderer*, double[], double[], double[]);
    void (*updateUniform)(renRenderer*, double[], double[]);
};

/* Sets the camera's rotation and translation, in a manner suitable for third-
person viewing. The camera is aimed at the world coordinates target. The camera 
itself is displaced from that target by a distance rho, in the direction 
specified by the spherical coordinates phi and theta (as in vec3Spherical). 
Under normal use the camera's up-direction is world-up, or as close to it as 
possible. */
void renLookAt(renRenderer *ren, double target[3], double rho, double phi, double theta) {
    double z[3], y[3], yStd[3] = {0.0, 1.0, 0.0}, zStd[3] = {0.0, 0.0, 1.0};
    vec3Spherical(1.0, phi, theta, z);
    vec3Spherical(1.0, M_PI / 2.0 - phi, theta + M_PI, y);
    mat33BasisRotation(yStd, zStd, y, z, ren->cameraRotation);
    vecScale(3, rho, z, ren->cameraTranslation);
    vecAdd(3, target, ren->cameraTranslation, ren->cameraTranslation);
}

/* Sets the camera's rotation and translation, in a manner suitable for first-
person viewing. The camera is positioned at the world coordinates position. 
From that position, the camera's sight direction is described by the spherical 
coordinates phi and theta (as in vec3Spherical). Under normal use the camera's 
up-direction is world-up, or as close to it as possible. */
void renLookFrom(renRenderer *ren, double position[3], double phi, double theta) {
    double negZ[3], y[3];
    double yStd[3] = {0.0, 1.0, 0.0}, negZStd[3] = {0.0, 0.0, -1.0};
    vec3Spherical(1.0, phi, theta, negZ);
    vec3Spherical(1.0, M_PI / 2.0 - phi, theta + M_PI, y);
    mat33BasisRotation(yStd, negZStd, y, negZ, ren->cameraRotation);
    vecCopy(3, position, ren->cameraTranslation);
}

/* Updates the renderer's viewing transformation, based on the camera. */
void renUpdateViewing(renRenderer *ren) {
    double projection[4][4];

    mat44Viewport((double)(ren->depth->width), (double)(ren->depth->height), ren->viewport);

    if (ren->projectionType == renORTHOGRAPHIC) {
        mat44Orthographic(
            ren->projection[renPROJL], ren->projection[renPROJR],
            ren->projection[renPROJB], ren->projection[renPROJT],
            ren->projection[renPROJF], ren->projection[renPROJN], projection);
    } else {
        mat44Perspective(
            ren->projection[renPROJL], ren->projection[renPROJR],
            ren->projection[renPROJB], ren->projection[renPROJT],
            ren->projection[renPROJF], ren->projection[renPROJN], projection);
    }

    // viewing
    double camera[4][4];
    mat44InverseIsometry(ren->cameraRotation, 
                         ren->cameraTranslation, 
                         camera);

    // printf("projection looks like this:\n");
    // int i, j;
    // for (j = 0; j < 4; j += 1) {
    //     for (i = 0; i < 4; i += 1) {
    //         printf("%f\t", projection[j][i]);
    //     }
    //     printf("\n");
    // }

    // printf("camera looks like this:\n");
    // for (j = 0; j < 4; j += 1) {
    //     for (i = 0; i < 4; i += 1) {
    //         printf("%f\t", camera[j][i]);
    //     }
    //     printf("\n");
    // }

    // printf("viewport looks like this:\n");
    // for (j = 0; j < 4; j += 1) {
    //     for (i = 0; i < 4; i += 1) {
    //         printf("%f\t", ren->viewport[j][i]);
    //     }
    //     printf("\n");
    // }

    // double transfer[4][4];
    mat444Multiply(projection, camera, (double(*)[4])(&ren->viewing));

    // mat444Multiply(tempViewport, transfer, (double(*)[4])(&ren->viewing));

    // printf("viewing looks like this:\n");
    // i = 0;
    // for (j = 0; j < 4; j += 1) {
    //     int lim = i;
    //     for (i = i; i < lim + 4; i += 1) {
    //         printf("%f\t", ren->viewing[i]);
    //     }
    //     printf("\n");
    // }


}

/* Sets the projection type, to either renORTHOGRAPHIC or renPERSPECTIVE. */
void renSetProjectionType(renRenderer *ren, int projType) {
    ren->projectionType = projType;
}

/* Sets all six projection parameters. */
void renSetProjection(renRenderer *ren, double proj[6]) {
    vecCopy(6, proj, ren->projection);
}

/* Sets one of the six projection parameters. */
void renSetOneProjection(renRenderer *ren, int i, double value) {
    ren->projection[i] = value;
}

/* Sets the projection type and the six projection parameters, based on the 
width and height and three other parameters. The camera looks down the center 
of the viewing volume. For perspective projection, fovy is the full (not half) 
vertical angle of the field of vision, in radians. focal > 0 is the distance 
from the camera to the 'focal' plane (where 'focus' is used in the sense of 
attention, not optics). ratio expresses the far and near clipping planes 
relative to focal: far = -focal * ratio and near = -focal / ratio. Reasonable 
values are fovy = M_PI / 6.0, focal = 10.0, and ratio = 10.0, so that 
far = -100.0 and near = -1.0. For orthographic projection, the projection 
parameters are set to produce the orthographic projection that, at the focal 
plane, is most similar to the perspective projection just described. */
void renSetFrustum(renRenderer *ren, int projType, double fovy, double focal, double ratio) {
    ren->projectionType = projType;
    ren->projection[renPROJF] = -focal * ratio;
    ren->projection[renPROJN] = -focal / ratio;

    if (projType == renPERSPECTIVE)
        ren->projection[renPROJT] = -ren->projection[renPROJN] * tan(fovy * 0.5);
    else
        ren->projection[renPROJT] = focal * tan(fovy * 0.5);

    ren->projection[renPROJB] = -ren->projection[renPROJT];
    ren->projection[renPROJR] = ren->projection[renPROJT] * 
            (double)(ren->depth->width) / (double)(ren->depth->height);
    ren->projection[renPROJL] = -ren->projection[renPROJR];
}
 