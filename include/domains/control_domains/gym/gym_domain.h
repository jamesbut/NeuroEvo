#ifndef _GYM_DOMAIN_H_
#define _GYM_DOMAIN_H_

/*
 * An OpenAI Gym environment; it uses the http gym binding to communicate with
 * a flask http server servicing OpenAI Gym.
 */

#include <domains/domain.h>
#include <util/python/python_binding.h>
#include <domains/control_domains/gym/space.h>
#include <phenotype/phenotype_specs/network_builder.h>
#include <util/maths/normalisation.h>

namespace NeuroEvo {

//Keyword arguments that can be handed to env.make() on the Python side
struct GymMakeKwargs
{
public:

    void set_kwarg(const std::string& kwarg_name, const double kwarg_val)
    {
        _kwargs.insert({kwarg_name, kwarg_val});
    }

    void remove_kwarg(const std::string& kwarg_name)
    {
        _kwargs.erase(kwarg_name);
    }

    std::map<const std::string, const double> get_kwargs() const
    {
        return _kwargs;
    }

    friend std::ostream& operator<<(std::ostream& os,
                                    const GymMakeKwargs& gym_make_kwargs)
    {
        for(const auto kwarg : gym_make_kwargs._kwargs)
            os << kwarg.first << " " << kwarg.second << std::endl;
        return os;
    }

private:

    std::map<const std::string, const double> _kwargs;
};

template <typename G>
class GymDomain : public Domain<G, double>
{

public:

    GymDomain(const std::string gym_env_id,
              const SpaceType action_space_type,
              const std::optional<GymMakeKwargs>& kwargs = std::nullopt,
              const double max_reward = 1e6,
              const bool render = false,
              const bool domain_trace = false,
              const std::optional<const unsigned> seed = std::nullopt) :
        Domain<G, double>(domain_trace, max_reward, seed, render),
        _kwargs(kwargs),
        _gym_module(initialise_gym_module()),
        _action_space_type(action_space_type)
    {

        //Make environment
        make_env(gym_env_id, kwargs);

        //Get state size
        _state_size = std::get<0>(_gym_module.call_function<unsigned>("state_size"));

        //Get action space
        if(_action_space_type == SpaceType::Box)
        {
            auto action_space_tup =
                _gym_module.call_function<std::vector<unsigned>,
                                          std::vector<double>,
                                          std::vector<double>>("action_space");
            _action_space = std::make_unique<BoxSpace>(std::get<0>(action_space_tup),
                                                       std::get<1>(action_space_tup),
                                                       std::get<2>(action_space_tup));
        } else if (_action_space_type == SpaceType::Discrete)
        {
            auto action_space_tup = _gym_module.call_function<const unsigned>(
                "action_space"
            );
            _action_space = std::make_unique<DiscreteSpace>(
                std::get<0>(action_space_tup)
            );
        }

    }

    ~GymDomain() = default;

    GymDomain(const GymDomain& gym_domain) :
        Domain<G, double>(gym_domain._domain_trace, gym_domain._completion_fitness,
                          gym_domain._seed, gym_domain._render),
        _kwargs(gym_domain._kwargs),
        _gym_module(gym_domain._gym_module),
        _state_size(gym_domain._state_size),
        _action_space_type(gym_domain._action_space_type),
        _action_space(gym_domain._action_space ?
		      gym_domain._action_space->clone() : nullptr) {}

    GymDomain(GymDomain&& gym_domain) = default;

    GymDomain& operator=(const GymDomain& gym_domain)
    {
        _kwargs = gym_domain._kwargs;
        _gym_module = gym_domain._gym_module;
        _state_size = gym_domain._state_size;
        _action_space_type = gym_domain._action_space_type;
	if(gym_domain._action_space)
            _action_space = gym_domain._action_space->clone();
	else
	    _action_space = nullptr;
    }

    GymDomain& operator=(GymDomain&& gym_domain) = default;

protected:

    void make_env(const std::string gym_env_id,
                  const std::optional<const GymMakeKwargs>& kwargs = std::nullopt) const
    {

        if(kwargs.has_value())
            _gym_module.call_function("make_env", gym_env_id, kwargs->get_kwargs());
        else
            _gym_module.call_function("make_env", gym_env_id);

    }

    void close_env() const
    {
        _gym_module.call_function("close");
    }

    void seed_env() const
    {
        //Seed
        if(this->_seed.has_value())
            _gym_module.call_function("seed", this->_seed.value());
    }

    std::optional<GymMakeKwargs> _kwargs;

private:

    PythonModule initialise_gym_module()
    {
        std::stringstream gym_dir;
        gym_dir << NEURO_EVO_CMAKE_SRC_DIR <<
            "/include/domains/control_domains/gym";

        PythonModule gym_module("gym_env", gym_dir.str());

        return gym_module;
    }

    double single_run(Organism<G, double>& org, unsigned rand_seed) override
    {

        auto pheno_net = dynamic_cast<const Network*>(&org.get_phenotype());
        if(!pheno_net)
            throw std::runtime_error("Cannot cast phenotype to Network in GymDomain"
                " single_run");
        const auto final_layer_activ_func = pheno_net->get_final_layer_activ_func();

        //Check final layer activation function is bounded on both sides
        if(!(final_layer_activ_func->get_lower_bound().has_value() &&
             final_layer_activ_func->get_upper_bound().has_value()))
            throw std::runtime_error("The activation function for the final "
                "layer of the control network for the gym domains must be "
                "asymtoted on both sides");

        double reward = 0.;
        bool done = false;

        std::vector<double> state = reset_env();

        while(!done)
        {
            //Activate control network
            const std::vector<double> net_outs = org.get_phenotype().activate(state);

            if(this->_domain_trace)
            {
                std::cout << "net_outs: ";
                for(auto v : net_outs)
                    std::cout << v << " ";
                std::cout << std::endl;
            }

            StepReturn step_return;

            //If action space is discrete, choose maximum net output index as action
            if(_action_space_type == SpaceType::Discrete)
            {
                const unsigned max_action =
                    std::max_element(net_outs.begin(), net_outs.end())
                    - net_outs.begin();

                if(this->_domain_trace)
                    std::cout << "Max action: " << max_action << std::endl;

                step_return = step(max_action);

            //If action space is box, scale the network outputs to the min and max
            //value that the action can take
            } else if(_action_space_type == SpaceType::Box)
            {
                //Cast Space to BoxSpace
                const BoxSpace* box_action_space =
                    dynamic_cast<BoxSpace*>(_action_space.get());

                //Scale net outputs correctly for action space
                std::vector<double> action_vals(net_outs.size());
                for(std::size_t i = 0; i < net_outs.size(); i++)
                    action_vals[i] = normalise(
                        net_outs[i],
                        final_layer_activ_func->get_lower_bound().get_value(),
                        final_layer_activ_func->get_upper_bound().get_value(),
                        box_action_space->get_lows()[i],
                        box_action_space->get_highs()[i]);

                if(this->_domain_trace)
                {
                    std::cout << "action_vals: ";
                    for(auto v : action_vals)
                        std::cout << v << " ";
                    std::cout << std::endl;
                }

                //const std::vector<double> action_vals = net_outs;

                step_return = step(action_vals);
            }

            state = step_return.state;
            reward += step_return.reward;
            done = step_return.done;

            if(this->_domain_trace)
            {
                step_return.print();
                std::cout << "Total reward: " << reward << std::endl;
            }

        }

        if(this->_domain_trace)
            std::cout << "--------------------" << std::endl;

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

    template <typename A>
    StepReturn step(const A& action) const
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

        if(network_builder->get_num_outputs() != _action_space->get_num_elements())
        {
            std::cerr << "Number of outputs must be " <<
                _action_space->get_num_elements() << " for this gym domain!" <<
                std::endl;
            return false;
        }

        return true;

    }

    void render() override {}

    void trial_reset(const unsigned trial_num) override {}

    void org_reset() override
    {
        seed_env();
    }

    PythonModule _gym_module;

    unsigned _state_size;

    const SpaceType _action_space_type;
    std::unique_ptr<Space> _action_space;

};

} // namespace NeuroEvo

#endif
