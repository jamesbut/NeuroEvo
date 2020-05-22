#ifndef _MATRIX_MAP_SPEC_
#define _MATRIX_MAP_SPEC_

#include <gp_map/gp_map_specs/gp_map_spec.h>
#include <gp_map/matrix_map.h>

namespace NeuroEvo {

class MatrixMapSpec : GPMapSpec<double>
{

public:

    // Create the identity Matrix Map
    MatrixMapSpec(const unsigned height, const unsigned width) :
        height(height),
        width(width) {}

    // Create a matrix map specified in a .im file
    MatrixMapSpec(const std::string& im_file_name) :
        IM_FILE_NAME(std::string(NEURO_EVO_CMAKE_SRC_DIR) + "/config/interaction_matrices/"
                     + im_file_name) {}

    GPMap* generate_gp_map() override 
    {

        if(im_file_name)
            return new MatrixMap(*im_file_name);
        else
            return new MatrixMap(*height, *width);

    }

    // This generates a matrix map from an organism file
    GPMap* generate_gp_map(const std::string& file_name) override 
    {
        return new MatrixMap(file_name);
    }

private:

    std::optional<const unsigned> _height;
    std::optional<const unsigned> _width;

    std::optional<const std::string> _im_file_name;

};

} // namespace NeuroEvo

#endif
