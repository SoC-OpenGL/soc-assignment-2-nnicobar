#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Camera.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other Libs
#include "SOIL2/SOIL2.h"

//CubeMap
#include "CubeMap.h"

// Properties
const GLuint WIDTH = 800, HEIGHT = 600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Function prototypes
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode );
void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset );
void MouseCallback( GLFWwindow *window, double xPos, double yPos );
void DoMovement( );

// Camera
Camera  camera(glm::vec3( 0.0f, 0.0f, 3.0f ) );
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


// The MAIN function, from here we start our application and run our Game loop
int main( )
{
    // Init GLFW
    glfwInit( );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    
    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr ); // Windowed
    
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
    glfwSetScrollCallback( window, ScrollCallback );
    
    // Options, removes the mouse cursor for a more immersive experience
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
    
    // Setup some OpenGL options
    glEnable( GL_DEPTH_TEST );
    
    // enable alpha support
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    // Setup and compile our shaders
    Shader ourShader( "resources/shaders/core.vs", "resources/shaders/frag.vs" );
    
    // Set up our vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] =
    {
        -0.05f, -0.05f, -0.05f,  0.0f, 0.0f,
        0.05f, -0.05f, -0.05f,  1.0f, 0.0f,
        0.05f,  0.05f, -0.05f,  1.0f, 1.0f,
        0.05f,  0.05f, -0.05f,  1.0f, 1.0f,
        -0.05f,  0.05f, -0.05f,  0.0f, 1.0f,
        -0.05f, -0.05f, -0.05f,  0.0f, 0.0f,
        
        -0.05f, -0.05f,  0.05f,  0.0f, 0.0f,
        0.05f, -0.05f,  0.05f,  1.0f, 0.0f,
        0.05f,  0.05f,  0.05f,  1.0f, 1.0f,
        0.05f,  0.05f,  0.05f,  1.0f, 1.0f,
        -0.05f,  0.05f,  0.05f,  0.0f, 1.0f,
        -0.05f, -0.05f,  0.05f,  0.0f, 0.0f,
        
        -0.05f,  0.05f,  0.05f,  1.0f, 0.0f,
        -0.05f,  0.05f, -0.05f,  1.0f, 1.0f,
        -0.05f, -0.05f, -0.05f,  0.0f, 1.0f,
        -0.05f, -0.05f, -0.05f,  0.0f, 1.0f,
        -0.05f, -0.05f,  0.05f,  0.0f, 0.0f,
        -0.05f,  0.05f,  0.05f,  1.0f, 0.0f,
        
        0.05f,  0.05f,  0.05f,  1.0f, 0.0f,
        0.05f,  0.05f, -0.05f,  1.0f, 1.0f,
        0.05f, -0.05f, -0.05f,  0.0f, 1.0f,
        0.05f, -0.05f, -0.05f,  0.0f, 1.0f,
        0.05f, -0.05f,  0.05f,  0.0f, 0.0f,
        0.05f,  0.05f,  0.05f,  1.0f, 0.0f,
        
        -0.05f, -0.05f, -0.05f,  0.0f, 1.0f,
        0.05f, -0.05f, -0.05f,  1.0f, 1.0f,
        0.05f, -0.05f,  0.05f,  1.0f, 0.0f,
        0.05f, -0.05f,  0.05f,  1.0f, 0.0f,
        -0.05f, -0.05f,  0.05f,  0.0f, 0.0f,
        -0.05f, -0.05f, -0.05f,  0.0f, 1.0f,
        
        -0.05f,  0.05f, -0.05f,  0.0f, 1.0f,
        0.05f,  0.05f, -0.05f,  1.0f, 1.0f,
        0.05f,  0.05f,  0.05f,  1.0f, 0.0f,
        0.05f,  0.05f,  0.05f,  1.0f, 0.0f,
        -0.05f,  0.05f,  0.05f,  0.0f, 0.0f,
        -0.05f,  0.05f, -0.05f,  0.0f, 1.0f
    };
    


    
    
    GLuint VBO, VAO;
    glGenVertexArrays( 1, &VAO );
    glGenBuffers( 1, &VBO );
    // Bind our Vertex Array Object first, then bind and set our buffers and pointers.
    glBindVertexArray( VAO );
    
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
    
    // Position attribute
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), ( GLvoid * )0 );
    glEnableVertexAttribArray( 0 );
    // TexCoord attribute
    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), ( GLvoid * )( 3 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray( 2 );
    
    glBindVertexArray( 0 ); // Unbind VAO
    
    // Load and create a texture
    GLuint texture;
    // --== TEXTURE == --
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture ); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
    // Set our texture parameters
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    // Set texture filtering
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    // Load, create texture and generate mipmaps
    int width, height;
    unsigned char *image = SOIL_load_image( "resources/images/image2.jpg", &width, &height, 0, SOIL_LOAD_RGB );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap( GL_TEXTURE_2D );
    SOIL_free_image_data( image );
    glBindTexture( GL_TEXTURE_2D, 0 ); // Unbind texture when done, so we won't accidentily mess up our texture.
    
    //Cubemap
    std::vector<std::string> faces =
    {
        "resources/images/right.jpg",
        "resources/images/left.jpg",
        "resources/images/top.jpg",
        "resources/images/bottom.jpg",
        "resources/images/front.jpg",
        "resources/images/back.jpg"
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
    
    glm::mat4 projection = glm::perspective( camera.GetZoom( ), ( float )SCREEN_WIDTH/( float )SCREEN_HEIGHT, 0.1f, 1000.0f );
    
    
    // Game loop
    while( !glfwWindowShouldClose( window ) )
    {
        // Set frame time
        GLfloat currentFrame = glfwGetTime( );
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Check and call events
        glfwPollEvents( );
        DoMovement( );
        
        // Clear the colorbuffer
        glClearColor( 0.05f, 0.05f, 0.05f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
        // Draw skybox as last
        glDepthMask( GL_FALSE );  // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Use( );

        
        
        glm::mat4 model(1);
        glm::mat4 view(1);
        view = glm::mat4( glm::mat3( camera.GetViewMatrix( ) ) );
        //view = camera.GetViewMatrix( );
        
        glUniformMatrix4fv( glGetUniformLocation( skyboxShader.Program, "view" ), 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( glGetUniformLocation( skyboxShader.Program, "projection" ), 1, GL_FALSE, glm::value_ptr( projection ) );
        
        
        // skybox cube
        glBindVertexArray( VAOcm );
        glBindTexture( GL_TEXTURE_CUBE_MAP, cubemapTexture );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        glDepthMask( GL_TRUE ); // Set depth function back to default
        
        // Draw our first triangle
        ourShader.Use( );
        
        // Bind Textures using texture units
        glActiveTexture( GL_TEXTURE0 );
        glBindTexture( GL_TEXTURE_2D, texture );
        glUniform1i( glGetUniformLocation( ourShader.Program, "texture1" ), 0 );
        
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation( ourShader.Program, "model" );
        GLint viewLoc = glGetUniformLocation( ourShader.Program, "view" );
        GLint projLoc = glGetUniformLocation( ourShader.Program, "projection" );
        
        // Pass the matrices to the shader
        glUniformMatrix4fv( viewLoc, 1, GL_FALSE, glm::value_ptr( view ) );
        glUniformMatrix4fv( projLoc, 1, GL_FALSE, glm::value_ptr( projection ) );
        
        glBindVertexArray( VAO );
        
        // Calculate the model matrix for each object and pass it to shader before drawing
        model = glm::translate( model, glm::vec3(-0.1f, 0.1f, -0.7f) );
        glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( model ) );
        glDrawArrays( GL_TRIANGLES, 0, 36 );
        glBindVertexArray( 0 );
        
        
 

        
        
        // Swap the buffers
        glfwSwapBuffers( window );
    }
    
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO );
    glDeleteVertexArrays( 1, &VAOcm );
    glDeleteBuffers( 1, &VBOcm );
    glfwTerminate( );
    
    return EXIT_SUCCESS;
}

// Moves/alters the camera positions based on user input
void DoMovement( )
{
    // Camera controls
    if( keys[GLFW_KEY_W] || keys[GLFW_KEY_UP] )
    {
        camera.ProcessKeyboard( FORWARD, deltaTime );
    }
    
    if( keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN] )
    {
        camera.ProcessKeyboard( BACKWARD, deltaTime );
    }
    
    if( keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT] )
    {
        camera.ProcessKeyboard( LEFT, deltaTime );
    }
    
    if( keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT] )
    {
        camera.ProcessKeyboard( RIGHT, deltaTime );
    }
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback( GLFWwindow *window, int key, int scancode, int action, int mode )
{
    if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if ( key >= 0 && key < 1024 )
    {
        if( action == GLFW_PRESS )
        {
            keys[key] = true;
        }
        else if( action == GLFW_RELEASE )
        {
            keys[key] = false;
        }
    }
}

void MouseCallback( GLFWwindow *window, double xPos, double yPos )
{
    if( firstMouse )
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


void ScrollCallback( GLFWwindow *window, double xOffset, double yOffset )
{
    camera.ProcessMouseScroll( yOffset );
}

