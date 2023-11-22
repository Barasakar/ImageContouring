#pragma once
#include "QtWidgetsApplication.h"
#include <qopenglfunctions.h>
#include <QtOpenGL/qopenglshaderprogram.h>

class QtOpenGLRender : public QOpenGLWidget {
public:
	QtOpenGLRender(QWidget* parent) : QOpenGLWidget(parent) { }
protected:
    void initializeGL() override
    {
        // Set up the rendering context, load shaders and other resources, etc.:
        QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
        f->glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set a white color with full opacity
        
        // load and compile shaders
        QOpenGLShader* vertexShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
        vertexShader->compileSourceCode("Vertex Shader GLSL code");

        QOpenGLShader* fragmentShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
        fragmentShader->compileSourceCode("Fragment Shader GLSL code");
        
        // setup buffers for vertices and etc.

        
    }

    //void resizeGL(int w, int h) override
    //{
    //    // Update projection matrix and other size related settings:
    //    m_projection.setToIdentity();
    //    m_projection.perspective(45.0f, w / float(h), 0.01f, 100.0f);
    //    
    //}

    //void paintGL() override
    //{
    //    // Draw the scene:
    //    QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();
    //    f->glClear(GL_COLOR_BUFFER_BIT);
    //    
    //}

};

