#include <domains/control_domains/single_cart_pole.h>
#include <phenotype/phenotype_specs/fixed_network_spec.h>
#include <cmath>
#include <fstream>
#include <chrono>
#include <thread>

namespace NeuroEvo {
namespace Domains {

SingleCartPole::SingleCartPole(const bool MARKOVIAN, const bool RANDOM_START,
                               const bool PRINT_STATE, const bool DOMAIN_TRACE,
                               const bool RENDER, const int MAX_STEPS) :
    _MAX_STEPS(MAX_STEPS),
    _MARKOVIAN(MARKOVIAN),
    _RANDOM_START(RANDOM_START),
    _PRINT_STATE_TO_FILE(PRINT_STATE),
    _STATE_FILE_NAME(std::string(DATA_PATH) + "/single_cp_state"),
    Domain(DOMAIN_TRACE, MAX_STEPS+1, RENDER) {

    //Remove previous state file
    remove(_STATE_FILE_NAME.c_str());

    //Set up some variables for cart pole
    _cart_pole.CART_WIDTH = _BOUNDARY * 2 / 10;
    _cart_pole.CART_HEIGHT = _cart_pole.CART_WIDTH / 2;

    _cart_pole.RENDER_SCALE = _SCREEN_WIDTH / (_BOUNDARY * 2);

}

double SingleCartPole::single_run(Organism& org, unsigned int rand_seed) {

    //Seed random number generator with same seed as other members of the population
    srand48(rand_seed);

    //Because of the modulus, the numbers will never be bigger than this
    double x_rand = (lrand48()%4800)/1000.0 - 2.4;
    double x_dot_rand = (lrand48()%2000)/1000.0 - 1.0;
    double theta_rand = (lrand48()%400)/1000.0 - 0.2;
    double theta_dot_rand = (lrand48()%3000)/1000.0 - 1.5;

    if(_RANDOM_START) {

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


    Phenotypes::Phenotype& phenotype = org.get_phenotype();

    int steps = 0;

    //Number of inputs for the markovian version is 4
    //Number of inputs for non-markovian (no velocity provided) is 2
    //Number of outputs is 2 (I personally would have just made it 1)
    std::vector<double> inputs;
    if(_MARKOVIAN) inputs.resize(4);
    else inputs.resize(2);

    std::vector<double> outputs(2);

    //Start interaction loop
    while(steps++ < _MAX_STEPS) {

        if(_PRINT_STATE_TO_FILE) print_state_to_file(_cart_pole);

        if(_DOMAIN_TRACE) {
            std::cout << "x: " << _cart_pole.x << std::endl;
            std::cout << "x_dot: " << _cart_pole.x_dot << std::endl;
            std::cout << "theta: " << _cart_pole.theta << std::endl;
            std::cout << "theta_dot: " << _cart_pole.theta_dot << std::endl;
        }

        //Not sure what these random constants are
        if(_MARKOVIAN) {

            inputs.at(0) = (_cart_pole.x + 2.4) / 4.8;
            inputs.at(1) = (_cart_pole.x_dot + 0.75) / 1.5;
            inputs.at(2) = (_cart_pole.theta + _cart_pole.TWELVE_DEGREES) / 0.41;
            inputs.at(3) = (_cart_pole.theta_dot + 1.0) / 2.0;

        } else {

            inputs.at(0) = (_cart_pole.x + 2.4) / 4.8;
            inputs.at(1) = (_cart_pole.theta + _cart_pole.TWELVE_DEGREES) / 0.41;

        }

        outputs = phenotype.activate(inputs);

        //Decide which way to push based on which output unit it greater
        bool action = (outputs.at(0) > outputs.at(1)) ? true : false;

        if(_DOMAIN_TRACE) std::cout << "Action: " << action << std::endl;

        //Apply action to cart pole
        double force = (action) ? _cart_pole.FORCE_MAG : -_cart_pole.FORCE_MAG;
        double cos_theta = cos(_cart_pole.theta);
        double sin_theta = sin(_cart_pole.theta);

        double temp = (force + _cart_pole.POLEMASS_LENGTH * _cart_pole.theta_dot * _cart_pole.theta_dot
                       * sin_theta) / _cart_pole.TOTAL_MASS;

        double thetaacc = (_cart_pole.GRAVITY * sin_theta - cos_theta * temp) /
                          (_cart_pole.POLE_HALF_LENGTH * (_cart_pole.FOUR_THIRDS - _cart_pole.POLE_MASS *
                           cos_theta * cos_theta / _cart_pole.TOTAL_MASS));

        double xacc = temp - _cart_pole.POLEMASS_LENGTH * thetaacc * cos_theta / _cart_pole.TOTAL_MASS;

        //Update the four state variables using Euler's method
        _cart_pole.x += _cart_pole.TAU * _cart_pole.x_dot;
        _cart_pole.x_dot += _cart_pole.TAU * xacc;
        _cart_pole.theta += _cart_pole.TAU * _cart_pole.theta_dot;
        _cart_pole.theta_dot += _cart_pole.TAU * thetaacc;

        //Render
        if(_RENDER) {

            render();

        }

        //Check for failure
        if (_cart_pole.x < -_BOUNDARY || _cart_pole.x > _BOUNDARY ||
            _cart_pole.theta < -_cart_pole.TWELVE_DEGREES || _cart_pole.theta > _cart_pole.TWELVE_DEGREES)
            return (double)steps;

    }

    return (double)steps;

}

bool SingleCartPole::check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) {

    Phenotypes::NetworkSpec* network_spec = dynamic_cast<Phenotypes::NetworkSpec*>(&pheno_spec);

    //If it is not a network
    if(network_spec == nullptr) {

        std::cout << "Only network specifications are allowed with" <<
                     " the single cart pole domain!" << std::endl;
        return false;

    }

    //If it has the correct number of inputs and outputs
    if(_MARKOVIAN) {

        if(network_spec->NUM_INPUTS != 4 || network_spec->NUM_OUTPUTS != 2) {

            std::cout << "Number of inputs must be 4 and number of outputs" <<
                         " must be 2 for the markovian single part cole domain" << std::endl;
            return false;

        }

    } else {

        if(network_spec->NUM_INPUTS != 2 || network_spec->NUM_OUTPUTS != 2) {

            std::cout << "Number of inputs must be 2 and number of outputs" <<
                         " must be 2 for the non-markovian single part cole domain" << std::endl;
            return false;

        }

    }

    return true;

}

void SingleCartPole::print_state_to_file(CartPole& cart_pole) {

    std::ofstream state_file;
    state_file.open(_STATE_FILE_NAME, std::fstream::app);

    state_file << cart_pole.x << ",";
    state_file << cart_pole.x_dot << ",";
    state_file << cart_pole.theta << ",";
    state_file << cart_pole.theta_dot << ",";

    state_file << std::endl;

    state_file.close();

}

void SingleCartPole::render() {

    sf::Event event;
    while (_window.pollEvent(event))
        if (event.type == sf::Event::Closed)
            _window.close();

    _window.clear(sf::Color::Black);

    //Render cart
    const float CART_RENDER_WIDTH = _cart_pole.CART_WIDTH * _cart_pole.RENDER_SCALE;
    const float CART_RENDER_HEIGHT = _cart_pole.CART_HEIGHT * _cart_pole.RENDER_SCALE;
    sf::Vector2f cart_size(CART_RENDER_WIDTH, CART_RENDER_HEIGHT);
    sf::RectangleShape cart(cart_size);

    float cart_pos_x = (_cart_pole.x + _BOUNDARY) * 
                       (_cart_pole.RENDER_SCALE) - 
                        _cart_pole.CART_WIDTH/2;
    float cart_pos_y = _SCREEN_HEIGHT/2;
    cart.setPosition(cart_pos_x, cart_pos_y);

    _window.draw(cart);

    //Render pole
    const float POLE_RENDER_WIDTH = CART_RENDER_WIDTH / 10;
    const float POLE_RENDER_HEIGHT = _cart_pole.POLE_HALF_LENGTH * 2 * _cart_pole.RENDER_SCALE;
    sf::Vector2f pole_size(POLE_RENDER_WIDTH, POLE_RENDER_HEIGHT);
    sf::RectangleShape pole(pole_size);

    pole.setOrigin(POLE_RENDER_WIDTH/2, POLE_RENDER_HEIGHT);

    float pole_pos_x = cart_pos_x + CART_RENDER_WIDTH / 2;
    float pole_pos_y = cart_pos_y;
    pole.setPosition(pole_pos_x, pole_pos_y);

    //Rotate pole
    pole.rotate(_cart_pole.theta * (180.0 / M_PI));

    _window.draw(pole);

    //Render floor
    sf::Vector2f floor_size(_SCREEN_WIDTH, 10.);
    sf::RectangleShape floor(floor_size);
    floor.setPosition(0., _SCREEN_HEIGHT/2 + CART_RENDER_HEIGHT);

    _window.draw(floor);

    //Render all
    _window.display();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

}

} // namespace Domains
} // namespace NeuroEvo
