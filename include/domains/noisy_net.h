#ifndef _NOISY_NET_H_
#define _NOISY_NET_H_

/*
 * Noisy net is a simple domain that requires a network to solve a simple arithmetic problem
 * with additional noise signals attached that it needs to discern
 */

#include <domains/domain.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <util/statistics/distributions/gaussian_distribution.h>
#include <util/statistics/distributions/uniform_real_distribution.h>
#include <util/statistics/distributions/bernoulli_distribution.h>

namespace NeuroEvo {

template <typename G>
class NoisyNet : public Domain<G, double>
{

public:

    NoisyNet(const double a, const bool domain_trace = false,
             const double completion_fitness = -1e-3) :
        Domain<G, double>(domain_trace, completion_fitness),
        _a(a),
        _noise_distr(0., 5.),
        _input_distr(0., 1.) {}

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) const override 
    {

        const NetworkBuilder* network_builder = dynamic_cast<const NetworkBuilder*>(&pheno_spec);

        //If it is not a network
        if(network_builder == nullptr) 
        {

            std::cout << "Only network builders are allowed with" <<
                        " the noisy net domain" << std::endl;
            return false;

        }

        //Check for 2 inputs and 1 output
        if(network_builder->get_num_inputs() != 2 || network_builder->get_num_outputs() != 1) 
        {

                std::cout << "Number of inputs must be 2 and number of outputs" <<
                            " must be 1 for the noisy net domain" << std::endl;
                return false;

        }

        return true;

    }

private:

    double single_run(Organism<G, double>& org, unsigned rand_seed) override 
    {
        //_noise_distr.set_seed(rand_seed);
        //_input_distr.set_seed(rand_seed);

        //const double noise = _noise_distr.next();
        const double input = _input_distr.next();
        const bool flag = _flag_distr.next();

        //The target is _a multiplied by the input
        //The aim is for the target to ascertain this linear relationship
        //const double target = _a * input;
        double target;
            if(flag)
                target = _a * input;
            else
                target = -_a * input;

        //std::vector<double> net_input = {noise, input}; 
        std::vector<double> net_input = {input, (double)flag};
        std::vector<double> net_output = org.get_phenotype().activate(net_input);

        if(this->_domain_trace)
        {
            //std::cout << "Noise: " << noise << std::endl;
            std::cout << "Flag: " << flag << std::endl;
            std::cout << "Input: " << input << std::endl;
            std::cout << "Net output: " << net_output << std::endl;
            std::cout << "Target: " << target << std::endl;
        }

        const double fitness = -fabs(net_output[0] - target);
        return fitness;

    }

    void render() override {}
    void reset_domain() override {}

    NoisyNet<G>* clone_impl() const override
    {
        return new NoisyNet<G>(*this);
    }

    //To determine the target of the network, the input is multiplied by _a
    const double _a;

    GaussianDistribution _noise_distr;
    UniformRealDistribution _input_distr;
    BernoulliDistribution _flag_distr;

};

} // namespace NeuroEvo

#endif
