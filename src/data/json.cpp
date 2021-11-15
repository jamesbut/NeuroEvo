#include <data/json.h>
#include <nlohmann/json.hpp>
#include <fstream>

#include <iostream>

JSON::JSON(const std::string& file_name)
{
    std::cout << file_name << std::endl;
    std::ifstream json_file(file_name);
    nlohmann::json j;
}
