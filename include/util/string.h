#ifndef _NEUROEVO_STRING_H_
#define _NEUROEVO_STRING_H_

/*
 * A number of functions to assist with the manpulation of strings
 */

#include <vector>
#include <string>

namespace NeuroEvo {

#define is_string_type(T)(std::is_same<T,std::string>::value || \
                          std::is_same<T,std::wstring>::value || \
                          std::is_same<T,std::u16string>::value || \
                          std::is_same<T,std::u32string>::value)

// Split a string by a delimiter
std::vector<std::string> split_string(std::string str, const std::string& delim);

} // namespace NeuroEvo

#endif
