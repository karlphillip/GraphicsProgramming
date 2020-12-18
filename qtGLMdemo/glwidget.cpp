#include "glwidget.h"
#include <GL/GLU.h>

#include <iostream>

#include <QApplication>
#include <QKeyEvent>
#include <QTimer>


GLWidget::GLWidget(QWidget *parent)
: QGLWidget(parent)
{
    resize(1280, 720);

    _width = 0;
    _height = 0;
    _obj_model = NULL;

    // initialize the orbit rotation angle
    _orbitAngle = 0;

    // initialize camera position
    _cameraPos[0] = 0.f;
    _cameraPos[1] = 1.5f;
    _cameraPos[2] = 2.6f;
}

GLWidget::~GLWidget()
{
    if (_obj_model)
        glmDelete(_obj_model);
}

void GLWidget::_tick()
{
    update();

    QTimer::singleShot(33, this, SLOT(_tick()));
}

void GLWidget::initializeGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
    glClearDepth(1.0f);									// Depth Buffer Setup

    glEnable(GL_CULL_FACE);                             // Discard back faces
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Anti-aliasing */

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    //glEnable(GL_POLYGON_SMOOTH);
    //glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    /* Enable lightning and light - GLM needs it */

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lightDiffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat lightAmbient[]  = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat lightPosition[] = { 0.0, 4.0, 3.0, 1.0 };
    GLfloat lightModel[]    = { 0.2, 0.2, 0.2, 1.0 };

    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lightModel);

    /* Load model with GLM */

    _obj_model = glmReadOBJ((char*)"data/porsche.obj");

    // Rescale the object to fit into the unity matrix.
    glmUnitize(_obj_model);

    /* glmFacetNormals() and glmVertexNormals() calculate triangle and
     * vertex normals from the geometry data. However, if you prefer,
     * you can use a 3D modelling to precalculate them and export it
     * to the Wavefront file.
     */
    glmFacetNormals(_obj_model);
    glmVertexNormals(_obj_model, 90.0);

    // preload the mesh into a proper display list
    _carList = glGenLists(1);
    glNewList(_carList, GL_COMPILE);
    glmDraw(_obj_model, GLM_SMOOTH | GLM_MATERIAL);
    glEndList();

    //std::cout << (char*)glGetString(GL_EXTENSIONS) << std::endl;

    /* Start the timer */

    _tick();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // orbit camera around the scene
    if (_orbitAngle > 360)
        _orbitAngle = 0;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* simulate camera orbit */

    gluLookAt(_cameraPos[0], _cameraPos[1], _cameraPos[2],  // eye
              0.0, 0.0, 0.0,                                // center
              0.0, 1.0, 0.0);                               // up vector

    glRotatef(_orbitAngle, 0.0, 1.0, 0.0);

    /* draw 3D objects */

    _draw_terrain();
    _draw_model();    

    glFlush();

    // increment the rotation angle for the orbit camera
    _orbitAngle += 1;
}

void GLWidget::_draw_terrain()
{
    // Disable Lighting to set line colors (else it won't work)
    glDisable(GL_LIGHTING);

    glPushMatrix();

    // the origin also defines the geometrical center of the drawing
    float centerPosX = 0.f;
    float centerPosY = 0.f;
    float centerPosZ = 0.f;
    glTranslatef(centerPosX, centerPosY, centerPosZ);

    // variables that control grid size and the number of lines draw
    int gridSize = 4;
    int gridLines = 10;

    glBegin(GL_LINES);
        glColor3ub(180, 180, 180);

        /* draw the horizontal lines first */

        float minZ = centerPosZ + (gridSize/2.f);   //  4
        int maxZ = centerPosZ - (gridSize/2.f);     // -4
        int zRange = std::abs(maxZ - minZ);
        float stepZ = zRange / (float)gridLines;
        //std::cout << "minZ=" << minZ << " maxZ=" << maxZ << " zRange=" << zRange << " stepZ=" << stepZ << std::endl;

        GLfloat x1 = centerPosX-(gridSize/2.f);
        GLfloat y1 = centerPosY;
        GLfloat x2 = centerPosX+(gridSize/2.f);
        GLfloat y2 = y1;

        for (; minZ >= maxZ; minZ -= stepZ)
        {
            //std::cout << "x1=" << x1 << " y1=" << y1 << " z1=" << minZ << "  x2=" << x2 << " y2=" << y2 << " z2=" << minZ << std::endl;
            glVertex3f(x1, y1, minZ); glVertex3f(x2, y2, minZ);
        };

        /* draw the vertical lines */

        //glColor3ub(0, 0, 255);

        float minX = centerPosX - (gridSize/2.f);
        int maxX = centerPosX + (gridSize/2.f);
        int xRange = maxX - minX;
        float stepX = xRange / (float)gridLines;
        //std::cout << "minX=" << minX << " maxX=" << maxX << " xRange=" << xRange << " stepX=" << stepX << std::endl;

        y1 = centerPosY;
        GLfloat z1 = centerPosZ-(gridSize/2.f);
        y2 = centerPosY;
        GLfloat z2 = centerPosZ+(gridSize/2.f);

        for (; minX <= maxX; minX += stepX)
        {
            glVertex3f(minX, y1, z1); glVertex3f(minX, y2, z2);
        };

    glEnd();

    glPopMatrix();

    // Enable Lighting again
    glEnable(GL_LIGHTING);
}

void GLWidget::_draw_model()
{
    glPushMatrix();

    // raise the model a little above the horizon line so the tires touch the grid lines
    glTranslatef(0.f, 0.3f, 0.f);

    // increasing model size messes up the light reflection on the car
    //glScalef(1.3f, 1.3f, 1.3f);

    // draw the car mesh
    glCallList(_carList);

    glPopMatrix();
}

void perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    GLdouble ymax = zNear * tan( fovy * M_PI / 360.0 );
    GLdouble ymin = -ymax;
    GLdouble xmin = ymin * aspect;
    GLdouble xmax = ymax * aspect;

    glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

void GLWidget::resizeGL( int w, int h)
{
    _width = w;
    _height = h;

    glViewport     ( 0, 0, w, h );
    glMatrixMode   ( GL_PROJECTION );
    glLoadIdentity ( );

    if (h == 0)
        perspective( 60, ( float ) w, 1.0, 100.0 );
    else
        perspective( 60, ( float ) w / ( float ) h, 1.0, 100.0 );

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GLWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Escape:
            QApplication::quit();
            break;

         default:
             break;
    }
}

