#ifndef _NOT_IMPLEMENTED_EXCEPTION_H_
#define _NOT_IMPLEMENTED_EXCEPTION_H_

/*
 * An exception that is thrown when a particular part of code has not yet been
 * implemented
 */

#include <stdexcept>

namespace NeuroEvo {

class NotImplementedException : public std::logic_error
{

public:

    //Argument is function name that has not been implemented
    NotImplementedException(const std::string& function_name);

};

} // namespace NeuroEvo

#endif
