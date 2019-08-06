#ifndef _MATRIX_MAP_SPEC_
#define _MATRIX_MAP_SPEC_

#include <gp_map/gp_map_specs/gp_map_spec.h>
#include <gp_map/matrix_map.h>

namespace NeuroEvo {
namespace GPMaps {

struct MatrixMapSpec : GPMapSpec {

public:

    MatrixMapSpec(const unsigned HEIGHT, const unsigned WIDTH) :
        HEIGHT(HEIGHT),
        WIDTH(WIDTH) {}

    GPMap* generate_gp_map() override {

        return new MatrixMap(HEIGHT, WIDTH);

    }

    GPMap* generate_gp_map(const std::string& file_name) override {

        return new MatrixMap(HEIGHT, WIDTH, file_name);

    }

private:

    const unsigned HEIGHT;
    const unsigned WIDTH;

};

} // namespace GPMaps
} // namespace NeuroEvo

#endif
