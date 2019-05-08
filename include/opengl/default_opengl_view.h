#pragma once

#ifndef DEFAULT_VIEW_H
#define DEFAULT_VIEW_H

#include "opengl/opengl_view.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "framework/vertex.h"
#include "imgui.h"

class DefaultOpenGLView : public OpenGLView {

  protected:
    virtual void initProgram();

    virtual void initBuffer();

    virtual void initTexture();

    virtual void initVertexArray();

    virtual void initFramebuffer();

    virtual void clean();

  public:
    DefaultOpenGLView(int renderWidth = 1920, int renderHeight = 1080);
    virtual ~DefaultOpenGLView() { clean(); }

    /* Inits an OpenGL view */
    virtual void init();

    /* Returns a texture as a ImTextureID pointer for ImGui to render to a renderview */
    virtual ImTextureID render();
};

#endif