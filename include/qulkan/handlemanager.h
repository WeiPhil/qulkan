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

enum Type { INT, FLOAT, DOUBLE, VEC2, VEC3, VEC4 };

struct Handle {

    std::string name;
    Type type;
    std::any value;

    Handle(std::string name, Type type, std::any value) : name(name), type(type), value(value) {}
    ~Handle() {}

    template <typename T> T getValue() { return std::any_cast<T>(value); }
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

    std::vector<std::shared_ptr<Handle>> getHandles() { return m_handles; }

    std::shared_ptr<Handle> getHandle(std::string name) {
        ASSERT(m_handles_map.find(name) != m_handles_map.end(), "No handle with that name found");
        return m_handles_map[name];
    }

    std::shared_ptr<Handle> operator()(std::string name) {
        return getHandle(name);
    }
};

#endif
