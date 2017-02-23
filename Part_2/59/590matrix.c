/*
 * 540matrix.c
 * by Ritvik Kar & Martin Green
 * CS 311: Computer Graphics
*/

/* 2x2Matrices */

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat22Print(GLdouble m[2][2]) {
	int i;
	for (i = 0; i < 2; i += 1)
		printf("%lf\t%lf\n", m[i][0], m[i][1]);
}

/* Returns the determinant of the matrix m. If the determinant is 0.0, then the 
matrix is not invertible, and mInv is untouched. If the determinant is not 0.0, 
then the matrix is invertible, and its inverse is placed into mInv. */
GLdouble mat22Invert(GLdouble m[2][2], GLdouble mInv[2][2]) {
	GLdouble determinant;
    
    determinant = m[0][0]*m[1][1] - m[0][1]*m[1][0];
    determinant = fabs(determinant);

    if(determinant != 0.0) {
        GLdouble scale = 1/determinant;
        mInv[0][0] = scale*m[1][1];
        mInv[0][1] = scale*(-m[0][1]);
        mInv[1][0] = scale*(-m[1][0]);
        mInv[1][1] = scale*m[0][0];
    }
    
    return determinant;
}

/* Multiplies a 2x2 matrix m by a 2-column v, storing the result in mTimesV. 
The output should not */
void mat221Multiply(GLdouble m[2][2], GLdouble v[2], GLdouble mTimesV[2]) {
	mTimesV[0] = v[0]*m[0][0]+v[1]*m[0][1];
    mTimesV[1] = v[0]*m[1][0]+v[1]*m[1][1];
}

/* Fills the matrix m from its two columns. */
void mat22Columns(GLdouble col0[2], GLdouble col1[2], GLdouble m[2][2]) {
	m[0][0] = col0[0];
    m[0][1] = col1[0];
    m[1][0] = col0[1];
    m[1][1] = col1[1];
}

/* 3x3Matrices */

/* Builds a 3x3 identity matrix */
void mat33Identity(GLdouble m[3][3]){
    int i, j;
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            m[i][j] = 0.0;
        }
    }
    m[0][0] = 1.0;
    m[1][1] = 1.0;
    m[2][2] = 1.0;
}

/* Multiplies the 3x3 matrix m by the 3x3 matrix n. */
void mat333Multiply(GLdouble m[3][3], GLdouble n[3][3], GLdouble mTimesN[3][3]) {
    int i, j;
    for(i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            mTimesN[i][j] = m[i][0]*n[0][j] + m[i][1]*n[1][j] + m[i][2]*n[2][j];
        }
    }
}

/* Multiplies the 3x3 matrix m by the 3x1 matrix v. */
void mat331Multiply(GLdouble m[3][3], GLdouble v[3], GLdouble mTimesV[3]) {
    int i;
    for (i = 0; i < 3; i++) {
        mTimesV[i] = m[i][0]*v[0] + m[i][1]*v[1] + m[i][2]*v[2];
    }
}

/* Builds a 3x3 matrix representing 2D rotation and translation in homogeneous
coordinates. More precisely, the transformation first rotates through the angle
theta (in radians, counterclockwise), and then translates by the vector (x, y).
*/
void mat33Isometry(GLdouble theta, GLdouble x, GLdouble y, GLdouble isom[3][3]) {    
    isom[0][0] = cos(theta);
    isom[0][1] = (-1)*sin(theta);
    isom[0][2] = x;

    isom[1][0] = sin(theta);
    isom[1][1] = cos(theta);
    isom[1][2] = y;

    isom[2][0] = 0;
    isom[2][1] = 0;
    isom[2][2] = 1;
}
/* Given a 3x3 matrix, return it's transpose, i.e. rows become
columns thus columns become rows */
void mat33Transpose(GLdouble m[3][3], GLdouble m_T[3][3]){
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            m_T[i][j] = m[j][i];
        }
    }
}

/* Given a length-1 3D vector axis and an angle theta (in radians), builds the 
rotation matrix for the rotation about that axis through that angle. Based on 
Rodrigues' rotation formula R = I + (sin theta) U + (1 - cos theta) U^2. */
void mat33AngleAxisRotation(GLdouble theta, GLdouble axis[3], GLdouble rot[3][3]) {
    //R = I + ((sin theta) * U) + ((1 - cos theta)*U^2)
    GLdouble U[3][3];
    GLdouble UU[3][3];//U^2

    //contents of U
    U[0][0] = 0.0;
    U[0][1] = (-1)*axis[2];
    U[0][2] = axis[1];
    U[1][0] = axis[2];
    U[1][1] = 0.0;
    U[1][2] = (-1)*axis[0];
    U[2][0] = (-1)*axis[1];
    U[2][1] = axis[0];
    U[2][2] = 0.0;

    mat333Multiply(U, U, UU);

    //identity matrix
    GLdouble I[3][3] = {{1.0, 0.0, 0.0},
                        {0.0, 1.0, 0.0},
                        {0.0, 0.0, 1.0}};
    
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            rot[i][j] = I[i][j] + (sin(theta)*U[i][j]) + ((1.0 - cos(theta))*(UU[i][j]));
        }
    }
}

/* Given two length-1 3D vectors u, v that are perpendicular to each other. 
Given two length-1 3D vectors a, b that are perpendicular to each other. Builds 
the rotation matrix that rotates u to a and v to b. */
void mat33BasisRotation(GLdouble u[3], GLdouble v[3], GLdouble a[3], GLdouble b[3], GLdouble rot[3][3]){
    //Form R [u v uxv] -> Then S [a b axb] -> Transpose R for R_T -> S x R_T = rot[3][3]

    GLdouble R[3][3];
    GLdouble S[3][3];

    GLdouble w[3];//u x v
    vec3Cross(u, v, w);
    int i, j;
    for (i = 0; i < 3 ; i++){
        R[i][0] = u[i];
        R[i][1] = v[i];
        R[i][2] = w[i];
    }//forming R out of [u v w] columns

    GLdouble c[3];//a x b
    vec3Cross(a, b, c);

    for (i = 0; i < 3 ; i++){
        S[i][0] = a[i];
        S[i][1] = b[i];
        S[i][2] = c[i];
    }//forming S out of [a b c] columns

    GLdouble R_T[3][3];//R Transpose
    mat33Transpose(R, R_T);
    mat333Multiply(S, R_T, rot);
}

/* 4x4Matrices */

/* Builds a 4x4 identity matrix */
void mat44Identity(GLdouble m[4][4]){
    int i, j;
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            m[i][j] = 0.0;
        }
    }
    m[0][0] = 1.0;
    m[1][1] = 1.0;
    m[2][2] = 1.0;
    m[3][3] = 1.0;    
}

/* Copies the 4X4 matrix m to the 4X4 matrix copy. */
void mat44Copy(GLdouble m[4][4], GLdouble copy[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            copy[i][j] = m[i][j];
        }
    }
}

/* Multiplies m by n, placing the answer in mTimesN. */
void mat444Multiply(GLdouble m[4][4], GLdouble n[4][4], GLdouble mTimesN[4][4]){
    int i, j;
    for(i = 0; i < 4; i++) {
        for(j = 0; j < 4; j++) {
            mTimesN[i][j] = m[i][0]*n[0][j] + m[i][1]*n[1][j] + m[i][2]*n[2][j] + m[i][3]*n[3][j];
        }
    }
}

/* Multiplies m by v, placing the answer in mTimesV. */
void mat441Multiply(GLdouble m[4][4], GLdouble v[4], GLdouble mTimesV[4]){
    int i;
    for (i = 0; i < 4; i++) {
        mTimesV[i] = m[i][0]*v[0] + m[i][1]*v[1] + m[i][2]*v[2] + m[i][3]*v[3];
    }
}

/* Given a rotation and a translation, forms the 4x4 homogeneous matrix 
representing the rotation followed in time by the translation. */
void mat44Isometry(GLdouble rot[3][3], GLdouble trans[3], GLdouble isom[4][4]){
    int i, j;
    for (i = 0; i < 3; i++){
        for (j = 0; j < 3; j++){
            isom[i][j] = rot[i][j];
        }
    }

    isom[0][3] = trans[0];
    isom[1][3] = trans[1];
    isom[2][3] = trans[2];

    isom[3][0] = 0.0;
    isom[3][1] = 0.0;
    isom[3][2] = 0.0;
    isom[3][3] = 1.0;
}

/* Given a rotation and translation, forms the 4x4 homogeneous matrix 
representing the inverse translation followed in time by the inverse rotation. 
That is, the isom produced by this function is the inverse to the isom 
produced by mat44Isometry on the same inputs. */
void mat44InverseIsometry(GLdouble rot[3][3], GLdouble trans[3], GLdouble isom[4][4]){
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            isom[i][j] = rot[j][i];
        }
    }

    isom[0][3] = (-rot[0][0]*trans[0])-(rot[1][0]*trans[1])-(rot[2][0]*trans[2]);
    isom[1][3] = (-rot[0][1]*trans[0])-(rot[1][1]*trans[1])-(rot[2][1]*trans[2]);
    isom[2][3] = (-rot[0][2]*trans[0])-(rot[1][2]*trans[1])-(rot[2][2]*trans[2]);

    isom[3][0] = 0.0;
    isom[3][1] = 0.0;
    isom[3][2] = 0.0;
    isom[3][3] = 1.0;
}

/* Builds a 4x4 matrix that maps a projected viewing volume 
[-1, 1] x [-1, 1] x [-1, 1] to screen [0, w - 1] x [0, h - 1] x [-1, 1]. */
void mat44Viewport(GLdouble width, GLdouble height, GLdouble view[4][4]){
    view[0][0] = width - 0.5;
    view[0][1] = 0.0;
    view[0][2] = 0.0;
    view[0][3] = width - 0.5;

    view[1][0] = 0;
    view[1][1] = height - 0.5;
    view[1][2] = 0;
    view[1][3] = height - 0.5;

    view[2][0] = 0;
    view[2][1] = 0;
    view[2][2] = 1;
    view[2][0] = 0;

    view[3][0] = 0;
    view[3][1] = 0;
    view[3][2] = 0;
    view[3][0] = 1;
}

/* Builds a 4x4 matrix representing orthographic projection with a boxy viewing 
volume [left, right] x [bottom, top] x [far, near]. That is, on the near plane 
the box is the rectangle R = [left, right] x [bottom, top], and on the far 
plane the box is the same rectangle R. Keep in mind that 0 > near > far. Maps 
the viewing volume to [-1, 1] x [-1, 1] x [-1, 1]. */
void mat44Orthographic(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, 
        GLdouble far, GLdouble near, GLdouble proj[4][4]){
    proj[0][0] = 2/(right-left);    
    proj[0][1] = 0.0;
    proj[0][2] = 0.0;
    proj[0][3] = (-right-left)/(right-left);

    proj[1][0] = 0.0;
    proj[1][1] = 2/(top-bottom);
    proj[1][2] = 0.0;
    proj[1][3] = (-top-bottom)/(top-bottom);

    proj[2][0] = 0.0;
    proj[2][1] = 0.0;
    proj[2][2] = -2/(near-far);
    proj[2][3] = (near+far)/(near-far);

    proj[3][0] = 0.0;
    proj[3][1] = 0.0;
    proj[3][2] = 0.0;
    proj[3][3] = 1;
}

/* Builds a 4x4 matrix representing perspective projection. The viewing frustum 
is contained between the near and far planes, with 0 > near > far. On the near 
plane, the frustum is the rectangle R = [left, right] x [bottom, top]. On the 
far plane, the frustum is the rectangle (far / near) * R. Maps the viewing 
volume to [-1, 1] x [-1, 1] x [-1, 1]. */
void mat44Perspective(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, 
        GLdouble far, GLdouble near, GLdouble proj[4][4])
{
    proj[0][0] = (-2*near)/(right-left);
    proj[0][1] = 0;
    proj[0][2] = (right+left)/(right-left);
    proj[0][3] = 0;

    proj[1][0] = 0;
    proj[1][1] = (-2*near)/(top-bottom);
    proj[1][2] = (top+bottom)/(top-bottom);
    proj[1][3] = 0;

    proj[2][0] = 0;
    proj[2][1] = 0;
    proj[2][2] = (near+far)/(near-far);
    proj[2][3] = (-2*near*far)/(near-far);

    proj[3][0] = 0;
    proj[3][1] = 0;
    proj[3][2] = -1;
    proj[3][3] = 0;
}

/* We want to pass matrices into OpenGL, but there are two obstacles. First, 
our matrix library uses GLdouble matrices, but OpenGL 2.x expects GLfloat 
matrices. Second, C matrices are implicitly stored one-row-after-another, while 
OpenGL expects matrices to be stored one-column-after-another. This function 
plows through both of those obstacles. */
void mat44OpenGL(GLdouble m[4][4], GLfloat openGL[4][4]) {
    for (int i = 0; i < 4; i += 1)
        for (int j = 0; j < 4; j += 1)
            openGL[i][j] = m[j][i];
}
