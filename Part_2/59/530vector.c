/*
 * 530vector.c
 * by Ritvik Kar & Martin Green
 * CS 331: Computer Graphics
*/


/* Assumes that there are dim + 2 arguments, the last dim of which are doubles. 
Sets the dim-dimensional vector v to those doubles. */
void vecSet(int dim, GLdouble v[], ...)
{
    va_list argumentPointer;
    va_start(argumentPointer, v);

    int i;
    for (i = 0; i < dim; i++) {
        v[i] = va_arg(argumentPointer,GLdouble);   
    }

    va_end(argumentPointer);
}

/* Prints the dim-dimensional vector v*/
void vecPrint(int dim, GLdouble v[]) {
	int i;
	for (i = 0; i < dim; i += 1)
		printf("%lf\n", v[i]);
    printf("\n");
}

/* Copies the dim-dimensional vector v to the dim-dimensional vector copy. */
void vecCopy(int dim, GLdouble v[], GLdouble copy[]) {    
    int i;
    for (i = 0; i < dim; i++) {
        copy[i] = v [i];
    }
}

/* Adds the dim-dimensional vectors v and w. */
void vecAdd(int dim, GLdouble v[], GLdouble w[], GLdouble vPlusW[]) {    
    int i;
    for (i = 0; i < dim; i++) {
        vPlusW[i] = v[i] + w[i];
    }
}

/* Subtracts the dim-dimensional vectors v and w. */
void vecSubtract(int dim, GLdouble v[], GLdouble w[], GLdouble vMinusW[]) {    
    int i;
    for (i = 0; i < dim; i++) {
        vMinusW[i] = v[i] - w[i];
    }
}

/* Scales the dim-dimensional vector w by the number c. */
void vecScale(int dim, GLdouble c, GLdouble w[], GLdouble cTimesW[]) {
    int i;
    for (i = 0; i < dim; i++) {
        cTimesW[i] = c*w[i];
    }	
}

/* Returns the dot product of the dim-dimensional vectors v and w. */
GLdouble vecDot(int dim, GLdouble v[], GLdouble w[]){
    int i;
    GLdouble dot = 0.0;
    for (i = 0; i < dim; ++i) {
        dot += v[i]*w[i];
    }
    return dot;
}

/* Returns the length of the dim-dimensional vector v. */
GLdouble vecLength(int dim, GLdouble v[]){
    GLdouble magnitude;
    int i;
    //||v||=√v.v
    GLdouble dot = vecDot(dim,v,v);
    magnitude = sqrt(dot);
    return magnitude;
}

/* Returns the length of the dim-dimensional vector v. If the length is 
non-zero, then also places a scaled version of v into the dim-dimensional 
vector unit, so that unit has length 1. */
GLdouble vecUnit(int dim, GLdouble v[], GLdouble unit[]){
    GLdouble magnitude = vecLength(dim,v);
    if(magnitude != 0){
        vecScale(dim,1/magnitude,v,unit);
    }
    return magnitude;
}

/* Computes the cross product of the 3-dimensional vectors v and w, and places 
it into vCrossW. */
void vec3Cross(GLdouble v[3], GLdouble w[3], GLdouble vCrossW[3]){
    vCrossW[0] = v[1]*w[2] - v[2]*w[1];
    vCrossW[1] = v[2]*w[0] - v[0]*w[2];
    vCrossW[2] = v[0]*w[1] - v[1]*w[0];
}

/* Computes the 3-dimensional vector v from its spherical coordinates. 
rho >= 0.0 is the radius. 0 <= phi <= pi is the co-latitude. -pi <= theta <= pi 
is the longitude or azimuth. */
void vec3Spherical(GLdouble rho, GLdouble phi, GLdouble theta, GLdouble v[3]) {
    v[0] = rho*sin(phi)*cos(theta);
    v[1] = rho*sin(phi)*sin(theta);
    v[2] = rho*cos(phi);
}

/* Performs a type conversion from GLdouble to GLfloat */
void vecOpenGL(int dim, GLdouble v[], GLfloat openGL[]) {
    int i;
    for (i = 0; i < dim; i += 1) {
        openGL[i] = v[i];
    }
}
