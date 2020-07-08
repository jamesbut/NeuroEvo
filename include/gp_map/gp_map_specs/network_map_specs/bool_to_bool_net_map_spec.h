#ifndef _BOOL_TO_BOOL_NET_MAP_SPEC_
#define _BOOL_TO_BOOL_NET_MAP_SPEC_

#include <gp_map/gp_map_specs/network_map_specs/network_map_spec.h>

namespace NeuroEvo {

class BoolToBoolNetMapSpec : public NetworkMapSpec<bool, bool>
{

public:

    BoolToBoolNetMapSpec(NetworkBuilder& net_builder);

    GPMap<bool, bool>* generate_gp_map() override;
    GPMap<bool, bool>* generate_gp_map(const std::string& file_name) override;

};

} // namespace NeuroEvo

#endif
