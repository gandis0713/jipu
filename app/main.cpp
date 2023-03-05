#include "application.h"
#include "utils/log.h"

using namespace vkt;

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    LOG_ERROR("argc: {}", argc);
    LOG_ERROR("argv: {}", argv[0]);

    Application app(argc, argv);

    try
    {
        app.run();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
