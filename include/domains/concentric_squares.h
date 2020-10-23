#ifndef _CONCENTRIC_SQUARES_H_
#define _CONCENTRIC_SQUARES_H_

/*
    Concentric squares is a domain where the aim is to match
    a target image of different coloured squares. The domain
    is described in Koutnis, Clune et al 2016.
*/

#include <domains/domain.h>
#include <phenotype/phenotype_specs/vector_phenotype_spec.h>

namespace NeuroEvo {

template <typename G>
class ConcentricSquares : public Domain<G, bool>
{

public: 

    ConcentricSquares(const unsigned image_width,
                      const bool render = true, const bool domain_trace = false) :
        Domain<G, bool>(domain_trace, calculate_completion_fitness(image_width), 
                        std::nullopt, render),
        _image_width(image_width),
        _target_image(create_target_image()) {}


private:

    double single_run(Organism<G, bool>& org, unsigned rand_seed) override 
    {
        if(this->_render)
            render();
        return 0.;
    }

    const Matrix<bool> create_target_image() const
    {
        //Sets whether the outer square is 1 or 0
        const bool outer_square = false;

        //The algorithm to create the target image is somewhat complicated to index - I do 
        //apoligise. Maybe there is an easier way to do it but this is what I figured out
        //for now
        Matrix<bool> target_image(_image_width, _image_width);
        target_image.fill(!outer_square);

        //Upper half
        for(unsigned i = 0; i < (target_image.get_height() / 2 + 1); i++)
        {
            //Alternating part  
            for(unsigned j = 0; j < i; j++)
                if((j % 2) == 0)
                    target_image.set(i, j, outer_square);

            //Const part
            if((i % 2) == 0)
                for(unsigned j = 0; j < (target_image.get_width() - 2 * i); j++)
                    target_image.set(i, j + i, outer_square);

            //Alternating part
            for(unsigned j = (target_image.get_width() - i); j < target_image.get_width(); j++)
                if((j % 2) == static_cast<int>(target_image.get_width() % 2 == 0))
                    target_image.set(i, j, outer_square);

        }

        //Lower half
        for(unsigned i = (target_image.get_height() / 2 + 1); i < target_image.get_height(); i++)
        {
            //Alternating part  
            for(unsigned j = 0; j < (target_image.get_width() - i); j++)
                if((j % 2) == 0)
                    target_image.set(i, j, outer_square);

            //Const part
            if((i % 2) == static_cast<int>((target_image.get_width() % 2) == 0))
            {
                const unsigned num_consts = 2 * i - target_image.get_width() + 2;
                for(unsigned j = 0; j < num_consts; j++)
                {
                    const unsigned j_index = target_image.get_width() - i - 1 + j;
                    target_image.set(i, j_index, outer_square);
                }
            }

            //Alternating part
            for(unsigned j = i; j < target_image.get_width(); j++)
                if((j % 2) == static_cast<int>(target_image.get_width() % 2 == 0))
                    target_image.set(i, j, outer_square);

        }

        std::cout << target_image << std::endl;

        return target_image;
    }

    double calculate_completion_fitness(const unsigned image_width) const
    {
        return static_cast<double>(_image_width * _image_width);
    }

    void render() override {}

    void exp_run_reset_impl() override {}
    void trial_reset(const unsigned trial_num) override {}

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) const override
    {
         auto vec_pheno_spec = dynamic_cast<const VectorPhenotypeSpec*>(&pheno_spec);

        if(vec_pheno_spec == nullptr)
        {
            std::cerr << "Only vector phenotype specifications are allowed" <<
                " for the Concentric Squares domain!" << std::endl;
            return false;
        }

        if(vec_pheno_spec->get_num_params() != (_image_width * _image_width))
        {
            std::cerr << "The number of phenotype parameters does not match the size of the" <<
                " target image" << std::endl;
            return false;
        }

        return true;
    }

    ConcentricSquares<G>* clone_impl() const override
    {
        return new ConcentricSquares<G>(*this);
    }

    const unsigned _image_width;
    const Matrix<bool> _target_image;

};

} // namespace NeuroEvo

#endif
