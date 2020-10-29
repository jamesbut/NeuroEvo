#include <util/formatting.h>

namespace NeuroEvo {

void print_horizontal_line(const unsigned width)
{
    for (unsigned i = 0; i < width; i++)
        std::cout << "-";
    std::cout << std::endl;
}


} // namespace NeuroEvo
