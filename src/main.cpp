#include <memory>
#include "Logger.h"
#include "Window.h"


int main(int argc, char *argv[])
{
    std::unique_ptr<Window> window = std::make_unique<Window>();

    if (!window->init(640, 480, "Test Window"))
    {
        Logger::error("Window init error\n", __FUNCTION__);
        return -1;
    }
    window->mainLoop();
    window->cleanup();

    return 0;
}
