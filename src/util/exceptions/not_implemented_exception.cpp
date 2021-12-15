#include <util/exceptions/not_implemented_exception.h>
#include <string>

namespace NeuroEvo {

NotImplementedException::NotImplementedException(const std::string& function_name) :
    std::logic_error(function_name + std::string(" not yet implemented")) {}

} // namespace NeuroEvo
