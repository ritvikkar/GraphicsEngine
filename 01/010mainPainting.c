/* In this tutorial our compiler invocation becomes more complicated. On macOS, 
we compile with...
    clang 000linking.c 000pixel.o -lglfw -framework OpenGL
This invocation causes our compiled code to be 'linked' with three other pieces 
of software, that have been compiled elsewhere: OpenGL, GLFW, and pixel.o. 
OpenGL is a cross-platform, standardized library for interacting with graphics 
hardware. It comes pre-installed on macOS and Linux, and is easily installed on 
Windows. GLFW is a cross-platform, minimalist user interface toolkit, that we 
can use to build simple OpenGL applications. Often it is not pre-installed, but 
it is easy to install. pixel.o is a library that I have written, to provide a 
simple foundation for this graphics course. You must have pixel.o and pixel.h 
in the same directory as linking.c. */

/* Notice that we include pixel.h, enclosed in quotation marks rather than 
angle brackets. Feel free to read pixel.h. It documents all of the public 
functionality of the library pixel.o. */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "000pixel.h"

/* This function is a user interface 'callback'. Once we register it with the 
user interface infrastructure, it is called whenever the user releases a 
keyboard key. For details, see the pixSetKeyUpHandler documentation. */
int color;
void handleKeyDown(int key, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown){
/*
    if(key == 82)
        pixClearRGB(1.0, 0.0, 0.0);
    
    else if(key == 71)
        pixClearRGB(0.0, 1.0, 0.0);
    
    else if(key == 66)
        pixClearRGB(0.0, 0.0, 1.0);
*/
    //R, G, B, C, M, Y, K, and W 
    if(key == 88)//clearscreen
    {
        pixClearRGB(0.0, 0.0, 0.0);
    }

    else if(key == 82 || key == 71 || key == 66 || key == 67 || key == 77 || key == 89 || key == 75 || key == 87)
        color = key;
        
    else if(key == 256) //exit sequence
        exit(1);
    
    printf("key down %d, shift %d, control %d, altOpt %d, supComm %d\n",
        key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
}

void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown) {
	printf("key up %d, shift %d, control %d, altOpt %d, supComm %d\n",
		key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
}

void handleKeyRepeat(int key, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown){
    printf("key down %d, shift %d, control %d, altOpt %d, supComm %d\n",
        key, shiftIsDown, controlIsDown, altOptionIsDown, superCommandIsDown);
}

/* Similarly, the following callbacks handle some mouse interactions. */
int down = 1;
int up = 0;

void handleMouseDown(int button, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown){
    down = button;
    up = 0;
    printf("mouse down %d, shift %d, control %d, altOpt %d, supComm %d\n",
        button, shiftIsDown, controlIsDown, altOptionIsDown, 
        superCommandIsDown);
      
}

void handleMouseUp(int button, int shiftIsDown, int controlIsDown,
		int altOptionIsDown, int superCommandIsDown) {
    up = 1;
	printf("mouse up %d, shift %d, control %d, altOpt %d, supComm %d\n",
		button, shiftIsDown, controlIsDown, altOptionIsDown, 
		superCommandIsDown); 
}

int stroke = 1;
void handleMouseScroll(double xOffset, double yOffset) {
    stroke = yOffset;
    printf("mouse scroll xOffset %f yOffset %f\n", xOffset, yOffset);
}

void handleMouseMove(double x, double y) {
	printf("mouse move x %f y %f\n", x, y);
    //    if(key == 82 || key == 71 || key == 66 || key == 67 || key == 77 || key == 89 || key == 75 || key == 87)
    if(stroke < 1)
        stroke = 1;//minimum stroke of 1
    if(down == 0 && up == 0){
        int i;
        if(color == 82){ //R
            for(i = 0; i< stroke; i++){
                pixSetRGB(x+i, y, 1.0, 0.0, 0.0);
                pixSetRGB(x-i, y, 1.0, 0.0, 0.0);
                pixSetRGB(x, y+i, 1.0, 0.0, 0.0);
                pixSetRGB(x, y-i, 1.0, 0.0, 0.0);
                pixSetRGB(x+i, y+i, 1.0, 0.0, 0.0);
                pixSetRGB(x+i, y-i, 1.0, 0.0, 0.0);
                pixSetRGB(x-i, y+i, 1.0, 0.0, 0.0);
                pixSetRGB(x-i, y-i, 1.0, 0.0, 0.0);
            }
        }
        
        else if(color == 71) //G
        {
            for(i = 0; i< stroke; i++){
                pixSetRGB(x+i, y, 0.0, 1.0, 0.0);
                pixSetRGB(x-i, y, 0.0, 1.0, 0.0);
                pixSetRGB(x, y+i, 0.0, 1.0, 0.0);
                pixSetRGB(x, y-i, 0.0, 1.0, 0.0);
                pixSetRGB(x+i, y+i, 0.0, 1.0, 0.0);
                pixSetRGB(x+i, y-i, 0.0, 1.0, 0.0);
                pixSetRGB(x-i, y+i, 0.0, 1.0, 0.0);
                pixSetRGB(x-i, y-i, 0.0, 1.0, 0.0);
            }
        }
        
        else if(color == 66) //B
        {
            for(i = 0; i< stroke; i++){
                pixSetRGB(x+i, y, 0.0, 0.0, 1.0);
                pixSetRGB(x-i, y, 0.0, 0.0, 1.0);
                pixSetRGB(x, y+i, 0.0, 0.0, 1.0);
                pixSetRGB(x, y-i, 0.0, 0.0, 1.0);
                pixSetRGB(x+i, y+i, 0.0, 0.0, 1.0);
                pixSetRGB(x+i, y-i, 0.0, 0.0, 1.0);
                pixSetRGB(x-i, y+i, 0.0, 0.0, 1.0);
                pixSetRGB(x-i, y-i, 0.0, 0.0, 1.0);
            }
        }
        
        else if(color == 67) //C
        {
            for(i = 0; i< stroke; i++){
                pixSetRGB(x+i, y, 0.0, 1.0, 1.0);
                pixSetRGB(x-i, y, 0.0, 1.0, 1.0);
                pixSetRGB(x, y+i, 0.0, 1.0, 1.0);
                pixSetRGB(x, y-i, 0.0, 1.0, 1.0);
                pixSetRGB(x+i, y+i, 0.0, 1.0, 1.0);
                pixSetRGB(x+i, y-i, 0.0, 1.0, 1.0);
                pixSetRGB(x-i, y+i, 0.0, 1.0, 1.0);
                pixSetRGB(x-i, y-i, 0.0, 1.0, 1.0);
            }
        }

        else if(color == 77) //M
        {
            for(i = 0; i< stroke; i++){
                pixSetRGB(x+i, y, 1.0, 0.0, 1.0);
                pixSetRGB(x-i, y, 1.0, 0.0, 1.0);
                pixSetRGB(x, y+i, 1.0, 0.0, 1.0);
                pixSetRGB(x, y-i, 1.0, 0.0, 1.0);
                pixSetRGB(x+i, y+i, 1.0, 0.0, 1.0);
                pixSetRGB(x+i, y-i, 1.0, 0.0, 1.0);
                pixSetRGB(x-i, y+i, 1.0, 0.0, 1.0);
                pixSetRGB(x-i, y-i, 1.0, 0.0, 1.0);
            }
        }
        
        else if(color == 89) //Y
        {
            for(i = 0; i< stroke; i++){
                pixSetRGB(x+i, y, 1.0, 1.0, 0.0);
                pixSetRGB(x-i, y, 1.0, 1.0, 0.0);
                pixSetRGB(x, y+i, 1.0, 1.0, 0.0);
                pixSetRGB(x, y-i, 1.0, 1.0, 0.0);
                pixSetRGB(x+i, y+i, 1.0, 1.0, 0.0);
                pixSetRGB(x+i, y-i, 1.0, 1.0, 0.0);
                pixSetRGB(x-i, y+i, 1.0, 1.0, 0.0);
                pixSetRGB(x-i, y-i, 1.0, 1.0, 0.0);
            }
        }
        
        else if(color == 75) //K
        {
            for(i = 0; i< stroke; i++){
                pixSetRGB(x+i, y, 0.0, 0.0, 0.0);
                pixSetRGB(x-i, y, 0.0, 0.0, 0.0);
                pixSetRGB(x, y+i, 0.0, 0.0, 0.0);
                pixSetRGB(x, y-i, 0.0, 0.0, 0.0);
                pixSetRGB(x+i, y+i, 0.0, 0.0, 0.0);
                pixSetRGB(x+i, y-i, 0.0, 0.0, 0.0);
                pixSetRGB(x-i, y+i, 0.0, 0.0, 0.0);
                pixSetRGB(x-i, y-i, 0.0, 0.0, 0.0);
            }
        }
        
        else //Ww
        {
            for(i = 0; i< stroke; i++){
                pixSetRGB(x+i, y, 1.0, 1.0, 1.0);
                pixSetRGB(x-i, y, 1.0, 1.0, 1.0);
                pixSetRGB(x, y+i, 1.0, 1.0, 1.0);
                pixSetRGB(x, y-i, 1.0, 1.0, 1.0);
                pixSetRGB(x+i, y+i, 1.0, 1.0, 1.0);
                pixSetRGB(x+i, y-i, 1.0, 1.0, 1.0);
                pixSetRGB(x-i, y+i, 1.0, 1.0, 1.0);
                pixSetRGB(x-i, y-i, 1.0, 1.0, 1.0);
            }
        }
    }
}


/* This callback is called once per animation frame. As parameters it receives 
the time for the current frame and the time for the previous frame. Both times 
are measured in seconds since some distant past time. */ 
void handleTimeStep(double oldTime, double newTime) {
	if (floor(newTime) - floor(oldTime) >= 1.0)
		printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
}
/* You can also set callbacks for key down, key repeat, and mouse down. See 
000pixel.h for details. */

int main(void) {
	/* Make a 512 x 512 window with the title 'Pixel Graphics'. This function 
	returns 0 if no error occurred. */
	if (pixInitialize(512, 512, "Pixel Graphics") != 0)
		return 1;
	else {
		/* Register the callbacks (defined above) with the user interface, so 
		that they are called as needed during pixRun (invoked below). */
        pixSetKeyDownHandler(handleKeyDown);
        pixSetKeyUpHandler(handleKeyUp);
        pixSetKeyRepeatHandler(handleKeyRepeat);
		pixSetMouseUpHandler(handleMouseUp);
        pixSetMouseDownHandler(handleMouseDown);
		pixSetMouseMoveHandler(handleMouseMove);
		pixSetMouseScrollHandler(handleMouseScroll);
		pixSetTimeStepHandler(handleTimeStep);
		/* Clear the window to black. */
		pixClearRGB(0.0, 0.0, 0.0);
		/* Draw the graph of y = 500 sin (x / 50) in yellow. Notice that 
		pixSetPixel ignores attempts to set pixels outside the window. */
		int i;
                
		/* Run the event loop. The callbacks that were registered above are 
		invoked as needed. At the end, the resources supporting the window are 
		deallocated. */
        pixRun();
		return 0;
	}
}
