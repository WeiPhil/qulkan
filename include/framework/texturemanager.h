#pragma once

#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <map>

class TextureManager {

  private:
    int m_max;
    std::map<std::string, GLuint> m_textures;

  public:
    TextureManager();
    ~TextureManager();

    void addTexture(std::string textureName, GLuint ref) {
        m_textures[textureName] = ref;
        ++m_max;
    }

    GLuint textureID(std::string textureName) { return m_textures[textureName]; }
};

#endif
