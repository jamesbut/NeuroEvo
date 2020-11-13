#ifndef _IMAGE_MATCHING_DOUBLE_H_
#define _IMAGE_MATCHING_DOUBLE_H_

#include <domains/image_matching/image_matching.h>

namespace NeuroEvo {

template <typename G>
class ImageMatchingDouble : public ImageMatching<G, double>
{

public:

    ImageMatchingDouble(const std::shared_ptr<VectorCreationPolicy<bool>> vector_creation_policy,
                        const bool render = false, 
                        const bool domain_trace = false) :
        ImageMatching<G, double>(vector_creation_policy, 1e-3, render, domain_trace) {}

private:

    void extract_org_images(Organism<G, double>& org) override
    {
        const std::vector<double> pheno_out = org.get_phenotype().activate();
        _org_image_double.emplace(this->_image_width, this->_image_width, pheno_out);

        // Set org image bool too this is used to render
        this->_org_image = cast_matrix_to_bool(_org_image_double.value());
        
    }

    double calculate_image_fitness() const override
    {
        double fitness = 0.;

        //Compare organism guess to target image
        for(unsigned i = 0; i < _org_image_double->get_height(); i++)
            for(unsigned j = 0; j < _org_image_double->get_width(); j++)
                    fitness += std::fabs(_org_image_double->at(i, j) - 
                                         static_cast<double>(this->_target_image->at(i, j)));

        return -(fitness / (this->_image_width * this->_image_width));

    }

    Matrix<bool> cast_matrix_to_bool(const Matrix<double>& m) const
    {
        Matrix<bool> bool_matrix(m.get_height(), m.get_width());

        for(unsigned i = 0; i < m.get_height(); i++)
            for(unsigned j = 0; j < m.get_width(); j++)
                if(m.at(i, j) > 0.5)
                    bool_matrix.set(i, j, true);
                else
                    bool_matrix.set(i, j, false);

        return bool_matrix;
    }

    ImageMatchingDouble* clone_impl() const override
    {
        return new ImageMatchingDouble(*this);
    }

    //This is stored in order to calculate the fitness
    std::optional<Matrix<double>> _org_image_double;

};

} // namespace NeuroEvo

#endif
