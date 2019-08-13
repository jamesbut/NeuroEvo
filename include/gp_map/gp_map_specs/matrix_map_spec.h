#ifndef _MATRIX_MAP_SPEC_
#define _MATRIX_MAP_SPEC_

#include <gp_map/gp_map_specs/gp_map_spec.h>
#include <gp_map/matrix_map.h>

namespace NeuroEvo {
namespace GPMaps {

struct MatrixMapSpec : GPMapSpec {

public:

    // Create the identity Matrix Map
    MatrixMapSpec(const unsigned HEIGHT, const unsigned WIDTH) :
        HEIGHT(HEIGHT),
        WIDTH(WIDTH) {}

    // Create a matrix map specified in a .im file
    MatrixMapSpec(const std::string& IM_FILE_NAME) :
        IM_FILE_NAME(std::string(NEURO_EVO_CMAKE_SRC_DIR) + "/config/interaction_matrices/"
                     + IM_FILE_NAME) {}

    GPMap* generate_gp_map() override {

        if(IM_FILE_NAME)
            return new MatrixMap(*IM_FILE_NAME);
        else
            return new MatrixMap(*HEIGHT, *WIDTH);

    }

    // This generates a matrix map from an organism file
    GPMap* generate_gp_map(const std::string& file_name) override {

        return new MatrixMap(file_name);

    }

private:

    std::optional<const unsigned> HEIGHT;
    std::optional<const unsigned> WIDTH;

    std::optional<const std::string> IM_FILE_NAME;

};

} // namespace GPMaps
} // namespace NeuroEvo

#endif
