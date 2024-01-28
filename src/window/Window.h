#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include <GLFW/glfw3.h>

class Window {
public:
    bool init(unsigned int width, unsigned int height, std::string title);
    void mainLoop();
    void cleanup();

private:
    GLFWwindow *m_window = nullptr;
};

#endif //ENGINE_WINDOW_H
