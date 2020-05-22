#ifndef _AND_H_
#define _AND_H_

#include <domains/domain.h>

namespace NeuroEvo {

template <typename G>
class AND : public Domain<G> {

public:

    AND(const bool domain_trace, const double completion_fitness = 3.9) :
        Domain<G>(domain_trace, completion_fitness) {}

    bool check_phenotype_spec(PhenotypeSpec<G>& pheno_spec) override 
    {

        NetworkBuilder<G>* network_builder = dynamic_cast<NetworkBuilder<G>*>(&pheno_spec);

        //If it is not a network
        if(network_builder == nullptr) 
        {

            std::cout << "Only network builders are allowed with" <<
                        " boolean domains!" << std::endl;
            return false;

        }

        //Check for 1 input and 1 output
        if(network_builder->get_num_inputs() != 2 || network_builder->get_num_outputs() != 1) 
        {

                std::cout << "Number of inputs must be 2 and number of outputs" <<
                            " must be 1 for the AND domain" << std::endl;
                return false;

        }

        return true;

    }

protected: 

    void render() override {}

private:

    double single_run(Organism<G>& org, unsigned rand_seed) override 
    {

        //Different AND inputs
        std::vector<std::vector<double> > and_inputs {{1.0, 1.0},
                                                    {1.0, 0.0},
                                                    {0.0, 1.0},
                                                    {0.0, 0.0}};

        //Correct outputs for the respective AND inputs
        std::vector<double> correct_outputs {1.0, 0.0, 0.0, 0.0};

        double fitness = 0.0;

        for(std::size_t i = 0; i < and_inputs.size(); i++) {

            std::vector<double> inputs = and_inputs.at(i);
            std::vector<double> output = org.get_phenotype().activate(inputs);

            if(this->_domain_trace)
                std::cout << correct_outputs.at(i) << " " << output.at(0) << std::endl;

            fitness += 1 - (fabs(correct_outputs.at(i) - output.at(0)));


        }

        return fitness;

    }

};

} // namespace NeuroEvo

#endif
