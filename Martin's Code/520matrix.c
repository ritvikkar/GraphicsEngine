/* Martin Green (greenm2), CS 311 Graphics */
/* January 2017 */

/*** 2 x 2 Matrices ***/

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat22Print(GLdouble m[2][2]) {
    int i;
    for (i = 0; i < 2; i += 1)
        printf("%8.2f  %8.2f\n", m[i][0], m[i][1]);
}

GLdouble mat22Det(GLdouble m[2][2]) {
    return m[0][0]*m[1][1] - m[0][1]*m[1][0];
}

void mat22Scale(GLdouble m[2][2], GLdouble scalar) {
    int i, j;
    for (i = 0; i < 2; i += 1)
        for (j = 0; j < 2; j += 1)
            m[i][j] = scalar * m[i][j];
}

/* Returns the determinant of the matrix m. If the determinant is 0.0, then the 
matrix is not invertible, and mInv is untouched. If the determinant is not 0.0, 
then the matrix is invertible, and its inverse is placed into mInv. */
GLdouble mat22Invert(GLdouble m[2][2], GLdouble mInv[2][2]) {
    GLdouble det = mat22Det(m);

    if (det != 0) {
        // Rearrange matrix
        mInv[0][0] = m[1][1];
        mInv[0][1] = -m[0][1];
        mInv[1][0] = -m[1][0];
        mInv[1][1] = m[0][0];

        // Scalar multiply
        mat22Scale(mInv, 1/det);
    }

    return det;
}

/* Multiplies a 2x2 matrix m by a 2-column v, storing the result in mTimesV. 
The output should not */
void mat221Multiply(GLdouble m[2][2], GLdouble v[2], GLdouble mTimesV[2]) {
    int i, j;

    for (i = 0; i < 2; i += 1)
        mTimesV[i] = 0.0;

    for (i = 0; i < 2; i += 1)
        for (j = 0; j < 2; j += 1)
            mTimesV[i] += m[i][j]*v[j];
}

/* Fills the matrix m from its two columns. */
void mat22Columns(GLdouble col0[2], GLdouble col1[2], GLdouble m[2][2]) {
    int i, j;
    for (i = 0; i < 2; i += 1) {
        m[i][0] = col0[i];
        m[i][1] = col1[i];
    }
}

/* Multiplies the 3x3 matrix m by the 3x3 matrix n. */
void mat333Multiply(GLdouble m[3][3], GLdouble n[3][3], GLdouble mTimesN[3][3]) {
    int i, j, k;

    for (i = 0; i < 3; i += 1)
        for (j = 0; j < 3; j += 1)
            mTimesN[i][j] = 0.0;

    for (i = 0; i < 3; i += 1)
        for (j = 0; j < 3; j += 1)
            for (k = 0; k < 3; k += 1)
                mTimesN[i][j] += m[i][k]*n[k][j];
}

/* Multiplies the 3x3 matrix m by the 3x1 matrix v. */
void mat331Multiply(GLdouble m[3][3], GLdouble v[3], GLdouble mTimesV[3]) {
    int i, j;

    for (i = 0; i < 3; i += 1)
        mTimesV[i] = 0.0;

    for (i = 0; i < 3; i += 1)
        for (j = 0; j < 3; j += 1)
            mTimesV[i] += m[i][j]*v[j];
}

/* Builds a 3x3 matrix representing 2D rotation and translation in homogeneous 
coordinates. More precisely, the transformation first rotates through the angle 
theta (in radians, counterclockwise), and then translates by the vector (x, y). 
*/
void mat33Isometry(GLdouble theta, GLdouble x, GLdouble y, GLdouble isom[3][3]) {
    isom[0][0] = cos(theta);
    isom[0][1] = -sin(theta);
    isom[1][0] = sin(theta);
    isom[1][1] = cos(theta);

    isom[0][2] = x;
    isom[1][2] = y;

    isom[2][0] = 0.0;
    isom[2][1] = 0.0;
    isom[2][2] = 1.0;
}

/* Given a length-1 3D vector axis and an angle theta (in radians), builds the 
rotation matrix for the rotation about that axis through that angle. Based on 
Rodrigues' rotation formula R = I + (sin theta) U + (1 - cos theta) U^2. */
void mat33AngleAxisRotation(GLdouble theta, GLdouble axis[3], GLdouble rot[3][3]) {
    GLdouble U[3][3] = {
        {0.0, -axis[2], axis[1]},
        {axis[2], 0.0, -axis[0]},
        {-axis[1], axis[0], 0.0}
    };
    GLdouble I[3][3] = {
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0}
    };
    GLdouble uSquare[3][3];
    mat333Multiply(U, U, uSquare);

    GLdouble sinTheta = sin(theta);
    GLdouble minusCosTheta = 1 - cos(theta);

    int i, j;
    for (i = 0; i < 3; i += 1)
        for (j = 0; j < 3; j += 1)
            rot[i][j] = I[i][j] + sinTheta*U[i][j] + minusCosTheta*uSquare[i][j];

}

/* Given two length-1 3D vectors u, v that are perpendicular to each other. 
Given two length-1 3D vectors a, b that are perpendicular to each other. 
Builds the rotation matrix that rotates u to a and v to b. */
void mat33BasisRotation(GLdouble u[3], GLdouble v[3], GLdouble a[3], GLdouble b[3], GLdouble rot[3][3]) {
    GLdouble w[3], c[3], rInv[3][3], s[3][3];
    vec3Cross(u, v, w);
    vec3Cross(a, b, c);

    int i;
    for (i = 0; i < 3; i += 1) {
        rInv[0][i] = u[i];
        rInv[1][i] = v[i];
        rInv[2][i] = w[i];

        s[i][0] = a[i];
        s[i][1] = b[i];
        s[i][2] = c[i];
    }

    mat333Multiply(s, rInv, rot);
}

void mat33Identity(GLdouble m[3][3]) {
    int i, j, size = 3;

    for (i = 0; i < size; i += 1)
        for (j = 0; j < size; j += 1)
            m[i][j] = 0.0;

    for (i = 0; i < size; i += 1)
        m[i][i] = 1.0;
}

void mat44Identity(GLdouble m[4][4]) {
    int i, j, size = 4;

    for (i = 0; i < size; i += 1)
        for (j = 0; j < size; j += 1)
            m[i][j] = 0.0;

    for (i = 0; i < size; i += 1)
        m[i][i] = 1.0;
}

/* Multiplies m by n, placing the answer in mTimesN. */
void mat444Multiply(GLdouble m[4][4], GLdouble n[4][4], GLdouble mTimesN[4][4]) {
    int i, j, k;

    for (i = 0; i < 4; i += 1)
        for (j = 0; j < 4; j += 1)
            mTimesN[i][j] = 0.0;

    for (i = 0; i < 4; i += 1)
        for (j = 0; j < 4; j += 1)
            for (k = 0; k < 4; k += 1)
                mTimesN[i][j] += m[i][k]*n[k][j];

}

/* Multiplies m by v, placing the answer in mTimesV. */
void mat441Multiply(GLdouble m[4][4], GLdouble v[4], GLdouble mTimesV[4]) {
    int i, j;

    for (i = 0; i < 4; i += 1)
        mTimesV[i] = 0.0;

    for (i = 0; i < 4; i += 1)
        for (j = 0; j < 4; j += 1)
            mTimesV[i] += m[i][j]*v[j];
}

/* We want to pass matrices into OpenGL, but there are two obstacles. First, 
our matrix library uses double matrices, but OpenGL 2.x expects GLfloat 
matrices. Second, C matrices are implicitly stored one-row-after-another, while 
OpenGL expects matrices to be stored one-column-after-another. This function 
plows through both of those obstacles. */
void mat44OpenGL(double m[4][4], GLfloat openGL[4][4]) {
    for (int i = 0; i < 4; i += 1)
        for (int j = 0; j < 4; j += 1)
            openGL[i][j] = m[j][i];
}

/* Given a rotation and a translation, forms the 4x4 homogeneous matrix 
representing the rotation followed in time by the translation. */
void mat44Isometry(GLdouble rot[3][3], GLdouble trans[3], GLdouble isom[4][4]) {
    int i, j;
    for (i = 0; i < 3; i += 1)
        for (j = 0; j < 3; j += 1)
            isom[i][j] = rot[i][j];
    
    for (i = 0; i < 3; i += 1)
        isom[i][3] = trans[i];
    
    for (j = 0; j < 3; j += 1)
        isom[3][j] = 0.0;
    isom[3][3] = 1.0;
}

/* Given a rotation and translation, forms the 4x4 homogeneous matrix 
representing the inverse translation followed in time by the inverse rotation. 
That is, the isom produced by this function is the inverse to the isom 
produced by mat44Isometry on the same inputs. */
void mat44InverseIsometry(GLdouble rot[3][3], GLdouble trans[3], GLdouble isom[4][4]) {
    GLdouble rotInv[4][4]= {
        {1.0, 0.0, 0.0, 0.0}, 
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };

    GLdouble transInv[4][4] = {
        {1.0, 0.0, 0.0, 0.0}, 
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };

    int i, j;
    for (i = 0; i < 3; i += 1)
        for (j = 0; j < 3; j += 1)
            rotInv[j][i] = rot[i][j];

    for (i = 0; i < 3; i += 1)
        transInv[i][3] = -trans[i];

    mat444Multiply(rotInv, transInv, isom);

}


/* Builds a 4x4 matrix representing orthographic projection with a boxy viewing 
volume [left, right] x [bottom, top] x [far, near]. That is, on the near plane 
the box is the rectangle R = [left, right] x [bottom, top], and on the far 
plane the box is the same rectangle R. Keep in mind that 0 > near > far. Maps 
the viewing volume to [-1, 1] x [-1, 1] x [-1, 1]. */
void mat44Orthographic(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, 
        GLdouble far, GLdouble near, GLdouble proj[4][4]) {

    int i, j;
    for (i = 0; i < 4; i += 1)
        for (j = 0; j < 4; j += 1)
            proj[i][j] = 0.0;

    proj[0][0] = 2 / (right - left);
    proj[0][3] = (-right - left) / (right - left);

    proj[1][1] = 2 / (top - bottom);
    proj[1][3] = (-top - bottom) / (top - bottom);

    proj[2][2] = 2 / (near - far);
    proj[2][3] = (-near - far) / (near - far);

    proj[3][3] = 1.0;
}

/* Builds a 4x4 matrix that maps a projected viewing volume 
[-1, 1] x [-1, 1] x [-1, 1] to screen [0, w - 1] x [0, h - 1] x [-1, 1]. */
void mat44Viewport(GLdouble width, GLdouble height, GLdouble view[4][4]) {
    GLdouble tempViewport[4][4] = {
        {(width - 1)/2, 0.0, 0.0, (width - 1)/2},
        {0.0, (height - 1)/2, 0.0, (height - 1)/2},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };

    int i;
    for (i = 0; i < 4; i += 1) 
        vecCopy(4, tempViewport[i], view[i]);

}

/* Builds a 4x4 matrix representing perspective projection. The viewing frustum 
is contained between the near and far planes, with 0 > near > far. On the near 
plane, the frustum is the rectangle R = [left, right] x [bottom, top]. On the 
far plane, the frustum is the rectangle (far / near) * R. Maps the viewing 
volume to [-1, 1] x [-1, 1] x [-1, 1]. */
void mat44Perspective(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, 
        GLdouble far, GLdouble near, GLdouble proj[4][4]) {

    int i, j;
    for (i = 0; i < 4; i += 1)
        for (j = 0; j < 4; j += 1)
            proj[i][j] = 0.0;

    proj[0][0] = (-2 * near) / (right - left);
    proj[0][2] = (right + left) / (right - left);

    proj[1][1] = (-2 * near) / (top - bottom);
    proj[1][2] = (top + bottom) / (top - bottom);

    proj[2][3] = (2 * near * far) / (near - far);
    proj[2][2] = (-near - far) / (near - far);

    proj[3][2] = -1.0;
}
