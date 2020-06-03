#ifndef _GP_MAP_SPEC_H_
#define _GP_MAP_SPEC_H_

namespace NeuroEvo {

template <typename G, typename T>
class GPMapSpec {

public:

    virtual ~GPMapSpec() = default;

    virtual GPMap<G, T>* generate_gp_map() = 0;
    virtual GPMap<G, T>* generate_gp_map(const std::string& file_name) = 0;

};

} // namespace NeuroEvo

#endif
