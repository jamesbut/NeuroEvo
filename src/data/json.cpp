#include <data/json.h>
#include <fstream>
#include <iostream>

namespace NeuroEvo {

JSON::JSON() {}

JSON::JSON(const std::string& file_name) : _j(read_json(file_name)) {}

JSON::JSON(const nlohmann::json& json) : _j(json) {}

nlohmann::json JSON::read_json(const std::string& file_path) const
{
    std::ifstream json_file(file_path);
    if(json_file.fail())
        throw std::ios_base::failure("Cannot read json at " + file_path);
    if(file_path.find(".json") == std::string::npos)
        throw std::invalid_argument(file_path + " does not have .json suffix");
    return nlohmann::json::parse(json_file);
}

const nlohmann::json JSON::at(const std::vector<const std::string>& keys) const
{
    nlohmann::json json = _j;
    for(const auto& key : keys)
        json = json.at(key);
    return json;
}

bool JSON::has_value(const std::vector<const std::string>& keys) const
{
    try {
        at(keys);
    } catch(nlohmann::detail::out_of_range&) {
        return false;
    }
    return true;
}

void JSON::emplace(const JSON& json)
{
    for(const auto& [key, value] : json.items())
        emplace(key, value);
}

void JSON::save_to_file(const std::string& file_path) const
{
    std::ofstream json_file(file_path + ".json");
    json_file << *this;
    json_file.close();
}

std::ostream& operator<<(std::ostream& s, const JSON& json)
{
    return s << std::setw(4) << json._j << std::endl;
}

} // namespace NeuroEvo
