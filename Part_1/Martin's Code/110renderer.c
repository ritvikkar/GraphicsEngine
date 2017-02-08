/* Martin Green (greenm2), CS 311 Graphics */
/* January 2017 */

typedef struct renRenderer renRenderer;
struct renRenderer {
    int unifDim, texNum, attrDim, varyDim;
    depthBuffer *depth;
    void (*colorPixel)(renRenderer*, double[], texTexture**, double[], double[]);
    void (*transformVertex)(renRenderer*, double[], double[], double[]);
    void (*updateUniform)(renRenderer*, double[], double[]);
};
