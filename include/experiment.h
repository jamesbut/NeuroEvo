#ifndef _EXPERIMENT_H_
#define _EXPERIMENT_H_

/*
    An experiment runs either an indivdual run or an evolutionary run according to a
    specification.
*/

#include <population.h>
#include <domains/domain.h>

namespace NeuroEvo {

class Experiment {

public:

    //Constructs an experiment conditional on whether the phenotype specification is appropriate
    //for the domain
    static std::optional<Experiment> 
        construct(Domains::Domain& domain,
                  Genotypes::GenotypeSpec& geno_spec,
                  Phenotypes::PhenotypeSpec& pheno_spec,
                  GPMaps::GPMapSpec* gp_map_spec = nullptr);

    void individual_run(const std::string& organism_folder_name);

    void evolutionary_run(const unsigned POP_SIZE,
                          const unsigned MAX_GENS,
                          Mutators::Mutation& mutator,
                          Selectors::Selection& selector,
                          const bool PARALLEL = false,
                          const unsigned NUM_RUNS = 1,
                          const unsigned NUM_TRIALS = 1);

private:

    //Cannot construct experiment because it is dependent on valid specifications
    Experiment(Domains::Domain& domain,
               Genotypes::GenotypeSpec& geno_spec,
               Phenotypes::PhenotypeSpec& pheno_spec,
               GPMaps::GPMapSpec* gp_map_spec);

    int ga_finished(Population& population, const unsigned MAX_GENS);

    //In most cases the domain, genotype spec and phenotype spec will be the same for an 
    //evolutionary run and an individual run so they are saved as member variables and taken as 
    //constructor arguments
    Domains::Domain& _domain;
    Genotypes::GenotypeSpec& _geno_spec;
    Phenotypes::PhenotypeSpec& _pheno_spec;
    GPMaps::GPMapSpec* _gp_map_spec;

};

} // namespace NeuroEvo

#endif
