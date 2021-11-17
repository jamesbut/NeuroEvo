#include <util/string.h>

#include <iostream>

namespace NeuroEvo {

std::vector<std::string> split_string(std::string str, const std::string& delim)
{
    std::vector<std::string> string_vec;

    std::size_t pos = 0;
    while((pos = str.find(delim)) != std::string::npos)
    {
        //Get substring from beginning of str to delimiter
        string_vec.push_back(str.substr(0, pos));
        //Remove substring and delimeter
        str.erase(0, pos + delim.length());
    }
    string_vec.push_back(str);

    return string_vec;
}

} // namespace NeuroEvo
