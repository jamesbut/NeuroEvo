#ifndef _NETWORK_MAP_SPEC_H_
#define _NETWORK_MAP_SPEC_H_

#include <gp_map/gp_map_specs/gp_map_spec.h>
#include <gp_map/network_map.h>

namespace NeuroEvo {

class NetworkMapSpec : public GPMapSpec<double, double> {

public:

    NetworkMapSpec(NetworkBuilder& net_builder) :
        _net_builder(net_builder) {}

    NetworkMapSpec(NetworkBuilder& net_builder, const std::string& file_name) :
        _net_builder(net_builder),
        _file_name(file_name) {}

    GPMap<double, double>* generate_gp_map() override 
    {
        if(_file_name)
            return new NetworkMap(_net_builder, _file_name.value());
        else
            return new NetworkMap(_net_builder);
    }

    GPMap<double, double>* generate_gp_map(const std::string& file_name) override 
    {
        return new NetworkMap(_net_builder, file_name);
    }

private:

    NetworkBuilder _net_builder;
    std::optional<const std::string> _file_name;


};

} // namespace NeuroEvo

#endif
