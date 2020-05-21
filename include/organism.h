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
#include <gp_map/gp_map_specs/gp_map_spec.h>

namespace NeuroEvo {

template <typename G>
class Organism {

public:

    Organism(GenotypeSpec<G>& genotype_spec, PhenotypeSpec<G>& phenotype_spec,
             GPMapSpec<G>* gp_map_spec) :
        _genotype(genotype_spec.generate_genotype()),
        _gp_map(gp_map_spec ? gp_map_spec->generate_gp_map() : nullptr),
        _phenotype(phenotype_spec.generate_phenotype(*_genotype, _gp_map.get())),
        _phenotype_spec(&phenotype_spec),
        _gp_map_spec(gp_map_spec),
        _fitness(0.0) {}

    Organism(PhenotypeSpec<G>& phenotype_spec, Genotype<G>& genotype,
             GPMapSpec<G>* gp_map_spec) :
        _genotype(genotype.clone()),
        _gp_map(gp_map_spec ? gp_map_spec->generate_gp_map() : nullptr),
        _phenotype(phenotype_spec.generate_phenotype(*_genotype, _gp_map.get())),
        _phenotype_spec(&phenotype_spec),
        _gp_map_spec(gp_map_spec),
        _fitness(0.0) {}
        
    Organism(GenotypeSpec<G>& genotype_spec, PhenotypeSpec<G>& phenotype_spec,
             GPMapSpec<G>* gp_map_spec, const std::string file_name) :
        _genotype(genotype_spec.generate_genotype(file_name)),
        _gp_map(gp_map_spec ? gp_map_spec->generate_gp_map(file_name) : nullptr),
        _phenotype(phenotype_spec.generate_phenotype(*_genotype, _gp_map.get())),
        _phenotype_spec(&phenotype_spec),
        _gp_map_spec(gp_map_spec),
        _fitness(0.0) {}

    Organism(const Organism& organism) :
        _genotype(organism.get_genotype().clone()),
        _gp_map(organism.get_gp_map() ? organism.get_gp_map()->clone() : nullptr),
        _phenotype(organism.get_phenotype().clone()),
        _phenotype_spec(&organism.get_phenotype_spec()),
        _gp_map_spec(organism.get_gp_map_spec()),
        _fitness(organism.get_fitness()) {}

    Organism& operator=(const Organism& organism) 
    {

        _genotype = organism.get_genotype().clone();
        _gp_map = organism.get_gp_map() ? organism.get_gp_map()->clone() : nullptr;
        _phenotype = organism.get_phenotype().clone();
        _phenotype_spec = &organism.get_phenotype_spec();
        _gp_map_spec = organism.get_gp_map_spec();
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

    const double get_fitness() const 
    {
        return _fitness;
    }

    Genotype<G>& get_genotype() const 
    {
        return *_genotype;
    }

    GPMap<G>* get_gp_map() const 
    {
        return _gp_map.get();
    }

    Phenotype& get_phenotype() const 
    {
        return *_phenotype;
    }

    PhenotypeSpec<G>& get_phenotype_spec() const 
    {
        return *_phenotype_spec;
    }

    GPMapSpec<G>* get_gp_map_spec() const 
    {
        return _gp_map_spec;
    }

    //Creates new phenotype out of modified genotype
    void genesis() 
    {
        _phenotype.reset(_phenotype_spec->generate_phenotype(*_genotype, _gp_map.get()));
    }

    void print_org(const std::string& file_name) 
    {
        
        //Print fitness and then the genotype
        std::ofstream org_file;
        org_file.open(file_name);

        org_file << _fitness;

        _genotype.get()->print_genotype(org_file);

        if(_gp_map)
            _gp_map.get()->print_gp_map(org_file);

        org_file.close();

    }

private:

    //Use raw pointer here because the phenotype spec
    //is created on the stack in main and I don't want
    //that to be deleted when the Organism gets deleted.
    //Maybe something cleaner can be done in future.
    PhenotypeSpec<G>* _phenotype_spec;
    GPMapSpec<G>* _gp_map_spec;

    std::unique_ptr<Genotype<G>> _genotype;
    std::unique_ptr<GPMap<G>> _gp_map;
    std::unique_ptr<Phenotype> _phenotype;

    double _fitness;

};

} // namespace NeuroEvo

#endif
