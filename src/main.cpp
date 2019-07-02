// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_style.h"

#include <GL/gl3w.h> // Initialize with gl3wInit()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

// Local includes
#include "qulkan/inputshandler.h"
#include "qulkan/logger.h"
#include "qulkan/render_view.h"
#include "qulkan/utils.h"
#include "qulkan/windows.h"

#include "examples/approx_reflection_aniso.h"
#include "examples/approx_reflection_aniso_two_bounces.h"
#include "examples/sphereProjection.h"

// Basic Examples
#include "examples/opengl/basic/camera.h"
#include "examples/opengl/basic/coordinatesystems.h"
#include "examples/opengl/basic/hellotriangle.h"
#include "examples/opengl/basic/textures.h"
#include "examples/opengl/basic/transformations.h"

// Lighting Examples
#include "examples/opengl/lighting/colors.h"
#include "examples/opengl/lighting/materials.h"

#include "examples/ggx_reflection.h"
#include "examples/gt_reflection_aniso.h"
#include "examples/gt_reflection_aniso_two_bounces.h"

#include "utils/pngwriter.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <stdio.h>
#include <vector>

static void glfw_error_callback(int error, const char *description) { Qulkan::Logger::Error("Glfw Error %d: %s\n", error, description); }

// #define LOG(fmt, ...)                                                                                                                                          \
//     fprintf(stdout, fmt, ##__VA_ARGS__);                                                                                                                       \
//     fflush(stdout);
// static void APIENTRY debug_output_logger(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void
// *userParam) {
//     char srcstr[32], typestr[32];

//     switch (source) {
//     case GL_DEBUG_SOURCE_API:
//         strcpy(srcstr, "OpenGL");
//         break;
//     case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
//         strcpy(srcstr, "Windows");
//         break;
//     case GL_DEBUG_SOURCE_SHADER_COMPILER:
//         strcpy(srcstr, "Shader Compiler");
//         break;
//     case GL_DEBUG_SOURCE_THIRD_PARTY:
//         strcpy(srcstr, "Third Party");
//         break;
//     case GL_DEBUG_SOURCE_APPLICATION:
//         strcpy(srcstr, "Application");
//         break;
//     case GL_DEBUG_SOURCE_OTHER:
//         strcpy(srcstr, "Other");
//         break;
//     default:
//         strcpy(srcstr, "???");
//         break;
//     };

//     switch (type) {
//     case GL_DEBUG_TYPE_ERROR:
//         strcpy(typestr, "Error");
//         break;
//     case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
//         strcpy(typestr, "Deprecated Behavior");
//         break;
//     case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
//         strcpy(typestr, "Undefined Behavior");
//         break;
//     case GL_DEBUG_TYPE_PORTABILITY:
//         strcpy(typestr, "Portability");
//         break;
//     case GL_DEBUG_TYPE_PERFORMANCE:
//         strcpy(typestr, "Performance");
//         break;
//     case GL_DEBUG_TYPE_OTHER:
//         strcpy(typestr, "Message");
//         break;
//     default:
//         strcpy(typestr, "???");
//         break;
//     }

//     if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM) {
//         LOG("djg_debug_output: %s %s\n"
//             "-- Begin -- GL_debug_output\n"
//             "%s\n"
//             "-- End -- GL_debug_output\n",
//             srcstr, typestr, message);

//         // abort();
//     } else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
//         LOG("djg_debug_output: %s %s\n"
//             "-- Begin -- GL_debug_output\n"
//             "%s\n"
//             "-- End -- GL_debug_output\n",
//             srcstr, typestr, message);

//         // abort();
//     }
// }

// void log_debug_output(void) {
//     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
//     glDebugMessageCallback(&debug_output_logger, NULL);
// }

int main(int, char **) {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
    // GL 3.3 + GLSL 330
    const char *glsl_version = "#version 330 ";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Create window with graphics context
    GLFWwindow *window = glfwCreateWindow(1920, 1080, "Qulkan", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = gl3wInit() != 0;
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
    // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::StyleColorsSober(&style);

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error
    // and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which
    // ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // io.Fonts->AddFontDefault();
    io.Fonts->AddFontFromFileTTF("../ext/imgui/misc/fonts/DroidSans.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../ext/imgui/misc/fonts/Roboto-Medium.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../data/fonts/monaco.ttf", 14.0f);
    io.Fonts->AddFontFromFileTTF("../ext/imgui/misc/fonts/Cousine-Regular.ttf", 16.0f);
    io.Fonts->AddFontFromFileTTF("../ext/imgui/misc/fonts/ProggyTiny.ttf", 10.0f);
    io.Fonts->AddFontFromFileTTF("../data/fonts/GeosansLight.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("../data/fonts/Rounded_Elegance.ttf", 16.0f);

    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    // IM_ASSERT(font != NULL);

    // Prepare all necesary stuff for main rendering loop

    bool show_demo_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::vector<std::reference_wrapper<Qulkan::RenderView>> renderViews;

    SphereProjection sphereProjection("Sphere Projection", 1024, 1024);
    // GTReflectionAniso gtReflectionAniso("GT Reflection Aniso", 512, 512);
    // GTReflectionAnisoTwoBounces gtReflectionAnisoTwoBounces("GT Reflection Aniso Two Bounces", 512, 512);
    // ApproxReflectionAniso approxReflectionAniso("Approx Reflection Aniso", 512, 512);
    // ApproxReflectionAnisoTwoBounces approxReflectionAnisoTwoBounces("Approx Reflection Aniso Two Bounces", 512, 512);

    // OpenGLExamples::HelloTriangle helloTriangleExample = OpenGLExamples::HelloTriangle("OpenGL Example: HelloTriangle", 512, 512);
    // OpenGLExamples::Textures textureExample = OpenGLExamples::Textures("OpenGL Example: Textures", 512, 512);
    // OpenGLExamples::Transformations transformationExample = OpenGLExamples::Transformations("OpenGL Example: Transformations", 512, 512);
    // OpenGLExamples::CoordinateSystems coordinateSystemsExample = OpenGLExamples::CoordinateSystems("OpenGL Example: CoordinateSystems", 512, 512);
    // OpenGLExamples::Camera cameraExample = OpenGLExamples::Camera("OpenGL Example: Camera", 512, 512);
    // OpenGLExamples::Colors colorsExample = OpenGLExamples::Colors("OpenGL Example: Colors", 512, 512);
    // OpenGLExamples::Materials materialsExample = OpenGLExamples::Materials("OpenGL Example: Materials", 1920, 1080);

    // renderViews.push_back(helloTriangleExample);
    // renderViews.push_back(textureExample);
    // renderViews.push_back(transformationExample);
    // renderViews.push_back(coordinateSystemsExample);
    // renderViews.push_back(cameraExample);
    // renderViews.push_back(colorsExample);
    // renderViews.push_back(materialsExample);

    renderViews.push_back(sphereProjection);
    // renderViews.push_back(gtReflectionAniso);
    // renderViews.push_back(gtReflectionAnisoTwoBounces);
    // renderViews.push_back(approxReflectionAniso);
    // renderViews.push_back(approxReflectionAnisoTwoBounces);

    Qulkan::initViews(renderViews);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        Qulkan::updateDeltaTime(glfwGetTime());

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        bool noViewActive = std::none_of(renderViews.begin(), renderViews.end(), [](Qulkan::RenderView &r) { return r.isActive(); });
        // Create Docking space
        Qulkan::dockingSpace();

        // Create Simple log window (see function for how to use it)
        Qulkan::Logger::Instance().Window();

        // Close on escape
        if (noViewActive && ImGui::IsKeyPressed(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        // IMPORTANT: keep this to have an easy accessible documentation of features
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        Qulkan::viewConfigurations(renderViews);

        Qulkan::renderWindows(renderViews);

        Qulkan::handleInputs(renderViews);

        // Rendering
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        //     GLFWwindow *backup_current_context = glfwGetCurrentContext();
        //     ImGui::UpdatePlatformWindows();
        //     ImGui::RenderPlatformWindowsDefault();
        //     glfwMakeContextCurrent(backup_current_context);
        // }

        glfwSwapBuffers(window);
        Qulkan::updateFrameNumber();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
