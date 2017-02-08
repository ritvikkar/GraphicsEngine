/*
 * 110triangle.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

/* interpolate the coordenates here and then
   compare depths and pass to colorPixel */
void interpolate(double a[], double b[], double c[],
        double x[], texTexture *tex[], renRenderer *ren, double unif[]){
    //INTERPOLATION BEGINS
    // x = a + p(b - a) +q(c - a)
    // [b-a|c-a]^-1 [x-a] = [p,q]

    double ba[2], ca[2], xa[2];
    vecSubtract(2,c,a,ca); //c-a
    vecSubtract(2,b,a,ba); //b-a
    vecSubtract(2,x,a,xa);

    double baca[2][2];//scratch space
    mat22Columns(ba,ca,baca);// [b-a|c-a]
    double bacaInv[2][2];
    mat22Invert(baca,bacaInv);//[baca]^-1

    if(mat22Invert(baca,bacaInv) <= 0)
        return ;

    double pq[2];
    mat221Multiply(bacaInv,xa,pq);//[p q]

    double bMinusA[renVARYDIMBOUND], cMinusA[renVARYDIMBOUND];
    vecSubtract(ren->varyDim,b,a,bMinusA);//b-a but sized varyDim
    vecSubtract(ren->varyDim,c,a,cMinusA);//c-a but sized varyDim

    double scaleP[ren->varyDim],scaleQ[ren->varyDim],addPQ[ren->varyDim];
    vecScale(ren->varyDim,pq[0],bMinusA,scaleP);//p(b-a)
    vecScale(ren->varyDim,pq[1],cMinusA,scaleQ);//q(c-a)

    vecAdd(ren->varyDim,scaleP,scaleQ,addPQ);
    double khi[ren->varyDim];
    vecAdd(ren->varyDim,addPQ,a,khi);
    
    //INTERPOLATION ENDS
    
    double rgbz[4];

    ren->colorPixel(ren, unif, tex, khi, rgbz); //run color pixel first
    
    /* depth is compared to depth buffer and if the depth 
       is higher then depth buffer is updated */

    if(khi[renVARYZ] > depthGetZ(ren->depth, x[0], x[1])){
        pixSetRGB(x[0],x[1], rgbz[0], rgbz[1], rgbz[2]);
        depthSetZ(ren->depth,x[0],x[1],khi[renVARYZ]);
    }
}

/* helper function recieves the vertices with a to the left
   and perform the actual rasterization */
void triangleALeft(renRenderer *ren, double unif[], texTexture *tex[],
        double a[], double b[], double c[])
{
    int i, j;
    double xHigh, xLow;
    if(c[0]==a[0]){
        for(i = ceil(a[0]); i<= floor(b[0]); i++){
            xHigh = c[1] + ((b[1] - c[1])/(b[0] - c[0]))*(i-c[0]); //lower bound
            xLow = a[1] + ((b[1] - a[1])/(b[0] - a[0]))*(i-a[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++){
                double x[2] = {i, j};
                interpolate(a,b,c,x,tex,ren,unif);
            }
        }
    }

    else if(a[0] == b[0])
    {
        for(i = ceil(a[0]); i<= floor(c[0]); i++){
            xHigh = a[1] + ((c[1] - a[1])/(c[0] - a[0]))*(i-a[0]); //lower bound
            xLow = b[1] + ((c[1] - b[1])/(c[0] - b[0]))*(i-b[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++){
                double x[2] = {i, j};
                interpolate(a,b,c,x,tex,ren,unif);
            }
        }
    }

    else if(c[0]==b[0]){

        for(i = ceil(a[0]); i<= floor(b[0]); i++){
            xHigh = a[1] + ((c[1] - a[1])/(c[0] - a[0]))*(i-a[0]); //lower bound
            xLow = a[1] + ((b[1] - a[1])/(b[0] - a[0]))*(i-a[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++){
                double x[2] = {i, j};
                interpolate(a,b,c,x,tex,ren,unif);
            }
        }
    }

    else if(b[0]<c[0]){
        for(i = ceil(a[0]); i<= floor(b[0]); i++){
            xHigh = a[1] + ((c[1] - a[1])/(c[0] - a[0]))*(i-a[0]); //lower bound
            xLow = a[1] + ((b[1] - a[1])/(b[0] - a[0]))*(i-a[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++){
                double x[2] = {i, j};
                interpolate(a,b,c,x,tex,ren,unif);
            }
        }

        for(i = ceil(b[0]); i<= floor(c[0]); i++){
            xHigh = a[1] + ((c[1] - a[1])/(c[0] - a[0]))*(i-a[0]); //lower bound
            xLow = b[1] + ((c[1] - b[1])/(c[0] - b[0]))*(i-b[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++){
                double x[2] = {i, j};
                interpolate(a,b,c,x,tex,ren,unif);
            }
        }
    }

    else if(c[0]<b[0]){
        for(i = ceil(a[0]); i<= floor(c[0]); i++){
            xHigh = a[1] + ((c[1] - a[1])/(c[0] - a[0]))*(i-a[0]); //lower bound
            xLow = a[1] + ((b[1] - a[1])/(b[0] - a[0]))*(i-a[0]); //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++){
                double x[2] = {i, j};
                interpolate(a,b,c,x,tex,ren,unif);
            }
        }

        for(i = ceil(c[0]); i<= floor(b[0]); i++){
            xHigh = c[1] + ((b[1] - c[1])/(b[0] - c[0]))*(i-c[0]); //lower bound
            xLow = a[1] + ((b[1] - a[1])/(b[0] - a[0]))*(i-a[0]);; //upper bound
            for(j = ceil(xLow); j<= floor(xHigh); j++){
                double x[2] = {i, j};
                interpolate(a,b,c,x,tex,ren,unif);
            }
        } 
    }
}

/* receives a triangle, and sends a triangle with 'a'
   as the left morst vertex */
void triRender(renRenderer *ren, double unif[], texTexture *tex[], double a[], 
        double b[], double c[])
{
    //check the orientation of the triangle
    if (((a[0]<=b[0]) && (a[0]<=c[0]))){
        triangleALeft(ren,unif,tex,a,b,c); // a is left most
    }
    else if (((b[0]<=c[0]) && (b[0]<=a[0]))){
        triangleALeft(ren,unif,tex,b,c,a); //b is left most
    }
    else if (((c[0]<=b[0]) && (c[0]<=a[0]))){
        triangleALeft(ren,unif,tex,c,a,b); //c is left most
    }
}
