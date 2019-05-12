
#include "qulkan/windows.h"

#include "imgui.h"
#include "qulkan/handlemanager.h"
#include "qulkan/logger.h"
#include "qulkan/utils.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>
#include <memory>

namespace Qulkan {

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

    /*! \brief Helper for creating the handles in the configuration view
     *
     *
     */
    void mirrorHandles(const HandleManager &srcHandler, const HandleManager &destHandler) {
        for (auto const &srcHandle : srcHandler.getHandles()) {
            for (auto const &destHandle : destHandler.getHandles()) {
                if (srcHandle->name == destHandle->name) {
                    destHandle->value = srcHandle->value;
                }
            }
        }
    }

    /*! \brief Helper for creating the handles in the configuration view
     *
     *
     */
    void handleParser(const HandleManager &handleManager) {

        for (auto const &handle : handleManager.getHandles()) {
            switch (handle->type) {
            case Type::INT: {
                ImGui::SliderInt(handle->name.c_str(), std::any_cast<int>(&handle->value), -10, 10, "%d");
                break;
            }
            case Type::FLOAT: {
                ImGui::SliderFloat(handle->name.c_str(), std::any_cast<float>(&handle->value), 0.0f, 1.0f, "%.4f");
                break;
            }
            case Type::VEC2: {
                auto val = std::any_cast<glm::vec2>(&handle->value);
                ImGui::SliderFloat2(handle->name.c_str(), (float *)val, -1.0f, 1.0f, "%.4f");
                glm::vec2 v = std::any_cast<glm::vec2>(handle->value);
                break;
            }
            case Type::VEC3: {
                auto val = std::any_cast<glm::vec3>(&handle->value);
                ImGui::SliderFloat3(handle->name.c_str(), (float *)val, 0.0f, 1.0f, "%.4f");
                glm::vec2 v = std::any_cast<glm::vec3>(handle->value);
                break;
            }
            case Type::TEXT: {
                ImGui::TextWrapped("%s", handle->getValue<const char *>());

                break;
            }
            default:
                ImGui::TextColored(ImVec4(0.8, 0.2, 0.2, 1.0), " %s: No implementation for this type of handler (%s)\n", handle->name.c_str(),
                                   toString(handle->type));
                break;
            }
        }
        if (handleManager.getHandles().size() == 0) {

            ImGui::TextWrapped("You haven't defined any handles for this view now.\n");
            ImGui::Spacing();
            ImGui::TextWrapped("You can do so by using in your implementation (handleManager herited from RenderView class):\n\n");
            ImGui::Indent();
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Handle yourHandle(\"handleName\", Type::SUPPORTED_TYPE, initialValue);\n"
                                                               "handleManager.addHandle(yourHandle);\n\n");
            ImGui::PopFont();

            ImGui::Unindent();
        }
    }

    /*! \brief Render the window for the main view
     *
     *
     */
    // TODO Make this function renderWindowSSS
    void renderWindow(RenderView &renderView, std::vector<RenderView *> &renderViews) {

        // ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));

        // Main window containing the OpenGL/Vulkan rendering
        ImGui::SetNextWindowSize(ImVec2(renderView.width(), renderView.height()), ImGuiCond_FirstUseEver);

        ImGui::Begin(renderView.name());

        ImGui::PushID(renderView.getId());

        // Setting view ratio
        float ratio = (float)renderView.width() / (float)renderView.height();

        bool keepTextureRatio = true;

        float w = ImGui::GetContentRegionAvail().x;
        float h = ImGui::GetContentRegionAvail().y;

        glm::vec2 startPos = ImGui::GetCursorScreenPos();
        glm::vec2 endPos = glm::vec2(startPos.x + w, startPos.y + h);
        glm::vec2 endPosNoRatio = endPos;

        ImTextureID tex = renderView.render();

        // Setting mouse position overlay
        glm::vec2 screen_pos = ImGui::GetCursorScreenPos();

        float space = 0.0f;

        // Render texture to window
        if (!keepTextureRatio)
            ImGui::GetWindowDrawList()->AddImage(tex, startPos, endPos);
        else {
            float minSize = glm::min(w, h * ratio);
            float maxSize = glm::max(w, h * ratio);

            bool widthIsMax = w > h * ratio;

            space = widthIsMax ? (maxSize - minSize) * 0.5f : (maxSize - minSize) * 0.5f / ratio;

            if (widthIsMax) {
                startPos.x += space;
                endPos.x -= space;
            } else {
                startPos.y += space;
                endPos.y -= space;
            }

            ImGui::GetWindowDrawList()->AddImage(tex, startPos, endPos, ImVec2(0, 1), ImVec2(1, 0));
        }

        ImGuiIO &io = ImGui::GetIO();

        glm::vec2 screenMousePos = glm::vec2(io.MousePos.x - startPos.x, io.MousePos.y - startPos.y);
        glm::vec2 diff = (endPosNoRatio - endPos) * 2.0f;
        glm::vec2 textureEndPos = glm::vec2(w, h) - diff;

        if (textureEndPos.x != 0.0f && textureEndPos.y != 0.0f)
            renderView.setMousePos(screenMousePos / textureEndPos); // set normalized coordinates

        // Keep focus on the window that has been last active
        if (ImGui::IsWindowFocused() && !renderView.hasFocus()) {

            for (auto const &_renderView : renderViews) {
                _renderView->setFocus(false);
            }
            renderView.setFocus(true);
        }

        if (renderView.getPreferenceManager().mouseOverlay && renderView.hasFocus()) {
            ImGui::SetNextWindowPos(startPos + glm::vec2(10, 10));
            ImGui::SetNextWindowBgAlpha(0.35f);
            if (ImGui::Begin("Mouse Overlay", &renderView.getPreferenceManager().mouseOverlay,
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav)) {
                ImGui::Text("Mouse View Position: (%.1f,%.1f)", screenMousePos.x, screenMousePos.y);
                ImGui::Text("Normalized View Position: (%.1f,%.1f)", screenMousePos.x / textureEndPos.x, screenMousePos.y / textureEndPos.y);
            }
            ImGui::End();
        }
        ImGui::PopID();

        ImGui::End();

        // ImGui::PopStyleColor();
    } // namespace Qulkan

    void viewConfigurations(std::vector<RenderView *> &renderViews) {

        // ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));

        // Main window containing the OpenGL/Vulkan rendering
        ImGui::SetNextWindowSize(ImVec2(300, 800), ImGuiCond_FirstUseEver);

        ImGui::Begin("View Configurations");

        //       src, dest
        std::map<int, int> newMousePos;
        std::map<int, int> copyHandles;

        int currIdx = 0;
        for (auto const &renderView : renderViews) {
            if (renderView->getPreferenceManager().mirrorWithCombo != 0) {
                if (copyHandles.find(currIdx) == copyHandles.end()) {
                    copyHandles[renderView->getPreferenceManager().mirrorWith] = currIdx;
                }

                if (renderView->getPreferenceManager().mouseMirror && newMousePos.find(currIdx) == newMousePos.end()) {
                    newMousePos[renderView->getPreferenceManager().mirrorWith] = currIdx;
                }
            }
            ++currIdx;
        }

        currIdx = 0;
        for (auto const &renderView : renderViews) {
            // Update Mouse Pos
            if (newMousePos.find(currIdx) != newMousePos.end()) {
                renderViews[newMousePos[currIdx]]->setMousePos(renderViews[currIdx]->getMousePos());
            }

            // View Creation
            ImGui::PushID(currIdx);
            if (ImGui::CollapsingHeader(renderView->name())) {

                std::vector<const char *> renderViewNames = {"None"};
                for (auto const &_renderView : renderViews) {
                    if (std::string(_renderView->name()) != renderView->name())
                        renderViewNames.push_back(_renderView->name());
                }

                if (ImGui::TreeNode("View Preferences")) {

                    ImGui::Checkbox("Mouse position overlay", &renderView->getPreferenceManager().mouseOverlay);

                    ImGui::PushItemWidth(-140);

                    ImGui::Combo("Mirror handles", &renderView->getPreferenceManager().mirrorWithCombo, &renderViewNames[0], renderViewNames.size(), 4);
                    // Correct index
                    if (renderView->getPreferenceManager().mirrorWithCombo != 0) {
                        renderView->getPreferenceManager().mirrorWith = renderView->getPreferenceManager().mirrorWithCombo <= currIdx
                                                                            ? renderView->getPreferenceManager().mirrorWithCombo - 1
                                                                            : renderView->getPreferenceManager().mirrorWithCombo;

                        ImGui::Indent();
                        ImGui::Checkbox("Mirror mouse", &renderView->getPreferenceManager().mouseMirror);
                        ImGui::Unindent();
                    }
                    ImGui::PopItemWidth();
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Handles")) {

                    ImGui::PushItemWidth(-140);
                    if (copyHandles.find(currIdx) != copyHandles.end()) {
                        // ImGui::Text("%s to %s , vec(%.1f,%.1f)", renderViews[currIdx]->name(), renderViews[copyHandles[currIdx]]->name(), temp.x, temp.y);

                        mirrorHandles(renderView->getHandleManager(), renderViews[copyHandles[currIdx]]->getHandleManager());
                    }
                    auto handleManager = renderView->getHandleManager();
                    handleParser(handleManager);
                    ImGui::PopItemWidth();
                    ImGui::TreePop();
                }
            }

            ImGui::PopID();
            currIdx++;
        }

        ImGui::End();
    }

} // namespace Qulkan