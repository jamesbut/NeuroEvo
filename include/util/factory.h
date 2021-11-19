#ifndef _FACTORY_H_
#define _FACTORY_H_

/*
 * A factory class that allows the registration of constructors that parse JSON files
 */

#include <map>
#include <boost/type_index.hpp>
#include <data/json.h>
#include <util/string.h>
#include <util/formatting.h>
#include <fort.hpp>


namespace NeuroEvo {

template <typename T>
class Factory
{

public:

    // Create class derived from T with json specification
    static std::shared_ptr<T> create(const JSON& json)
    {
        // Get name of registered class from json
        const std::string derived_class_name = json.at({_T_class_name, "name"});
        // Find function pointer to constructor of that class in factory map
        auto it = _factory_map.find(derived_class_name);
        // If constructor pointer is in factory map, call constructor
        if(it != _factory_map.end())
            return it->second(JSON(json.at(_T_class_name)));
        else
            throw std::invalid_argument(derived_class_name + " is not in " +
                                        _T_class_name + " factory map");
    }

    // A class to register JSON constructors of derived classes
    class Registrar
    {
    public:
        // Add constructor pointer to factory map
        Registrar(const std::string& derived_class_name,
                  std::function<std::shared_ptr<T>(const JSON&)>func)
        {
            static Factory<T> factory;
            factory._factory_map[derived_class_name] = func;

        }

        // Macro to make registration easier
        // TODO: Come back to this to make it work more generically
        #define REGISTER(DERIVED_CLASS, BASE_CLASS, T) \
        static Factory<BASE_CLASS<T, T>>::Registrar registrar( \
            #DERIVED_CLASS, \
            [](const JSON& json) {return std::make_shared<DERIVED_CLASS<T>>(json);});

    };

    // Print factory map state
    static void print()
    {
        fort::char_table table;
            table << fort::header << _T_class_name + " Factory Map" << fort::endr;

        table[0][0].set_cell_span(2);
        table[0].set_cell_row_type(fort::row_type::header);

        for(const auto& fm_element : _factory_map)
            table << fm_element.first << &fm_element.second << fort::endr;

        std::cout << table.to_string();
    }

private:

    //Get parsed class name of T
    static const std::string retrieve_class_name()
    {
        //Get class name of T (base class)
        const std::string T_class_name = boost::typeindex::type_id<T>().pretty_name();

        //Remove namespaces and templates
        return split_string(split_string(T_class_name, "<").front(), "::").back();
    }

    // Maps derived class name strings to constructor function pointers
    inline static
        std::map<const std::string,
                 std::function<std::shared_ptr<T>(const JSON& json)>> _factory_map;

    // Base class name string
    inline static const std::string _T_class_name = retrieve_class_name();

};

} // namespace NeuroEvo

#endif
