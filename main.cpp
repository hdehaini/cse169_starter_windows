// http://glew.sourceforge.net/
#include <GL/glew.h>

// http://freeglfw.sourceforge.net/
#include "core.h"

// http://glm.g-truc.net/
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <sstream>
#include <chrono>
#include <cstddef>

struct Vertex
{
    glm::vec4 pos;
    glm::vec4 color;
};
std::vector< Vertex > verts;
GLuint vbo = 0;
GLuint dlist = 0;

void init()
{
    // init geometry
    for( size_t i = 0; i < 10000000; i++ )
    {
        Vertex vert;
        vert.pos = glm::vec4( glm::linearRand( glm::vec3( -1.0f, -1.0f, -1.0f ), glm::vec3( 1.0f, 1.0f, 1.0f ) ), 1.0f );
        vert.color = glm::vec4( glm::linearRand( glm::vec3( 0.00f, 0.0f, 0.0f ), glm::vec3( 1.0f, 1.0f, 1.0f ) ), 1.0f );
        verts.push_back( vert );
    }

    // create display list
    dlist = glGenLists( 1 );
    glNewList( dlist, GL_COMPILE );
    glBegin( GL_POINTS );
    for( size_t i = 0; i < verts.size(); ++i )
    {
        glColor4fv( glm::value_ptr( verts[i].color) );
        glVertex4fv( glm::value_ptr( verts[i].pos) );
    }
    glEnd();
    glEndList();

    // create VBO
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( Vertex ) * verts.size(), verts.data(), GL_STATIC_DRAW );
}

unsigned int method = 0;
void keyboard( unsigned char key, int x, int y )
{
    if( 'n' == key )
    {
        method++;
        if( method > 3 ) method = 0;
    }
}

void display()
{
    // timekeeping
    static std::chrono::steady_clock::time_point prv = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point cur = std::chrono::steady_clock::now();
    const float dt = std::chrono::duration< float >( cur - prv ).count();
    prv = cur;

    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    double w = 800;
    double h = 800;
    gluPerspective( 60.0, w / h, 0.1, 10.0 );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt( 2, 2, 2, 0, 0, 0, 0, 0, 1 );

    static float angle = 0.0f;
    angle += dt * 6.0f;
    glRotatef( angle, 0, 0, 1 );

    // render
    switch( method )
    {
    case 0:
        // VBO
        glBindBuffer( GL_ARRAY_BUFFER, vbo );
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_COLOR_ARRAY );
        glVertexPointer( 4, GL_FLOAT, sizeof( Vertex ), (void*)offsetof( Vertex, pos ) );
        glColorPointer( 4, GL_FLOAT, sizeof( Vertex ), (void*)offsetof( Vertex, color ) );
        glDrawArrays( GL_POINTS, 0, verts.size() );
        glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_COLOR_ARRAY );
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
        break;

    case 1:
        // vertex array
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_COLOR_ARRAY );
        glVertexPointer( 4, GL_FLOAT, sizeof( Vertex ), &verts[0].pos );
        glColorPointer( 4, GL_FLOAT, sizeof( Vertex ), &verts[0].color );
        glDrawArrays( GL_POINTS, 0, verts.size() );
        glDisableClientState( GL_VERTEX_ARRAY );
        glDisableClientState( GL_COLOR_ARRAY );
        break;

    case 2:
        // display list
        glCallList( dlist );
        break;

    case 3:
        // immediate mode
        glBegin( GL_POINTS );
        for( size_t i = 0; i < verts.size(); ++i )
        {
            glColor4fv( glm::value_ptr( verts[i].color) );
            glVertex4fv( glm::value_ptr( verts[i].pos) );
        }
        glEnd();
        break;
    }

    // info/frame time output
    std::stringstream msg;
    msg << "Using ";
    switch( method )
    {
    case 0: msg << "vertex buffer object"; break;
    case 1: msg << "vertex array"; break;
    case 2: msg << "display list"; break;
    case 3: msg << "immediate mode"; break;
    }
    msg << std::endl;
    msg << "Frame time: " << (dt * 1000.0f) << " ms";
    glColor3ub( 255, 255, 0 );
    glWindowPos2i( 10, 25 );
    glfwBitmapString( glfw_BITMAP_9_BY_15, (unsigned const char*)( msg.str().c_str() ) );

    glfwSwapBuffers();
}

int main(int argc, char **argv)
{
    glfwInit( &argc, argv );
    glfwInitDisplayMode( glfw_RGBA | glfw_DOUBLE );
    glfwInitWindowSize( 600, 600 );
    glfwCreateWindow( "glfw" );
    glewInit();
    init();
    glfwDisplayFunc( display );
    glfwKeyboardFunc( keyboard );
    glfwIdleFunc( display );
    glfwMainLoop();
    return 0;
}