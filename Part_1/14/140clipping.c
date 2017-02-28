/*
 * 140clipping.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

/* clipViewPort performs the viewport transformation and the homogeneous division
 * on a given vertex
 */
void clipViewPort(renRenderer *ren, double vertex[], double vertexViewPort[]) {
    double scale[4];
    vecScale(4, 1.0/vertex[renVARYW], vertex, scale);
    mat441Multiply(ren->viewport, scale, vertexViewPort);
    int i;    
    vecCopy(ren->varyDim-4,&vertex[4],&vertexViewPort[4]);    
}

/* clipScale takes in two points a,b and finds a new point between them */
void clipScale(renRenderer *ren, double unif[], texTexture *tex[],
                double a[], double b[], double newPoint[])
{
    double bMinusA[renVARYDIMBOUND];
    vecSubtract(ren->varyDim,b,a,bMinusA);
    double t1 = (a[3]-a[2])/(a[3]-a[2]+b[2]-b[3]);
    vecScale(ren->attrDim, t1, bMinusA, newPoint);
}

/* clipOne, i.e. one side is clipped, will emit two triangles */
void clipOne(renRenderer *ren, double unif[], texTexture *tex[], 
				double a[], double b[], double c[], int clip[]) {
    //new points = t(b-a) & t(c-a)
    //t = (a3 - a2) / (a3 - a2 + b2 - b3)

    double point1[renVARYDIMBOUND],point2[renVARYDIMBOUND];    
    clipScale(ren,unif,tex,a,b,point1);
    clipScale(ren,unif,tex,a,b,point2);        

    double point1_ViewPort[renVARYDIMBOUND], point2_ViewPort[renVARYDIMBOUND],b_ViewPort[renVARYDIMBOUND], c_ViewPort[renVARYDIMBOUND];

    clipViewPort(ren,point1,point1_ViewPort);
    clipViewPort(ren,point2,point2_ViewPort);    
    clipViewPort(ren,b,b_ViewPort);
    clipViewPort(ren,c,c_ViewPort);

    triRender(ren, unif, tex, point1_ViewPort, b_ViewPort, c_ViewPort);
    triRender(ren, unif, tex, point2_ViewPort, b_ViewPort, point1_ViewPort);    
}

/* clipTwo, i.e. two sides are clipped, will emit one triangle */
void clipTwo(renRenderer *ren, double unif[], texTexture *tex[], 
				double a[], double b[], double c[], int clip[]) {
    
    double point1[renVARYDIMBOUND],point2[renVARYDIMBOUND];    
    clipScale(ren,unif,tex,a,b,point1);
    clipScale(ren,unif,tex,a,b,point2);        

    double point1_ViewPort[renVARYDIMBOUND], point2_ViewPort[renVARYDIMBOUND],b_ViewPort[renVARYDIMBOUND];

    clipViewPort(ren,point1,point1_ViewPort);
    clipViewPort(ren,point2,point2_ViewPort);    
    clipViewPort(ren,b,b_ViewPort);
    triRender(ren, unif, tex, point2_ViewPort, b_ViewPort, point1_ViewPort);
}

/* chipChecker checks the type of clipping that has occured and assigns
 * which helper should be called
 */
void clipChecker(renRenderer *ren, double unif[], texTexture *tex[], 
						double a[], double b[], double c[]){
    //backface culling
    double ba[2], ca[2], xa[2];
    vecSubtract(2,c,a,ca); //c-a
    vecSubtract(2,b,a,ba); //b-a

    double baca[2][2];//scratch space
    mat22Columns(ba,ca,baca);// [b-a|c-a]
    double bacaInv[2][2];
    mat22Invert(baca,bacaInv);//[baca]^-1

    if(mat22Invert(baca,bacaInv) <= 0)
        return ;
    
    int clip[3];//array to keep flags for clipping
    clip[0]=clip[1]=clip[2]=0;

	//a is clipped
    if (a[renVARYW] <= 0 || a[renVARYZ] > a[renVARYW])
    	clip[0] = 1;

	//b is clipped
    if (b[renVARYW] <= 0 || b[renVARYZ] > b[renVARYW])
    	clip[1] = 1;
    
    //c is clipped
    if (c[renVARYW] <= 0 || c[renVARYZ] > c[renVARYW])
    	clip[2] = 1;

    //all vertices are clipped,render nothing
    if (clip[0] == 1 && clip[1] == 1 && clip[2] == 1)
    	return	;
    
    //none are clipped, render triangle
    else if (clip[0] == 0 && clip[1] == 0 && clip[2] == 0)
    {
    	//put the vertices through the viewport transformation
        double a_viewPort[renVARYDIMBOUND], b_ViewPort[renVARYDIMBOUND], c_ViewPort[renVARYDIMBOUND];        
	    clipViewPort(ren,a,a_viewPort);
	    clipViewPort(ren,b,b_ViewPort);
	    clipViewPort(ren,c,c_ViewPort);
	    triRender(ren, unif, tex, a_viewPort, b_ViewPort, c_ViewPort);
    }

    //six remaining cases of clipping
    else{
    	if(clip[0] == 1 && clip[1] == 0 && clip[2] == 0)
    		clipOne(ren,unif, tex, a, b, c, clip);
    	else if(clip[0] == 0 && clip[1] == 1 && clip[2] == 0)
    		clipOne(ren,unif, tex, a, b, c, clip);
    	else if(clip[0] == 0 && clip[1] == 0 && clip[2] == 1) 
    		clipOne(ren,unif, tex, a, b, c, clip);   		
    	else if(clip[0] == 1 && clip[1] == 1 && clip[2] == 0)
       		clipTwo(ren,unif, tex, a, b, c, clip);
    	else if(clip[0] == 0 && clip[1] == 1 && clip[2] == 1)
       		clipTwo(ren,unif, tex, a, b, c, clip);
    	else if(clip[0] == 1 && clip[1] == 0 && clip[2] == 1)
       		clipTwo(ren,unif, tex, a, b, c, clip);
    }
}

void clipRender(renRenderer *ren, double unif[], texTexture *tex[], 
					double a[], double b[], double c[]) {
    //check the orientation of the triangle
    if (((a[renVARYW]<=b[renVARYW]) && (a[renVARYW]<=c[renVARYW]))){
        clipChecker(ren,unif,tex,a,b,c); // a is left most
    }
    else if (((b[renVARYW]<=c[renVARYW]) && (b[renVARYW]<=a[renVARYW]))){
        clipChecker(ren,unif,tex,b,c,a); //b is left most
    }
    else if (((c[renVARYW]<=b[renVARYW]) && (c[renVARYW]<=a[renVARYW]))){
        clipChecker(ren,unif,tex,c,a,b); //c is left most
    }
}