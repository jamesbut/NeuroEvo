#include <data/json.h>
#include <fstream>
#include <iostream>


JSON::JSON(const std::string& file_name) : _j(read_json(file_name))
{
    std::cout << std::setw(4) << _j << std::endl;
}

nlohmann::json JSON::read_json(const std::string& file_path)
{
    std::ifstream json_file(file_path);
    if(json_file.fail())
    {
        throw std::ios_base::failure("Cannot read json at " + file_path);
    }
    return nlohmann::json::parse(json_file);
}
