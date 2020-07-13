#ifndef _HEBBS_SPEC_H_
#define _HEBBS_SPEC_H_

/* Contains information about a Hebbian specification. 
* A Hebbian network can either evolve the initial weights before hebbian training
* or it can not evolve them - if it doesn't evolve them, the question is whether
* the intial weights are set randomly or to a some default value.
*/

#include <optional>
#include <string>

namespace NeuroEvo {

struct HebbsSpec
{

public:

    HebbsSpec(const bool evolve_init_weights,
              const std::optional<const double> default_init_weight = std::nullopt,
              const bool print_weights_to_file = false,
              const std::optional<const std::string>& weights_file_name = std::nullopt,
              const std::optional<const std::string>& outputs_file_name = std::nullopt);

    bool get_evolve_init_weights() const;
    bool get_print_weights_to_file() const;
    const std::string& get_weights_file_name() const;
    const std::string& get_outputs_file_name() const;
    const std::optional<const double>& get_default_weight_init() const;

private:

    const bool _evolve_init_weights;
    const std::optional<const double> _default_init_weight;
    const bool _print_weights_to_file;
    const std::optional<const std::string> _weights_file_name;
    const std::optional<const std::string> _outputs_file_name;

};

} // namespace NeuroEvo

#endif
