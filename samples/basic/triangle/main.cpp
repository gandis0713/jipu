#include "application.h"

using namespace vkt;

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    spdlog::error("argc: {}", argc);
    spdlog::error("argv: {}", argv[0]);

    Application app(argc, argv);

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
