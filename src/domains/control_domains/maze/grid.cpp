#include <domains/control_domains/maze/grid.h>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>

namespace NeuroEvo {

Grid::Grid(const std::string file_name, const unsigned agent_start_x, 
           const unsigned agent_start_y) :
    _file_name(file_name) {

    //Build grid from file
    build_grid(_file_name);

    _height = _squares.size();
    _width = _squares.at(0).size();

    _agent_x = agent_start_x;
    _agent_y = agent_start_y;

}

void Grid::build_grid(const std::string file_name) {

    //Read grid from file
    std::vector<std::vector<int> > square_types = read_grid(file_name, 1);
    std::vector<std::vector<int> > square_rewards = read_grid(file_name, 2);

    //Build grid squares
    for(unsigned i = 0; i < square_types.size(); i++) {

        std::vector<GridSquare> square_row(square_types.at(i).size());

        for(unsigned j = 0; j < square_types.at(i).size(); j++)
            square_row.at(j) = GridSquare{j, i, 
                                          square_types.at(i).at(j), 
                                          square_rewards.at(i).at(j)};

        _squares.push_back(square_row);

    }

}

std::vector<std::vector<int> > Grid::read_grid(const std::string file_name, 
                                               const unsigned grid_num) {

    std::vector<std::vector<int> > square_types;

    //Open and read from .mz file
    std::ifstream maze_file;
    maze_file.open(file_name);

    if(maze_file.is_open()) {

        std::string line;

        //Get to the correct grid
        unsigned num_empty_lines_found = 0;
        while(num_empty_lines_found < grid_num) {

            getline(maze_file, line);
            if(line.empty())
                num_empty_lines_found++;

        }

        //Begin reading grid
        while(getline(maze_file, line)) {

            //Stop reading if line is empty
            if(line.empty()) break;

            //Split line by white spaces
            std::istringstream iss(line);
            std::vector<std::string> split_string{std::istream_iterator<std::string>{iss},
                                                  std::istream_iterator<std::string>{}};

            //Convert strings to ints
            std::vector<int> square_types_row(split_string.size());

            for(std::size_t i = 0; i < split_string.size(); i++)
                square_types_row.at(i) = std::stoi(split_string.at(i));

            square_types.push_back(square_types_row);

        }

        maze_file.close();

    } else {

        std::cerr << "Could not open: " << file_name << std::endl;
        exit(EXIT_FAILURE);

    }

    return square_types;

}

void Grid::move_agent(const unsigned x, const unsigned y) {

    _agent_x = x;
    _agent_y = y;

}

void Grid::move_reward(const unsigned x, const unsigned y) {

    //This function can be more generic in the future

    //Remove previous reward
    for(std::size_t i = 0; i < _squares.size(); i++)
        for(std::size_t j = 0; j < _squares.at(i).size(); j++) {
            if(_squares.at(i).at(j).reward != 0)
                _squares.at(i).at(j).reward = 0;
        }

    //Add new reward
    _squares.at(y).at(x).reward = 5;

}

int Grid::get_square_type(const unsigned x, const unsigned y) {
    return _squares.at(y).at(x).square_type;
}

int Grid::get_reward(const unsigned x, const unsigned y) {
    return _squares.at(y).at(x).reward;
}

void Grid::print_grid() {

    for(std::size_t i = 0; i < _squares.size(); i++) {

        for(std::size_t j = 0; j < _squares.at(i).size(); j++) {

            char square_char;
            int square_type = _squares.at(i).at(j).square_type;

            switch(square_type) {

                case 0:
                    square_char = ' ';
                    break;
                case 1:
                    square_char = 'H';
                    break;

            }

            if(_squares.at(i).at(j).reward != 0)
                square_char = 'O';

            if(_agent_x == j && _agent_y == i)
                square_char = 'X';

            std::cout << square_char;

        }

        std::cout << std::endl;

    }

    std::cout << std::endl;

}

} // namespace NeuroEvo
