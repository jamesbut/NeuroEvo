#ifndef _ORGANISM_H_
#define _ORGANISM_H_

/*
    This class contains everything that constitutes
    an Organism, including its genotype, phenotype
    and genotype-phenotype map.
*/

#include <genotype/genotype.h>
#include <phenotype/phenotype_specs/phenotype_spec.h>
#include <genotype/genotype_specs/genotype_spec.h>
#include <memory>
#include <iostream>
#include <gp_map/gp_map.h>
#include <gp_map/gp_map_specs/gp_map_spec.h>

namespace NeuroEvo {

class Organism {

public:

    Organism(Genotypes::GenotypeSpec& genotype_spec, Phenotypes::PhenotypeSpec& phenotype_spec,
             GPMaps::GPMapSpec* gp_map_spec);
    Organism(Phenotypes::PhenotypeSpec& phenotype_spec, Genotypes::Genotype& genotype,
             GPMaps::GPMapSpec* gp_map_spec);
    Organism(Genotypes::GenotypeSpec& genotype_spec, Phenotypes::PhenotypeSpec& phenotype_spec,
             GPMaps::GPMapSpec* gp_map_spec, const std::string file_name);

    Organism(const Organism& organism);
    Organism& operator=(const Organism& organism);
    ~Organism() = default;
    Organism(Organism&& organism) = default;
    Organism& operator=(Organism&& organism) = default;


    void set_fitness(double fitness);
    const double get_fitness() const;

    Genotypes::Genotype& get_genotype() const;
    GPMaps::GPMap* get_gp_map() const;
    Phenotypes::Phenotype& get_phenotype() const;

    Phenotypes::PhenotypeSpec& get_phenotype_spec() const;
    GPMaps::GPMapSpec* get_gp_map_spec() const;

    void genesis();

    void print_org(std::string file_name);

private:

    //Use raw pointer here because the phenotype spec
    //is created on the stack in main and I don't want
    //that to be deleted when the Organism gets deleted.
    //Maybe something cleaner can be done in future.
    Phenotypes::PhenotypeSpec* _phenotype_spec;
    GPMaps::GPMapSpec* _gp_map_spec;

    std::unique_ptr<Genotypes::Genotype> _genotype;
    std::unique_ptr<GPMaps::GPMap> _gp_map;
    std::unique_ptr<Phenotypes::Phenotype> _phenotype;

    double _fitness;

};

} // namespace NeuroEvo

#endif
