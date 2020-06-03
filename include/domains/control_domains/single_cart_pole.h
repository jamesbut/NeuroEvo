#ifndef _SINGLE_CART_POLE_H_
#define _SINGLE_CART_POLE_H_

/*
    The single cart pole task consists of a cart
    with a pole under the influence of gravity. A
    neural network aims to keep the pole balanced
    by applying forces to the cart.
    A lot of this code is taken from Ken Stanley's
    NEAT C++ code.
    This implementation can also be markovian or
    non-markovian.
*/

#include <domains/domain.h>
#include <thread>

namespace NeuroEvo {

template <typename G>
class SingleCartPole : public Domain<G, double> 
{

public:

    SingleCartPole(const bool markovian, const bool random_start,
                   const bool print_state, const bool domain_trace,
                   const bool render = false, const int max_steps = 100000) :
        _max_steps(max_steps),
        _markovian(markovian),
        _random_start(random_start),
        _print_state_to_file(print_state),
        _state_file_name(std::string(DATA_PATH) + "/single_cp_state"),
        Domain<G, double>(domain_trace, max_steps+1, render) 
    {

        //Remove previous state file
        remove(_state_file_name.c_str());

        //Set up some variables for cart pole
        _cart_pole.cart_width = _boundary * 2 / 10;
        _cart_pole.cart_height = _cart_pole.cart_width / 2;

        _cart_pole.render_scale = this->_screen_width / (_boundary * 2);

    }


protected:

    void render() override 
    {
        sf::Event event;
        while (this->_window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                this->_window.close();

        this->_window.clear(sf::Color::Black);

        //Render cart
        const float cart_render_width = _cart_pole.cart_width * _cart_pole.render_scale;
        const float cart_render_height = _cart_pole.cart_height * _cart_pole.render_scale;
        sf::Vector2f cart_size(cart_render_width, cart_render_height);
        sf::RectangleShape cart(cart_size);

        float cart_pos_x = (_cart_pole.x + _boundary) * 
                           (_cart_pole.render_scale) - 
                            _cart_pole.cart_width/2;
        float cart_pos_y = this->_screen_height/2;
        cart.setPosition(cart_pos_x, cart_pos_y);

        this->_window.draw(cart);

        //Render pole
        const float pole_render_width = cart_render_width / 10;
        const float pole_render_height = _cart_pole.pole_half_length * 2 * _cart_pole.render_scale;
        sf::Vector2f pole_size(pole_render_width, pole_render_height);
        sf::RectangleShape pole(pole_size);

        pole.setOrigin(pole_render_width/2, pole_render_height);

        float pole_pos_x = cart_pos_x + cart_render_width / 2;
        float pole_pos_y = cart_pos_y;
        pole.setPosition(pole_pos_x, pole_pos_y);

        //Rotate pole
        pole.rotate(_cart_pole.theta * (180.0 / M_PI));

        this->_window.draw(pole);

        //Render floor
        sf::Vector2f floor_size(this->_screen_width, 10.);
        sf::RectangleShape floor(floor_size);
        floor.setPosition(0., this->_screen_height/2 + cart_render_height);

        this->_window.draw(floor);

        //Render all
        this->_window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }

private:

    double single_run(Organism<G, double>& org, unsigned rand_seed) override 
    {

        //Seed random number generator with same seed as other members of the population
        srand48(rand_seed);

        //Because of the modulus, the numbers will never be bigger than this
        double x_rand = (lrand48()%4800)/1000.0 - 2.4;
        double x_dot_rand = (lrand48()%2000)/1000.0 - 1.0;
        double theta_rand = (lrand48()%400)/1000.0 - 0.2;
        double theta_dot_rand = (lrand48()%3000)/1000.0 - 1.5;

        if(_random_start) {

            _cart_pole.x = x_rand;
            _cart_pole.x_dot = x_dot_rand;
            _cart_pole.theta = theta_rand;
            _cart_pole.theta_dot = theta_dot_rand;

        } else {

            _cart_pole.x = 0.0;
            _cart_pole.x_dot = 0.0;
            _cart_pole.theta = 0.0;
            _cart_pole.theta_dot = 0.0;

        }

        int steps = 0;

        //Number of inputs for the markovian version is 4
        //Number of inputs for non-markovian (no velocity provided) is 2
        //Number of outputs is 2 (I personally would have just made it 1)
        std::vector<double> inputs;
        if(_markovian) inputs.resize(4);
        else inputs.resize(2);

        std::vector<double> outputs(2);

        //Start interaction loop
        while(steps++ < _max_steps) 
        {

            if(_print_state_to_file) print_state_to_file(_cart_pole);

            if(this->_domain_trace) 
            {
                std::cout << "x: " << _cart_pole.x << std::endl;
                std::cout << "x_dot: " << _cart_pole.x_dot << std::endl;
                std::cout << "theta: " << _cart_pole.theta << std::endl;
                std::cout << "theta_dot: " << _cart_pole.theta_dot << std::endl;
            }

            //Not sure what these random constants are
            if(_markovian) 
            {

                inputs.at(0) = (_cart_pole.x + 2.4) / 4.8;
                inputs.at(1) = (_cart_pole.x_dot + 0.75) / 1.5;
                inputs.at(2) = (_cart_pole.theta + _cart_pole.twelve_degrees) / 0.41;
                inputs.at(3) = (_cart_pole.theta_dot + 1.0) / 2.0;

            } else 
            {

                inputs.at(0) = (_cart_pole.x + 2.4) / 4.8;
                inputs.at(1) = (_cart_pole.theta + _cart_pole.twelve_degrees) / 0.41;

            }

            outputs = org.get_phenotype().activate(inputs);

            //Decide which way to push based on which output unit it greater
            bool action = (outputs.at(0) > outputs.at(1)) ? true : false;

            if(this->_domain_trace) std::cout << "action: " << action << std::endl;

            //Apply action to cart pole
            double force = (action) ? _cart_pole.force_mag : -_cart_pole.force_mag;
            double cos_theta = cos(_cart_pole.theta);
            double sin_theta = sin(_cart_pole.theta);

            double temp = (force + _cart_pole.polemass_length * _cart_pole.theta_dot * 
                           _cart_pole.theta_dot * sin_theta) / _cart_pole.total_mass;

            double thetaacc = (_cart_pole.gravity * sin_theta - cos_theta * temp) /
                              (_cart_pole.pole_half_length * 
                                    (_cart_pole.four_thirds - _cart_pole.pole_mass *
                                     cos_theta * cos_theta / _cart_pole.total_mass));

            double xacc = temp - _cart_pole.polemass_length * thetaacc * cos_theta / 
                          _cart_pole.total_mass;

            //Update the four state variables using Euler's method
            _cart_pole.x += _cart_pole.tau * _cart_pole.x_dot;
            _cart_pole.x_dot += _cart_pole.tau * xacc;
            _cart_pole.theta += _cart_pole.tau * _cart_pole.theta_dot;
            _cart_pole.theta_dot += _cart_pole.tau * thetaacc;

            //Render
            if(this->_render)
                render();

            //Check for failure
            if (_cart_pole.x < -_boundary || _cart_pole.x > _boundary ||
                _cart_pole.theta < -_cart_pole.twelve_degrees || 
                _cart_pole.theta > _cart_pole.twelve_degrees)
                return (double)steps;

        }

        return (double)steps;

    }

    bool check_phenotype_spec(PhenotypeSpec<G, double>& pheno_spec) override 
    {

        NetworkBuilder* network_builder = dynamic_cast<NetworkBuilder*>(&pheno_spec);

        //If it is not a network
        if(network_builder == nullptr) 
        {
            std::cout << "Only network specifications are allowed with" <<
                        " the single cart pole domain!" << std::endl;
            return false;
        }

        //If it has the correct number of inputs and outputs
        if(_markovian) 
        {

            if(network_builder->get_num_inputs() != 4 || network_builder->get_num_outputs() != 2) 
            {
                std::cout << "Number of inputs must be 4 and number of outputs" <<
                            " must be 2 for the markovian single part cole domain" << std::endl;
                return false;
            }

        } else 
        {
            if(network_builder->get_num_inputs() != 2 || network_builder->get_num_outputs() != 2) 
            {
                std::cout << "Number of inputs must be 2 and number of outputs" <<
                            " must be 2 for the non-markovian single part cole domain" << 
                            std::endl;
                return false;
            }

        }

        return true;

    }

    //Cart Pole struct to store cart variables
    struct CartPole 
    {
    
        const double gravity = 9.8;
        const double cart_mass = 1.0;
        const double pole_mass = 0.1;
        const double total_mass = cart_mass + pole_mass;
        const double pole_half_length = 0.5;
        const double polemass_length = (pole_mass * pole_half_length);
        const double force_mag = 10.0;
        const double tau = 0.02;    //seconds between state updates

        //Rendering properties
        double cart_width;
        double cart_height;
        double render_scale;

        const double twelve_degrees = 0.2094384;
        const double ninety_degrees = M_PI/4;
        const double four_thirds = 1.333333333333;

        double x;
        double x_dot;
        double theta;
        double theta_dot;

    };

    void print_state_to_file(CartPole& cart_pole) 
    {

        std::ofstream state_file;
        state_file.open(_state_file_name, std::fstream::app);

        state_file << cart_pole.x << ",";
        state_file << cart_pole.x_dot << ",";
        state_file << cart_pole.theta << ",";
        state_file << cart_pole.theta_dot << ",";

        state_file << std::endl;

        state_file.close();

    }

    const int _max_steps;
    const bool _markovian;
    const bool _random_start;

    const bool _print_state_to_file;
    const std::string _state_file_name;

    CartPole _cart_pole;

    const double _boundary = 2.4;

};

} // namespace NeuroEvo

#endif
