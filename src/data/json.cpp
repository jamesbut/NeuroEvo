#include <data/json.h>
#include <fstream>
#include <iostream>

namespace NeuroEvo {

JSON::JSON(const std::string& file_name) : _j(read_json(file_name)) {}

JSON::JSON(const nlohmann::json& json) : _j(json) {}

nlohmann::json JSON::read_json(const std::string& file_path) const
{
    std::ifstream json_file(file_path);
    if(json_file.fail())
        throw std::ios_base::failure("Cannot read json at " + file_path);
    return nlohmann::json::parse(json_file);
}

const nlohmann::json JSON::at(const std::vector<const std::string>& keys) const
{
    nlohmann::json json = _j;
    for(const auto& key : keys)
        json = json.at(key);
    return json;
}

std::ostream& operator<<(std::ostream& s, const JSON& json)
{
    return s << std::setw(4) << json._j << std::endl;
}

} // namespace NeuroEvo
