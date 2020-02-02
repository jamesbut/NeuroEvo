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

namespace NeuroEvo {
namespace Domains {

class SingleCartPole : public Domain {

public:

    SingleCartPole(const bool MARKOVIAN, const bool RANDOM_START,
                   const bool PRINT_STATE, const bool DOMAIN_TRACE,
                   const bool RENDER = false, const int MAX_STEPS = 100000);

protected:

    void render() override;

private:

    double single_run(Organism& org, unsigned int rand_seed) override;

    bool check_phenotype_spec(Phenotypes::PhenotypeSpec& pheno_spec) override;

    //Cart Pole struct to store cart variables
    struct CartPole {
    
        const double GRAVITY = 9.8;
        const double CART_MASS = 1.0;
        const double POLE_MASS = 0.1;
        const double TOTAL_MASS = CART_MASS + POLE_MASS;
        const double POLE_HALF_LENGTH = 0.5;
        const double POLEMASS_LENGTH = (POLE_MASS * POLE_HALF_LENGTH);
        const double FORCE_MAG = 10.0;
        const double TAU = 0.02;    //Seconds between state updates

        //Rendering properties
        double CART_WIDTH;
        double CART_HEIGHT;
        double RENDER_SCALE;

        const double TWELVE_DEGREES = 0.2094384;
        const double NINETY_DEGREES = M_PI/4;
        const double FOUR_THIRDS = 1.333333333333;

        double x;
        double x_dot;
        double theta;
        double theta_dot;

    };

    void print_state_to_file(CartPole& cart_pole);

    const int _MAX_STEPS;
    const bool _MARKOVIAN;
    const bool _RANDOM_START;

    const bool _PRINT_STATE_TO_FILE;
    const std::string _STATE_FILE_NAME;

    CartPole _cart_pole;

    const double _BOUNDARY = 2.4;

};

} // namespace Domains
} // namespace NeuroEvo

#endif
