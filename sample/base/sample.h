#pragma once

#include "window.h"

#include <GLFW/glfw3.h>
#include <filesystem>
#include <memory>

class Sample
{
public:
    Sample() = default;
    Sample(int argc, char** argv);
    virtual ~Sample() = default;

public:
    int exec();
    void* getNativeWindow();

protected:
    virtual void draw() = 0;

private:
    void mainLoop();

private:
    std::unique_ptr<Window> m_window = nullptr;

public:
    static std::filesystem::path getPath()
    {
        return path;
    }
    static std::filesystem::path getDir()
    {
        return dir;
    }

private:
    static std::filesystem::path path;
    static std::filesystem::path dir;
};
