#ifndef _GP_MAP_SPEC_H_
#define _GP_MAP_SPEC_H_

namespace NeuroEvo {
namespace GPMaps {

struct GPMapSpec {

public:

    virtual ~GPMapSpec() = default;

    virtual GPMap* generate_gp_map() = 0;
    virtual GPMap* generate_gp_map(const std::string& file_name) = 0;

};

} // namespace GPMaps
} // namespace NeuroEvo

#endif
