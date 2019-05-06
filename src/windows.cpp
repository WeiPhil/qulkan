
#include "windows.h"
#include "imgui.h"

#include "qulkan/logger.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
/*! \brief Create a Docking Space
 *         The docking space needs to be rendered before any other window.
 *
 *
 *  For more information about the docking space refer to imgui_demo.cpp
 *
 */
void dockingSpace() {
    bool opt_fullscreen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window
    // not dockable into, because it would be confusing to have two docking
    // targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render
    // our background and handle the pass-thru hole, so we ask Begin() to not
    // render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Test", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiIO &io = ImGui::GetIO();
    // DockSpace
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Create New Project");
            ImGui::Separator();
            ImGui::MenuItem("Exit");

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            ImGui::MenuItem("Preferences...");
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            ImGui::MenuItem("About Qulkan...");
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End(); // End DockSpace Test

    return;
}

/*! \brief Render the window for the main view
 *
 *
 */
void renderWindow(RenderView &renderView) {

    // Main window containing the OpenGL/Vulkan rendering
    ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);

    ImGui::Begin("Rendering View");

    float ratio = 1920.0f / 1080.0f;

    bool keepTextureRatio = true;

    float w = ImGui::GetContentRegionAvail().x;
    float h = ImGui::GetContentRegionAvail().y;

    ImVec2 startPos = ImGui::GetCursorScreenPos();
    ImVec2 endPos = ImVec2(startPos.x + w, startPos.y + h);

    ImTextureID tex = renderView.renderToTexture();

    if (!keepTextureRatio)
        ImGui::GetWindowDrawList()->AddImage(tex, startPos, endPos);
    else {
        float minSize = glm::min(w, h * ratio);
        float maxSize = glm::max(w, h * ratio);

        bool widthIsMax = w > h * ratio;

        float space = widthIsMax ? (maxSize - minSize) * 0.5f : (maxSize - minSize) * 0.5f / ratio;

        if (widthIsMax) {
            startPos.x += space;
            endPos.x -= space;
        } else {
            startPos.y += space;
            endPos.y -= space;
        }

        ImGui::GetWindowDrawList()->AddImage(tex, startPos, endPos);
    }

    ImGui::End();
}