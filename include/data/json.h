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

    JSON();
    JSON(const std::string& file_path);
    JSON(const nlohmann::json& json);

    //Retrieve element from JSON with a vector of key strings
    //If no argument is given a copy of the underlying nlohmann::json object is returned
    const nlohmann::json at(
        const std::vector<const std::string>& keys = std::vector<const std::string>()
    ) const;

    //Retrieve json value as T
    template <typename T>
    T get(const std::vector<const std::string>& keys) const
    {
        return at(keys).get<T>();
    }

    //The same as at() but with a default value
    template <typename T>
    T value(const std::vector<const std::string>& keys,
            const T& default_value) const
    {
        try {
            return at(keys);
        } catch(nlohmann::detail::out_of_range&) {
            return default_value;
        }
    }

    //Emplace key value pair in json
    template <typename T>
    void emplace(const std::string& key, const T& value)
    {
        if constexpr(std::is_same<T, JSON>::value)
            _j[key] = value.at();
        else
            _j.emplace(key, value);
    }

    //Emplace all values of given json in this json with no key value
    void emplace(const JSON& json);

    template <typename T>
    void emplace_back(const T& value)
    {
        _j.emplace_back(value);
    }

    auto items() const
    {
        return _j.items();
    }

    //Returns true if value is in json dictionary
    bool has_value(const std::vector<const std::string>& keys) const;

    //Saves json to file
    void save_to_file(const std::string& file_path) const;

    friend std::ostream& operator<<(std::ostream& s, const JSON& json);

private:

    nlohmann::json read_json(const std::string& file_path) const;

    nlohmann::json _j;

};

} // namespace NeuroEvo

#endif
