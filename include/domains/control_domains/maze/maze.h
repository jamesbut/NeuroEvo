#ifndef _MAZE_H_
#define _MAZE_H_

/*
    This maze domain consists of an agent
    trying to locate a reward in a maze.
*/

#include <domains/domain.h>
#include <domains/control_domains/maze/grid.h>
#include <random>
#include <thread>

namespace NeuroEvo {

template <typename G>
class Maze : public Domain<G> {

public:

    Maze(const std::string grid_file_name, const bool reward_input, const bool action_input,
         const bool print_map, const unsigned max_steps = 200) :
        _grid_file_name(grid_file_name),
        _max_steps(max_steps),
        _print_map(print_map),
        _reward_input(reward_input),
        _action_input(action_input),
        _total_reward(0),
        _prev_reward(0),
        _prev_action(std::vector<bool>(4, false)),
        _agent_init_x(5),
        _agent_init_y(3),
        _grid(grid_file_name, _agent_init_x, _agent_init_y),
        _rng((std::random_device())()),
        _uni_dist(0, (int)_grid.get_width()-1),
        Domain<G>(print_map, 120) {}

    bool check_phenotype_spec(PhenotypeSpec<G>& pheno_spec) override {

        Phenotypes::NetworkSpec* network_spec = dynamic_cast<Phenotypes::NetworkSpec*>(&pheno_spec);

        //If it is not a network
        if(network_spec == nullptr) {

            std::cout << "Only network specifications are allowed with" <<
                        " the maze domain!" << std::endl;
            return false;

        }

        //Check for 4 outputs
        if(network_spec->num_outputs != 4) {

            std::cerr << "Number of outputs must be 4 for the maze domain" << std::endl;
            return false;

        }

        //Check inputs
        if(network_spec->num_inputs != 9 && !_reward_input && !_action_input) {

            std::cerr << "Number of inputs must be 9 if there are no reward or action inputs" 
                << std::endl;
            return false;

        }

        if(network_spec->num_inputs != 10 && _reward_input && !_action_input) {

            std::cerr << "Number of inputs must be 10 if there is a reward input but no action" <<
                " inputs" << std::endl;
            return false;

        }

        if(network_spec->num_inputs != 14 && _reward_input && _action_input) {

            std::cerr << "Number of inputs must be 14 if there are reward and action inputs" << 
                std::endl;
            return false;

        }

        return true;

    }

private:

    double single_run(Organism<G>& org, unsigned rand_seed) override {

        _rng = std::mt19937(rand_seed);

        //Randomly place reward and agent
        //rand_move_reward();
        //rand_agent_teleport();

        //No need to remake these vectors all the time in move_agent,
        //so I am just making them here and passing by reference

        //Input size depends on what inputs are provided
        std::vector<double> inputs(9);
        if(_reward_input)
            inputs.resize(inputs.size() + 1);
        if(_action_input)
            inputs.resize(inputs.size() + 4);

        std::vector<double> outputs(4);
        unsigned steps = 0;

        while(++steps < _max_steps) {

            if(_print_map)
                std::cout << "Steps: " << steps << std::endl;

            move_agent(org, inputs, outputs);

        }

        double fitness = (double)_total_reward;

        //Reset values
        _grid.move_agent(_agent_init_x, _agent_init_y);
        _grid.move_reward(1, 6);
        _total_reward = 0;

        return fitness;

    }

    void move_agent(Organism<G>& org, 
                    std::vector<double>& inputs, 
                    std::vector<double>& outputs) {

        if(_print_map)
            _grid.print_grid();

        //Sense environment
        auto inputs_bools = sense_environment();

        //Convert to doubles for input to network
        for(std::size_t i = 0; i < inputs_bools.size(); i++)
            for(std::size_t j = 0; j < inputs_bools.at(i).size(); j++)
                inputs.at(i * inputs_bools.at(i).size() + j) = (double)inputs_bools.at(i).at(j);

        //Add reward input if there is one
        if(_reward_input && !_action_input)
            inputs.at(9) = _prev_reward;

        //Add action input if there is one
        if(!_reward_input && _action_input) {
            for(std::size_t i = 0; i < _prev_action.size(); i++)
                inputs.at(i+9) = (double)_prev_action.at(i);
        }

        //Add both inputs if that is the case
        if(_reward_input && _action_input) {
            inputs.at(9) = _prev_reward;
            for(std::size_t i = 0; i < _prev_action.size(); i++)
                inputs.at(i+10) = (double)_prev_action.at(i);
        }


        if(_print_map) {
            for(const auto &input : inputs)
                std::cout << input << " ";
            std::cout << std::endl;
        }

        outputs = org.get_phenotype().activate(inputs);

        if(_print_map) {
            for(const auto &output : outputs)
                std::cout << output << " ";
            std::cout << std::endl;
        }

        auto max_element_index = std::max_element(outputs.begin(), 
                                                  outputs.end()) - outputs.begin();

        //Receive reward from new state
        int reward = _grid.get_reward(_grid.get_agent_x(), _grid.get_agent_y());
        _total_reward += reward;
        _prev_reward = reward;

        std::vector<bool> action_bools(4, false);

        //Move reward
        if(_total_reward > 20)
            _grid.move_reward(9, 6);
            //_grid.move_reward(7, 1);

        //Move agent
        if(reward > 0)
            _grid.move_agent(_agent_init_x, _agent_init_y);
        else {

            //Apply action
            apply_action(max_element_index);

            switch(max_element_index) {
                case 0:
                    action_bools.at(0) = true;
                    break;
                case 1:
                    action_bools.at(1) = true;
                    break;
                case 2:
                    action_bools.at(2) = true;
                    break;
                case 3:
                    action_bools.at(3) = true;
                    break;
            }

        }

        //Assign previous action
        _prev_action.assign(action_bools.begin(), action_bools.end());

        if(_print_map) {
            std::cout << "State reward: " << reward << std::endl;
            std::cout << "Total reward: " << _total_reward << std::endl << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

    }

    std::vector<std::vector<bool> > sense_environment() {

        //3x3 binary vector centered on the neighbourhood of the agent
        //Hard coded numbers for now but can make this generic in the future
        std::vector<std::vector<bool> > inputs(3, std::vector<bool>(3));

        const unsigned agent_x = _grid.get_agent_x();
        const unsigned agent_y = _grid.get_agent_y();

        for(unsigned i = agent_y-1; i <= agent_y+1; i++)
            for(unsigned j = agent_x-1; j <= agent_x+1; j++)
                inputs.at(i - agent_y+1).at(j - agent_x+1) = (bool)_grid.get_square_type(j, i);

        return inputs;

    }

    void apply_action(const unsigned action) {

        const unsigned agent_x = _grid.get_agent_x();
        const unsigned agent_y = _grid.get_agent_y();

        switch(action) {

            //Up
            case 0:
                if(_grid.get_square_type(agent_x, agent_y-1) == 0)
                    _grid.move_agent(agent_x, agent_y-1);
                break;

            //Down
            case 1:
                if(_grid.get_square_type(agent_x, agent_y+1) == 0)
                    _grid.move_agent(agent_x, agent_y+1);
                break;

            //Left
            case 2:
                if(_grid.get_square_type(agent_x-1, agent_y) == 0)
                    _grid.move_agent(agent_x-1, agent_y);
                break;

            //Right
            case 3:
                if(_grid.get_square_type(agent_x+1, agent_y) == 0)
                    _grid.move_agent(agent_x+1, agent_y);
                break;

        }

    }

    void rand_agent_teleport() {

        //Generate numbers until an empty square is landed
        int x;
        int y;

        do {

            x = _uni_dist(_rng);
            y = _uni_dist(_rng);

        } while(_grid.get_square_type(x, y) != 0 || _grid.get_reward(x, y) != 0);

        _grid.move_agent(x, y);

    }

    void rand_move_reward() {

        //Generate numbers until an empty square is landed
        int x;
        int y;

        do {

            x = _uni_dist(_rng);
            y = _uni_dist(_rng);

        } while(_grid.get_square_type(x, y) != 0);

        _grid.move_reward(x, y);

    }

    const unsigned _agent_init_x;
    const unsigned _agent_init_y;

    const std::string _grid_file_name;
    Grid _grid;

    const unsigned _max_steps;

    const bool _print_map;

    const bool _reward_input;
    const bool _action_input;
    int _prev_reward;
    std::vector<bool> _prev_action;

    int _total_reward;

    //Random uniform distribution for generating coordinates in grid
    std::mt19937 _rng;
    std::uniform_int_distribution<int> _uni_dist;

};

} // namespace NeuroEvo

#endif
