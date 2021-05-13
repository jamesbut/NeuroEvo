#ifndef _GYM_DOMAIN_H_
#define _GYM_DOMAIN_H_

/*
 * An OpenAI Gym environment; it uses the http gym binding to communicate with
 * a flask http server servicing OpenAI Gym.
 */

#include <domains/domain.h>
#include <util/python/python_binding.h>

namespace NeuroEvo {

//Keyword arguments that can be handed to env.make() on the Python side
struct GymMakeKwargs
{
public:

    void set_kwarg(const std::string& kwarg_name, const double kwarg_val)
    {
        _kwargs.insert({kwarg_name, kwarg_val});
    }

    std::map<const std::string, const double> get_kwargs() const
    {
        return _kwargs;
    }

private:

    std::map<const std::string, const double> _kwargs;
};

template <typename G>
class GymDomain : public Domain<G, double>
{

public:

    GymDomain(const std::string gym_env_id,
              const std::optional<const GymMakeKwargs>& kwargs = std::nullopt,
              const double max_reward = 1e6,
              const bool render = false,
              const bool domain_trace = false,
              const std::optional<const unsigned> seed = std::nullopt) :
        Domain<G, double>(domain_trace, max_reward, seed, render),
        _gym_module(initialise_gym_module())
    {

        //Make environment
        if(kwargs.has_value())
            _gym_module.call_function("make_env", gym_env_id, kwargs->get_kwargs());
        else
            _gym_module.call_function("make_env", gym_env_id);

        //Get state and action sizes
        _state_size = std::get<0>(_gym_module.call_function<unsigned>("state_size"));
        _num_actions = std::get<0>(_gym_module.call_function<unsigned>("num_actions"));

    }

    ~GymDomain()
    {
        //Clean up
    }


private:

    PythonModule initialise_gym_module()
    {
        std::stringstream gym_dir;
        gym_dir << NEURO_EVO_CMAKE_SRC_DIR <<
            "/include/domains/control_domains/gym";

        PythonModule gym_module("gym_env", gym_dir.str());
        _gym_module.initialise();

        return gym_module;
    }

    double single_run(Organism<G, double>& org, unsigned rand_seed) override
    {
        double reward = 0.;
        bool done = false;

        std::vector<double> state = reset_env();

        while(!done)
        {
            const std::vector<double> net_outs = org.get_phenotype().activate(state);

            const unsigned max_action =
                std::max_element(net_outs.begin(), net_outs.end()) - net_outs.begin();

            const StepReturn step_return = step(max_action);

            state = step_return.state;
            reward += step_return.reward;
            done = step_return.done;


            if(this->_domain_trace)
            {
                std::cout << "net_outs: ";
                for(auto v : net_outs)
                    std::cout << v << " ";
                std::cout << std::endl;
                std::cout << "Max action: " << max_action << std::endl;
                step_return.print();
                std::cout << "Total reward: " << reward << std::endl;
            }

        }

        return reward;
    }

    std::vector<double> reset_env() const
    {
        return std::get<0>(_gym_module.call_function<std::vector<double>>("reset"));
    }

    struct StepReturn
    {
        std::vector<double> state;
        double reward;
        bool done;

        void print() const
        {
            std::cout << "State: [ ";
            for(std::size_t i = 0; i < state.size(); i++)
                std::cout << state[i] << " ";
            std::cout << " ]   ";
            std::cout << "Reward: " << reward << "   ";
            std::cout << "Done: " << done << std::endl;
        }
    };

    StepReturn step(const unsigned action) const
    {
        auto step_return = _gym_module.call_function<std::vector<double>, double, bool>
            ("step", action, this->_render);
        return StepReturn{std::get<0>(step_return),
                          std::get<1>(step_return),
                          std::get<2>(step_return)};
    }

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) const override
    {
        const NetworkBuilder* network_builder =
            dynamic_cast<const NetworkBuilder*>(&pheno_spec);

        //If it is not a network
        if(network_builder == nullptr)
        {
            std::cout << "Only network specifications are allowed with" <<
                        " gym domains!" << std::endl;
            return false;
        }

        //If it has the correct number of inputs and outputs
        if(network_builder->get_num_inputs() != _state_size)
        {
            std::cerr << "Number of inputs must be " << _state_size <<
                " for this gym domain!" << std::endl;
            return false;
        }

        if(network_builder->get_num_outputs() != _num_actions)
        {
            std::cerr << "Number of outputs must be " << _num_actions <<
                " for this gym domain!" << std::endl;
            return false;
        }

        return true;

    }

    void render() override {}

    void exp_run_reset_impl(const unsigned run_num, const unsigned run_seed) override
    {
        //Seed
        if(this->_seed.has_value())
            _gym_module.call_function("seed", run_seed);
    }

    void trial_reset(const unsigned trial_num) override {}

    void org_reset() override
    {
        //Seed
        if(this->_seed.has_value())
            _gym_module.call_function("seed", this->_seed.value());
    }


    PythonModule _gym_module;

    unsigned _state_size;
    unsigned _num_actions;

};

} // namespace NeuroEvo

#endif
