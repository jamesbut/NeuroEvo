#include <phenotype/phenotype_specs/phenotype_spec.h>

namespace NeuroEvo {

PhenotypeSpec::PhenotypeSpec(const unsigned num_params, const bool trace) :
    _num_params(num_params),
    _trace(trace),
    _print_traits(false) {}

PhenotypeSpec::PhenotypeSpec(const JSON& json) :
    _num_params(json.at({"num_params"})),
    _trace(json.value({"trace"}, false)),
    _print_traits(false) {}

unsigned PhenotypeSpec::get_num_params() const
{
    return _num_params;
}

bool PhenotypeSpec::get_trace() const
{
    return _trace;
}

bool PhenotypeSpec::get_print_traits() const
{
    return _print_traits;
}

void PhenotypeSpec::set_trace(const bool trace)
{
    _trace = trace;
}

JSON PhenotypeSpec::to_json() const
{
    JSON json;
    json.emplace("num_params", _num_params);
    json.emplace("trace", _trace);
    json.emplace("print_traits", _print_traits);
    //Add all key value pairs from derived class
    json.emplace(to_json_impl());
    return json;
}

} // namespace NeuroEvo
