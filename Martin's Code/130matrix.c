/* Martin Green (greenm2), CS 311 Graphics */
/* January 2017 */

/*** 2 x 2 Matrices ***/

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat22Print(double m[2][2]) {
    int i;
    for (i = 0; i < 2; i += 1)
        printf("%8.2f  %8.2f\n", m[i][0], m[i][1]);
}

double mat22Det(double m[2][2]) {
    return m[0][0]*m[1][1] - m[0][1]*m[1][0];
}

void mat22Scale(double m[2][2], double scalar) {
    int i, j;
    for (i = 0; i < 2; i += 1)
        for (j = 0; j < 2; j += 1)
            m[i][j] = scalar * m[i][j];
}

/* Returns the determinant of the matrix m. If the determinant is 0.0, then the 
matrix is not invertible, and mInv is untouched. If the determinant is not 0.0, 
then the matrix is invertible, and its inverse is placed into mInv. */
double mat22Invert(double m[2][2], double mInv[2][2]) {
    double det = mat22Det(m);

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
void mat221Multiply(double m[2][2], double v[2], double mTimesV[2]) {
    int i, j;

    for (i = 0; i < 2; i += 1)
        mTimesV[i] = 0.0;

    for (i = 0; i < 2; i += 1)
        for (j = 0; j < 2; j += 1)
            mTimesV[i] += m[i][j]*v[j];
}

/* Fills the matrix m from its two columns. */
void mat22Columns(double col0[2], double col1[2], double m[2][2]) {
    int i, j;
    for (i = 0; i < 2; i += 1) {
        m[i][0] = col0[i];
        m[i][1] = col1[i];
    }
}

/* Multiplies the 3x3 matrix m by the 3x3 matrix n. */
void mat333Multiply(double m[3][3], double n[3][3], double mTimesN[3][3]) {
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
void mat331Multiply(double m[3][3], double v[3], double mTimesV[3]) {
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
void mat33Isometry(double theta, double x, double y, double isom[3][3]) {
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
void mat33AngleAxisRotation(double theta, double axis[3], double rot[3][3]) {
    double U[3][3] = {
        {0.0, -axis[2], axis[1]},
        {axis[2], 0.0, -axis[0]},
        {-axis[1], axis[0], 0.0}
    };
    double I[3][3] = {
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0}
    };
    double uSquare[3][3];
    mat333Multiply(U, U, uSquare);

    double sinTheta = sin(theta);
    double minusCosTheta = 1 - cos(theta);

    int i, j;
    for (i = 0; i < 3; i += 1)
        for (j = 0; j < 3; j += 1)
            rot[i][j] = I[i][j] + sinTheta*U[i][j] + minusCosTheta*uSquare[i][j];

}

/* Given two length-1 3D vectors u, v that are perpendicular to each other. 
Given two length-1 3D vectors a, b that are perpendicular to each other. 
Builds the rotation matrix that rotates u to a and v to b. */
void mat33BasisRotation(double u[3], double v[3], double a[3], double b[3], double rot[3][3]) {
    double w[3], c[3], rInv[3][3], s[3][3];
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

/* Multiplies m by n, placing the answer in mTimesN. */
void mat444Multiply(double m[4][4], double n[4][4], double mTimesN[4][4]) {
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
void mat441Multiply(double m[4][4], double v[4], double mTimesV[4]) {
    int i, j;

    for (i = 0; i < 4; i += 1)
        mTimesV[i] = 0.0;

    for (i = 0; i < 4; i += 1)
        for (j = 0; j < 4; j += 1)
            mTimesV[i] += m[i][j]*v[j];
}

/* Given a rotation and a translation, forms the 4x4 homogeneous matrix 
representing the rotation followed in time by the translation. */
void mat44Isometry(double rot[3][3], double trans[3], double isom[4][4]) {
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
void mat44InverseIsometry(double rot[3][3], double trans[3], double isom[4][4]) {
    double rotInv[4][4]= {
        {1.0, 0.0, 0.0, 0.0}, 
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };

    double transInv[4][4] = {
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
void mat44Orthographic(double left, double right, double bottom, double top, 
        double far, double near, double proj[4][4]) {

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
void mat44Viewport(double width, double height, double view[4][4]) {
    double tempViewport[4][4] = {
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
void mat44Perspective(double left, double right, double bottom, double top, 
        double far, double near, double proj[4][4]) {

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
