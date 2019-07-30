#ifndef _MAZE_H_
#define _MAZE_H_

/*
    This maze domain consists of an agent
    trying to locate a reward in a maze.
*/

#include <domains/domain.h>
#include <domains/control_domains/maze/grid.h>
#include <random>

namespace NeuroEvo {
namespace Domains {

class Maze : public Domain {

public:

    Maze(const std::string GRID_FILE_NAME, const bool REWARD_INPUT, const bool ACTION_INPUT,
         const bool PRINT_MAP, const unsigned MAX_STEPS = 200);

    bool check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) override;

private:

    double single_run(Organism& org, unsigned rand_seed) override;

    void move_agent(Organism& org, std::vector<double>& inputs, std::vector<double>& outputs);
    std::vector<std::vector<bool> > sense_environment();
    void apply_action(const unsigned ACTION);

    void rand_agent_teleport();
    void rand_move_reward();

    const unsigned _AGENT_INIT_X;
    const unsigned _AGENT_INIT_Y;

    const std::string _GRID_FILE_NAME;
    Grid _grid;

    const unsigned _MAX_STEPS;

    const bool _PRINT_MAP;

    const bool _REWARD_INPUT;
    const bool _ACTION_INPUT;
    int _prev_reward;
    std::vector<bool> _prev_action;

    int _total_reward;

    //Random uniform distribution for generating coordinates in grid
    std::mt19937 _rng;
    std::uniform_int_distribution<int> _uni_dist;

};

} // namespace Domains
} // namespace NeuroEvo

#endif
