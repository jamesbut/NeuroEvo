#ifndef _FORMATTING_H_
#define _FORMATTING_H_

/*
 * A collection of functions for formating and displaying text to cout
 */

#include <iomanip>
#include <iostream>
#include <vector>

namespace NeuroEvo {

void print_horizontal_line(const unsigned width);

template <typename T>
void print_table_element(const T& element, const unsigned width)
{
    std::cout << std::left << std::setw(width) << std::setfill(' ') << element;
}

//Prints a formatted table row with column width
template <typename T>
void print_table_row(const std::vector<T>& elements, const unsigned width,
                     const bool line_below = false, const bool line_above = false)
{

    if(line_above)
    {
        const unsigned line_size = (width + 1) * elements.size() + 1;
        print_horizontal_line(line_size);
    }

    std::cout << "|";
    for(const auto& element : elements) {
        print_table_element(element, width);
        std::cout << "|";
    }
    std::cout << std::endl;

    if(line_below)
    {
        const unsigned line_size = (width + 1) * elements.size() + 1;
        print_horizontal_line(line_size);
    }

}

//Overload << operator for vector
template <typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& vec)
{
    //If T is a string, put quotes around the values
    std::string quote = "";
    if(std::is_same<T, std::string>::value)
        quote = "\"";

    os << "[";
    for(const auto& v : vec)
        os << " " << quote << v << quote;
    os << " ]" << std::endl;
    return os;
}

} // namespace NeuroEvo

#endif

