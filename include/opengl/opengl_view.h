#pragma once

#ifndef OPENGL_VIEW_H
#define OPENGL_VIEW_H

#include "imgui.h"
#include "render_view.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

class OpenGLView : public RenderView {

  protected:
    std::vector<GLuint> programNames;
    std::vector<GLuint> vertexArrayNames;
    std::vector<GLuint> bufferNames;
    std::vector<GLuint> textureNames;
    std::vector<GLuint> framebufferNames;

    bool initialized;
    bool error;

    virtual void initProgram() = 0;

    virtual void initBuffer() = 0;

    virtual void initTexture() = 0;

    virtual void initVertexArray() = 0;

    virtual void initFramebuffer() = 0;

    virtual void clean() = 0;

  public:
    OpenGLView(const char *viewName = "OpenGL View", int renderWidth = 1920, int renderHeight = 1080)
        : RenderView(viewName, renderWidth, renderHeight), error(false), initialized(false) {}
    virtual ~OpenGLView(){};

    /* Inits the render view */
    virtual void init() = 0;

    /* Returns a texture/rendered image as a ImTextureID pointer for ImGui to render to a renderview */
    virtual ImTextureID render() = 0;
};

#endif