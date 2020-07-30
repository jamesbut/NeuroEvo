#ifndef _XOR_H_
#define _XOR_H_

#include <domains/domain.h>

namespace NeuroEvo {

template <typename G>
class XOR : public Domain<G, double> {

public:

    XOR(const bool domain_trace = false, const double completion_fitness = 3.9) :
        Domain<G, double>(domain_trace, completion_fitness) {}

    XOR(const XOR& xor_domain) :
        Domain<G, double>(xor_domain._domain_trace, xor_domain._completion_fitness) {}

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) override 
    {

        const NetworkBuilder* network_builder = dynamic_cast<const NetworkBuilder*>(&pheno_spec);

        //If it is not a network
        if(network_builder == nullptr) {

            std::cout << "Only network builders are allowed with" <<
                        " boolean domains!" << std::endl;
            return false;

        }

        //Check for 2 inputs and 1 output
        if(network_builder->get_num_inputs() != 2 || network_builder->get_num_outputs() != 1) {

                std::cout << "Number of inputs must be 2 and number of outputs" <<
                            " must be 1 for the XOR domain" << std::endl;
                return false;

        }

        return true;

    }

private:

    double single_run(Organism<G, double>& org, unsigned rand_seed) override 
    {

        //Different XOR inputs
        const std::vector<std::vector<double> > xor_inputs {{1.0, 1.0},
                                                            {1.0, 0.0},
                                                            {0.0, 1.0},
                                                            {0.0, 0.0}};

        //Correct outputs for the respective XOR inputs
        const std::vector<double> correct_outputs {0.0, 1.0, 1.0, 0.0};

        double fitness = 0.0;

        for(std::size_t i = 0; i < xor_inputs.size(); i++) 
        {
            const std::vector<double> inputs = xor_inputs.at(i);
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

    XOR<G>* clone_impl() const override
    {
        return new XOR<G>(*this);
    }

};

} // namespace NeuroEvo

#endif
