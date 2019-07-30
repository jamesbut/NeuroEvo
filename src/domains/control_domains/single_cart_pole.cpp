#include <domains/control_domains/single_cart_pole.h>
#include <phenotype/phenotype_specs/fixed_network_spec.h>
#include <cmath>
#include <fstream>

namespace NeuroEvo {
namespace Domains {

SingleCartPole::SingleCartPole(const bool MARKOVIAN, const bool RANDOM_START,
                               const bool PRINT_STATE, const bool DOMAIN_TRACE,
                               const int MAX_STEPS) :
    _MAX_STEPS(MAX_STEPS),
    _MARKOVIAN(MARKOVIAN),
    _RANDOM_START(RANDOM_START),
    _PRINT_STATE_TO_FILE(PRINT_STATE),
    _STATE_FILE_NAME("../../data/single_cp_state"),
    Domain(DOMAIN_TRACE, MAX_STEPS+1) {

    //Remove previous state file
    remove(_STATE_FILE_NAME.c_str());

}

double SingleCartPole::single_run(Organism& org, unsigned int rand_seed) {

    //Seed random number generator with same seed as other members of the population
    srand48(rand_seed);

    CartPole cart_pole;

    //Because of the modulus, the numbers will never be bigger than this
    double x_rand = (lrand48()%4800)/1000.0 - 2.4;
    double x_dot_rand = (lrand48()%2000)/1000.0 - 1.0;
    double theta_rand = (lrand48()%400)/1000.0 - 0.2;
    double theta_dot_rand = (lrand48()%3000)/1000.0 - 1.5;

    if(_RANDOM_START) {

        cart_pole.x = x_rand;
        cart_pole.x_dot = x_dot_rand;
        cart_pole.theta = theta_rand;
        cart_pole.theta_dot = theta_dot_rand;

    } else {

        cart_pole.x = 0.0;
        cart_pole.x_dot = 0.0;
        cart_pole.theta = 0.0;
        cart_pole.theta_dot = 0.0;

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

        if(_PRINT_STATE_TO_FILE) print_state_to_file(cart_pole);

        if(_DOMAIN_TRACE) {
            std::cout << "x: " << cart_pole.x << std::endl;
            std::cout << "x_dot: " << cart_pole.x_dot << std::endl;
            std::cout << "theta: " << cart_pole.theta << std::endl;
            std::cout << "theta_dot: " << cart_pole.theta_dot << std::endl;
        }

        //Not sure what these random constants are
        if(_MARKOVIAN) {

            inputs.at(0) = (cart_pole.x + 2.4) / 4.8;
            inputs.at(1) = (cart_pole.x_dot + 0.75) / 1.5;
            inputs.at(2) = (cart_pole.theta + cart_pole.TWELVE_DEGREES) / 0.41;
            inputs.at(3) = (cart_pole.theta_dot + 1.0) / 2.0;

        } else {

            inputs.at(0) = (cart_pole.x + 2.4) / 4.8;
            inputs.at(1) = (cart_pole.theta + cart_pole.TWELVE_DEGREES) / 0.41;

        }

        outputs = phenotype.activate(inputs);

        //Decide which way to push based on which output unit it greater
        bool action = (outputs.at(0) > outputs.at(1)) ? true : false;

        if(_DOMAIN_TRACE) std::cout << "Action: " << action << std::endl;

        //Apply action to cart pole
        double force = (action) ? cart_pole.FORCE_MAG : -cart_pole.FORCE_MAG;
        double cos_theta = cos(cart_pole.theta);
        double sin_theta = sin(cart_pole.theta);

        double temp = (force + cart_pole.POLEMASS_LENGTH * cart_pole.theta_dot * cart_pole.theta_dot
                       * sin_theta) / cart_pole.TOTAL_MASS;

        double thetaacc = (cart_pole.GRAVITY * sin_theta - cos_theta * temp) /
                          (cart_pole.POLE_HALF_LENGTH * (cart_pole.FOUR_THIRDS - cart_pole.POLE_MASS *
                           cos_theta * cos_theta / cart_pole.TOTAL_MASS));

        double xacc = temp - cart_pole.POLEMASS_LENGTH * thetaacc * cos_theta / cart_pole.TOTAL_MASS;

        //Update the four state variables using Euler's method
        cart_pole.x += cart_pole.TAU * cart_pole.x_dot;
        cart_pole.x_dot += cart_pole.TAU * xacc;
        cart_pole.theta += cart_pole.TAU * cart_pole.theta_dot;
        cart_pole.theta_dot += cart_pole.TAU * thetaacc;

        //Check for failure
        if (cart_pole.x < -2.4 || cart_pole.x > 2.4 ||
            cart_pole.theta < -cart_pole.TWELVE_DEGREES || cart_pole.theta > cart_pole.TWELVE_DEGREES)
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

} // namespace Domains
} // namespace NeuroEvo
