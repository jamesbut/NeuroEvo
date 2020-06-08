#ifndef _GENOTYPE_REFLECTOR_SPEC_H_
#define _GENOTYPE_REFLECTOR_SPEC_H_

#include <gp_map/gp_map_specs/gp_map_spec.h>
#include <gp_map/genotype_reflector_map.h>

namespace NeuroEvo {

template <typename G>
class GenotypeReflectorSpec : public GPMapSpec<G, G>
{

public:

    GenotypeReflectorSpec() {}

    GPMap<G, G>* generate_gp_map() override
    {
        return new GenotypeReflectorMap<G>();
    }

    GPMap<G, G>* generate_gp_map(const std::string& file_name) override
    {
        std::cerr << "Write generate_gp_map in GenotypeReflectionSpec" << std::endl;
        exit(0);
    }
    
};
  
} // namespace NeuroEvo

#endif
