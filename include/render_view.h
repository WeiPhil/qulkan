#pragma once

#ifndef RENDER_VIEW_H
#define RENDER_VIEW_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

struct OpenGLInits {
    GLuint shaderProgram;
    GLuint VAO;
    int num_triangles;

    OpenGLInits(GLuint const &shaderProgram, GLuint const &VAO, int num_triangles) : shaderProgram(shaderProgram), VAO(VAO), num_triangles(num_triangles) {}
};

/* Constructs the rendering view */
void renderingView();

/* Sets all array buffer etc.. */
OpenGLInits initRenderView();

/* Renders a simple OpenGL triangle in the rendering view */
void renderOpenGLView(const OpenGLInits &openGLInits);

#endif