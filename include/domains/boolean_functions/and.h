#ifndef _AND_H_
#define _AND_H_

#include <domains/domain.h>

namespace NeuroEvo {

template <typename G>
class AND : public Domain<G, double> {

public:

    AND(const bool domain_trace = false, const double completion_fitness = 3.9) :
        Domain<G, double>(domain_trace, completion_fitness) {}

    AND(const AND& and_domain) :
        Domain<G, double>(and_domain._domain_trace, and_domain._completion_fitness) {}

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) override 
    {

        const NetworkBuilder* network_builder = dynamic_cast<const NetworkBuilder*>(&pheno_spec);

        //If it is not a network
        if(network_builder == nullptr) 
        {

            std::cout << "Only network builders are allowed with" <<
                        " boolean domains!" << std::endl;
            return false;

        }

        //Check for 2 inputs and 1 output
        if(network_builder->get_num_inputs() != 2 || network_builder->get_num_outputs() != 1) 
        {

                std::cout << "Number of inputs must be 2 and number of outputs" <<
                            " must be 1 for the AND domain" << std::endl;
                return false;

        }

        return true;

    }



private:

    double single_run(Organism<G, double>& org, unsigned rand_seed) override 
    {

        //Different AND inputs
        const std::vector<std::vector<double> > and_inputs {{1.0, 1.0},
                                                            {1.0, 0.0},
                                                            {0.0, 1.0},
                                                            {0.0, 0.0}};

        //Correct outputs for the respective AND inputs
        const std::vector<double> correct_outputs {1.0, 0.0, 0.0, 0.0};

        double fitness = 0.0;

        for(std::size_t i = 0; i < and_inputs.size(); i++) 
        {
            const std::vector<double> inputs = and_inputs.at(i);
            const std::vector<double> output = org.get_phenotype().activate(inputs);

            if(this->_domain_trace)
            {
                std::cout << "Inputs: ";
                for(const auto& input : inputs)
                    std::cout << input << " ";
                std::cout << "| Target: " << correct_outputs.at(i) << " " << 
                    "| Network output: " << output.at(0) << std::endl;
            }

            fitness += 1 - (fabs(correct_outputs.at(i) - output.at(0)));
        }

        return fitness;

    }

    void render() override {}
    void reset_domain() override {}

    AND<G>* clone_impl() const override
    {
        return new AND<G>(*this);
    }

};

} // namespace NeuroEvo

#endif
