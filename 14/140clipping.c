/*
 * 140clipping.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

void clipOne(renRenderer *ren, double unif[], texTexture *tex[], 
				double a[], double b[], double c[], int clip[]) {
	
}

void clipTwo(renRenderer *ren, double unif[], texTexture *tex[], 
				double a[], double b[], double c[], int clip[]) {

}

void clipViewPort(double vert[]) {
	double scale[renVARYDIMBOUND];

	vecScale(ren->attrDim, 1.0/vert[renVARYW], vert, scale);
	mat441Multiply(ren->viewport, scale, vert);
}

void clipChecker(renRenderer *ren, double unif[], texTexture *tex[], 
						double a[], double b[], double c[]){

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

	//a is clipped
    if (a[renVARYW] <= 0 || a[renVARYZ] > a[renVARYW])
    	clip[0] = 1;

	//b is clipped
    if (b[renVARYW] <= 0 || b[renVARYZ] > b[renVARYW])
    	clip[1] = 1;
    
    //c is clipped
    if (c[renVARYW] <= 0 || c[renVARYZ] > c[renVARYW])
    	clip[2] = 1;

    //all vertices are clipped, render nothing
    if (clip[0] == 1 && clip[1] == 1 && clip[2] == 1)
    	return	;

    //none are clipped, render triangle
    else if (clip[0] == 0 && clip[1] == 0 && clip[2] == 0)
    {
    	//put the vertices through the viewport transformation
	    clipViewPort(a);
	    clipViewPort(b);
	    clipViewPort(c);
	    triRender(ren, unif, tex, a, b, c);
    }

    //six different cases of clipping
    else{
    	if(clip[0] == 1 && clip[1] == 0 && clip[2] == 0)
    		clipOne(unif, tex, a, b, c, clip);
    	else if(clip[0] == 0 && clip[1] == 1 && clip[2] == 0)
    		clipOne(unif, tex, a, b, c, clip);
    	else if(clip[0] == 0 && clip[1] == 0 && clip[2] == 1) 
    		clipOne(unif, tex, a, b, c, clip);   		
    	else if(clip[0] == 1 && clip[1] == 1 && clip[2] == 0)
       		clipTwo(unif, tex, a, b, c, clip);
    	else if(clip[0] == 0 && clip[1] == 1 && clip[2] == 1)
       		clipTwo(unif, tex, a, b, c, clip);
    	else if(clip[0] == 1 && clip[1] == 0 && clip[2] == 1)
       		clipTwo(unif, tex, a, b, c, clip);
    }
}

void clipRender(renRenderer *ren, double unif[], texTexture *tex[], 
					double a[], double b[], double c[]) {
    //check the orientation of the triangle
    if (((a[0]<=b[0]) && (a[0]<=c[0]))){
        clipChecker(ren,unif,tex,a,b,c); // a is left most
    }
    else if (((b[0]<=c[0]) && (b[0]<=a[0]))){
        clipChecker(ren,unif,tex,b,c,a); //b is left most
    }
    else if (((c[0]<=b[0]) && (c[0]<=a[0]))){
        clipChecker(ren,unif,tex,c,a,b); //c is left most
    }
}