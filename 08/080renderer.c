/*
 * 080render.c
 * by Ritvik Kar
 * CS 331: Computer Graphics
*/
#include <stdio.h>
typedef struct renRenderer renRenderer;
struct renRenderer {
	int unifDim; //dimensions for double unif[], eg. rgb, 3
    int texNum; //dimensions for double tex[], eg. brangelina, 1
    int attrDim;//dimensions for double a[], b[], c[], eg. a, α, b, ß, c, γ
    int varyDim;// transformed vertices
}; 