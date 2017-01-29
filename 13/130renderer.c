/*
 * 130renderer.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

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
    int unifDim; //dimensions for double unif[], eg. rgb, 3
    int texNum; //dimensions for double tex[], eg. brangelina, 1
    int attrDim;//dimensions for double a[], b[], c[], eg. a, α, b, ß, c, γ
    int varyDim;// transformed vertices
    void (*colorPixel)(renRenderer *, double[], texTexture *[], double[], double[]);
    //pointer to the color pixel function
    void (*transformVertex)(renRenderer *, double[], double[], double[]);
    //pointer to the transform vertex function
    void (*updateUniform)(renRenderer *,double[],double[]);
    //pointer to the update uniform function
    depthBuffer *depth;//depth buffer
    double cameraRotation[3][3];
    double cameraTranslation[3];
    double viewing[4][4];
    double projection[6];
    int projectionType;
    double viewport[4][4];
};

/* Sets the camera's rotation and translation, in a manner suitable for third-
person viewing. The camera is aimed at the world coordinates target. The camera
itself is displaced from that target by a distance rho, in the direction
specified by the spherical coordinates phi and theta (as in vec3Spherical).
Under normal use, where 0 < phi < pi, the camera's up-direction is world-up, or
as close to it as possible. */
void renLookAt(renRenderer *ren, double target[3], double rho, double phi,
        double theta) {
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
coordinates phi and theta (as in vec3Spherical). Under normal use, where
0 < phi < pi, the camera's up-direction is world-up, or as close to it as
possible. */
void renLookFrom(renRenderer *ren, double position[3], double phi,
        double theta) {
    double negZ[3], y[3];
    double yStd[3] = {0.0, 1.0, 0.0}, negZStd[3] = {0.0, 0.0, -1.0};
    vec3Spherical(1.0, phi, theta, negZ);
    vec3Spherical(1.0, M_PI / 2.0 - phi, theta + M_PI, y);
    mat33BasisRotation(yStd, negZStd, y, negZ, ren->cameraRotation);
    vecCopy(3, position, ren->cameraTranslation);
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
void renSetFrustum(renRenderer *ren, int projType, double fovy, double focal, 
    double ratio) {
    ren->projectionType = projType;
    ren->projection[renPROJF] = -focal * ratio;
    ren->projection[renPROJN] = -focal / ratio;
    if (projType == renPERSPECTIVE)
        ren->projection[renPROJT] = -ren->projection[renPROJN] 
          * tan(fovy * 0.5);
    else
        ren->projection[renPROJT] = focal * tan(fovy * 0.5);
    ren->projection[renPROJB] = -ren->projection[renPROJT];
    ren->projection[renPROJR] = ren->projection[renPROJT] * 
      (double)(ren->depth->width) / (double)(ren->depth->height);
    ren->projection[renPROJL] = -ren->projection[renPROJR];
}

/* Updates the renderer's viewing transformation, based on the camera. */
void renUpdateViewing(renRenderer *ren) {
    mat44InverseIsometry(ren->cameraRotation, ren->cameraTranslation, ren->viewing);
    
    double view[4][4];
    mat44Viewport(ren->depth->width, ren->depth->height, ren->viewport);
    
/*    double proj[4][4];
    if(ren->projectionType == renORTHOGRAPHIC){
        mat44Orthographic(ren->projection[renPROJL], ren->projection[renPROJR], 
                          ren->projection[renPROJB], ren->projection[renPROJT], 
                          ren->projection[renPROJF], ren->projection[renPROJN], proj[4][4]);  
    }

    else if(ren->projectionType == renPERSPECTIVE){
        mat44Perspective(ren->projection[renPROJL], ren->projection[renPROJR], 
                          ren->projection[renPROJB], ren->projection[renPROJT], 
                          ren->projection[renPROJF], ren->projection[renPROJN], proj[4][4]);  

    }

    mat444Multiply(view,proj,ren->viewport);
*/}

