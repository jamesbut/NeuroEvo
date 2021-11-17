#ifndef _JSON_H_
#define _JSON_H_

/*
 * An interface for interacting with json files
 */

#include <string>
#include <nlohmann/json.hpp>
#include <iostream>

namespace NeuroEvo {

class JSON
{

public:

    JSON(const std::string& file_path);
    JSON(const nlohmann::json& json);

    //Retrieve element from JSON with a vector of key strings
    const nlohmann::json at(const std::vector<const std::string>& keys) const;
    const nlohmann::json at(const std::string& key) const;

    friend std::ostream& operator<<(std::ostream& s, const JSON& json);

private:

    nlohmann::json read_json(const std::string& file_path) const;

    nlohmann::json _j;

};

} // namespace NeuroEvo

#endif
