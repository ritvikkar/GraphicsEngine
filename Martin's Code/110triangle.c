/* Martin Green (greenm2), CS 311 Graphics */
/* January 2017 */

char dimTri[6] = {'X', 'Y', 'Z', 'W', 'S', 'T'};

/* Inverts the matrix [b-a | c-a], places it 
   into invert and returns the determinant */
double invertMatrix(double a[], double b[], double c[], double invert[2][2]) {
    double bMinusA[2];
    double cMinusA[2];

    double abcMatrix[2][2];

    vecSubtract(2, b, a, bMinusA);
    vecSubtract(2, c, a, cMinusA);


    mat22Columns(bMinusA, cMinusA, abcMatrix);

    double det = mat22Invert(abcMatrix, invert);

    return det;
}

/* Finds p and q scalars for interpretation. Expects scalars to be empty.
   Fills with 0 if there is no inverse for [b-a | c-a] */
void findScalars(double a[], double invert[2][2], double x[], double scalars[]) {
    double xMinusA[2];

    vecSubtract(2, x, a, xMinusA);
    mat221Multiply(invert, xMinusA, scalars);
}

/* Interpolates alpha, beta, and gamma to vary via scalars. Expects vary to be empty. */
void interpolateScalars(double alpha[], double beta[], double gamma[], 
                        double scalars[2], double vary[], renRenderer *ren) {
    double bMinusA[renVARYDIMBOUND];
    double cMinusA[renVARYDIMBOUND];
    double baScale[renVARYDIMBOUND];
    double caScale[renVARYDIMBOUND];

    vecSubtract(ren->varyDim, beta, alpha, bMinusA);
    vecSubtract(ren->varyDim, gamma, alpha, cMinusA);

    vecScale(ren->varyDim, scalars[0], bMinusA, baScale);
    vecScale(ren->varyDim, scalars[1], cMinusA, caScale);

    vecAdd(ren->varyDim, baScale, caScale, vary);
    vecAdd(ren->varyDim, alpha, vary, vary);
}

void triRenderOrdered(double left[], double right[], double top[], 
                        renRenderer *ren, double unif[], texTexture *tex[]) {

    left[renVARYX] = floor(left[renVARYX]);
    left[renVARYY] = floor(left[renVARYY]);
    right[renVARYX] = floor(right[renVARYX]);
    right[renVARYY] = floor(right[renVARYY]);
    top[renVARYX] = floor(top[renVARYX]);
    top[renVARYY] = floor(top[renVARYY]);

    // int i;
    // for (i = 0; i < ren->varyDim; i += 1) {
    //     printf("L  %c: %f\t", dimTri[i], left[i]);
    // }
    // printf("\n");
    // for (i = 0; i < ren->varyDim; i += 1) {
    //     printf("R  %c: %f\t", dimTri[i], right[i]);
    // }
    // printf("\n");
    // for (i = 0; i < ren->varyDim; i += 1) {
    //     printf("T  %c: %f\t", dimTri[i], top[i]);
    // }
    // printf("\n\n");

    // Ends if triangle is facing away from screen
    double inverse[2][2];
    if (invertMatrix(left, right, top, inverse) <= 0) { return; }

    int triangleType;
    if (top[0] < right[0])
        triangleType = 0;
    else
        triangleType = 1;

    // int i;
    // for (i = 0; i < ren->varyDim; i += 1) {
    //     printf("L %c: %15f\t", dimTri[i], left[i]);
    // }
    // printf("\n");
    // for (i = 0; i < ren->varyDim; i += 1) {
    //     printf("R %c: %15f\t", dimTri[i], right[i]);
    // }
    // printf("\n");
    // for (i = 0; i < ren->varyDim; i += 1) {
    //     printf("T %c: %15f\t", dimTri[i], top[i]);
    // }
    // printf("\n\n");


    // printf("triRender %g %g, %g %g, %g %g\n", left[renVARYX], left[renVARYY],
    //     right[renVARYX], right[renVARYY], top[renVARYX], top[renVARYY]);


    int x, bottomY, topY;
    int width = ren->depth->width;
    int height = ren->depth->height;
    double mBottom = (right[1] - left[1])/(right[0] - left[0]);

    if (triangleType == 0) {
        if ((top[0] - left[0]) != 0) {
            double mTopLeft = (top[1] - left[1])/(top[0] - left[0]);
            for (x = left[0]; x < top[0]; x += 1) {
                if (x > width) { break; }
                else if (x < 0) { continue; }

                topY = floor(left[1] + mTopLeft*(x - left[0]));
                bottomY = ceil(left[1] + mBottom*(x - left[0]));

                int i;
                for (i = bottomY; i <= topY; i += 1) {
                    if (i > height) { break; }
                    else if (i < 0) { continue; }
                    double coords[2] = {(double)x, (double)i};
                    double scalars[2];
                    double vary[renVARYDIMBOUND];
                    double pixelRGBZ[4];

                    findScalars(left, inverse, coords, scalars);
                    interpolateScalars(left, right, top, scalars, vary, ren);
                    ren->colorPixel(ren, unif, tex, vary, pixelRGBZ);

                    if (pixelRGBZ[3] > depthGetZ(ren->depth, x, i)) {
                        depthSetZ(ren->depth, x, i, pixelRGBZ[3]);
                        pixSetRGB(x, i, pixelRGBZ[0], pixelRGBZ[1], pixelRGBZ[2]);
                    }
                }
            }
        }
        if ((right[0] - top[0]) != 0) {
            double mTopRight = (right[1] - top[1])/(right[0] - top[0]);
            for (x = top[0]; x < right[0]; x += 1) {
                if (x > width) { break; }
                else if (x < 0) { continue; }
                topY = floor(top[1] + mTopRight*(x - top[0]));
                bottomY = ceil(left[1] + mBottom*(x - left[0]));

                int i;
                for (i = bottomY; i <= topY; i += 1) {
                    if (i > height) { break; }
                    else if (i < 0) { continue; }
                    double coords[2] = {(double)x, (double)i};
                    double scalars[2];
                    double vary[renVARYDIMBOUND];
                    double pixelRGBZ[4];

                    findScalars(left, inverse, coords, scalars);
                    interpolateScalars(left, right, top, scalars, vary, ren);
                    ren->colorPixel(ren, unif, tex, vary, pixelRGBZ);

                    if (pixelRGBZ[3] > depthGetZ(ren->depth, x, i)) {
                        depthSetZ(ren->depth, x, i, pixelRGBZ[3]);
                        pixSetRGB(x, i, pixelRGBZ[0], pixelRGBZ[1], pixelRGBZ[2]);
                    }
                }
            }
        }
    } else {
        // We avoid divide by zero error because any triangle with a vertical
        // slope is already handled in the above section
        double mTopLeft = (top[1] - left[1])/(top[0] - left[0]);
        double mTopRight = (right[1] - top[1])/(right[0] - top[0]);
        for (x = left[0]; x < right[0]; x += 1) {
            if (x > width) { break; }
            else if (x < 0) { continue; }
            topY = floor(left[1] + mTopLeft*(x - left[0]));
            bottomY = ceil(left[1] + mBottom*(x - left[0]));
            
            int i;
            for (i = bottomY; i <= topY; i += 1) {
                if (i > height) { break; }
                else if (i < 0) { continue; }
                double coords[2] = {(double)x, (double)i};
                double scalars[2];
                double vary[renVARYDIMBOUND];
                double pixelRGBZ[4];

                findScalars(left, inverse, coords, scalars);
                interpolateScalars(left, right, top, scalars, vary, ren);
                ren->colorPixel(ren, unif, tex, vary, pixelRGBZ);

                if (pixelRGBZ[3] > depthGetZ(ren->depth, x, i)) {
                    depthSetZ(ren->depth, x, i, pixelRGBZ[3]);
                    pixSetRGB(x, i, pixelRGBZ[0], pixelRGBZ[1], pixelRGBZ[2]);
                }
            }
        }
        for (x = right[0]; x < top[0]; x += 1) {
            if (x > width) { break; }
            else if (x < 0) { continue; }
            topY = floor(left[1] + mTopLeft*(x - left[0]));
            bottomY = ceil(right[1] + mTopRight*(x - right[0]));

            int i;
            for (i = bottomY; i <= topY; i += 1) {
                if (i > height) { break; }
                else if (i < 0) { continue; }
                double coords[2] = {(double)x, (double)i};
                double scalars[2];
                double vary[renVARYDIMBOUND];
                double pixelRGBZ[4];

                findScalars(left, inverse, coords, scalars);
                interpolateScalars(left, right, top, scalars, vary, ren);
                ren->colorPixel(ren, unif, tex, vary, pixelRGBZ);

                if (pixelRGBZ[3] > depthGetZ(ren->depth, x, i)) {
                    depthSetZ(ren->depth, x, i, pixelRGBZ[3]);
                    pixSetRGB(x, i, pixelRGBZ[0], pixelRGBZ[1], pixelRGBZ[2]);
                }
            }
        }
    }
}

void triRender(renRenderer *ren, double unif[], texTexture *tex[], 
                                double a[], double b[], double c[]) {

    // printf("a: %g %g %g %g\n", a[0], a[1], a[2], a[3]);
    // double viewA[renVARYDIMBOUND];
    // mat441Multiply(ren->viewport, a, viewA);
    // printf("viewA: %g %g %g %g\n", viewA[0], viewA[1], viewA[2], viewA[3]);

    

    // int i;
    // for (i = 0; i < ren->varyDim; i += 1) {
    //     printf("C %c: %f\t", dimTri[i], c[i]);
    // }
    // printf("\n");

    if (a[0] < b[0] && a[0] < c[0])
        triRenderOrdered(a, b, c, ren, unif, tex);
    else if (b[0] < c[0])   // I ignore (b[0] < a[0]) because that produces a visual error
        triRenderOrdered(b, c, a, ren, unif, tex);
    else
        triRenderOrdered(c, a, b, ren, unif, tex);
}
