#include "triangle_sample.h"

#include <spdlog/spdlog.h>

using namespace vkt;

int main(int argc, char** argv)
{
    spdlog::set_level(spdlog::level::trace);

    TriangleSample sample(argc, argv);

    return sample.exec();
}
