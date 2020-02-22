#pragma once

#ifndef INTERFACEHANDLER_H
#define INTERFACEHANDLER_H

#include "imgui.h"

#include "qulkan/utils.h"
#include <any>
#include <glm/glm.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

enum Type { BOOL_HANDLE, INT_HANDLE, FLOAT_HANDLE, RADIAN_FLOAT_HANDLE, DOUBLE_HANDLE, TEXT_HANDLE, VEC2_HANDLE, VEC3_HANDLE, VEC4_HANDLE, COLOR3_HANDLE };

inline const char *toString(Type type) {
    switch (type) {
    case BOOL_HANDLE:
        return "BOOL";
    case INT_HANDLE:
        return "INT";
    case FLOAT_HANDLE:
        return "FLOAT";
    case RADIAN_FLOAT_HANDLE:
        return "RADIAN_FLOAT";
    case DOUBLE_HANDLE:
        return "DOUBLE";
    case TEXT_HANDLE:
        return "TEXT";
    case VEC2_HANDLE:
        return "VEC2";
    case VEC3_HANDLE:
        return "VEC3";
    case VEC4_HANDLE:
        return "VEC4";
    case COLOR3_HANDLE:
        return "COLOR3";
    default:
        return "[Unknown Type]";
    }
}

struct Handle {

    std::string name;
    std::string displayName;
    Type type;
    std::any value;
    std::any invValue; // only for bool
    std::any minValues;
    std::any maxValues;
    bool *active;

    Handle(std::string name, Type type, std::any value, bool *active = nullptr) : name(name), type(type), value(value), active(active) {
        displayName = name;
        // setting default min and max values and inverse
        switch (type) {
        case BOOL_HANDLE:
            minValues = false;
            maxValues = true;
            invValue = !std::any_cast<bool>(value);
            break;
        case INT_HANDLE:
            minValues = 0;
            maxValues = 1;
            break;
        case FLOAT_HANDLE:
            minValues = 0.0f;
            maxValues = 1.0f;
            break;
        case RADIAN_FLOAT_HANDLE:
            minValues = 0.0f;
            maxValues = 1.0f;
            break;
        case DOUBLE_HANDLE:
            minValues = 0.0;
            maxValues = 1.0;
            break;
        case TEXT_HANDLE:
            minValues = "";
            maxValues = "";
            break;
        case VEC2_HANDLE:
            minValues = glm::vec2(0.0f);
            maxValues = glm::vec2(1.0f);
            break;
        case VEC3_HANDLE:
            minValues = glm::vec3(0.0f);
            maxValues = glm::vec3(1.0f);
            break;
        case VEC4_HANDLE:
            minValues = glm::vec4(0.0f);
            maxValues = glm::vec4(1.0f);
            break;
        case COLOR3_HANDLE:
            minValues = glm::vec3(0.0f);
            maxValues = glm::vec3(1.0f);
            break;
        default:
            break;
        }
    }
    Handle(std::string name, Type type, std::any value, std::any minValues, std::any maxValues, bool *active = nullptr)
        : name(name), type(type), value(value), minValues(minValues), maxValues(maxValues), active(active) {}
    ~Handle() {}

    bool isActive() {
        if (active != nullptr) {
            return *active;
        } else {
            return true;
        }
    }

    template <typename T> T getInvValue() { return std::any_cast<T>(invValue); }
    template <typename T> T getValue() { return std::any_cast<T>(value); }
    template <typename T> T getMinValues() { return std::any_cast<T>(minValues); }
    template <typename T> T getMaxValues() { return std::any_cast<T>(maxValues); }
};

class HandleManager {

  private:
    std::map<std::string, std::shared_ptr<Handle>> m_handles_map;
    std::vector<std::shared_ptr<Handle>> m_handles;

  public:
    HandleManager() {}
    ~HandleManager() {}
    void addHandle(Handle handle) {
        auto newHandle = std::make_shared<Handle>(handle);
        m_handles_map[handle.name] = newHandle;
        m_handles.push_back(newHandle);
    }

    void emptyHandles() {
        m_handles_map.clear();
        m_handles.clear();
    }

    std::vector<std::shared_ptr<Handle>> getHandles() const { return m_handles; }

    std::shared_ptr<Handle> getHandle(std::string name) {
        std::string message = "No handle with that name found: " + name;
        ASSERT(m_handles_map.find(name) != m_handles_map.end(), message);
        return m_handles_map[name];
    }

    std::shared_ptr<Handle> operator()(std::string name) { return getHandle(name); }
};

#endif
