#ifndef _GRID_H_
#define _GRID_H_

#include <vector>
#include <string>

namespace NeuroEvo {

//Set up the 2d grid structure of any maze
class Grid {

public:

    Grid(const std::string file_name, const unsigned agent_start_x, const unsigned agent_start_y);

    void move_agent(const unsigned X, const unsigned Y);
    void move_reward(const unsigned X, const unsigned Y);

    int get_square_type(const unsigned X, const unsigned Y);
    int get_reward(const unsigned X, const unsigned Y);
    unsigned get_agent_x() {return _agent_x;};
    unsigned get_agent_y() {return _agent_y;};
    unsigned get_height() {return _height;};
    unsigned get_width() {return _width;};

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

    void build_grid(const std::string file_name);
    std::vector<std::vector<int> > read_grid(const std::string file_name, const unsigned grid_num);

    std::vector<std::vector<GridSquare> > _squares;

    const std::string _file_name;

    unsigned _height;
    unsigned _width;

    //Store agent position so it doesn't have to be searched for all the time
    unsigned _agent_x;
    unsigned _agent_y;

};

} // namespace NeuroEvo

#endif
