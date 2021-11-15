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

    //Give read only access to nlohmann::json object
    const nlohmann::json& get() const;

    friend std::ostream& operator<<(std::ostream& s, const JSON& json);

private:

    nlohmann::json read_json(const std::string& file_path) const;

    nlohmann::json _j;

};

} // namespace NeuroEvo

#endif
