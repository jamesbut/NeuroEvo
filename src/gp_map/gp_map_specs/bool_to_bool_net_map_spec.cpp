#include <gp_map/gp_map_specs/network_map_specs/bool_to_bool_net_map_spec.h>
#include <gp_map/network_maps/bool_to_bool_net_map.h>

namespace NeuroEvo {

BoolToBoolNetMapSpec::BoolToBoolNetMapSpec(NetworkBuilder& net_builder) :
    NetworkMapSpec<bool, bool>(net_builder) {}


GPMap<bool, bool>* BoolToBoolNetMapSpec::generate_gp_map() 
{
   return new BoolToBoolNetMap(_net_builder); 
}

GPMap<bool, bool>* BoolToBoolNetMapSpec::generate_gp_map(const std::string& file_name) 
{
    std::cerr << "Write generate_gp_map(const std::string& file_name) in BoolToBoolNetMapSpec" 
        << std::endl;
    exit(0);
}

} // namespace NeuroEvo
