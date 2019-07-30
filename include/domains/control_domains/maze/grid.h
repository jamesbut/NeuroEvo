#ifndef _GRID_H_
#define _GRID_H_

#include <vector>
#include <string>

namespace NeuroEvo {
namespace Domains {

//Set up the 2d grid structure of any maze
class Grid {

public:

    Grid(const std::string FILE_NAME, const unsigned AGENT_START_X, const unsigned AGENT_START_Y);

    void move_agent(const unsigned X, const unsigned Y);
    void move_reward(const unsigned X, const unsigned Y);

    int get_square_type(const unsigned X, const unsigned Y);
    int get_reward(const unsigned X, const unsigned Y);
    unsigned get_agent_x() {return _agent_x;};
    unsigned get_agent_y() {return _agent_y;};
    unsigned get_height() {return _HEIGHT;};
    unsigned get_width() {return _WIDTH;};

    void print_grid();

private:

    struct GridSquare {

        unsigned int x;
        unsigned int y;

        /*
        0 = empty
        1 = wall
        */
        int square_type;

        int reward;

    };

    void build_grid(const std::string FILE_NAME);
    std::vector<std::vector<int> > read_grid(const std::string FILE_NAME, const unsigned GRID_NUM);

    std::vector<std::vector<GridSquare> > _squares;

    const std::string _FILE_NAME;

    unsigned _HEIGHT;
    unsigned _WIDTH;

    //Store agent position so it doesn't have to be searched for all the time
    unsigned _agent_x;
    unsigned _agent_y;

};

} // namespace Domains
} // namespace NeuroEvo

#endif
