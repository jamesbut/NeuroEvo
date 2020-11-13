#ifndef _IMAGE_MATCHING_BOOL_H_
#define _IMAGE_MATCHING_BOOL_H_

#include <domains/image_matching/image_matching.h>

namespace NeuroEvo {

template <typename G>
class ImageMatchingBool : public ImageMatching<G, bool>
{

public:

    ImageMatchingBool(const std::shared_ptr<VectorCreationPolicy<bool>> vector_creation_policy,
                      const bool render = false, 
                      const bool domain_trace = false) :
        ImageMatching<G, bool>(vector_creation_policy, 1., render, domain_trace) {}

private: 

    void extract_org_images(Organism<G, bool>& org) override
    {
        const std::vector<bool> pheno_out = org.get_phenotype().activate();
        this->_org_image.emplace(this->_image_width, this->_image_width, pheno_out);
    }

    double calculate_image_fitness() const override
    {
        double fitness = 0.;

        //Compare organism guess to target image
        for(unsigned i = 0; i < this->_org_image->get_height(); i++)
            for(unsigned j = 0; j < this->_org_image->get_width(); j++)
                if(this->_org_image->at(i, j) == this->_target_image->at(i, j))
                    fitness += 1.;

        return (fitness / (this->_image_width * this->_image_width));

    }

    ImageMatchingBool* clone_impl() const override
    {
        return new ImageMatchingBool(*this);
    }

};

} // namespace NeuroEvo

#endif
