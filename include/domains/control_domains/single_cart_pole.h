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

    struct CartPoleSpecs 
    {

        double gravity = 9.8;
        double cart_mass = 1.0;
        double pole_mass = 0.1;
        double total_mass = cart_mass + pole_mass;
        double pole_half_length = 0.5;
        double polemass_length = (pole_mass * pole_half_length);
        double force_mag = 10.0;
        double tau = 0.02;    //seconds between state updates

    };

    SingleCartPole(const bool markovian, const bool random_start,
                   const bool continuous_actuator = true,
                   const CartPoleSpecs& cart_pole_specs = CartPoleSpecs(),
                   const bool render = false, const bool domain_trace = false,
                   const bool print_state = false, const unsigned max_steps = 100000,
                   const double starting_x = 0., const double starting_x_dot = 0.,
                   const double starting_theta = 0., const double starting_theta_dot = 0.) :
        Domain<G, double>(domain_trace, max_steps, std::nullopt, render),
        _max_steps(max_steps),
        _markovian(markovian),
        _random_start(random_start),
        _continuous_actuator(continuous_actuator),
        _print_state_to_file(print_state),
        _state_file_name(std::string(DATA_PATH) + "/single_cp_state"),
        _starting_x(starting_x),
        _starting_x_dot(starting_x_dot),
        _starting_theta(starting_theta),
        _starting_theta_dot(starting_theta_dot)
    {

        //Remove previous state file
        remove(_state_file_name.c_str());

        _x_max = 1.2*_boundary;

        _cart_pole.render_scale = this->_screen_width / (_x_max * 2);

        //Set up some variables for cart pole
        _cart_pole.cart_width = _boundary * 2 / 10;
        _cart_pole.cart_height = _cart_pole.cart_width / 2;

        _cart_pole.cart_render_width = _cart_pole.cart_width * _cart_pole.render_scale;
        _cart_pole.cart_render_height = _cart_pole.cart_height * _cart_pole.render_scale;

        _cart_pole.specs = cart_pole_specs;

    }

private:

    double single_run(Organism<G, double>& org, unsigned rand_seed) override 
    {

        //Seed random number generator with same seed as other members of the population
        srand48(rand_seed);

        if(_random_start) {

            double x_lb, x_ub, x_dot_lb, x_dot_ub, 
                   theta_lb, theta_ub, theta_dot_lb, theta_dot_ub;
            if(_markovian)
            {
               x_lb = -2.; 
               x_ub = 2.;
               x_dot_lb = -1.;
               x_dot_ub = 1.;
               theta_lb = -0.16;
               theta_ub = 0.16;
               theta_dot_lb = -1.;
               theta_dot_ub = 1.;
            } else
            {
               x_lb = -1.; 
               x_ub = 1.;
               x_dot_lb = -1.;
               x_dot_ub = 1.;
               theta_lb = -0.2;
               theta_ub = 0.2;
               theta_dot_lb = -1.;
               theta_dot_ub = 1.;
            }

            //[-2.4, 2.4]
            //const double x_rand = (lrand48()%4800)/1000.0 - 2.4;
            const double x_rand = UniformRealDistribution::get(x_lb, x_ub, rand_seed);
            //[-1., 1.]
            //const double x_dot_rand = (lrand48()%2000)/1000.0 - 1.0;
            const double x_dot_rand = UniformRealDistribution::get(x_dot_lb, x_dot_ub, 
                                                                   rand_seed);
            //[-0.2, 0.2]
            //const double theta_rand = (lrand48()%400)/1000.0 - 0.2
            const double theta_rand = UniformRealDistribution::get(theta_lb, theta_ub, 
                                                                   rand_seed);
            //[-1.5, 1.5]
            //const double theta_dot_rand = (lrand48()%3000)/1000.0 - 1.5;
            const double theta_dot_rand = UniformRealDistribution::get(theta_dot_lb, 
                                                                       theta_dot_ub,
                                                                       rand_seed);

            /*
            std::cout << "x rand: " << x_rand << std::endl;
            std::cout << "x_dot rand: " << x_dot_rand << std::endl;
            std::cout << "theta rand: " << theta_rand << std::endl;
            std::cout << "theta_dot rand: " << theta_dot_rand << std::endl;
            */

            _cart_pole.x = x_rand;
            _cart_pole.x_dot = x_dot_rand;
            _cart_pole.theta = theta_rand;
            _cart_pole.theta_dot = theta_dot_rand;

        } else {

            _cart_pole.x = _starting_x;
            _cart_pole.x_dot = _starting_x_dot;
            _cart_pole.theta = _starting_theta;
            _cart_pole.theta_dot = _starting_theta_dot;

        }

        unsigned steps = 0;

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

            double force = 0.;
            if(_continuous_actuator)
                force = calculate_continuous_force(outputs);
            else
                force = calculate_discrete_force(outputs);

            const double cos_theta = cos(_cart_pole.theta);
            const double sin_theta = sin(_cart_pole.theta);

            const double temp = (force + _cart_pole.specs.polemass_length * 
                                 _cart_pole.theta_dot * _cart_pole.theta_dot * sin_theta) / 
                                _cart_pole.specs.total_mass;

            const double thetaacc = (_cart_pole.specs.gravity * sin_theta - cos_theta * temp) /
                                    (_cart_pole.specs.pole_half_length * 
                                    (_cart_pole.four_thirds - _cart_pole.specs.pole_mass *
                                     cos_theta * cos_theta / _cart_pole.specs.total_mass));

            const double xacc = temp - _cart_pole.specs.polemass_length * thetaacc * cos_theta /
                                _cart_pole.specs.total_mass;

            //Update the four state variables using Euler's method
            _cart_pole.x += _cart_pole.specs.tau * _cart_pole.x_dot;
            _cart_pole.x_dot += _cart_pole.specs.tau * xacc;
            _cart_pole.theta += _cart_pole.specs.tau * _cart_pole.theta_dot;
            _cart_pole.theta_dot += _cart_pole.specs.tau * thetaacc;

            //Render
            if(this->_render)
                render();

            //Check for failure
            if (_cart_pole.x < -_boundary || _cart_pole.x > _boundary ||
                //_cart_pole.theta < -_cart_pole.twelve_degrees || 
                //_cart_pole.theta > _cart_pole.twelve_degrees)
                _cart_pole.theta < -_cart_pole.fortyfive_degrees || 
                _cart_pole.theta > _cart_pole.fortyfive_degrees)
                return (double)steps;

        }

        return (double)steps;

    }

    double calculate_discrete_force(const std::vector<double>& net_outputs) const
    {
        //Decide which way to push based on which output unit it greater
        bool action = true;
        if(net_outputs.size() == 2)
            action = (net_outputs.at(0) > net_outputs.at(1)) ? true : false;
        //Or whether the value is below or above 0.5
        else if(net_outputs.size() == 1)
            action = (net_outputs.at(0) > 0.5) ? true : false;

        if(this->_domain_trace) 
            std::cout << "action: " << action << std::endl;

        //Apply action to cart pole
        const double force = (action) ? _cart_pole.specs.force_mag : 
                                        -_cart_pole.specs.force_mag;
        return force;

    }

    double calculate_continuous_force(const std::vector<double>& net_outputs) const
    {
        return net_outputs[0] * 2 * _cart_pole.specs.force_mag - _cart_pole.specs.force_mag;
    }

    bool check_phenotype_spec(const PhenotypeSpec& pheno_spec) const override 
    {

        const NetworkBuilder* network_builder = 
            dynamic_cast<const NetworkBuilder*>(&pheno_spec);

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

            if(network_builder->get_num_inputs() != 4)
            {
                std::cout << "Number of inputs must be 4 " <<
                            "for the markovian single part cole domain" << std::endl;
                return false;
            }

        } else 
        {
            if(network_builder->get_num_inputs() != 2)
            {
                std::cout << "Number of inputs must be 2 " <<
                            "for the non-markovian single part cole domain" << 
                            std::endl;
                return false;
            }
        }

        if((network_builder->get_num_outputs() != 1) && 
           (network_builder->get_num_outputs() != 2))
        {
            std::cerr << "Number of outputs must be 1 or 2 for SCP domain" << std::endl;
            return false;
        }

        return true;

    }

    //Cart Pole struct to store cart variables
    struct CartPole 
    {
    
        CartPoleSpecs specs;

        //Rendering properties
        double cart_width;
        double cart_height;
        double cart_render_width;
        double cart_render_height;
        //This is like pixels per unit x
        double render_scale;

        const double twelve_degrees = 0.2094384;
        const double ninety_degrees = M_PI/4;
        const double fortyfive_degrees = 0.785398;
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

    void render() override 
    {
#if SFML_FOUND
        sf::Event event;
        while(this->_window.pollEvent(event))
            if(event.type == sf::Event::Closed)
                this->_window.close();

        this->_window.clear(sf::Color::Black);

        //Render cart
        sf::Vector2f cart_size(_cart_pole.cart_render_width, _cart_pole.cart_render_height);
        sf::RectangleShape cart(cart_size);

        const float cart_pos_x = x_to_pixel_x(_cart_pole.x) - _cart_pole.cart_render_width / 2;
        const float cart_pos_y = this->_screen_height/2;
        cart.setPosition(cart_pos_x, cart_pos_y);

        this->_window.draw(cart);

        //Render pole
        const float pole_render_width = _cart_pole.cart_render_width / 10;
        const float pole_render_height = _cart_pole.specs.pole_half_length * 2 * 
                                         _cart_pole.render_scale;
        sf::Vector2f pole_size(pole_render_width, pole_render_height);
        sf::RectangleShape pole(pole_size);

        pole.setOrigin(pole_render_width/2, pole_render_height);

        const float pole_pos_x = cart_pos_x + _cart_pole.cart_render_width / 2;
        const float pole_pos_y = cart_pos_y;
        pole.setPosition(pole_pos_x, pole_pos_y);

        //Rotate pole
        pole.rotate(_cart_pole.theta * (180.0 / M_PI));

        this->_window.draw(pole);

        //Render floor
        sf::Vector2f floor_size(this->_screen_width, 10.);
        sf::RectangleShape floor(floor_size);
        const float floor_height = this->_screen_height/2 + _cart_pole.cart_render_height;
        floor.setPosition(0., floor_height);

        this->_window.draw(floor);

        //Render boundaries
        sf::Vector2f boundary_marker_size(2, 50);
        const float boundary_marker_y = floor_height - boundary_marker_size.y;

        sf::RectangleShape boundary_marker_r(boundary_marker_size);
        const float boundary_marker_r_x = x_to_pixel_x(_boundary) +
                                          _cart_pole.cart_render_width / 2;
        boundary_marker_r.setPosition(boundary_marker_r_x, boundary_marker_y);
        boundary_marker_r.setFillColor(sf::Color::Red);
        this->_window.draw(boundary_marker_r);

        sf::RectangleShape boundary_marker_l(boundary_marker_size);
        const float boundary_marker_l_x = x_to_pixel_x(-_boundary) - 
                                          _cart_pole.cart_render_width / 2;
        boundary_marker_l.setPosition(boundary_marker_l_x, boundary_marker_y);
        boundary_marker_l.setFillColor(sf::Color::Red);
        this->_window.draw(boundary_marker_l);

        //Render all
        this->_window.display();

        std::this_thread::sleep_for(std::chrono::milliseconds(25));
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
    }

    float x_to_pixel_x(const float x) const
    {
        return (this->_screen_width / (2 * _x_max)) * x + (this->_screen_width / 2);
    }

    void exp_run_reset_impl(const unsigned run_num, const unsigned run_seed) override {}

    void trial_reset(const unsigned trial_num) override
    {
        /*
        switch(trial_num)
        {
            case 0:
                _cart_pole.specs.gravity = 1.;
                break;
            case 1:
                _cart_pole.specs.gravity = 5.;
                break;
            case 2:
                _cart_pole.specs.gravity = 10.;
                break;
            case 3:
                _cart_pole.specs.gravity = 15.;
                break;
            case 4:
                _cart_pole.specs.gravity = 20.;
                break;
        }
        */
    }

    SingleCartPole<G>* clone_impl() const override
    {
        return new SingleCartPole<G>(*this);
    }

    const unsigned _max_steps;
    const bool _markovian;
    const bool _random_start;
    const bool _continuous_actuator;

    const bool _print_state_to_file;
    const std::string _state_file_name;

    CartPole _cart_pole;

    const double _boundary = 2.4;
    double _x_max;

    const double _starting_x;
    const double _starting_x_dot;
    const double _starting_theta;
    const double _starting_theta_dot;

};

} // namespace NeuroEvo

#endif
