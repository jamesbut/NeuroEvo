#ifndef _MATRIX_MAP_H_
#define _MATRIX_MAP_H_

/*
    A matrix map transforms a genotype into a phenotype
    via a matrix multiplication of the genotype vector -
    this results in a phenotype vector.
    An identity interaction_matrix results in a one-to-one
    mapping.
*/

#include <gp_map/gp_map.h>
#include <util/maths/matrix.h>

namespace NeuroEvo {
namespace GPMaps {

class MatrixMap : public GPMap {

public:

    MatrixMap(const unsigned genotype_size, const unsigned phenotype_size);
    MatrixMap(const unsigned genotype_size, const unsigned phenotype_size,
              const std::string& file_name);

    Phenotypes::Phenotype* map(Genotypes::Genotype& genotype,
                               Phenotypes::PhenotypeSpec& pheno_spec) override;

    void print_gp_map(std::ofstream& file) override;

    inline Utils::Matrix<double>& get_map() override { return _interaction_matrix; };

protected:

    MatrixMap* clone_impl() const override { return new MatrixMap(*this); };

private:

    //Matrix defining the affect the the genome has on each of the phenotypic
    //traits
    Utils::Matrix<double> _interaction_matrix;

    std::vector<double> read_matrix(const std::string& file_name);

};

} // namespace GPMaps
} // namespace NeuroEvo

#endif
