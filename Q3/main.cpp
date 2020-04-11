#include <iostream>
#include <cmath>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "SOIL2/SOIL2.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "CubeMap.h"


// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera  camera( glm::vec3( 0.0f, 0.0f, 3.0f ) );
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos( 0.1f, 0.1f, 0.1f );

//Keep B key pressed to display Bill-phong shading
GLfloat blinn = 0.0;
bool blinnkeypressed = false;

//Keep F pressed to display only directional light
GLfloat db = 0.0;
bool dbkeypressed = false;

// Deltatime
GLfloat deltaTime = 0.0f;    // Time between current frame and last frame
GLfloat lastFrame = 0.0f;      // Time of last frame

// The MAIN function, from here we start the application and run the game loop
int main( )
{
    // Init GLFW
    glfwInit( );
    // Set all the required options for GLFW
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr );
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        
        return EXIT_FAILURE;
    }
    
    glfwMakeContextCurrent( window );
    
    glfwGetFramebufferSize( window, &SCREEN_WIDTH, &SCREEN_HEIGHT );
    
    // Set the required callback functions
    glfwSetKeyCallback( window, KeyCallback );
    glfwSetCursorPosCallback( window, MouseCallback );
    
    // GLFW Options
    glfwSetInputMode( window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT );
    
    // OpenGL options
    glEnable( GL_DEPTH_TEST );
    
    
    // Build and compile our shader program
    Shader PointShader( "resources/shaders/core.vs", "resources/shaders/frag.vs" );
    Shader lampShader( "resources/shaders/lightcore.vs", "resources/shaders/lightfrag.vs" );
    
    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] =
    {
        // Positions            // Normals              // Texture Coords       //aTangent           //aBitangent
        -0.2f, -0.2f, -0.2f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,            0.0f, 0.1f, 0.0f,    1.0f, 0.0f, 0.0f,
        0.2f, -0.2f, -0.2f,     0.0f,  0.0f, -1.0f,     1.0f,  0.0f,            0.0f, 0.1f, 0.0f,    1.0f, 0.0f, 0.0f,
        0.2f,  0.2f, -0.2f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,            0.0f, 0.1f, 0.0f,    1.0f, 0.0f, 0.0f,
        0.2f,  0.2f, -0.2f,     0.0f,  0.0f, -1.0f,     1.0f,  1.0f,            0.0f, 0.1f, 0.0f,    1.0f, 0.0f, 0.0f,
        -0.2f,  0.2f, -0.2f,    0.0f,  0.0f, -1.0f,     0.0f,  1.0f,            0.0f, 0.1f, 0.0f,    1.0f, 0.0f, 0.0f,
        -0.2f, -0.2f, -0.2f,    0.0f,  0.0f, -1.0f,     0.0f,  0.0f,            0.0f, 0.1f, 0.0f,    1.0f, 0.0f, 0.0f,
        
        -0.2f, -0.2f,  0.2f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,            0.0f, 0.1f, 0.0f,    -1.0f, 0.0f, 0.0f,
        0.2f, -0.2f,  0.2f,     0.0f,  0.0f,  1.0f,     1.0f,  0.0f,            0.0f, 0.1f, 0.0f,    -1.0f, 0.0f, 0.0f,
        0.2f,  0.2f,  0.2f,     0.0f,  0.0f,  1.0f,     1.0f,  1.0f,            0.0f, 0.1f, 0.0f,    -1.0f, 0.0f, 0.0f,
        0.2f,  0.2f,  0.2f,     0.0f,  0.0f,  1.0f,      1.0f,  1.0f,           0.0f, 0.1f, 0.0f,    -1.0f, 0.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,    0.0f,  0.0f,  1.0f,     0.0f,  1.0f,            0.0f, 0.1f, 0.0f,    -1.0f, 0.0f, 0.0f,
        -0.2f, -0.2f,  0.2f,    0.0f,  0.0f,  1.0f,     0.0f,  0.0f,            0.0f, 0.1f, 0.0f,    -1.0f, 0.0f, 0.0f,
        
        -0.2f,  0.2f,  0.2f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,            0.0f, 0.0f, -1.0f,   0.0f, -1.0f, 0.0f,
        -0.2f,  0.2f, -0.2f,    -1.0f,  0.0f,  0.0f,    1.0f,  1.0f,            0.0f, 0.0f, -1.0f,   0.0f, -1.0f, 0.0f,
        -0.2f, -0.2f, -0.2f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,            0.0f, 0.0f, -1.0f,   0.0f, -1.0f, 0.0f,
        -0.2f, -0.2f, -0.2f,    -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,            0.0f, 0.0f, -1.0f,   0.0f, -1.0f, 0.0f,
        -0.2f, -0.2f,  0.2f,    -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,            0.0f, 0.0f, -1.0f,   0.0f, -1.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,    -1.0f,  0.0f,  0.0f,    1.0f,  0.0f,            0.0f, 0.0f, -1.0f,   0.0f, -1.0f, 0.0f,
        
        0.2f,  0.2f,  0.2f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,            0.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        0.2f,  0.2f, -0.2f,     1.0f,  0.0f,  0.0f,     1.0f,  1.0f,            0.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        0.2f, -0.2f, -0.2f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,            0.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        0.2f, -0.2f, -0.2f,     1.0f,  0.0f,  0.0f,     0.0f,  1.0f,            0.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        0.2f, -0.2f,  0.2f,     1.0f,  0.0f,  0.0f,     0.0f,  0.0f,            0.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        0.2f,  0.2f,  0.2f,     1.0f,  0.0f,  0.0f,     1.0f,  0.0f,            0.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
        
        -0.2f, -0.2f, -0.2f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,            0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,
        0.2f, -0.2f, -0.2f,     0.0f, -1.0f,  0.0f,     1.0f,  1.0f,            0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,
        0.2f, -0.2f,  0.2f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,            0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,
        0.2f, -0.2f,  0.2f,     0.0f, -1.0f,  0.0f,     1.0f,  0.0f,            0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,
        -0.2f, -0.2f,  0.2f,    0.0f, -1.0f,  0.0f,     0.0f,  0.0f,            0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,
        -0.2f, -0.2f, -0.2f,    0.0f, -1.0f,  0.0f,     0.0f,  1.0f,            0.0f, 0.0f, -1.0f,   1.0f, 0.0f, 0.0f,
        
        -0.2f,  0.2f, -0.2f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,            0.0f, 0.0f, -1.0f,   -1.0f, 0.0f, 0.0f,
        0.2f,  0.2f, -0.2f,     0.0f,  1.0f,  0.0f,     1.0f,  1.0f,            0.0f, 0.0f, -1.0f,   -1.0f, 0.0f, 0.0f,
        0.2f,  0.2f,  0.2f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,            0.0f, 0.0f, -1.0f,   -1.0f, 0.0f, 0.0f,
        0.2f,  0.2f,  0.2f,     0.0f,  1.0f,  0.0f,     1.0f,  0.0f,            0.0f, 0.0f, -1.0f,   -1.0f, 0.0f, 0.0f,
        -0.2f,  0.2f,  0.2f,    0.0f,  1.0f,  0.0f,     0.0f,  0.0f,            0.0f, 0.0f, -1.0f,   -1.0f, 0.0f, 0.0f,
        -0.2f,  0.2f, -0.2f,    0.0f,  1.0f,  0.0f,     0.0f,  1.0f,            0.0f, 0.0f, -1.0f,   -1.0f, 0.0f, 0.0f
    };
    
    
    
    // First, set the container's VAO (and VBO)
    GLuint VBO, boxVAO;
    glGenVertexArrays( 1, &boxVAO );
    glGenBuffers( 1, &VBO );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );
    
    glBindVertexArray( boxVAO );
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof( GLfloat ), ( GLvoid * )0 );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof( GLfloat ), ( GLvoid * )( 3 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof( GLfloat ), ( GLvoid * )( 6 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray( 2 );
    glVertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof( GLfloat ), ( GLvoid * )( 8 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray(3);
    glVertexAttribPointer( 4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof( GLfloat ), ( GLvoid * )( 11 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray(4);
    glBindVertexArray( 0 );
    
    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    GLuint lightVAO;
    glGenVertexArrays( 1, &lightVAO );
    glBindVertexArray( lightVAO );
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    // Set the vertex attributes (only position data for the lamp))
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof( GLfloat ), ( GLvoid * )0 ); // Note that we skip over the other data in our buffer object (we don't need the normals/textures, only positions).
    glEnableVertexAttribArray( 0 );
    glBindVertexArray( 0 );
    
    
    // Load textures
    GLuint diffuseMap, specularMap, normalMap;
    glGenTextures( 1, &diffuseMap );
    glGenTextures( 1, &specularMap );
    glGenTextures(1, &normalMap);
    
    int imageWidth, imageHeight;
    unsigned char *image;
    
    // Diffuse map
    image = SOIL_load_image( "resources/images/ROCK035_2K_Color.jpg", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB );
    glBindTexture( GL_TEXTURE_2D, diffuseMap );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap( GL_TEXTURE_2D );
    SOIL_free_image_data( image );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST );
    
    // Specular map
    image = SOIL_load_image( "resources/images/ROCK035_2K_Displacement.jpg", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB );
    glBindTexture( GL_TEXTURE_2D, specularMap );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap( GL_TEXTURE_2D );
    SOIL_free_image_data( image );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST );
    glBindTexture( GL_TEXTURE_2D, 0 );
    
    //Normal map
    image = SOIL_load_image( "resources/images/ROCK035_2K_Normal.jpg", &imageWidth, &imageHeight, 0, SOIL_LOAD_RGB );
    glBindTexture( GL_TEXTURE_2D, normalMap );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap( GL_TEXTURE_2D );
    SOIL_free_image_data( image );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST );
    glBindTexture( GL_TEXTURE_2D, 0 );
    
    
    // Set texture units
    PointShader.Use( );
    glUniform1i( glGetUniformLocation( PointShader.Program, "material.diffuse" ), 0 );
    glUniform1i( glGetUniformLocation( PointShader.Program, "material.specular" ), 1 );
    glUniform1i( glGetUniformLocation( PointShader.Program, "material.normal" ), 2 );

    
    //Skybox
    std::vector<std::string> faces =
    {
        "resources/images/right.png",
        "resources/images/left.png",
        "resources/images/top.png",
        "resources/images/bottom.png",
        "resources/images/front.png",
        "resources/images/back.png"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    
    Shader skyboxShader( "resources/shaders/skycore.vs", "resources/shaders/skyfrag.vs" );
    
    GLuint VBOcm, VAOcm;
    glGenVertexArrays( 1, &VAOcm );
    glGenBuffers( 1, &VBOcm );
    // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
    glBindVertexArray( VAOcm );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBOcm );
    glBufferData( GL_ARRAY_BUFFER, sizeof( skyboxVertices ), skyboxVertices, GL_STATIC_DRAW );
    
    // Position attribute
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), ( GLvoid * )0 );
    glEnableVertexAttribArray( 0 );
    glBindVertexArray( 0 );
    
    glm::mat4 projection = glm::perspective( camera.GetZoom( ), ( GLfloat )SCREEN_WIDTH / ( GLfloat )SCREEN_HEIGHT, 0.1f, 100.0f );
    
    // Game loop
    //Moving light
    GLfloat theta = 45.0f;
    while ( !glfwWindowShouldClose( window ) )
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents( );
        DoMovement( );
        
        //Move the Spotlight
        lightPos.x = 0.2 * cos(glm::radians(theta));
        lightPos.z = 0.2 * sin(glm::radians(theta));
        theta-=0.7f;
        
        
        // Clear the colorbuffer
        glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // Draw skybox as last
        glDepthMask( GL_FALSE );  // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Use();
        
        // Create camera transformations
        glm::mat4 view(1);
        view = glm::mat4( glm::mat3( camera.GetViewMatrix( ) ) );
        
        glUniformMatrix4fv( glGetUniformLocation( skyboxShader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( glGetUniformLocation( skyboxShader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
        
        
        // skybox cube
        glBindVertexArray( VAOcm );
        glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        glDepthMask( GL_TRUE ); // Set depth function back to default
        
        
        //Shader dependent
        
        PointShader.Use();
        // Use cooresponding shader when setting uniforms/drawing objects
        GLint lightPosLoc = glGetUniformLocation( PointShader.Program, "point.position" );
        GLint viewPosLoc = glGetUniformLocation( PointShader.Program, "viewPos" );
        glUniform3f( lightPosLoc, lightPos.x, lightPos.y, lightPos.z );
        glUniform3f( viewPosLoc,  camera.GetPosition( ).x, camera.GetPosition( ).y, camera.GetPosition( ).z );
        // Set lights properties
        glUniform3f( glGetUniformLocation( PointShader.Program, "point.ambient" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( PointShader.Program, "point.diffuse" ), 2.0f, 2.0f, 2.0f );
        glUniform3f( glGetUniformLocation( PointShader.Program, "point.specular" ), 1.0f, 1.0f, 1.0f );
        glUniform1f( glGetUniformLocation( PointShader.Program, "point.constant" ), 1.0f );
        glUniform1f( glGetUniformLocation( PointShader.Program, "point.linear" ), 0.0 );
        glUniform1f( glGetUniformLocation( PointShader.Program, "point.quadratic" ), 3.0f );
        glUniform3f( glGetUniformLocation( PointShader.Program, "direction.dir" ), 0.5f, 0.5f, 0.5f );
        glUniform3f( glGetUniformLocation( PointShader.Program, "direction.ambient" ), 0.3f, 0.3f, 0.3f );
        glUniform3f( glGetUniformLocation( PointShader.Program, "direction.diffuse" ), 0.2f, 0.2f, 0.2f );
        glUniform3f( glGetUniformLocation( PointShader.Program, "direction.specular" ), 0.0f, 0.0f, 0.0f );
        glUniform1f( glGetUniformLocation( PointShader.Program, "blinn" ), blinn );
        glUniform1f( glGetUniformLocation( PointShader.Program, "db" ), db );
        // Set material properties
        glUniform1f( glGetUniformLocation( PointShader.Program, "material.shininess"), 5.0f );
        
        
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation( PointShader.Program, "model" );
        GLint viewLoc  = glGetUniformLocation( PointShader.Program, "view" );
        GLint projLoc  = glGetUniformLocation( PointShader.Program, "projection" );
        // Pass the matrices to the shader
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        // Bind diffuse map
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, diffuseMap );
        // Bind specular map
        glActiveTexture( GL_TEXTURE1 );
        glBindTexture( GL_TEXTURE_2D, specularMap );
        //Bind normal map
        glActiveTexture( GL_TEXTURE2 );
        glBindTexture( GL_TEXTURE_2D, normalMap );
        
        
        //Draw the box
        glm::mat4 model(1);
        model = glm::translate( model, glm::vec3(-0.4f, 0.4f, -0.4f) );
        glBindVertexArray( boxVAO );
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        
        
        // Also draw the lamp object, again binding the appropriate shader
        lampShader.Use( );
        // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
        modelLoc = glGetUniformLocation( lampShader.Program, "model" );
        viewLoc = glGetUniformLocation( lampShader.Program, "view" );
        projLoc = glGetUniformLocation( lampShader.Program, "projection" );
        // Set matrices
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        model = glm::mat4(1);
        model = glm::translate( model, lightPos );
        model = glm::scale( model, glm::vec3( 0.05f ) ); // Make it a smaller cube
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
        // Draw the light object (using light's vertex attributes)
        glBindVertexArray( lightVAO );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        
        
        // Swap the screen buffers
        glfwSwapBuffers(window);
    }
    
    glDeleteVertexArrays( 1, &boxVAO );
    glDeleteVertexArrays( 1, &lightVAO );
    glDeleteBuffers( 1, &VBO );
    
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate( );
    
    return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement( )
{
    // Camera controls
    if ( keys[GLFW_KEY_W] || keys[GLFW_KEY_UP] )
    {
        camera.ProcessKeyboard( FORWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN] )
    {
        camera.ProcessKeyboard( BACKWARD, deltaTime );
    }
    
    if ( keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] )
    {
        camera.ProcessKeyboard( LEFT, deltaTime );
    }
    
    if ( keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT] )
    {
        camera.ProcessKeyboard( RIGHT, deltaTime );
    }
    
    if (keys[GLFW_KEY_B] && !blinnkeypressed)
    {
        blinn = 1.0;
        blinnkeypressed = true;
    }
    else
    {
        blinn = 0.0;
        blinnkeypressed = false;
    }
    
    if (keys[GLFW_KEY_F] && !dbkeypressed)
    {
        db = 1.0;
        dbkeypressed = true;
    }
    else
    {
        db = 0.0;
        dbkeypressed = false;
    }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if ( GLFW_KEY_ESCAPE == key && GLFW_PRESS == action )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if ( key >= 0 && key < 1024 )
    {
        if ( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if ( action == GLFW_RELEASE )
        {
            keys[key] = false;
        }
    }
}

void MouseCallback( GLFWwindow *window, double xPos, double yPos )
{
    if ( firstMouse )
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left
    
    lastX = xPos;
    lastY = yPos;
    
    camera.ProcessMouseMovement( xOffset, yOffset );
}
