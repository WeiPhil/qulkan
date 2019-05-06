#pragma once

#ifndef OPENGL_VIEW_H
#define OPENGL_VIEW_H

#include "render_view.h"

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "framework/vertex.h"

class OpenGLView : public RenderView {

  private:
    std::vector<GLuint> programName;
    std::vector<GLuint> vertexArrayName;
    std::vector<GLuint> bufferName;
    std::vector<GLuint> textureName;

    bool initialized;
    bool error;

    void initProgram();

    void initBuffer();

    void initTexture();

    bool initVertexArray();

  public:
    OpenGLView();

    /* Inits an OpenGL view */
    virtual void init();

    /* Renders a simple OpenGL triangle in the rendering view */
    virtual void render();

    /* Returns a texture as a void* pointer for ImGui to render to a window */
    virtual void *renderToTexture();
};

#endif