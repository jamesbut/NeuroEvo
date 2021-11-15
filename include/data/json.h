#ifndef _JSON_H_
#define _JSON_H_

/*
 * An interface for interacting with json files
 */

#include <string>
#include <nlohmann/json.hpp>

class JSON
{

public:

    JSON(const std::string& file_path);

private:

    nlohmann::json read_json(const std::string& file_path);

    nlohmann::json _j;

};

#endif
