#include <organism.h>
#include <iostream>
#include <fstream>

// The member variables are created by calling their specific generate
// functions from the PhenotypeSpec (the PhenotypeSpec knows how to create
// them)
Organism::Organism(PhenotypeSpec& phenotype_spec) :
    _genotype(phenotype_spec.generate_genotype()),
    _gp_map(phenotype_spec.generate_gp_map()),
    _phenotype(phenotype_spec.generate_phenotype(*_genotype, _gp_map.get())),
    _phenotype_spec(&phenotype_spec),
    _fitness(0.0) {}

Organism::Organism(PhenotypeSpec& phenotype_spec, Genotype& genotype, GPMap* gp_map) :
    _genotype(genotype.clone()),
    _gp_map(gp_map ? gp_map->clone() : nullptr),
    _phenotype(phenotype_spec.generate_phenotype(*_genotype, _gp_map.get())),
    _phenotype_spec(&phenotype_spec),
    _fitness(0.0) {}

Organism::Organism(PhenotypeSpec& phenotype_spec, const std::string file_name) :
    _genotype(phenotype_spec.generate_genotype(file_name)),
    _gp_map(phenotype_spec.generate_gp_map(file_name)),
    _phenotype(phenotype_spec.generate_phenotype(*_genotype, _gp_map.get())),
    _phenotype_spec(&phenotype_spec),
    _fitness(0.0) {}

Organism::Organism(const Organism& organism) :
    _genotype(organism.get_genotype().clone()),
    _gp_map(organism.get_gp_map() ? organism.get_gp_map()->clone() : nullptr),
    _phenotype(organism.get_phenotype().clone()),
    _phenotype_spec(&organism.get_phenotype_spec()),
    _fitness(organism.get_fitness()) {}

Organism& Organism::operator=(const Organism& organism) {

    _genotype = organism.get_genotype().clone();
    _gp_map = organism.get_gp_map() ? organism.get_gp_map()->clone() : nullptr;
    _phenotype = organism.get_phenotype().clone();
    _phenotype_spec = &organism.get_phenotype_spec();
    _fitness = organism.get_fitness();

    return *this;

}

//Creates new phenotype out of newly modified genotype
void Organism::genesis() {

    _phenotype.reset(_phenotype_spec->generate_phenotype(*_genotype, _gp_map.get()));

}

void Organism::print_org(std::string file_name) {

    //Print fitness and then the genotype
    std::ofstream org_file;
    org_file.open(file_name);

    org_file << _fitness;

    _genotype.get()->print_genotype(org_file);

    if(_gp_map)
        _gp_map.get()->print_gp_map(org_file);

    org_file.close();

}

void Organism::set_fitness(double fitness) {

    _fitness = fitness;

}

const double Organism::get_fitness() const {

    return _fitness;

}

Genotype& Organism::get_genotype() const {

    return *_genotype;

}

GPMap* Organism::get_gp_map() const {

    return _gp_map.get();

}

Phenotype& Organism::get_phenotype() const {

    return *_phenotype;

}

PhenotypeSpec& Organism::get_phenotype_spec() const {

    return *_phenotype_spec;

}
