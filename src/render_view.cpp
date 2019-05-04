#include "render_view.h"
#include "imgui.h"

#include <iostream>

#include "utils/logger.h"
#include <glm/glm.hpp>

/* Sets all array buffer etc.. */
OpenGLInits initRenderView() {
    Qulkan::Logger::Info("Preparing Render View\n");

    // Trying out to output a simple triangle
    //--------------------------------------------------

    // vertices we want to use (a triangle here)
    float vertices[] = {
        0.5f,  0.5f,  0.0f, // top right
        0.5f,  -0.5f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f,  0.0f  // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    int num_vertices = sizeof(vertices) / (3 * sizeof(float));
    int num_triangles = sizeof(vertices) / sizeof(vertices[0]);

    // Create vertex array object
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    // Create vertex buffer object
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // Create an ellement buffer object for indexed drawing
    GLuint EBO;
    glGenBuffers(1, &EBO);

    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "\n"
                                     "void main()\n"
                                     "{\n"
                                     "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\n";

    GLuint vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Assign shader from source and compile shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        Qulkan::Logger::Error("SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLog);
    }

    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "uniform vec4 ourColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f)*ourColor;\n"
                                       "}";

    GLuint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Assign shader from source and compile shader
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        Qulkan::Logger::Error("SHADER::FRAGMENT::COMPILATION_FAILED\n%s", infoLog);
    }

    // Create shader program and attach the shaders to it
    GLuint shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        Qulkan::Logger::Error("SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
    }

    // Use the linked shader programm and delete shaders now useless
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use from the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3. then set our vertex attributes pointers
    // In order parameters reason are :
    //      location=0 for position, xyz values, float type, no normalization,
    //      only one type of data (position) hence 3 float, 0 offset in this case
    //      since only position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);     // location attribbute
    glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind VBO buffer (because you can)

    // Bind the ellement buffer object
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //--------------------------------------------------
    if (!success)
        Qulkan::Logger::Error("An error occurred during preparation\n");
    else
        Qulkan::Logger::Info("Render View Setup\n");

    return {shaderProgram, VAO, num_triangles};
}

/* Renders a simple OpenGL triangle in the rendering view */
void renderOpenGLView(const OpenGLInits &inputs) {

    const auto [shaderProgram, VAO, num_triangles] = inputs;

    float timeValue = glfwGetTime();
    float redGreenValue = (sin(timeValue) / 2.0f) + 0.5f;
    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
    glUseProgram(shaderProgram);
    glUniform4f(vertexColorLocation, redGreenValue, redGreenValue, 0.0f, 1.0f);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, num_triangles, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}