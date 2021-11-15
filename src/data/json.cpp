#include <data/json.h>
#include <fstream>
#include <iostream>

namespace NeuroEvo {

JSON::JSON(const std::string& file_name) : _j(read_json(file_name)) {}

nlohmann::json JSON::read_json(const std::string& file_path)
{
    std::ifstream json_file(file_path);
    if(json_file.fail())
        throw std::ios_base::failure("Cannot read json at " + file_path);
    return nlohmann::json::parse(json_file);
}

const nlohmann::json& JSON::get() const
{
    return _j;
}

std::ostream& operator<<(std::ostream& s, const JSON& json)
{
    return s << std::setw(4) << json._j << std::endl;
}

} // namespace NeuroEvo
