#ifndef _NETWORK_MAP_SPEC_H_
#define _NETWORK_MAP_SPEC_H_

#include <gp_map/gp_map_specs/gp_map_spec.h>
#include <gp_map/network_map.h>

namespace NeuroEvo {
namespace GPMaps {

struct NetworkMapSpec : GPMapSpec {

public:

    NetworkMapSpec(Phenotypes::FixedNetworkSpec& net_spec, const std::string& file_name) :
        _net_spec(net_spec),
        _file_name(file_name) {}

    GPMap* generate_gp_map() override {

        return new NetworkMap(_net_spec, _file_name);

    }

    GPMap* generate_gp_map(const std::string& file_name) override {

        return new NetworkMap(_net_spec, file_name);

    }

private:

    Phenotypes::FixedNetworkSpec _net_spec;
    const std::string _file_name;


};

} // namespace GPMaps
} // namespace NeuroEvo

#endif
