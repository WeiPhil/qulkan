#pragma once

#ifndef DEFAULT_VIEW_H
#define DEFAULT_VIEW_H

#include "render_view.h"

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include "framework/vertex.h"
#include "imgui.h"

class DefaultOpenglView : public RenderView {

  private:
    std::vector<GLuint> programNames;
    std::vector<GLuint> vertexArrayNames;
    std::vector<GLuint> bufferNames;
    std::vector<GLuint> textureNames;
    std::vector<GLuint> framebufferNames;

    bool initialized;
    bool error;

    void initProgram();

    void initBuffer();

    void initTexture();

    void initVertexArray();

    void initFramebuffer();

    void clean();

  public:
    DefaultOpenglView(int renderWidth = 1920, int renderHeight = 1080);
    virtual ~DefaultOpenglView() { clean(); }

    /* Inits an OpenGL view */
    virtual void init();

    /* Returns a texture as a ImTextureID pointer for ImGui to render to a renderview */
    virtual ImTextureID render();
};

#endif