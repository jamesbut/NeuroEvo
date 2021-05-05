#ifndef _GYM_DOMAIN_H_
#define _GYM_DOMAIN_H_

/*
 * An OpenAI Gym environment; it uses the http gym binding to communicate with
 * a flask http server servicing OpenAI Gym.
 */

#include <domains/domain.h>
#include <Python.h>

namespace NeuroEvo {

template <typename G>
class GymDomain : public Domain<G, double>
{

public:

    GymDomain(const std::string gym_env_id, const double max_reward = 1e6,
              const bool render = false,
              const bool domain_trace = false,
              const std::optional<const unsigned> seed = std::nullopt) :
        Domain<G, double>(domain_trace, max_reward, seed, render),
        _gym_env_id(gym_env_id)
    {
        initialise_gym();
        create_gym_env();

        if(!_state_size.has_value())
            _state_size = state_size();
        if(!_num_actions.has_value())
            _num_actions = num_actions();
    }

    ~GymDomain()
    {
        //Clean up
        //I need to see whether this is going to lead to memory leaks
        //Py_DECREF(module);
        //Py_DECREF(pName);

        close();
        //Py_Finalize();
    }


private:

    double single_run(Organism<G, double>& org, unsigned rand_seed) override
    {
        double reward = 0.;
        bool done = false;

        std::vector<double> state = reset();

        while(!done)
        {
            const std::vector<double> net_outs = org.get_phenotype().activate(state);

            const unsigned max_action =
                std::max_element(net_outs.begin(), net_outs.end()) - net_outs.begin();

            const StepTuple step_return = step(max_action);

            state = step_return.state;
            reward += step_return.reward;
            done = step_return.done;

            if(this->_domain_trace)
            {
                std::cout << "net_outs: ";
                for(auto v : net_outs)
                    std::cout << v << " ";
                std::cout << std::endl;
                std::cout << "max action: " << max_action << std::endl;
                std::cout << "New state: " << step_return.state << std::endl;
                std::cout << "Reward: " << step_return.reward << std::endl;
                std::cout << "Done: " << step_return.done << std::endl;
                std::cout << "Total reward: " << reward << std::endl;
            }

        }

        return reward;
    }

    void initialise_gym()
    {

        Py_Initialize();

        //Add correct path to python interpreter
        std::stringstream gym_dir;
        gym_dir << NEURO_EVO_CMAKE_SRC_DIR <<
            "/include/domains/control_domains/gym";
        std::string py_command = "import sys; sys.path.insert(0, '" + gym_dir.str()
            + "')";
        PyRun_SimpleString(py_command.c_str());

        //Load the module object
        PyObject* module_name = PyUnicode_FromString((char*)"gym_env");
        PyObject* module = PyImport_Import(module_name);

        if(module == NULL)
            PyErr_Print();

        //Turn module into dictionary
        _module_dict = PyModule_GetDict(module);

    }

    void create_gym_env() const
    {
        //Give env id string to specify environment
        PyObject* args = PyTuple_New(1);
        PyObject* string_arg = PyUnicode_FromString(_gym_env_id.c_str());
        PyTuple_SetItem(args, 0, string_arg);

        PyObject* make_env_func = PyDict_GetItemString(_module_dict, (char*)"make_env");

        if(make_env_func == NULL)
            PyErr_Print();

        if(PyCallable_Check(make_env_func))
            PyObject_CallObject(make_env_func, args);
        else
            PyErr_Print();
    }

    struct StepTuple
    {
        std::vector<double> state;
        double reward;
        bool done;
    };

    StepTuple step(const unsigned action) const
    {
        //Prepare step function call
        PyObject* args = PyTuple_New(2);
        PyTuple_SetItem(args, 0, PyLong_FromLong(action));
        PyTuple_SetItem(args, 1, this->_render ? Py_True : Py_False);

        PyObject* step_func = PyDict_GetItemString(_module_dict, (char*)"step");

        if(step_func == NULL)
            PyErr_Print();

        StepTuple step_tuple;

        if(PyCallable_Check(step_func))
        {
            //Call step function
            PyObject* step_return = PyObject_CallObject(step_func, args);

            //Parse new state
            PyObject* state_list = PyTuple_GetItem(step_return, 0);
            PyObject* state_tuple = PyList_AsTuple(state_list);

            const unsigned state_length = PyObject_Length(state_tuple);
            step_tuple.state.resize(state_length);
            for(std::size_t i = 0; i < step_tuple.state.size(); i++)
                step_tuple.state[i] = PyFloat_AsDouble(PyTuple_GetItem(state_tuple, i));

            //Parse reward and done
            step_tuple.reward = PyFloat_AsDouble(PyTuple_GetItem(step_return, 1));
            step_tuple.done = PyLong_AsLong(PyTuple_GetItem(step_return, 2));

        } else
            PyErr_Print();

        return step_tuple;

    }

    std::vector<double> reset() const
    {
        PyObject* reset_func = PyDict_GetItemString(_module_dict, (char*)"reset");

        if(reset_func == NULL)
            PyErr_Print();

        std::vector<double> init_state;

        if(PyCallable_Check(reset_func))
        {
            PyObject* reset_return = PyObject_CallObject(reset_func, NULL);

            //Parse new state
            PyObject* init_state_py = PyList_AsTuple(reset_return);

            const unsigned state_length = PyObject_Length(init_state_py);
            init_state.resize(state_length);
            for(std::size_t i = 0; i < init_state.size(); i++)
                init_state[i] = PyFloat_AsDouble(PyTuple_GetItem(init_state_py, i));
        }
        else
            PyErr_Print();

        return init_state;
    }

    void close() const
    {
        PyObject* close_func = PyDict_GetItemString(_module_dict, (char*)"close");

        if(close_func == NULL)
            PyErr_Print();

        if(PyCallable_Check(close_func))
            PyObject_CallObject(close_func, NULL);
        else
            PyErr_Print();
    }

    unsigned state_size() const
    {
        PyObject* state_size_func = PyDict_GetItemString(_module_dict,
                                                         (char*)"state_size");

        if(state_size_func == NULL)
            PyErr_Print();

        unsigned state_size;

        if(PyCallable_Check(state_size_func))
        {
            PyObject* state_size_return = PyObject_CallObject(state_size_func, NULL);
            state_size = PyLong_AsLong(state_size_return);
        }
        else
            PyErr_Print();

        return state_size;
    }

    unsigned num_actions() const
    {
        PyObject* num_actions_func = PyDict_GetItemString(_module_dict,
                                                          (char*)"num_actions");

        if(num_actions_func == NULL)
            PyErr_Print();

        unsigned num_actions;

        if(PyCallable_Check(num_actions_func))
        {
            PyObject* state_size_return = PyObject_CallObject(num_actions_func, NULL);
            num_actions = PyLong_AsLong(state_size_return);
        }
        else
            PyErr_Print();

        return num_actions;
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
        if(network_builder->get_num_inputs() != _state_size.value())
        {
            std::cerr << "Number of inputs must be " << _state_size.value() <<
                " for this gym domain!" << std::endl;
            return false;
        }

        if(network_builder->get_num_outputs() != _num_actions.value())
        {
            std::cerr << "Number of outputs must be " << _num_actions.value() <<
                " for this gym domain!" << std::endl;
            return false;
        }

        return true;

    }

    void render() override {}
    void exp_run_reset_impl(const unsigned run_num, const unsigned run_seed) override {}
    void trial_reset(const unsigned trial_num) override {}


    const std::string _gym_env_id;
    PyObject* _module_dict;

    static inline std::optional<unsigned> _state_size = std::nullopt;
    static inline std::optional<unsigned> _num_actions = std::nullopt;

};

} // namespace NeuroEvo

#endif
