#ifndef _MOUNTAIN_CAR_H_
#define _MOUNTAIN_CAR_H_

/*
 * MountainCar uses the OpenAI gym library
 */

#include <domains/domain.h>
#include <gym/gym.h>
#include <Python.h>
#include <signal.h>

namespace NeuroEvo {

template <typename G>
class MountainCar : public Domain<G, double>
{

public:

    MountainCar(const bool render = false, const bool domain_trace = false,
                const unsigned max_steps = 1000,
                const std::optional<const unsigned> seed = std::nullopt) :
        Domain<G, double>(domain_trace, max_steps, seed, render)
    {
        //Run python gym server as separate process
        pid_t ch_pid = ::fork();
        if(ch_pid == 0)
            run_python_gym_server();

        sleep(5);
        std::cout << "Killing process: " << ch_pid << std::endl;
        kill(ch_pid, SIGKILL);

        /*
        _client = Gym::client_create("127.0.0.1", 5000);
        _env = _client->make("MountainCar-v0");

        _observation_space = _env->observation_space();
        _action_space = _env->action_space();
        */
    }

    ~MountainCar()
    {
        std::cout << "Destructor" << std::endl;
    }

private:

    double single_run(Organism<G, double>& org, unsigned rand_seed) override
    {
        std::cout << "Running" << std::endl;
        return 0.;
    }

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) const override
    {
        const NetworkBuilder* network_builder =
            dynamic_cast<const NetworkBuilder*>(&pheno_spec);

        //If it is not a network
        if(network_builder == nullptr)
        {
            std::cout << "Only network specifications are allowed with" <<
                        " the mountain car domain!" << std::endl;
            return false;
        }

        const std::vector<int> obs_shape = _observation_space->box_shape;
        const std::vector<int> action_shape = _action_space->box_shape;

        for(const auto& s : obs_shape)
            std::cout << s << " ";
        std::cout << std::endl;
        std::cout << "Action shape" << std::endl;
        for(const auto& s : action_shape)
            std::cout << s << " ";
        std::cout << std::endl;

        //TODO: Write this
        //If it has the correct number of inputs and outputs
        /*
        if(network_builder->get_num_inputs() != 2)
        {
            std::cout << "Number of inputs must be 2 " <<
                        "for the non-markovian single part cole domain" <<
                        std::endl;
            return false;
        }

        if((network_builder->get_num_outputs() != 1) &&
           (network_builder->get_num_outputs() != 2))
        {
            std::cerr << "Number of outputs must be 1 or 2 for SCP domain" << std::endl;
            return false;
        }
        */

        return true;

    }

    static void run_python_gym_server()
    {
        /*
        std::stringstream gym_server_script_path;
        gym_server_script_path << NEURO_EVO_CMAKE_SRC_DIR <<
            "/lib/gym-http-api/gym_http_server.py";
        const std::string python_command = "python3 " + gym_server_script_path.str();
        system(python_command.c_str());
        */


        std::stringstream gym_server_script_path;
        gym_server_script_path << NEURO_EVO_CMAKE_SRC_DIR <<
            "/lib/gym-http-api/gym_http_server.py";

        Py_Initialize();

        FILE* gym_server_script_fpt = _Py_fopen(gym_server_script_path.str().c_str(),
                                                "r");
        PyRun_SimpleFile(gym_server_script_fpt, gym_server_script_path.str().c_str());

        Py_Finalize();

    }

    MountainCar<G>* clone_impl() const override
    {
        return new MountainCar<G>(*this);
    }

    void render() override {}
    void exp_run_reset_impl(const unsigned run_num, const unsigned run_seed) override {}
    void trial_reset(const unsigned trial_num) override {}

    std::shared_ptr<Gym::Client> _client;
    std::shared_ptr<Gym::Environment> _env;
    std::shared_ptr<Gym::Space> _observation_space;
    std::shared_ptr<Gym::Space> _action_space;

};

} // namespace NeuroEvo

#endif
