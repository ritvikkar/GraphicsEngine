/*
 * 090renderer.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/

typedef struct renRenderer renRenderer;
struct renRenderer {
	int unifDim; //dimensions for double unif[], eg. rgb, 3
    int texNum; //dimensions for double tex[], eg. brangelina, 1
    int attrDim;//dimensions for double a[], b[], c[], eg. a, α, b, ß, c, γ
    int varyDim;// transformed vertices
    void (*colorPixel)(renRenderer *, double[], texTexture *[], double[], double[]);
    //pointer to the color pixel function
	void (*transformVertex)(renRenderer *, double[], double[], double[]);
	//pointer to the transform vertex function
	void (*updateUniform)(renRenderer *,double[],double[]);
	//pointer to the update uniform function
}; 