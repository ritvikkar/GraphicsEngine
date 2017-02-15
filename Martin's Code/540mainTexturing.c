


/* On macOS, compile with...
    clang 530mainScene.c -lglfw -framework OpenGL
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <GLFW/glfw3.h>

#include "500shader.c"
#include "530vector.c"
#include "510mesh.c"
#include "520matrix.c"
#include "520camera.c"
#include "540texture.c"
#include "540scene.c"

#define triNum 8
#define vertNum 6
#define attrDim 8

GLdouble alpha = 0.0;
GLuint program;
GLint attrLocs[3];
GLint viewingLoc, modelingLoc;
GLint unifLocs[1];
GLuint textureA, textureB, textureC;
texTexture texA, texB, texC;
GLint texCoordsLoc[3], textureLoc[3];
camCamera cam;
/* Allocate three meshes and three scene graph nodes. */
meshGLMesh rootMesh, childMesh, siblingMesh;
sceneNode rootNode, childNode, siblingNode;

void handleError(int error, const char *description) {
    fprintf(stderr, "handleError: %d\n%s\n", error, description);
}

void handleResize(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    camSetWidthHeight(&cam, width, height);
}

void handleKey(GLFWwindow *window, int key, int scancode, int action,
        int mods) {
    int shiftIsDown = mods & GLFW_MOD_SHIFT;
    int controlIsDown = mods & GLFW_MOD_CONTROL;
    int altOptionIsDown = mods & GLFW_MOD_ALT;
    int superCommandIsDown = mods & GLFW_MOD_SUPER;
    if (action == GLFW_PRESS && key == GLFW_KEY_L) {
        camSwitchProjectionType(&cam);
    } else if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_O)
            camAddTheta(&cam, -0.1);
        else if (key == GLFW_KEY_P)
            camAddTheta(&cam, 0.1);
        else if (key == GLFW_KEY_I)
            camAddPhi(&cam, -0.1);
        else if (key == GLFW_KEY_K)
            camAddPhi(&cam, 0.1);
        else if (key == GLFW_KEY_U)
            camAddDistance(&cam, -0.1);
        else if (key == GLFW_KEY_J)
            camAddDistance(&cam, 0.1);
    }
}

// void initializeMesh(void) {
//     /* Append two texture coordinates to each vertex from 500openGL20b.c. 
//     Notice that attrDim, above, is now 8. */
//     GLdouble attributes[vertNum * attrDim] = {
//         1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.5, 
//         -1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.5, 0.5, 
//         0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.25, 0.5, 
//         0.0, -1.0, 0.0, 0.0, 1.0, 1.0, 0.75, 0.5, 
//         0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0, 
//         0.0, 0.0, -1.0, 1.0, 0.0, 1.0, 0.0, 0.0};
//     /* The rest of this function is the same as in 500openGL20b.c. */
//     GLuint triangles[triNum * 3] = {
//         0, 2, 4, 
//         2, 1, 4, 
//         1, 3, 4, 
//         3, 0, 4, 
//         2, 0, 5, 
//         1, 2, 5, 
//         3, 1, 5, 
//         0, 3, 5};
//     glGenBuffers(2, buffers);
    
//     glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
//     glBufferData(GL_ARRAY_BUFFER, vertNum * attrDim * sizeof(GLdouble),
//         (GLvoid *)attributes, GL_STATIC_DRAW);

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, triNum * 3 * sizeof(GLuint),
//         (GLvoid *)triangles, GL_STATIC_DRAW);
// }

/* Returns 0 on success, non-zero on failure. Warning: If initialization fails 
midway through, then does not properly deallocate all resources. But that's 
okay, because the program terminates almost immediately after this function 
returns. */
int initializeScene(void) {
    /* Initialize meshes. */
    meshMesh mesh;
    if (meshInitializeCapsule(&mesh, 0.5, 2.0, 16, 32) != 0)
        return 1;
    meshGLInitialize(&rootMesh, &mesh);
    meshDestroy(&mesh);
    if (meshInitializeBox(&mesh, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5) != 0)
        return 2;
    meshGLInitialize(&childMesh, &mesh);
    meshDestroy(&mesh);
    if (meshInitializeSphere(&mesh, 0.5, 16, 32) != 0)
        return 3;
    meshGLInitialize(&siblingMesh, &mesh);
    meshDestroy(&mesh);
    /* Initialize scene graph nodes. */
    if (sceneInitialize(&siblingNode, 2, 2, &siblingMesh, NULL, NULL) != 0)
        return 4;
    if (sceneInitialize(&childNode, 2, 2, &childMesh, NULL, NULL) != 0)
        return 5;
    // if (sceneInitialize(&rootNode, 2, 2, &rootMesh, &childNode, &siblingNode) != 0)
    if (sceneInitialize(&rootNode, 2, 2, &rootMesh, NULL, NULL) != 0)
        return 6;
    /* Customize the uniforms. */
    GLdouble trans[3] = {1.0, 0.0, 0.0};
    sceneSetTranslation(&childNode, trans);
    vecSet(3, trans, 0.0, 1.0, 0.0);
    sceneSetTranslation(&siblingNode, trans);
    GLdouble unif[2] = {1.0, 1.0};
    sceneSetUniform(&siblingNode, unif);
    sceneSetUniform(&childNode, unif);
    sceneSetUniform(&rootNode, unif);
    return 0;
}

void destroyScene(void) {
    meshGLDestroy(&siblingMesh);
    meshGLDestroy(&childMesh);
    meshGLDestroy(&rootMesh);
    sceneDestroyRecursively(&rootNode);
}

/* Returns 0 on success, non-zero on failure. */
int initializeShaderProgram(void) {
    /* GLchar vertexCode[] = "\
        uniform mat4 viewing;\
        uniform mat4 modeling;\
        attribute vec3 position;\
        attribute vec2 texCoords;\
        attribute vec3 normal;\
        uniform vec2 spice;\
        varying vec4 rgba;\
        void main() {\
            gl_Position = viewing * modeling * vec4(position, 1.0);\
            rgba = vec4(texCoords, spice) + vec4(normal, 1.0);\
         }"; */
    GLchar vertexCode[] = "\
        uniform mat4 viewing;\
        uniform mat4 modeling;\
        attribute vec3 position;\
        attribute vec3 color;\
        attribute vec2 texCoords;\
        varying vec4 rgba;\
        varying vec2 st;\
        void main() {\
            gl_Position = viewing * modeling * vec4(position, 1.0);\
            rgba = vec4(color, 1.0);\
            st = texCoords;\
        }";
    /* GLchar fragmentCode[] = "\
        varying vec4 rgba;\
        void main() {\
            gl_FragColor = rgba;\
        }"; */
    GLchar fragmentCode[] = "\
        uniform sampler2D texture;\
        varying vec4 rgba;\
        varying vec2 st;\
        void main() {\
            gl_FragColor = rgba * texture2D(texture, st);\
        }";
    program = makeProgram(vertexCode, fragmentCode);
    if (program != 0) {
        glUseProgram(program);
        attrLocs[0] = glGetAttribLocation(program, "position");
        attrLocs[1] = glGetAttribLocation(program, "texCoords");
        // attrLocs[2] = glGetAttribLocation(program, "normal");
        attrLocs[2] = glGetAttribLocation(program, "color");
        viewingLoc = glGetUniformLocation(program, "viewing");
        modelingLoc = glGetUniformLocation(program, "modeling");
        unifLocs[0] = glGetUniformLocation(program, "spice");
        texCoordsLoc[0] = glGetAttribLocation(program, "texCoords");
        textureLoc[0] = glGetUniformLocation(program, "texture");
    }
    return (program == 0);
}

// /* Loads the given image file into an OpenGL texture. The width and height of 
// the image must be powers of 2. Returns 0 on success, non-zero on failure. On 
// success, the user must later deallocate the texture using a call like 
// glDeleteTextures(1, texture). */
// int initializeTexture(GLuint *texture, char *path) {
//     /* Use STB Image to load the texture data from the file. */
//     int width, height, texelDim;
//     unsigned char *rawData;
//     rawData = stbi_load(path, &width, &height, &texelDim, 0);
//     if (rawData == NULL) {
//         fprintf(stderr, "initializeTexture: failed to load %s\n", path);
//         fprintf(stderr, "with STB Image reason: %s.\n", stbi_failure_reason());
//         return 1;
//     }
//     /* Right now we support only 3-channel images. */
//     if (texelDim != 3) {
//         fprintf(stderr, "initializeTexture: %d != 3 channels.\n", texelDim);
//         return 2;
//     }
//     /* Load the data into OpenGL. The calls to glTexParameteri demonstrate how 
//     you might set border and filtering behavior. */
//     glGenTextures(1, texture);
//     glBindTexture(GL_TEXTURE_2D, *texture);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, 
//         GL_UNSIGNED_BYTE, rawData);
//     stbi_image_free(rawData);
//     if (glGetError() != GL_NO_ERROR) {
//         fprintf(stderr, "initializeTexture: OpenGL error.\n");
//         glDeleteTextures(1, texture);
//         return 3;
//     }
//     glBindTexture(GL_TEXTURE_2D, 0);
//     return 0;
// }

int initializeTex() {
    /* A 'texture unit' is a piece of machinery inside the GPU that performs 
    texture mapping. A GPU might have many texture units, allowing you to map 
    many textures onto your meshes in complicated ways. The glActiveTexture 
    function selects which texture unit is affected by subsequent OpenGL calls. 
    In this tutorial, we use only texture unit 0, so we activate it here, once 
    and for all. */
    // glEnable(GL_TEXTURE_2D);

    // glActiveTexture(GL_TEXTURE0);
    // if (initializeTexture(&textureA, "rainbow.jpg") != 0)
    //     return 1;

    if (texInitializeFile(&texA, "rainbow.jpg", GL_LINEAR, GL_LINEAR, 
            GL_REPEAT, GL_REPEAT) != 0)
        return 1;



    // int texInitializeFile(texTexture *tex, char *path, GLint minification, 
        //GLint magnification, GLint leftRight, GLint bottomTop) {
    // glActiveTexture(GL_TEXTURE1);
    // if (initializeTexture(&textureB, "jupiter.jpg") != 0)
    //     return 2;

    // glActiveTexture(GL_TEXTURE2);
    // if (initializeTexture(&textureC, "puppy.jpg") != 0)
    //     return 3;

    sceneSetOneTexture(&rootNode, 0, &texA);
    // sceneSetOneTexture(&rootNode, 1, &texB);

    return 0;
}

void render(void) {
    /* This part is the same as in 520mainCamera.c. */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program);
    camRender(&cam, viewingLoc);
    /* This animation code is different from that in 520mainCamera.c. */
    GLdouble rot[3][3], identity[4][4], axis[3] = {1.0, 1.0, 1.0};
    // vecUnit(3, axis, axis);
    // alpha += 0.01;
    // mat33AngleAxisRotation(alpha, axis, rot);
    // sceneSetRotation(&rootNode, rot);
    // sceneSetOneUniform(&rootNode, 0, 0.5 + 0.5 * sin(alpha * 7.0));
    /* This rendering code is different from that in 520mainCamera.c. */
    mat44Identity(identity);
    GLuint unifDims[1] = {2};
    GLuint attrDims[3] = {3, 2, 3};
    sceneRender(&rootNode, identity, modelingLoc, 1, unifDims, unifLocs, 3, 
        attrDims, attrLocs, textureLoc);
}

int main(void) {
    glfwSetErrorCallback(handleError);
    if (glfwInit() == 0)
        return 1;
    GLFWwindow *window;
    window = glfwCreateWindow(512, 512, "Scene Graph", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return 2;
    }
    glfwSetWindowSizeCallback(window, handleResize);
    glfwSetKeyCallback(window, handleKey);
    glfwMakeContextCurrent(window);
    fprintf(stderr, "main: OpenGL %s, GLSL %s.\n", 
        glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    glEnable(GL_DEPTH_TEST);
    glDepthRange(1.0, 0.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    /* Initialize a whole scene, rather than just one mesh. */
    if (initializeScene() != 0)
        return 3;
    if (initializeShaderProgram() != 0)
        return 4;
    /* A 'texture unit' is a piece of machinery inside the GPU that performs 
    texture mapping. A GPU might have many texture units, allowing you to map 
    many textures onto your meshes in complicated ways. The glActiveTexture 
    function selects which texture unit is affected by subsequent OpenGL calls. 
    In this tutorial, we use only texture unit 0, so we activate it here, once 
    and for all. */
    // initializeMesh();
    if (initializeTex() != 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 5;
    }
    GLdouble target[3] = {0.0, 0.0, 0.0};
    camSetControls(&cam, camPERSPECTIVE, M_PI / 6.0, 10.0, 512.0, 512.0, 10.0, 
        M_PI / 4.0, M_PI / 4.0, target);
    while (glfwWindowShouldClose(window) == 0) {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteProgram(program);
    /* Don't forget to destroy the whole scene. */
    texDestroy(&texA);
    destroyScene();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


