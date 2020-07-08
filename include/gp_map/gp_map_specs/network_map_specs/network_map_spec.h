#ifndef _NETWORK_MAP_SPEC_H_
#define _NETWORK_MAP_SPEC_H_

#include <gp_map/gp_map_specs/gp_map_spec.h>
#include <gp_map/network_maps/network_map.h>

namespace NeuroEvo {

template<typename G, typename T>
class NetworkMapSpec : public GPMapSpec<G, T> 
{

public:

    NetworkMapSpec(NetworkBuilder& net_builder) :
        _net_builder(net_builder) {}

    NetworkMapSpec(NetworkBuilder& net_builder, const std::string& file_name) :
        _net_builder(net_builder),
        _file_name(file_name) {}

    GPMap<G, T>* generate_gp_map() override = 0;
    GPMap<G, T>* generate_gp_map(const std::string& file_name) override = 0;

protected:

    NetworkBuilder _net_builder;
    std::optional<const std::string> _file_name;

};

} // namespace NeuroEvo

#endif
