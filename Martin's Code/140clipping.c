/* Martin Green (greenm2), CS 311 Graphics */
/* February 2017 */

/* Rescales an array in place using the viewport transformation */
void clipViewport(renRenderer *ren, double a[]) {
    double transferA[4];
    vecScale(4, (double)(1 / a[3]), a, a);
    mat441Multiply(ren->viewport, a, transferA);
    vecCopy(4, transferA, a);
}

/* a is clipped and b isn't. Returns the scaled array in d. */
void clipScaleArray(int varyDim, double a[], double b[], double d[]) {
    double t = (a[renVARYW] - a[renVARYZ]) / 
                (a[renVARYW] - a[renVARYZ] + b[renVARYZ] - b[renVARYW]);
    double bMinusA[renVARYDIMBOUND];
    vecSubtract(varyDim, b, a, bMinusA);
    vecScale(varyDim, t, bMinusA, bMinusA);
    vecAdd(varyDim, a, bMinusA, d);
}

void clipRender(renRenderer *ren, double unif[], texTexture *tex[], 
                                double a[], double b[], double c[]) {

    double viewA[renVARYDIMBOUND];
    vecCopy(renVARYDIMBOUND, a, viewA);

    double viewB[renVARYDIMBOUND];
    vecCopy(renVARYDIMBOUND, b, viewB);

    double viewC[renVARYDIMBOUND];
    vecCopy(renVARYDIMBOUND, c, viewC);

    int aClip = 0, bClip = 0, cClip = 0;

    if (viewA[renVARYW] <= 0.0 || viewA[renVARYW] < viewA[renVARYZ])
        aClip = 1;
    if (viewB[renVARYW] <= 0.0 || viewB[renVARYW] < viewB[renVARYZ])
        bClip = 1;
    if (viewC[renVARYW] <= 0.0 || viewC[renVARYW] < viewC[renVARYZ])
        cClip = 1;

    double viewD[renVARYDIMBOUND];
    double viewE[renVARYDIMBOUND];

    if (aClip && bClip && cClip) {
        // do nothing
    } else if (!aClip && !bClip && !cClip) {
        clipViewport(ren, viewA);
        clipViewport(ren, viewB);
        clipViewport(ren, viewC);

        triRender(ren, unif, tex, viewA, viewB, viewC); // render normally
    } else if (aClip && bClip) {
        clipScaleArray(ren->varyDim, viewA, viewC, viewD);  // find D instead of A
        clipScaleArray(ren->varyDim, viewB, viewC, viewE);  // find E instead of B

        clipViewport(ren, viewD);
        clipViewport(ren, viewE);
        clipViewport(ren, viewC);

        triRender(ren, unif, tex, viewD, viewE, viewC);
    } else if (bClip && cClip) {
        clipScaleArray(ren->varyDim, viewC, viewA, viewD);  // find D instead of C
        clipScaleArray(ren->varyDim, viewB, viewA, viewE);  // find E instead of B

        clipViewport(ren, viewA);
        clipViewport(ren, viewE);
        clipViewport(ren, viewD);

        triRender(ren, unif, tex, viewA, viewE, viewD);
    } else if (aClip && cClip) {
        clipScaleArray(ren->varyDim, viewA, viewB, viewD);  // find D instead of A
        clipScaleArray(ren->varyDim, viewC, viewB, viewE);  // find E instead of C

        clipViewport(ren, viewD);
        clipViewport(ren, viewE);
        clipViewport(ren, viewC);

        triRender(ren, unif, tex, viewD, viewE, viewC);
    } else if (aClip) {
        clipScaleArray(ren->varyDim, viewA, viewC, viewD);  // find D from A to C
        clipScaleArray(ren->varyDim, viewA, viewB, viewE);  // find E from A to B

        clipViewport(ren, viewD);
        clipViewport(ren, viewB);
        clipViewport(ren, viewC);
        clipViewport(ren, viewE);
        
        triRender(ren, unif, tex, viewD, viewB, viewC);
        triRender(ren, unif, tex, viewE, viewB, viewD);
    } else if (bClip) {
        clipScaleArray(ren->varyDim, viewB, viewC, viewD);  // find D from B to C
        clipScaleArray(ren->varyDim, viewB, viewA, viewE);  // find E from B to A

        clipViewport(ren, viewA);
        clipViewport(ren, viewE);
        clipViewport(ren, viewC);
        clipViewport(ren, viewD);

        triRender(ren, unif, tex, viewA, viewE, viewC);
        triRender(ren, unif, tex, viewE, viewD, viewC);
    } else if (cClip) {
        clipScaleArray(ren->varyDim, viewC, viewB, viewD);  // find D from C to B
        clipScaleArray(ren->varyDim, viewC, viewA, viewE);  // find E from C to A

        clipViewport(ren, viewA);
        clipViewport(ren, viewB);
        clipViewport(ren, viewD);
        clipViewport(ren, viewE);

        triRender(ren, unif, tex, viewA, viewB, viewD);
        triRender(ren, unif, tex, viewA, viewD, viewE);
    }
}
