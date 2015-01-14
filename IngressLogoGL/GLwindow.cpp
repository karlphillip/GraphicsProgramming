/* Copyright (C) 2012 Karl Phillip Buhr <karlphillip@gmail.com>
 *
 * This work is licensed under the Creative Commons Attribution-ShareAlike License.
 * To view a copy of this license, visit:
 *      https://creativecommons.org/licenses/by-sa/2.5/legalcode
 *
 * Or to read the human-readable summary of the license:
 *      https://creativecommons.org/licenses/by-sa/2.5/
 *
 *
 * Ingress logo with glowing effect, based on Nehe lesson 36.
 */
#include "GLwindow.h"

#include <GL/glu.h>
#include <iostream>

#include <QKeyEvent>
#include <QTimer>


GLwindow::GLwindow(QWidget *parent)
: QGLWidget(parent)
{
    _width = 0;
    _height = 0;
}

GLwindow::~GLwindow()
{
    glDeleteTextures(1, &_blur_texture);
}

void GLwindow::_tick()
{
    update(); // triggers paintGL()
    QTimer::singleShot(33, this, SLOT(_tick()));
}

void GLwindow::initializeGL()
{
    _blur_texture = _gen_empty_texture();

    GLfloat global_ambient[4]={0.2f, 0.2f,  0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    glEnable(GL_LIGHTING);

    GLfloat light0pos[4]      = { 0.0f, 2.0f, 10.0f, 1.0f };
    GLfloat light0ambient[4]  = { 0.2f, 0.2f,  0.2f, 1.0f };
    GLfloat light0diffuse[4]  = { 0.3f, 0.3f,  0.3f, 1.0f };
    GLfloat light0specular[4] = { 0.8f, 0.8f,  0.8f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, light0pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light0ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0specular);
    glEnable(GL_LIGHT0);

    glMateriali(GL_FRONT, GL_SHININESS, 128);

    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    _tick();
}

GLuint GLwindow::_gen_empty_texture()
{
    // Create storage space for texture data (256x256x4)
    unsigned int* data = (unsigned int*) new GLuint[((256 * 256)* 4 * sizeof(unsigned int))];
    memset(data, 0, ((256 * 256)* 4 * sizeof(unsigned int)));

    GLuint txtnumber = 0;
    glGenTextures(1, &txtnumber);
    glBindTexture(GL_TEXTURE_2D, txtnumber);

    glTexImage2D(GL_TEXTURE_2D, 0, 4, 256, 256, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data)
        delete[] data;

    return txtnumber;
}

void GLwindow::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    _render_to_texture();
    _draw_ingress_logo();
    _draw_blur(8, 0.008f);
}

void GLwindow::_render_to_texture()
{
    glViewport(0, 0, 256, 256);

    _draw_ingress_logo();

    glBindTexture(GL_TEXTURE_2D, _blur_texture);

    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 0, 0, 256, 256, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.5);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, _width ,_height);
}

void GLwindow::_draw_ingress_logo()
{
    GLfloat material_color[] = { 0.5f, 0.5f, 1.0f, 1.0f };		// Set the material color
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, material_color);

    GLfloat specular[] = { 0.5f, 0.5f, 1.0f, 1.0f };             // Sets the specular lighting
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);

    glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);

    _draw_quad();
    _draw_triangle();
}

void GLwindow::_draw_blur(int times, float inc)
{
    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);

    glEnable(GL_TEXTURE_2D);									// Enable 2D Texture Mapping
    glDisable(GL_DEPTH_TEST);									// Disable Depth Testing
    glBlendFunc(GL_SRC_ALPHA,GL_ONE);							// Set Blending Mode
    glEnable(GL_BLEND);											// Enable Blending
    glBindTexture(GL_TEXTURE_2D, _blur_texture);                // Bind To The Blur Texture
    _view_ortho();												// Switch To An Ortho View

    float alpha = 0.2f;                                         // Starting Alpha Value
    float alphainc = alpha / times;								// alphainc=0.3f / Times To Render Blur

    int num;                                                    // Starting Alpha Value
    float spost = 0.0f;											// Starting Texture Coordinate Offset
    glBegin(GL_QUADS);											// Begin Drawing Quads
        for (num = 0; num < times; num++)                       // Number Of Times To Render Blur
        {
            glColor4f(1.0f, 1.0f, 1.0f, alpha);					// Set The Alpha Value (Starts At 0.2)

            glTexCoord2f(0+spost, 1-spost);						// Texture Coordinate	( 0, 1 )
            glVertex2f(0,0);									// First Vertex		    ( 0, 0 )

            glTexCoord2f(0+spost, 0+spost);						// Texture Coordinate	( 0, 0 )
            glVertex2f(0, _height);								// Second Vertex        ( 0, _height )

            glTexCoord2f(1-spost, 0+spost);						// Texture Coordinate	( 1, 0 )
            glVertex2f(_width, _height);						// Third Vertex         ( _width, _height )

            glTexCoord2f(1-spost, 1-spost);						// Texture Coordinate	( 1, 1 )
            glVertex2f(_width, 0);                              // Fourth Vertex	    ( _width, 0 )

            spost += inc;										// Gradually Increase spost (Zooming Closer To Texture Center)
            alpha = alpha - alphainc;							// Gradually Decrease alpha (Gradually Fading Image Out)
        }
    glEnd();													// Done Drawing Quads

    _view_perspective();                                        // Switch To A Perspective View

    glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
    glDisable(GL_TEXTURE_2D);									// Disable 2D Texture Mapping
    glDisable(GL_BLEND);										// Disable Blending
    glBindTexture(GL_TEXTURE_2D,0);								// Unbind The Blur Texture
}

void GLwindow::_draw_triangle()
{
    glPushMatrix();

    glTranslatef(0.0f, 4.0f, -9.0f);
    glScalef(2.f, 2.f, 2.f);

    glLineWidth(5.0f);
    glColor3f(1.0f, 1.0f, 1.0f);                  // White
    glBegin(GL_LINES);

        glVertex3f(-1.4f, -1.5f, -1.0f);         // Top
        glVertex3f( 1.4f, -1.5f, -1.0f);

        glVertex3f( 1.4f, -1.5f, -1.0f);         // Right
        glVertex3f( 0.0f, -3.8f, -1.0f);

        glVertex3f(-1.4f, -1.5f, -1.0f);         // Left
        glVertex3f( 0.0f, -3.8f, -1.0f);

        glVertex3f( 0.0f, -2.3f, -1.0f);         // Right diagonal
        glVertex3f( 1.4f, -1.5f, -1.0f);

        glVertex3f( 0.0f, -2.3f, -1.0f);         // Left diagonal
        glVertex3f(-1.4f, -1.5f, -1.0f);

        glVertex3f( 0.0f, -2.3f, -1.0f);         // Bottom diagonal
        glVertex3f( 0.0f, -3.8f, -1.0f);

    glEnd();

    glPopMatrix();
}

void GLwindow::_draw_quad()
{
    glPushMatrix();

    glTranslatef(0.0f, 4.0f, -9.0f);
    glScalef(2.f, 2.f, 2.f);

    glLineWidth(5.0f);
    glColor3f(1.0f, 1.0f, 1.0f);                 // White

    glBegin(GL_LINES);
        glVertex3f( 0.0f,  0.0f, -1.0f);         // Top vertical line
        glVertex3f( 0.0f, -1.0f, -1.0f);

        glVertex3f( 1.9f, -3.2f, -1.0f);         // Right diagonal line
        glVertex3f( 1.1f, -2.7f, -1.0f);

        glVertex3f(-1.9f, -3.2f, -1.0f);         // Left diagonal line
        glVertex3f(-1.1f, -2.7f, -1.0f);

        glVertex3f( 0.0f,  0.0f, -1.0f);         // Right Of The Quad (Top)
        glVertex3f( 2.0f, -1.2f, -1.0f);

        glVertex3f( 2.0f, -1.2f, -1.0f);         // Right Of The Quad (Center)
        glVertex3f( 2.0f, -3.2f, -1.0f);

        glVertex3f( 2.0f, -3.2f, -1.0f);         // Right Of The Quad (Bottom)
        glVertex3f( 0.0f, -4.4f, -1.0f);

        glVertex3f( 0.0f,  0.0f, -1.0f);         // Left Of The Quad (Top)
        glVertex3f(-2.0f, -1.2f, -1.0f);

        glVertex3f(-2.0f, -1.2f, -1.0f);         // Left Of The Quad (Center)
        glVertex3f(-2.0f, -3.2f, -1.0f);

        glVertex3f(-2.0f, -3.2f, -1.0f);         // Left Of The Quad (Bottom)
        glVertex3f( 0.0f, -4.4f, -1.0f);
    glEnd();

    glPopMatrix();
}

void GLwindow::resizeGL( int w, int h)
{
    _width = w;
    _height = h;
    glViewport(0, 0, _width, _height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Compute aspect ratio of the window
    if (_height == 0)
    {
       gluPerspective (60, (float) _width, 1.0, 50.0);
    }
    else
    {
       gluPerspective (60, (float) _width / (float) _height, 1.0, 50.0);
    }

    gluLookAt(0.0,  0.0, 2.0,   // eye
              0.0,  0.0, 0.0,   // center
              0.0,  1.0, 0.0);  // up
}

void GLwindow::_view_ortho()                                    // Set Up An Ortho View
{
    glMatrixMode(GL_PROJECTION);								// Select Projection
    glPushMatrix();												// Push The Matrix
    glLoadIdentity();											// Reset The Matrix
    glOrtho( 0, _width , _height, 0, -1, 1 );                   // Select Ortho Mode (640x480)
    glMatrixMode(GL_MODELVIEW);									// Select Modelview Matrix
    glPushMatrix();												// Push The Matrix
    glLoadIdentity();											// Reset The Matrix
}

void GLwindow::_view_perspective()                              // Set Up A Perspective View
{
    glMatrixMode( GL_PROJECTION );								// Select Projection
    glPopMatrix();												// Pop The Matrix
    glMatrixMode( GL_MODELVIEW );								// Select Modelview
    glPopMatrix();												// Pop The Matrix
}
