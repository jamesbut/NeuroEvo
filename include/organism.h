#ifndef _ORGANISM_H_
#define _ORGANISM_H_

/*
    This class contains everything that constitutes
    an Organism, including its genotype, phenotype
    and genotype-phenotype map.
*/

#include <genotype/genotype.h>
#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <memory>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <gp_map/gp_map.h>

namespace NeuroEvo {

template <typename G, typename T>
class Organism {

public:

    Organism(const Genotype<G>& genotype, std::shared_ptr<GPMap<G, T>> gp_map) :
        _genotype(genotype.clone()),
        _gp_map(gp_map->clone()),
        _phenotype(gp_map->map(*_genotype)),
        _fitness(std::nullopt),
        _domain_winner(false) {}

    Organism(std::shared_ptr<GPMap<G, T>> gp_map,
             const std::string file_name) :
        _genotype(new Genotype<G>(file_name)),
        _gp_map(gp_map->clone()),
        _phenotype(gp_map->map(*_genotype)),
        _fitness(std::nullopt),
        _domain_winner(false) {}

    Organism(const Organism& organism) :
        _genotype(organism.get_genotype().clone()),
        _gp_map(organism._gp_map->clone()),
        _phenotype(organism.get_phenotype().clone_phenotype()),
        _fitness(organism.get_fitness()),
        _domain_winner(organism._domain_winner) {}

    Organism& operator=(const Organism& organism)
    {
        _genotype = organism.get_genotype().clone();
        _gp_map = organism._gp_map->clone();
        _phenotype = organism.get_phenotype().clone_phenotype();
        _fitness = organism.get_fitness();
        _domain_winner = organism._domain_winner;

        return *this;
    }

    ~Organism() = default;
    Organism(Organism&& organism) = default;
    Organism& operator=(Organism&& organism) = default;

    //Pass in domain completion fitness too in order to determine whether the
    //organism is a domain winner
    void set_fitness(const double fitness, const double domain_completion_fitness)
    {
        if(fitness >= domain_completion_fitness)
            _domain_winner = true;
        _fitness = fitness;
    }

    const std::optional<const double> get_fitness() const
    {
        return _fitness;
    }

    Genotype<G>& get_genotype() const
    {
        return *_genotype;
    }

    GPMap<G, T>& get_gp_map() const
    {
        return *_gp_map;
    }

    Phenotype<T>& get_phenotype() const
    {
        return *_phenotype;
    }

    bool is_domain_winner() const
    {
        return _domain_winner;
    }

    void set_domain_winner(const bool winner)
    {
        _domain_winner = winner;
    }

    //Creates new phenotype out of modified genotype
    void genesis()
    {
        _phenotype.reset(_gp_map->map(*_genotype));
    }

    //TODO: Remove
    void save_org_to_file(
        const std::string& file_name,
        const std::optional<std::vector<double>>& domain_hyperparams = std::nullopt)
            const
    {

        //Print fitness and then the genotype
        std::ofstream org_file;
        org_file.open(file_name);

        org_file << _fitness.value();

        org_file << std::setprecision(std::numeric_limits<double>::max_digits10);

        org_file << "," << *_genotype;

        //if(_gp_map)
        //    _gp_map.get()->print(org_file);

        //If domain hyperparams are given write them to a new line
        if(domain_hyperparams.has_value())
        {
            org_file << "\n";
            for(std::size_t i = 0; i < domain_hyperparams.value().size(); i++)
            {
                org_file << domain_hyperparams.value()[i];
                if(i != domain_hyperparams.value().size()-1)
                    org_file << ",";
            }
        }

        org_file.close();

    }

    JSON to_json() const
    {
        JSON json;
        if(_fitness.has_value())
            json.emplace("fitness", _fitness.value());
        json.emplace("domain_winner", _domain_winner);
        json.emplace("genes", _genotype->to_json().at({"genes"}));
        json.emplace("GPMap", _gp_map->to_json());
        json.emplace("Phenotype", _phenotype->to_json());
        return json;
    }

    friend std::ostream& operator<<(std::ostream& os, const Organism& organism)
    {
        //Just print genotype for now
        os << "Genotype: " << std::endl << "[" << *organism._genotype << "]"
            << std::endl
           << "Phenotype: " << std::endl << *organism._phenotype << std::endl
           << "Fitness: ";
        if(organism._fitness.has_value())
            os << organism._fitness.value();
        else
            os << "N/A";
        return os;
    }

private:

    std::unique_ptr<Genotype<G>> _genotype;
    std::unique_ptr<GPMap<G, T>> _gp_map;
    std::unique_ptr<Phenotype<T>> _phenotype;

    std::optional<double> _fitness;
    bool _domain_winner;

};

} // namespace NeuroEvo

#endif
