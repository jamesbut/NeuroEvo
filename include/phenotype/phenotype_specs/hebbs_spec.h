#ifndef _HEBBS_SPEC_H_
#define _HEBBS_SPEC_H_

/* Contains information about a Hebbian specification. 
* A Hebbian network can either evolve the initial weights before hebbian training
* or it can not evolve them - if it doesn't evolve them, the question is whether
* the intial weights are set randomly or to a some default value.
*/
struct HebbsSpec
{

public:

    HebbsSpec(const bool evolve_init_weights,
                const std::optional<double> default_init_weight = std::nullopt,
                const bool print_weights_to_file = false,
                const std::optional<std::string>& weights_file_name = std::nullopt,
                const std::optional<std::string>& outputs_file_name = std::nullopt) :
        _evolve_init_weights(evolve_init_weights),
        _default_init_weight(default_init_weight), 
        _print_weights_to_file(print_weights_to_file), 
        _weights_file_name(weights_file_name), 
        _outputs_file_name(outputs_file_name) {}

    bool get_evolve_init_weights() const 
    {
        return _evolve_init_weights;
    }

    bool get_print_weights_to_file() const
    {
        return _print_weights_to_file;
    }

    const std::string& get_weights_file_name() const
    {
        return *_weights_file_name;
    }

    const std::string& get_outputs_file_name() const
    {
        return *_outputs_file_name;
    }

    const std::optional<double>& get_default_weight_init() const
    {
        return _default_init_weight;
    }

private:

    const bool _evolve_init_weights;
    const std::optional<double> _default_init_weight;
    const bool _print_weights_to_file;
    const std::optional<std::string> _weights_file_name;
    const std::optional<std::string> _outputs_file_name;

};

#endif
