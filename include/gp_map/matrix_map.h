#ifndef _MATRIX_MAP_H_
#define _MATRIX_MAP_H_

/*
    A matrix map transforms a genotype into a phenotype
    via a matrix multiplication of the genotype vector -
    this results in a phenotype vector.
    An identity interaction_matrix results in a one-to-one
    mapping.
    For now a matrix map is only defined for doubles but
    this can be changed to be generic in the future.
    Also it is only defined for network phenotypes for now.
*/

#include <gp_map/gp_map.h>
#include <util/maths/matrix.h>

namespace NeuroEvo {

class MatrixMap : public GPMap<double, double> 
{

public:

    MatrixMap(const unsigned genotype_size, const unsigned phenotype_size,
              const PhenotypeSpec* pheno_spec);
    MatrixMap(const std::string& file_name, const PhenotypeSpec* pheno_spec);

    Phenotype<double>* map(Genotype<double>& genotype) override;

    void print_gp_map(std::ofstream& file) const override;

protected:

    MatrixMap* clone_impl() const override 
    { 
        return new MatrixMap(*this); 
    };

private:

    //Matrix defining the affect the the genome has on each of the phenotypic
    //traits
    Matrix<double> _interaction_matrix;

    Matrix<double> read_matrix(const std::string& file_name);

};

} // namespace NeuroEvo

#endif
