#include <domains/control_domains/maze/maze.h>
#include <cmath>
#include <algorithm>
#include <phenotype/phenotype_specs/fixed_network_spec.h>
#include <thread>
#include <chrono>

Maze::Maze(const std::string GRID_FILE_NAME, const bool REWARD_INPUT, const bool ACTION_INPUT,
           const bool PRINT_MAP, const unsigned MAX_STEPS) :
    _GRID_FILE_NAME(GRID_FILE_NAME),
    _MAX_STEPS(MAX_STEPS),
    _PRINT_MAP(PRINT_MAP),
    _REWARD_INPUT(REWARD_INPUT),
    _ACTION_INPUT(ACTION_INPUT),
    _total_reward(0),
    _prev_reward(0),
    _prev_action(std::vector<bool>(4, false)),
    _AGENT_INIT_X(5),
    _AGENT_INIT_Y(3),
    _grid(GRID_FILE_NAME, _AGENT_INIT_X, _AGENT_INIT_Y),
    _rng((std::random_device())()),
    _uni_dist(0, (int)_grid.get_width()-1),
    Domain(PRINT_MAP, 120) {}

double Maze::single_run(Organism& org, unsigned rand_seed) {

    _rng = std::mt19937(rand_seed);

    //Randomly place reward and agent
    //rand_move_reward();
    //rand_agent_teleport();

    //No need to remake these vectors all the time in move_agent,
    //so I am just making them here and passing by reference

    //Input size depends on what inputs are provided
    std::vector<double> inputs(9);
    if(_REWARD_INPUT)
        inputs.resize(inputs.size() + 1);
    if(_ACTION_INPUT)
        inputs.resize(inputs.size() + 4);

    std::vector<double> outputs(4);
    unsigned steps = 0;

    while(++steps < _MAX_STEPS) {

        if(_PRINT_MAP)
            std::cout << "Steps: " << steps << std::endl;

        move_agent(org, inputs, outputs);

    }

    double fitness = (double)_total_reward;

    //Reset values
    _grid.move_agent(_AGENT_INIT_X, _AGENT_INIT_Y);
    _grid.move_reward(1, 6);
    _total_reward = 0;

    return fitness;

}

void Maze::move_agent(Organism& org, std::vector<double>& inputs, std::vector<double>& outputs) {

    if(_PRINT_MAP)
        _grid.print_grid();

    //Sense environment
    auto inputs_bools = sense_environment();

    //Convert to doubles for input to network
    for(std::size_t i = 0; i < inputs_bools.size(); i++)
        for(std::size_t j = 0; j < inputs_bools.at(i).size(); j++)
            inputs.at(i * inputs_bools.at(i).size() + j) = (double)inputs_bools.at(i).at(j);

    //Add reward input if there is one
    if(_REWARD_INPUT && !_ACTION_INPUT)
        inputs.at(9) = _prev_reward;

    //Add action input if there is one
    if(!_REWARD_INPUT && _ACTION_INPUT) {
        for(std::size_t i = 0; i < _prev_action.size(); i++)
            inputs.at(i+9) = (double)_prev_action.at(i);
    }

    //Add both inputs if that is the case
    if(_REWARD_INPUT && _ACTION_INPUT) {
        inputs.at(9) = _prev_reward;
        for(std::size_t i = 0; i < _prev_action.size(); i++)
            inputs.at(i+10) = (double)_prev_action.at(i);
    }


    if(_PRINT_MAP) {
        for(const auto &input : inputs)
            std::cout << input << " ";
        std::cout << std::endl;
    }

    outputs = org.get_phenotype().activate(inputs);

    if(_PRINT_MAP) {
        for(const auto &output : outputs)
            std::cout << output << " ";
        std::cout << std::endl;
    }

    auto max_element_index = std::max_element(outputs.begin(), outputs.end()) - outputs.begin();

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
        _grid.move_agent(_AGENT_INIT_X, _AGENT_INIT_Y);
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

    if(_PRINT_MAP) {
        std::cout << "State reward: " << reward << std::endl;
        std::cout << "Total reward: " << _total_reward << std::endl << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

}

std::vector<std::vector<bool> > Maze::sense_environment() {

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

void Maze::apply_action(const unsigned ACTION) {

    const unsigned agent_x = _grid.get_agent_x();
    const unsigned agent_y = _grid.get_agent_y();

    switch(ACTION) {

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

void Maze::rand_agent_teleport() {

    //Generate numbers until an empty square is landed
    int x;
    int y;

    do {

        x = _uni_dist(_rng);
        y = _uni_dist(_rng);

    } while(_grid.get_square_type(x, y) != 0 || _grid.get_reward(x, y) != 0);

    _grid.move_agent(x, y);

}

void Maze::rand_move_reward() {

    //Generate numbers until an empty square is landed
    int x;
    int y;

    do {

        x = _uni_dist(_rng);
        y = _uni_dist(_rng);

    } while(_grid.get_square_type(x, y) != 0);

    _grid.move_reward(x, y);

}

bool Maze::check_phenotype_spec(PhenotypeSpec& pheno_spec) {

    NetworkSpec* network_spec = dynamic_cast<NetworkSpec*>(&pheno_spec);

    //If it is not a network
    if(network_spec == nullptr) {

        std::cout << "Only network specifications are allowed with" <<
                     " the maze domain!" << std::endl;
        return false;

    }

    //Check for 4 outputs
    if(network_spec->NUM_OUTPUTS != 4) {

        std::cerr << "Number of outputs must be 4 for the maze domain" << std::endl;
        return false;

    }

    //Check inputs
    if(network_spec->NUM_INPUTS != 9 && !_REWARD_INPUT && !_ACTION_INPUT) {

        std::cerr << "Number of inputs must be 9 if there are no reward or action inputs" << std::endl;
        return false;

    }

    if(network_spec->NUM_INPUTS != 10 && _REWARD_INPUT && !_ACTION_INPUT) {

        std::cerr << "Number of inputs must be 10 if there is a reward input but no action inputs" << std::endl;
        return false;

    }

    if(network_spec->NUM_INPUTS != 14 && _REWARD_INPUT && _ACTION_INPUT) {

        std::cerr << "Number of inputs must be 14 if there are reward and action inputs" << std::endl;
        return false;

    }

    return true;

}
