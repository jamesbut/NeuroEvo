#ifndef _ORGANISM_H_
#define _ORGANISM_H_

/*
    This class contains everything that constitutes
    an Organism, including its genotype, phenotype
    and genotype-phenotype map.
*/

#include <genotype/genotype.h>
#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <genotype/genotype_spec.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <gp_map/gp_map.h>

namespace NeuroEvo {

template <typename G, typename T>
class Organism {

public:

    Organism(GenotypeSpec<G>& genotype_spec, PhenotypeSpec<G, T>& phenotype_spec,
             GPMap<G, T>& gp_map) :
        _phenotype_spec(&phenotype_spec),
        _genotype(genotype_spec.generate_genotype()),
        _gp_map(gp_map.clone()),
        _phenotype(gp_map.map(*_genotype, phenotype_spec)),
        _fitness(0.0) {}

    Organism(Genotype<G>& genotype, PhenotypeSpec<G, T>& phenotype_spec, 
             GPMap<G, T>& gp_map) :
        _phenotype_spec(&phenotype_spec),
        _genotype(genotype.clone()),
        _gp_map(gp_map.clone()),
        _phenotype(gp_map.map(*_genotype, phenotype_spec)),
        _fitness(0.0) {}
        
    Organism(GenotypeSpec<G>& genotype_spec, PhenotypeSpec<G, T>& phenotype_spec,
             GPMap<G, T>& gp_map, const std::string file_name) :
        _phenotype_spec(&phenotype_spec),
        _genotype(genotype_spec.generate_genotype(file_name)),
        _gp_map(gp_map.clone()),
        _phenotype(gp_map.map(*_genotype, phenotype_spec)),
        _fitness(0.0) {}

    Organism(const Organism& organism) :
        _phenotype_spec(&organism.get_phenotype_spec()),
        _genotype(organism.get_genotype().clone()),
        _gp_map(organism._gp_map->clone()),
        _phenotype(organism.get_phenotype().clone_phenotype()),
        _fitness(organism.get_fitness()) {}

    Organism& operator=(const Organism& organism) 
    {

        _genotype = organism.get_genotype().clone();
        _gp_map = organism._gp_map->clone();
        _phenotype = organism.get_phenotype().clone_phenotype();
        _phenotype_spec = &organism.get_phenotype_spec();
        _fitness = organism.get_fitness();

        return *this;

    }

    ~Organism() = default;
    Organism(Organism&& organism) = default;
    Organism& operator=(Organism&& organism) = default;


    void set_fitness(const double fitness) 
    {
        _fitness = fitness;
    }

    double get_fitness() const 
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

    PhenotypeSpec<G, T>& get_phenotype_spec() const 
    {
        return *_phenotype_spec;
    }

    //Creates new phenotype out of modified genotype
    void genesis() 
    {
        _phenotype.reset(_gp_map->map(*_genotype, *_phenotype_spec));
    }

    void save_org_to_file(const std::string& file_name) const
    {
        
        //Print fitness and then the genotype
        std::ofstream org_file;
        org_file.open(file_name);

        org_file << _fitness;

        org_file << std::setprecision(std::numeric_limits<double>::max_digits10);

        org_file << "," << *_genotype;

        if(_gp_map)
            _gp_map.get()->print_gp_map(org_file);

        org_file.close();

    }

    friend std::ostream& operator<<(std::ostream& os, const Organism organism)
    {
        //Just print genotype for now
        os << "[" << *organism._genotype << "]" << " Fitness: " << organism._fitness;
        return os;
    }

private:

    //Use raw pointer here because the phenotype spec
    //is created on the stack in main and I don't want
    //that to be deleted when the Organism gets deleted.
    //Maybe something cleaner can be done in future.
    PhenotypeSpec<G, T>* _phenotype_spec;

    std::unique_ptr<Genotype<G>> _genotype;
    std::unique_ptr<GPMap<G, T>> _gp_map;
    std::unique_ptr<Phenotype<T>> _phenotype;

    double _fitness;

};

} // namespace NeuroEvo

#endif
