#ifndef _CMAES_
#define _CMAES_

#include "genotype/genotype.h"
#include "population.h"
#include <optimiser/optimiser.h>
#include <util/maths/matrix.h>
#include <util/statistics/distributions/gaussian_distribution.h>

#include <Eigen/Dense>

namespace NeuroEvo {

template <typename T>
class CMAES : public Optimiser<double, T>
{

public:

    CMAES(const std::vector<double>& init_mean,
          double init_sigma,
          GPMap<double, T>& gp_map,
          const unsigned num_genes,
          const unsigned max_gens,
          const unsigned pop_size,
          const unsigned num_trials = 1) :
        Optimiser<double, T>(gp_map, num_genes, max_gens, pop_size, num_trials),
        _mean(Eigen::VectorXd::Zero(this->_num_genes)),
        _sigma(init_sigma),
        _C(Eigen::MatrixXd::Identity(this->_num_genes, this->_num_genes)),
        _mu(pop_size/2),
        _weights(calculate_weights()),
        _gauss_distr(0, 1) 
    {

        if(init_mean.size() != this->_num_genes)
        {
            std::cerr << "Initial mean vector to CMAES is not the same size as number of genes"
                << std::endl;
            exit(0);
        }
        for(std::size_t i = 0; i < init_mean.size(); i++)
            _mean(i) = init_mean[i];

    }

    Population<double, T> step() override
    {

        //Sort population 
        std::vector<std::size_t> sorted_pop_indices(this->_pop_size);
        std::iota(sorted_pop_indices.begin(), sorted_pop_indices.end(), 0);
        std::sort(sorted_pop_indices.begin(), sorted_pop_indices.end(),
                  [this](std::size_t i, std::size_t j) 
                  {
                      auto fitnesses = this->_population.get_fitnesses();
                      return fitnesses[i] > fitnesses[j];
                  });
        //std::cout << this->_population << std::endl;
        
        //Get genotypes into eigen matrix

        //Compute new mean
        _mean = Eigen::VectorXd::Zero(this->_num_genes);
        const auto& orgs = this->_population.get_organisms();
        for(unsigned i = 0; i <_mu; i++)
        {
            const auto& genes = orgs[sorted_pop_indices[i]].get_genotype().genes();
            Eigen::VectorXd elite_genes = Eigen::VectorXd::Zero(this->_num_genes);
            for(std::size_t i = 0; i < genes.size(); i++)
                elite_genes(i) = genes[i];
            _mean += elite_genes * _weights[i];
        }
        
        //std::cout << "Weights: " << std::endl << _weights << std::endl;
        std::cout << "Mean: " << std::endl << _mean << std::endl;

        return sample_population();

    }

private: 

    Population<double, T> initialise_population() override
    {
        auto samp_pop = sample_population(); 
        return samp_pop;
    }

    Population<double, T> sample_population() 
    {

        std::vector<Genotype<double>> genotypes;
        genotypes.reserve(this->_pop_size);

        //Spectral decomposition of covariance matrix _C in order to sample from the
        //multivariate normal distribution
        _es.compute(_C);
        if(_es.info() == Eigen::Success)
        {

            Eigen::MatrixXd B = _es.eigenvectors();
            Eigen::MatrixXd D = _es.eigenvalues().asDiagonal();
            for(unsigned i = 0; i < this->_num_genes; i++)
                D(i, i) = std::sqrt(std::max(1e-20, D(i, i)));

            for(unsigned i = 0; i < this->_pop_size; i++)
            {
                Eigen::VectorXd norm_rands = Eigen::VectorXd::Zero(this->_num_genes);
                for(unsigned j = 0; j < this->_num_genes; j++)
                    norm_rands(j) = _gauss_distr.next();

                //Sample from multivariate normal
                Eigen::VectorXd genes = _mean + (_sigma * B * D * norm_rands);

                std::vector<double> genes_vec(genes.data(), 
                                              genes.data() + genes.rows() * genes.cols());
                genotypes.push_back(Genotype<double>(genes_vec));
            }

        } else
        {
            std::cerr << "Eigen decomposition of _C failed!" << std::endl;
            exit(0);
        }

        return Population<double, T>(genotypes, this->_gp_map);
    }

    Eigen::VectorXd calculate_weights() const
    {
        Eigen::VectorXd weights = Eigen::VectorXd::Zero(_mu);

        //All weights are equal
        for(unsigned i = 0; i < _mu; i++)
            weights(i) = 1 / (double)_mu;

        /*
        //Weight creation procedure in pagmo
        for(unsigned i = 0; i < _mu; i++)
            weights(i) = std::log((double)_mu + 0.5) - std::log((double)i + 1);
        weights /= weights.sum();
        */

        return weights;

    }

    CMAES* clone_impl() const override
    {
        return new CMAES(*this);
    }

    void reset() override {}

    Eigen::VectorXd _mean;
    double _sigma;
    Eigen::MatrixXd _C;
    //Number of parents used to calculate statistics for next gen
    unsigned _mu;
    Eigen::VectorXd _weights;

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> _es;

    GaussianDistribution _gauss_distr;

};

} // namespace NeuroEvo

#endif
