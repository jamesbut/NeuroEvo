#ifndef _NAND_H_
#define _NAND_H_

#include <domains/domain.h>

namespace NeuroEvo {

template <typename G>
class NAND : public Domain<G> {

public:

    NAND(const bool domain_trace, const double completion_fitness = 3.9) :
        Domain(domain_trace, completion_fitness) {}

    bool check_phenotype_spec(PhenotypeSpec<G>& pheno_spec) override {

        Phenotypes::NetworkSpec* network_spec = dynamic_cast<Phenotypes::NetworkSpec*>(&pheno_spec);

        //If it is not a network
        if(network_spec == nullptr) {

            std::cout << "Only network specifications are allowed with" <<
                        " boolean domains!" << std::endl;
            return false;

        }

        //Check for 1 input and 1 output
        if(network_spec->NUM_INPUTS != 2 || network_spec->NUM_OUTPUTS != 1) {

                std::cout << "Number of inputs must be 2 and number of outputs" <<
                            " must be 1 for the NAND domain" << std::endl;
                return false;

        }

        return true;

    }

private:

    double single_run(Organism<G>& org, unsigned rand_seed) override {

        //Different NAND inputs
        std::vector<std::vector<double> > nand_inputs {{1.0, 1.0},
                                                    {1.0, 0.0},
                                                    {0.0, 1.0},
                                                    {0.0, 0.0}};

        //Correct outputs for the respective NAND inputs
        std::vector<double> correct_outputs {0.0, 1.0, 1.0, 1.0};

        double fitness = 0.0;

        for(std::size_t i = 0; i < nand_inputs.size(); i++) {

            std::vector<double> inputs = nand_inputs.at(i);
            std::vector<double> output = org.get_phenotype().activate(inputs);

            if(_DOMAIN_TRACE)
                std::cout << correct_outputs.at(i) << " " << output.at(0) << std::endl;

            fitness += 1 - (fabs(correct_outputs.at(i) - output.at(0)));


        }

        return fitness;

    }

};

} // namespace NeuroEvo

#endif
