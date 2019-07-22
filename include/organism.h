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
#include <gp_map/gp_map.h>

class Organism {

public:

    Organism(PhenotypeSpec& phenotype_spec);
    Organism(PhenotypeSpec& phenotype_spec, Genotype& genotype, GPMap* gp_map);
    Organism(PhenotypeSpec& phenotype_spec, const std::string file_name);

    Organism(const Organism& organism);
    Organism& operator=(const Organism& organism);
    ~Organism() = default;
    Organism(Organism&& organism) = default;
    Organism& operator=(Organism&& organism) = default;


    void set_fitness(double fitness);
    const double get_fitness() const;

    Genotype& get_genotype() const;
    GPMap* get_gp_map() const;
    Phenotype& get_phenotype() const;

    PhenotypeSpec& get_phenotype_spec() const;

    void genesis();

    void print_org(std::string file_name);

private:

    //Use raw pointer here because the phenotype spec
    //is created on the stack in main and I don't want
    //that to be deleted when the Organism gets deleted.
    //Maybe something cleaner can be done in future.
    PhenotypeSpec* _phenotype_spec;

    std::unique_ptr<Genotype> _genotype;
    std::unique_ptr<GPMap> _gp_map;
    std::unique_ptr<Phenotype> _phenotype;

    double _fitness;

};

#endif
