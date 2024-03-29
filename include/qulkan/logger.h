#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include "imgui.h"

#include <stdio.h>

namespace Qulkan {
    /*! \brief Simple Logger
     *         A logger with different types of logging, INFO , WARN ,ERROR
     *
     *
     *  You can use the logger simply in the following manner:
     *
     *       Qulkan::Logger::Info("My value is : %u",myValue);
     *       or
     *       Qulkan::Logger::Warn("My value is : %u",myValue);
     *       or
     *       Qulkan::Logger::Error("My value is : %u",myValue);
     *
     *   The logger support printf formatting style.
     *
     */
    class Logger {
      public:
        enum LogLevel { INFO_LOG, WARN_LOG, ERROR_LOG };

        static Logger &Instance() {
            static Logger instance; // Guaranteed to be destroyed.
                                    // Instantiated on first use.
            return instance;
        }

      private:
        Logger() {
            AutoScroll = true;
            ScrollToBottom = false;
            Clear();
        }

      public:
        Logger(Logger const &) = delete;
        void operator=(Logger const &) = delete;

      private:
        ImGuiTextBuffer Buf;
        ImGuiTextFilter Filter;
        ImVector<int> LineOffsets; // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
        bool AutoScroll;
        bool ScrollToBottom;

      public:
        void Clear() {
            Buf.clear();
            LineOffsets.clear();
            LineOffsets.push_back(0);
        }

        template <typename... Args> static void Info(const char *fmt, Args... args) { Instance().Log(LogLevel::INFO_LOG, fmt, args...); }

        template <typename... Args> static void Warning(const char *fmt, Args... args) { Instance().Log(LogLevel::WARN_LOG, fmt, args...); }

        template <typename... Args> static void Error(const char *fmt, Args... args) {
            // fprintf(stderr, fmt, args...);
            Instance().Log(LogLevel::ERROR_LOG, fmt, args...);
        }

        void Log(const LogLevel logLevel, const char *fmt, ...) {
            int old_size = Buf.size();
            va_list args;
            va_start(args, fmt);
            switch (logLevel) {
            case LogLevel::INFO_LOG:
                Buf.append("[INFO] ");
                break;
            case LogLevel::WARN_LOG:
                Buf.append("[WARN] ");
                break;

            case LogLevel::ERROR_LOG:
                Buf.append("[ERROR] ");
                break;
            default:
                break;
            }
            Buf.appendfv(fmt, args);
            va_end(args);
            for (int new_size = Buf.size(); old_size < new_size; old_size++)
                if (Buf[old_size] == '\n')
                    LineOffsets.push_back(old_size + 1);
            if (AutoScroll)
                ScrollToBottom = true;
        }

        void Window(bool *p_open = NULL) {

            if (!ImGui::Begin("Logger", p_open)) {
                ImGui::End();
                return;
            }

            // Options menu
            if (ImGui::BeginPopup("Options")) {
                if (ImGui::Checkbox("Auto-scroll", &AutoScroll))
                    if (AutoScroll)
                        ScrollToBottom = true;
                ImGui::EndPopup();
            }

            // Main window
            if (ImGui::Button("Options"))
                ImGui::OpenPopup("Options");
            ImGui::SameLine();
            bool clear = ImGui::Button("Clear");
            ImGui::SameLine();
            bool copy = ImGui::Button("Copy");
            ImGui::SameLine();
            Filter.Draw("Filter", -100.0f);

            ImGui::Separator();
            ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

            if (clear)
                Clear();
            if (copy)
                ImGui::LogToClipboard();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const char *buf = Buf.begin();
            const char *buf_end = Buf.end();
            if (Filter.IsActive()) {
                // In this example we don't use the clipper when Filter is enabled.
                // This is because we don't have a random access on the result on our filter.
                // A real application processing logs with ten of thousands of entries may want to store the result of search/filter.
                // especially if the filtering function is not trivial (e.g. reg-exp).
                for (int line_no = 0; line_no < LineOffsets.Size; line_no++) {
                    const char *line_start = buf + LineOffsets[line_no];
                    const char *line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    if (Filter.PassFilter(line_start, line_end))
                        ImGui::TextUnformatted(line_start, line_end);
                }
            } else {
                // The simplest and easy way to display the entire buffer:
                //   ImGui::TextUnformatted(buf_begin, buf_end);
                // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward to skip non-visible lines.
                // Here we instead demonstrate using the clipper to only process lines that are within the visible area.
                // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them on your side is recommended.
                // Using ImGuiListClipper requires A) random access into your data, and B) items all being the  same height,
                // both of which we can handle since we an array pointing to the beginning of each line of text.
                // When using the filter (in the block of code above) we don't have random access into the data to display anymore, which is why we don't use
                // the clipper. Storing or skimming through the search result would make it possible (and would be recommended if you want to search through
                // tens of thousands of entries)
                ImGuiListClipper clipper;
                clipper.Begin(LineOffsets.Size);
                while (clipper.Step()) {
                    for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++) {
                        const char *line_start = buf + LineOffsets[line_no];
                        const char *line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                        ImGui::TextColored(ImVec4(0.8, 0.8, 0.8, 1.0), line_start, line_end);
                    }
                }
                clipper.End();
            }
            ImGui::PopStyleVar();

            if (ScrollToBottom)
                ImGui::SetScrollHereY(1.0f);
            ScrollToBottom = false;
            ImGui::EndChild();
            ImGui::End();
        }
    };

} // namespace Qulkan

#endif