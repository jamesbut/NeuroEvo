#include <phenotype/phenotype_specs/hebbs_spec.h>

namespace NeuroEvo {

HebbsSpec::HebbsSpec(const bool evolve_init_weights,
                     const std::optional<const double> default_init_weight,
                     const bool print_weights_to_file,
                     const std::optional<const std::string>& weights_file_name,
                     const std::optional<const std::string>& outputs_file_name) :
    _evolve_init_weights(evolve_init_weights),
    _default_init_weight(default_init_weight), 
    _print_weights_to_file(print_weights_to_file), 
    _weights_file_name(weights_file_name), 
    _outputs_file_name(outputs_file_name) {}

bool HebbsSpec::get_evolve_init_weights() const
{
    return _evolve_init_weights;
}

bool HebbsSpec::get_print_weights_to_file() const
{
    return _print_weights_to_file;
}

const std::string& HebbsSpec::get_weights_file_name() const
{
    return *_weights_file_name;
}

const std::string& HebbsSpec::get_outputs_file_name() const
{
    return *_outputs_file_name;
}

const std::optional<const double>& HebbsSpec::get_default_weight_init() const
{
    return _default_init_weight;
}

} // namespace NeuroEvo
