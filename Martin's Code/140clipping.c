/* Martin Green (greenm2), CS 311 Graphics */
/* January 2017 */

char dim[6] = {'X', 'Y', 'Z', 'W', 'S', 'T'};

void clipRenderOrdered(renRenderer *ren, double unif[], texTexture *tex[], 
                            double viewA[], double viewB[], double viewC[]) {



    // int i;
    // for (i = 0; i < ren->varyDim; i += 1) {
        // printf("A %c: %f\t", dim[i], viewA[i]);
    // }
    // printf("\n");
    // for (i = 0; i < ren->varyDim; i += 1) {
        // printf("B %c: %f\t", dim[i], viewB[i]);
    // }
    // printf("\n");
    // for (i = 0; i < ren->varyDim; i += 1) {
        // printf("C %c: %f\t", dim[i], viewC[i]);
    // }
    // printf("\n");

    double transferA[4];
    vecScale(4, (double)(1 / viewA[3]), viewA, viewA);
    mat441Multiply(ren->viewport, viewA, transferA);
    vecCopy(4, transferA, viewA);

    double transferB[4];
    vecScale(4, (double)(1 / viewB[3]), viewB, viewB);
    mat441Multiply(ren->viewport, viewB, transferB);
    vecCopy(4, transferB, viewB);

    double transferC[4];
    vecScale(4, (double)(1 / viewC[3]), viewC, viewC);
    mat441Multiply(ren->viewport, viewC, transferC);
    vecCopy(4, transferC, viewC);

    triRender(ren, unif, tex, viewA, viewB, viewC);
}

/* a is clipped and b isn't. Returns the scaled array in d. */
void clipScaleArray(int varyDim, double a[], double b[], double e[]) {
    double t = (a[renVARYW] - a[renVARYZ]) / (a[renVARYW] - a[renVARYZ] + b[renVARYZ] - b[renVARYW]);
    double bMinusA[renVARYDIMBOUND];
    vecSubtract(varyDim, b, a, bMinusA);
    vecScale(varyDim, t, bMinusA, bMinusA);
    vecAdd(varyDim, a, bMinusA, e);
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

    if (viewA[renVARYW] <= 0 || viewA[renVARYW] < viewA[renVARYW])
        aClip = 1;
    if (viewB[renVARYW] <= 0 || viewB[renVARYW] < viewB[renVARYW])
        bClip = 1;
    if (viewC[renVARYW] <= 0 || viewC[renVARYW] < viewC[renVARYW])
        cClip = 1;

    double viewD[renVARYDIMBOUND];
    double viewE[renVARYDIMBOUND];
    double t;

    if (aClip && bClip && cClip) {
        // do nothing
    } else if (!aClip && !bClip && !cClip) {
        // render normally
        clipRenderOrdered(ren, unif, tex, viewA, viewB, viewC);
    } else if (aClip && bClip) {
        printf("ab clip\n");
        // find D instead of A
        // t = (viewA[renVARYW] - viewA[renVARYZ]) / (viewA[renVARYW] - viewA[renVARYZ] + viewC[renVARYZ] - viewC[renVARYW]);
        // double cMinusA[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewC, viewA, cMinusA);
        // vecScale(ren->varyDim, t, cMinusA, cMinusA);
        // vecAdd(ren->varyDim, viewA, cMinusA, viewD);
        clipScaleArray(ren->varyDim, viewA, viewC, viewD);

        // find E instead of B
        // t = (viewB[renVARYW] - viewB[renVARYZ]) / (viewB[renVARYW] - viewB[renVARYZ] + viewC[renVARYZ] - viewC[renVARYW]);
        // double cMinusB[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewC, viewB, cMinusB);
        // vecScale(ren->varyDim, t, cMinusB, cMinusB);
        // vecAdd(ren->varyDim, viewB, cMinusB, viewE);
        clipScaleArray(ren->varyDim, viewB, viewC, viewE);

        clipRenderOrdered(ren, unif, tex, viewD, viewE, viewC);
    } else if (bClip && cClip) {
        printf("bc clip\n");
        // find D instead of C
        clipScaleArray(ren->varyDim, viewC, viewA, viewD);
        // t = (viewC[renVARYW] - viewC[renVARYZ]) / (viewC[renVARYW] - viewC[renVARYZ] + viewA[renVARYZ] - viewA[renVARYW]);
        // double aMinusC[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewA, viewC, aMinusC);
        // vecScale(ren->varyDim, t, aMinusC, aMinusC);
        // vecAdd(ren->varyDim, viewC, aMinusC, viewD);

        // find E instead of B
        clipScaleArray(ren->varyDim, viewB, viewA, viewE);
        // t = (viewB[renVARYW] - viewB[renVARYZ]) / (viewB[renVARYW] - viewB[renVARYZ] + viewA[renVARYZ] - viewA[renVARYW]);
        // double aMinusB[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewA, viewB, aMinusB);
        // vecScale(ren->varyDim, t, aMinusB, aMinusB);
        // vecAdd(ren->varyDim, viewB, aMinusB, viewE);

        clipRenderOrdered(ren, unif, tex, viewA, viewE, viewD);
    } else if (aClip && cClip) {
        printf("ac clip\n");
        // find D instead of A
        // clipScaleArray(int varyDim, double a[], double b[], double e[])
        clipScaleArray(ren->varyDim, viewA, viewB, viewD);

        // t = (viewA[renVARYW] - viewA[renVARYZ]) / (viewA[renVARYW] - viewA[renVARYZ] + viewB[renVARYZ] - viewB[renVARYW]);
        // double bMinusA[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewB, viewA, bMinusA);
        // vecScale(ren->varyDim, t, bMinusA, bMinusA);
        // vecAdd(ren->varyDim, viewA, bMinusA, viewD);

        // find E instead of C
        clipScaleArray(ren->varyDim, viewC, viewB, viewE);
        // t = (viewC[renVARYW] - viewC[renVARYZ]) / (viewC[renVARYW] - viewC[renVARYZ] + viewB[renVARYZ] - viewB[renVARYW]);
        // double bMinusC[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewB, viewC, bMinusC);
        // vecScale(ren->varyDim, t, bMinusC, bMinusC);
        // vecAdd(ren->varyDim, viewC, bMinusC, viewE);

        clipRenderOrdered(ren, unif, tex, viewD, viewE, viewC);
    } else if (aClip) {
        printf("a clip\n");
        // find D from A to C
        // t = (viewA[renVARYW] - viewA[renVARYZ]) / (viewA[renVARYW] - viewA[renVARYZ] + viewC[renVARYZ] - viewC[renVARYW]);
        // double cMinusA[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewC, viewA, cMinusA);
        // vecScale(ren->varyDim, t, cMinusA, cMinusA);
        // vecAdd(ren->varyDim, viewA, cMinusA, viewD);
        clipScaleArray(ren->varyDim, viewA, viewC, viewD);

        // find E from A to B
        // t = (viewA[renVARYW] - viewA[renVARYZ]) / (viewA[renVARYW] - viewA[renVARYZ] + viewB[renVARYZ] - viewB[renVARYW]);
        // double bMinusA[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewB, viewA, bMinusA);
        // vecScale(ren->varyDim, t, bMinusA, bMinusA);
        // vecAdd(ren->varyDim, viewA, bMinusA, viewE);
        clipScaleArray(ren->varyDim, viewA, viewB, viewE);

        clipRenderOrdered(ren, unif, tex, viewD, viewB, viewC);
        clipRenderOrdered(ren, unif, tex, viewE, viewB, viewD);
    } else if (bClip) {
        printf("b clip\n");
        // find D from B to C
        // t = (viewB[renVARYW] - viewB[renVARYZ]) / (viewB[renVARYW] - viewB[renVARYZ] + viewC[renVARYZ] - viewC[renVARYW]);
        // double cMinusB[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewC, viewB, cMinusB);
        // vecScale(ren->varyDim, t, cMinusB, cMinusB);
        // vecAdd(ren->varyDim, viewB, cMinusB, viewD);
        clipScaleArray(ren->varyDim, viewB, viewC, viewD);

        // find E from B to A
        // t = (viewB[renVARYW] - viewB[renVARYZ]) / (viewB[renVARYW] - viewB[renVARYZ] + viewA[renVARYZ] - viewA[renVARYW]);
        // double aMinusB[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewA, viewB, aMinusB);
        // vecScale(ren->varyDim, t, aMinusB, aMinusB);
        // vecAdd(ren->varyDim, viewB, aMinusB, viewE);
        clipScaleArray(ren->varyDim, viewB, viewA, viewE);

        clipRenderOrdered(ren, unif, tex, viewA, viewE, viewC);
        clipRenderOrdered(ren, unif, tex, viewE, viewD, viewC);
    } else if (cClip) {
        printf("c clip\n");
        // find D from C to B
        // t = (viewC[renVARYW] - viewC[renVARYZ]) / (viewC[renVARYW] - viewC[renVARYZ] + viewB[renVARYZ] - viewB[renVARYW]);
        // double bMinusC[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewB, viewC, bMinusC);
        // vecScale(ren->varyDim, t, bMinusC, bMinusC);
        // vecAdd(ren->varyDim, viewC, bMinusC, viewD);
        clipScaleArray(ren->varyDim, viewC, viewB, viewD);

        // find E from C to A
        // t = (viewC[renVARYW] - viewC[renVARYZ]) / (viewC[renVARYW] - viewC[renVARYZ] + viewA[renVARYZ] - viewA[renVARYW]);
        // double aMinusC[renVARYDIMBOUND];
        // vecSubtract(ren->varyDim, viewA, viewC, aMinusC);
        // vecScale(ren->varyDim, t, aMinusC, aMinusC);
        // vecAdd(ren->varyDim, viewC, aMinusC, viewE);
        clipScaleArray(ren->varyDim, viewC, viewA, viewE);

        clipRenderOrdered(ren, unif, tex, viewA, viewB, viewD);
        clipRenderOrdered(ren, unif, tex, viewA, viewD, viewE);
    }






    // double viewing[renVARYDIMBOUND * 3];
    // double transfer[4];

    // vecCopy(renVARYDIMBOUND, a, &viewing[renVARYDIMBOUND * 0]);
    // vecCopy(renVARYDIMBOUND, b, &viewing[renVARYDIMBOUND * 1]);
    // vecCopy(renVARYDIMBOUND, c, &viewing[renVARYDIMBOUND * 2]);

    // int i;
    // for (i = 0; i < 3; i += 1){
    //     vecScale(4, (double)(1 / viewing[3 + renVARYDIMBOUND * i]), 
    //        &viewing[renVARYDIMBOUND * i], &viewing[renVARYDIMBOUND * i]);
    //     mat441Multiply(ren->viewport, &viewing[renVARYDIMBOUND * i], transfer);
    //     vecCopy(4, transfer, &viewing[renVARYDIMBOUND * i]);
    // }

    // triRender(ren, unif, tex, &viewing[renVARYDIMBOUND], 
    //    &viewing[renVARYDIMBOUND * 1], &viewing[renVARYDIMBOUND * 2]);
}
