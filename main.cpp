// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <algorithm>
#include <cmath>
#include <typeinfo>
#include <vector>

// specify that we want the OpenGL core profile before including GLFW headers
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

using namespace std;

// GLOBAL VARIABLES
const long double PI = 3.141592653589793238L;
int PART = 1;   // Specifies which part of the assignment is: 1 for A (Square and Diamond)
                //, 2 for B (Spiral) and 3 for C (Sierinski Triangle)
int LEVEL = 1;  // It refers to the number of iterations or revolutions of the shape, it goes from 1 to 6

struct MyShader
{
    // OpenGL names for vertex and fragment shaders, shader program
    GLuint  vertex;
    GLuint  fragment;
    GLuint  program;

    // initialize shader and program names to zero (OpenGL reserved value)
    MyShader() : vertex(0), fragment(0), program(0)
    {}
};
MyShader shader;

struct MyGeometry
{
    // OpenGL names for array buffer objects, vertex array object
    GLuint  vertexBuffer;
    GLuint  colourBuffer;
    GLuint  vertexArray;
    GLsizei elementCount;

    // initialize object names to zero (OpenGL reserved value)
    MyGeometry() : vertexBuffer(0), colourBuffer(0), vertexArray(0), elementCount(0)
    {}
};

MyGeometry geometry;
GLuint renderMode;

// END OF GLOBAL VARIABLES
// --------------------------------------------------------------------------
// OpenGL utility and support function prototypes

void QueryGLVersion();
bool CheckGLErrors();

// Function Prototypes
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

// --------------------------------------------------------------------------
// Functions to set up OpenGL shader programs for rendering

// load, compile, and link shaders, returning true if successful
bool InitializeShaders(MyShader *shader)
{
    // load shader source from files
    string vertexSource = LoadSource("vertex.glsl");
    string fragmentSource = LoadSource("fragment.glsl");
    if (vertexSource.empty() || fragmentSource.empty()) return false;

    // compile shader source into shader objects
    shader->vertex = CompileShader(GL_VERTEX_SHADER, vertexSource);
    shader->fragment = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    // link shader program
    shader->program = LinkProgram(shader->vertex, shader->fragment);

    // check for OpenGL errors and return false if error occurred
    return !CheckGLErrors();
}

// deallocate shader-related objects
void DestroyShaders(MyShader *shader)
{
    // unbind any shader programs and destroy shader objects
    glUseProgram(0);
    glDeleteProgram(shader->program);
    glDeleteShader(shader->vertex);
    glDeleteShader(shader->fragment);
}


// --------------------------------------------------------------------------
// Functions to set up OpenGL buffers for storing geometry data

// -------------------------------------------------------------------------
// Functions related to the Square and Diamond

// Fills the geometry data taking into account the numbers of levels
void SetupVertexBufferSquareAndDiamond(int maxLevel, MyGeometry *geometry) 
{
    // Base shape for all levels
    vector<GLfloat> vertices;

    // Number of vertices in current level
    int counter = 0;
    while (counter < maxLevel)
    {
        float factor = (1/pow(2, counter));
        vertices.push_back(-1.0 * factor); // Square vertices
        vertices.push_back(-1.0 * factor);
        vertices.push_back(1.0 * factor);
        vertices.push_back(1.0 * factor);
        vertices.push_back(1.0 * factor);
        vertices.push_back(-1.0 * factor);
        vertices.push_back(-1.0 * factor);
        vertices.push_back(-1.0 * factor);
        vertices.push_back(1.0 * factor);
        vertices.push_back(1.0 * factor);
        vertices.push_back(-1.0 * factor);
        vertices.push_back(1.0 * factor);

        vertices.push_back(0.0 * factor); // Diamond vertices
        vertices.push_back(-1.0 * factor);
        vertices.push_back(1.0 * factor);
        vertices.push_back(0.0 * factor);
        vertices.push_back(0.0 * factor);
        vertices.push_back(1.0 * factor);
        vertices.push_back(0.0 * factor);
        vertices.push_back(-1.0 * factor);
        vertices.push_back(-1.0 * factor);
        vertices.push_back(0.0 * factor);
        vertices.push_back(0.0 * factor);
        vertices.push_back(1.0 * factor);

       counter++;
    }

    // Placing the data into the buffer
    geometry->elementCount = vertices.size()/2;
    glGenBuffers(1, &geometry->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
}

// Generates the colour for the square and diamond and binds this data to the buffer
void SetupColourBufferSquareAndDiamond(int maxLevel, MyGeometry *geometry) 
{
    float red = 0.42;
    float green = 0.1;
    float blue = 0.7f;
    float diamondRed = 0.58;
    float diamondGreen = 0.9f;
    float diamondBlue = 0.3f;
    // Base colour for level
    vector<GLfloat> colours;
    for (int i = 0; i < 6; i++)
    {
        colours.push_back(red);
        colours.push_back(green);
        colours.push_back(blue);
    }
    for (int j = 0; j < 6; j++)
    {
        colours.push_back(diamondRed);
        colours.push_back(diamondGreen);
        colours.push_back(diamondBlue);
    }
    int counter = 1;

    float step = 0.08f;
    while (counter <= maxLevel)
    {
        red = red - step;
        green = green - step;
        blue = blue - step;
        diamondRed = diamondRed - step;
        diamondGreen = diamondGreen - step;
        diamondBlue = diamondBlue - step;
        for (int i = 0; i < 6; i++)
        {
            colours.push_back(red);
            colours.push_back(green);
            colours.push_back(blue);
        }
        for (int j = 0; j < 6; j++)
        {
            colours.push_back(diamondRed);
            colours.push_back(diamondGreen);
            colours.push_back(diamondBlue);
        }
        counter++;
    }

    // create another one for storing our colours
    glGenBuffers(1, &geometry->colourBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(GLfloat), colours.data(), GL_STATIC_DRAW);
}

// Create buffers and fill with geometry data, returning true if successful
bool InitializeSquareAndDiamond(MyGeometry *geometry)
{   
    const GLuint VERTEX_INDEX = 0;
    const GLuint COLOUR_INDEX = 1;
    // Generate arrays with data for the level
    SetupVertexBufferSquareAndDiamond(LEVEL, geometry);
    SetupColourBufferSquareAndDiamond(LEVEL, geometry);

    // create a vertex array object encapsulating all our vertex attributes
    glGenVertexArrays(1, &geometry->vertexArray);
    glBindVertexArray(geometry->vertexArray);

    // associate the position array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VERTEX_INDEX);

    // associate the colour array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(COLOUR_INDEX);
    // unbind our buffers, resetting to default state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // check for OpenGL errors and return false if error occurred
    return !CheckGLErrors();
}


// Deallocate geometry-related objects
void DestroyGeometry(MyGeometry *geometry)
{
    // unbind and destroy our vertex array object and associated buffers
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &geometry->vertexArray);
    glDeleteBuffers(1, &geometry->vertexBuffer);
    glDeleteBuffers(1, &geometry->colourBuffer);
}

// --------------------------------------------------------------------------
// Rendering function that draws our scene to the frame buffer

void RenderScene(MyGeometry *geometry, MyShader *shader, GLuint renderMode)
{
    // clear screen to a dark grey colour
    glClearColor(0.2, 0.2, 0.2, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // bind our shader program and the vertex array object containing our
    // scene geometry, then tell OpenGL to draw our geometry
    glUseProgram(shader->program);
    glBindVertexArray(geometry->vertexArray);
    glDrawArrays(renderMode, 0, geometry->elementCount);

    // reset state to default (no shader or geometry bound)
    glBindVertexArray(0);
    glUseProgram(0);

    // check for an report any OpenGL errors
    CheckGLErrors();
}

// -------------------------------------------------------------------------
// Spiral functions

// Creation of the vectors that contains the geometry and the colour data, then binds it to the buffer
bool InitializeSpirals(MyGeometry *geometry)
{
    const GLuint VERTEX_INDEX = 0;
    const GLuint COLOUR_INDEX = 1;
    vector<GLfloat> vertices;
    vector<GLfloat> colours;
    int verticesCounter = 0;

    // Fill vectors with the geometry data for the spiral
    float radius = 1.0f;
    for (int i = 0; i < LEVEL; i++)
    {
        float numberSegments = 400.0 * LEVEL;
        for (float j = 0.0f; j < numberSegments; j+=0.25f)
        {
            // the calculation for the x and y coordinates was based on: http://stackoverflow.com/a/18893438
            float theta = ((LEVEL-0.5) * 2.0 * PI * j )/ numberSegments;
            float x1 = -(radius/numberSegments) * j * cosf(theta);
            float y1 = (radius/numberSegments) * j * sinf(theta);
            float x2 = x1 - 0.01f * cosf(theta);
            float y2 = y1 + 0.01f * sinf(theta);
            vertices.push_back(x1);
            vertices.push_back(y1);
            vertices.push_back(x2);
            vertices.push_back(y2);
            verticesCounter+=2;
        }
    }

    // Number of vertices in current level
    geometry->elementCount = vertices.size() / 2;
    glGenBuffers(1, &geometry->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    // Fill buffer array with the colour data for the spiral
    GLfloat red = 1.0f;
    GLfloat green = 1.0f;
    GLfloat blue = 1.0f;
    
    int modifyColour = 1;
    float direction = -1.0f;
    float step = (3.5f*LEVEL)/verticesCounter;
    for (int i = 0; i <=verticesCounter; i++)
    {   
        if (modifyColour == 1)
        {
            red = red + direction * step;
            if (red > 1.0f)
            {
                modifyColour = 2;
                red = 1.0f;
            }
            else if (red < 0.0f) {
                modifyColour = 2;
                red = 0.0f;
            }
        }
        else if (modifyColour == 2)
        {
            green = green + direction * step;
            if (green > 1.0f)
            {
                modifyColour = 3;
                green = 1.0f;
            }
            else if (green < 0.0f) {
                modifyColour = 3;
                green = 0.0f;
            }
        }
        else if (modifyColour == 3)
        {
            blue = blue + direction * step;
            if (blue > 1.0f)
            {
                modifyColour = 1;
                blue = 1.0f;
                direction = direction * -1.0f;
            }
            else if (blue < 0.0f) {
                modifyColour = 1;
                blue = 0.0f;
                direction = direction * -1.0f;
            }
        }
        colours.push_back(red);
        colours.push_back(green);
        colours.push_back(blue);
    }
    
    // create another one for storing our colours
    glGenBuffers(1, &geometry->colourBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(float), colours.data(), GL_STATIC_DRAW);

    // create a vertex array object encapsulating all our vertex attributes
    glGenVertexArrays(1, &geometry->vertexArray);
    glBindVertexArray(geometry->vertexArray);

    // associate the position array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VERTEX_INDEX);

    // associate the colour array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(COLOUR_INDEX);
    // unbind our buffers, resetting to default state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // check for OpenGL errors and return false if error occurred
    return !CheckGLErrors();
}

// ----------------------------------------------------------------------------------
// Functions related to the Sierpinski Triangle


// Calculations of the coordinates of the triangles within an iteration
void renderTriangleLevel(vector<GLfloat> *vertices, float x1, float y1, float x2, float y2, float x3, float y3, int level, int maxLevel)
{
    // Inverted triangle coordinates
    float x1Inv = (x1+x2)/2.0f;
    float y1Inv = (y2+y1)/2.0f;
    float x2Inv = (x3+x2)/2.0f;
    float y2Inv = (y3+y2)/2.0f;
    float x3Inv = (x3+x1)/2.0f;
    float y3Inv = (y3+y1)/2.0f;

    // Inverted white triangle
    vertices->push_back(x1Inv);
    vertices->push_back(y1Inv);
    vertices->push_back(x2Inv);
    vertices->push_back(y2Inv);
    vertices->push_back(x3Inv);
    vertices->push_back(y3Inv);

    // First triangle left bottom
    vertices->push_back(x1);
    vertices->push_back(y1);
    vertices->push_back(x1Inv);
    vertices->push_back(y1Inv);
    vertices->push_back(x3Inv);
    vertices->push_back(y3Inv);

    // Second triangle right bottom
    vertices->push_back(x1Inv);
    vertices->push_back(y1Inv);
    vertices->push_back(x2);
    vertices->push_back(y2);
    vertices->push_back(x2Inv);
    vertices->push_back(y2Inv);

    // Third triangle on top
    vertices->push_back(x3Inv);
    vertices->push_back(y3Inv);
    vertices->push_back(x2Inv);
    vertices->push_back(y2Inv);
    vertices->push_back(x3);
    vertices->push_back(y3);

    if (level < maxLevel)
    {   
        level++;
        renderTriangleLevel(vertices, x1, y1, x1Inv, y1Inv, x3Inv, y3Inv, level, maxLevel);
        renderTriangleLevel(vertices, x1Inv, y1Inv, x2, y2, x2Inv, y2Inv, level, maxLevel);
        renderTriangleLevel(vertices, x3, y3, x2Inv, y2Inv, x3Inv, y3Inv, level, maxLevel);
    }
}

// Assigning colour to each level recursively
void coloursForLevelSierpinski(vector<GLfloat>* colours, GLfloat red, GLfloat green, GLfloat blue, int level, int maxLevel)
{

    for (int j = 0; j < 3; j++)
    {
        colours->push_back(red);
        colours->push_back(green);
        colours->push_back(blue);
    }
    if (level < maxLevel)
    {
        level++;
        GLfloat newRed = red + 0.1f;
        GLfloat newGreen = green + 0.05f;
        GLfloat newBlue = blue + 0.05f;
        coloursForLevelSierpinski(colours, newRed, newGreen, newBlue, level, maxLevel);
        coloursForLevelSierpinski(colours, newRed, newGreen, newBlue, level, maxLevel);
        coloursForLevelSierpinski(colours, newRed, newGreen, newBlue, level, maxLevel);

    }
}


void fillOneTriangleColour(vector<GLfloat> *colours, float red, float green, float blue)
{
    for (int j = 0; j < 3; j++)
    {
        colours->push_back(red);
        colours->push_back(green);
        colours->push_back(blue);
    }
}
// Initial base triangle before the first iteration
void assignColoursToLevelSierpinski(vector<GLfloat> *colours, int level, int maxLevel, int totalElements)
{
    int coloursAdded = 0;
    float factor = 1.0f / (maxLevel * 72.0f);
    // Red triangle
    float redR = 0.9f;
    float redG = 0.0f;
    float redB = 0.3f;

    // Cyan triangle
    float greenR = 0.0f;
    float greenG = 0.7f;
    float greenB = 0.5f;

    // Blue triangle
    float blueR = 0.5f;
    float blueG = 0.0f;
    float blueB = 1.0f;

    int iterateUntil = 0;
    switch(maxLevel){
        case 1:
            iterateUntil = 1;
            break;
        case 2:
            iterateUntil = 4;
            break;
        case 3:
            iterateUntil = 13;
            break;
        case 4:
            iterateUntil = 40;
            break;
        case 5:
            iterateUntil = 121;
            break;
        case 6:
            iterateUntil = 364;
            break;

    }
    for (int i = 0; i < iterateUntil; i++)
    {
        fillOneTriangleColour(colours, 1.0f, 1.0f, 1.0f); // White triangle
        fillOneTriangleColour(colours, redR, redG, redB); // Red triangle
        fillOneTriangleColour(colours, greenR, greenG, greenB); // Cyan triangle
        fillOneTriangleColour(colours, blueR, blueG, blueB); // Blue triangle
        coloursAdded=+4;

        // Modify red triangles
        redR = redR - pow(factor, 2);
        redG = redG + factor;
        redB = redB + (factor/2.0f);

        // Modify Cyan triangles
        greenR = greenR + factor;
        greenG = greenR - (factor + 0.1 * pow(-1.0,i));
        greenB = greenB + (factor/2.0f);

        // Modify blue triangles
        blueR = blueR + (factor + 0.1 * pow(-1.0,i));
        blueG = blueG + factor;
        blueB = blueG - factor/4.0f;
    }
}

// Initialization of the Sierpinski Triangle
bool InitializeSierpinksiTriangle(MyGeometry *geometry)
{
    const GLuint VERTEX_INDEX = 0;
    const GLuint COLOUR_INDEX = 1;
    vector<GLfloat> vertices;
    vector<GLfloat> colours;

    // Generate arrays with the vertex

    // Base triangle vertices
    float x1 = -0.8f;
    float y1 = -0.6f;
    float x2 =  0.8f;
    float y2 = -0.6f;
    float x3 =  0.0f;
    float y3 =  0.8f;

    // First iteration for geometric and colour data
    
    renderTriangleLevel(&vertices, x1, y1, x2, y2, x3, y3, 1, LEVEL);
    int totalVertices = vertices.size() / 2;
    assignColoursToLevelSierpinski(&colours, 1, LEVEL, totalVertices);

    // number of vertices in current level
    geometry->elementCount = totalVertices;
    // create an array buffer object for storing our vertices
    glGenBuffers(1, &geometry->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    // Generate buffer array for the colour
    glGenBuffers(1, &geometry->colourBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glBufferData(GL_ARRAY_BUFFER, colours.size() * sizeof(GLfloat), colours.data(), GL_STATIC_DRAW);

    // create a vertex array object encapsulating all our vertex attributes
    glGenVertexArrays(1, &geometry->vertexArray);
    glBindVertexArray(geometry->vertexArray);

    // associate the position array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBuffer);
    glVertexAttribPointer(VERTEX_INDEX, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(VERTEX_INDEX);

    // associate the colour array with the vertex array object
    glBindBuffer(GL_ARRAY_BUFFER, geometry->colourBuffer);
    glVertexAttribPointer(COLOUR_INDEX, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(COLOUR_INDEX);
    // unbind our buffers, resetting to default state
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // check for OpenGL errors and return false if error occurred

    return !CheckGLErrors();
}

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
    cout << "GLFW ERROR " << error << ":" << endl;
    cout << description << endl;
}

// Function that depending on what's the value of PART initializes a different shape
void initializeTheShape()
{
    if (PART == 1)
    {
        DestroyGeometry(&geometry);
        renderMode = GL_TRIANGLES;
        if (!InitializeSquareAndDiamond(&geometry))
        {
            cout << "Program failed to intialize geometry!" << endl;
        }
    }
    else if (PART == 2) {
        //DestroyGeometry(&geometry);
        renderMode = GL_LINES;
        if (!InitializeSpirals(&geometry))
        {
            cout << "Program failed to intialize spirals!" << endl;
        }

    }
    else if (PART == 3) {
        DestroyGeometry(&geometry);
        renderMode = GL_TRIANGLES;
        if (!InitializeSierpinksiTriangle(&geometry))
        {
            cout << "Program failed to intialize sierpinski triangles!" << endl;
        }
    }
}


// Handles keyboard input events, ignoring non-GLFW_PRESS actions and keys that do not trigger rendering of shapes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Map the keys that will produce a change in the program
    int inputKeys [] = {
        GLFW_KEY_A, GLFW_KEY_B, GLFW_KEY_C, GLFW_KEY_1, GLFW_KEY_2, 
        GLFW_KEY_3, GLFW_KEY_4, GLFW_KEY_5, GLFW_KEY_6
    };
    bool keyFound = find(begin(inputKeys), end(inputKeys), key) != end(inputKeys);

    // When action is not press or key was not found in the input key return
    if (action != GLFW_PRESS || !keyFound )
    {
        return;
    }

    // Depending on the key pressed draw a different shape or different level
    else 
    {
        if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
            
        else if (key == GLFW_KEY_A)
        {
            PART = 1;
        }
        else if(key == GLFW_KEY_B)
        {
            PART = 2;
        }
        else if (key == GLFW_KEY_C)
        {  
            PART = 3;
        }
        else if (key == GLFW_KEY_1)
        {
            LEVEL = 1;
        }
        else if (key == GLFW_KEY_2)
        {
            LEVEL = 2;
        }
        else if (key == GLFW_KEY_3)
        {
            LEVEL = 3;
        }
        else if (key == GLFW_KEY_4)
        {
            LEVEL = 4;
        }
        else if (key == GLFW_KEY_5)
        {
            LEVEL = 5;
        }
        else if (key == GLFW_KEY_6)
        {
            LEVEL = 6;
        }
        initializeTheShape();
    }
}


// ==========================================================================
// PROGRAM ENTRY POINT

int main(int argc, char *argv[])
{   
    // initialize the GLFW windowing system
    if (!glfwInit()) {
        cout << "ERROR: GLFW failed to initilize, TERMINATING" << endl;
        return -1;
    }
    glfwSetErrorCallback(ErrorCallback);

    // By default render triangles
    renderMode = GL_TRIANGLES;
    // attempt to create a window with an OpenGL 4.1 core profile context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window;
    window = glfwCreateWindow(512, 512, "CPSC 453 Assignment #1 Maria Diaz", 0, 0);
    if (!window) {
        cout << "Program failed to create GLFW window, TERMINATING" << endl;
        glfwTerminate();
        return -1;
    }

    // set keyboard callback function and make our context current (active)
    glfwSetKeyCallback(window, KeyCallback);
    glfwMakeContextCurrent(window);
    // query and print out information about our OpenGL environment
    QueryGLVersion();

    // call function to load and compile shader programs
    if (!InitializeShaders(&shader)) {
        cout << "Program could not initialize shaders, TERMINATING" << endl;
        return -1;
    }
    // By default initializes the square and diamond on level 1
    if (!InitializeSquareAndDiamond(&geometry))
    {
        cout << "Program failed to intialize geometry!" << endl;
    }

    while(!glfwWindowShouldClose(window))
    {
        // Draw scene
        RenderScene(&geometry, &shader, renderMode);

        // scene is rendered to the back buffer, so swap to front for display
        glfwSwapBuffers(window);

        // sleep until next event before drawing again
        glfwWaitEvents();
    }

    // clean up allocated resources before exit
    DestroyGeometry(&geometry);
    DestroyShaders(&shader);
    glfwDestroyWindow(window);
    glfwTerminate();  

    cout << "Goodbye!" << endl;
    return 0;
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
    // query opengl version and renderer information
    string version  = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    string glslver  = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    cout << "OpenGL [ " << version << " ] "
         << "with GLSL [ " << glslver << " ] "
         << "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors()
{
    bool error = false;
    for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
    {
        cout << "OpenGL ERROR:  ";
        switch (flag) {
        case GL_INVALID_ENUM:
            cout << "GL_INVALID_ENUM" << endl; break;
        case GL_INVALID_VALUE:
            cout << "GL_INVALID_VALUE" << endl; break;
        case GL_INVALID_OPERATION:
            cout << "GL_INVALID_OPERATION" << endl; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
        case GL_OUT_OF_MEMORY:
            cout << "GL_OUT_OF_MEMORY" << endl; break;
        default:
            cout << "[unknown error code]" << endl;
        }
        error = true;
    }
    return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
    string source;

    ifstream input(filename.c_str());
    if (input) {
        copy(istreambuf_iterator<char>(input),
             istreambuf_iterator<char>(),
             back_inserter(source));
        input.close();
    }
    else {
        cout << "ERROR: Could not load shader source from file "
             << filename << endl;
    }

    return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
    // allocate shader object name
    GLuint shaderObject = glCreateShader(shaderType);

    // try compiling the source as a shader of the given type
    const GLchar *source_ptr = source.c_str();
    glShaderSource(shaderObject, 1, &source_ptr, 0);
    glCompileShader(shaderObject);

    // retrieve compile status
    GLint status;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
        cout << "ERROR compiling shader:" << endl << endl;
        cout << source << endl;
        cout << info << endl;
    }

    return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    // allocate program object name
    GLuint programObject = glCreateProgram();

    // attach provided shader objects to this program
    if (vertexShader)   glAttachShader(programObject, vertexShader);
    if (fragmentShader) glAttachShader(programObject, fragmentShader);

    // try linking the program with given attachments
    glLinkProgram(programObject);

    // retrieve link status
    GLint status;
    glGetProgramiv(programObject, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
        string info(length, ' ');
        glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
        cout << "ERROR linking shader program:" << endl;
        cout << info << endl;
    }

    return programObject;
}


// ==========================================================================
