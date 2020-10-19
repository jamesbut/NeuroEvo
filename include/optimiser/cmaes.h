#ifndef _CMAES_
#define _CMAES_

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
          const unsigned num_trials = 1,
          const bool adapt_C = true,
          const std::optional<unsigned>& seed = std::nullopt) :
        Optimiser<double, T>(gp_map, num_genes, max_gens, pop_size, num_trials, seed),
        _mean(Eigen::VectorXd::Zero(this->_num_genes)),
        _mean_old(Eigen::VectorXd::Zero(this->_num_genes)),
        _sigma(init_sigma),
        _B(Eigen::MatrixXd::Identity(this->_num_genes, this->_num_genes)),
        _D(Eigen::MatrixXd::Identity(this->_num_genes, this->_num_genes)),
        _invD(Eigen::MatrixXd::Identity(this->_num_genes, this->_num_genes)),
        _C(_D * _D),
        _C_old(Eigen::MatrixXd::Identity(this->_num_genes, this->_num_genes)),
        _invsqrtC(Eigen::MatrixXd::Identity(this->_num_genes, this->_num_genes)),
        _p_c(Eigen::VectorXd::Zero(this->_num_genes)),
        _p_sigma(Eigen::VectorXd::Zero(this->_num_genes)),
        _count_eval(0),
        _eigen_eval(0),
        _gauss_distr(0, 1),
        _adapt_C(adapt_C)
    {

        if(init_mean.size() != this->_num_genes)
        {
            std::cerr << "Initial mean vector to CMAES is not the same size as number of genes"
                << std::endl;
            exit(0);
        }
        //Initialise mean
        for(std::size_t i = 0; i < init_mean.size(); i++)
            _mean(i) = init_mean[i];

        for(unsigned i = 0; i < this->_num_genes; i++)
        {
            _invsqrtC(i, i) = 1. / _D(i, i);
            _invD(i, i) = 1. / _D(i, i);
        }

        //Initialise constants
        _mu = pop_size / 2;
        _weights = calculate_weights();
        _mu_eff = 1. / (_weights.transpose() * _weights);

        _N = static_cast<double>(this->_num_genes);
        _lambda = static_cast<double>(this->_pop_size);

        _c_c = (4. + _mu_eff / _N) / (_N + 4. + 2. * _mu_eff / _N);
        _c_sig = (_mu_eff + 2.) / (_N + _mu_eff + 5.);
        const double alpha_cov = 2.;
        _c_1 = alpha_cov / ((_N + 1.3) * (_N + 1.3) + _mu_eff);
        _c_mu = std::min(1 - _c_1,
                         alpha_cov * (_mu_eff -2. + 1. / _mu_eff) / 
                                     ((_N + 2.) * (_N + 2.) + alpha_cov * _mu_eff / 2.));  

        _d_sig = 1. + 2. * std::max(0., std::sqrt((_mu_eff - 1.) / (_N + 1.)) - 1.) + _c_sig; 
        //Expectation of ||N(0,I)|| == norm(randn(N,1))
        _chiN = std::sqrt(_N) * (1. - 1. / (4. * _N) + 1. / (21. * _N * _N)); 

    }

    Population<double, T> step() override
    {

        _count_eval += this->_pop_size;

        //Sort population 
        std::vector<std::size_t> sorted_pop_indices(this->_pop_size);
        std::iota(sorted_pop_indices.begin(), sorted_pop_indices.end(), 0);
        std::sort(sorted_pop_indices.begin(), sorted_pop_indices.end(),
                  [this](std::size_t i, std::size_t j) 
                  {
                      auto fitnesses = this->_population.get_fitnesses();
                      return fitnesses[i] > fitnesses[j];
                  });
        
        //Place elite genomes into Eigen vector
        std::vector<Eigen::VectorXd> elites(_mu, Eigen::VectorXd::Zero(this->_num_genes));
        const auto& orgs = this->_population.get_organisms();
        for(unsigned i = 0; i <_mu; i++)
        {
            const auto& genes = orgs[sorted_pop_indices[i]].get_genotype().genes();
            for(std::size_t j = 0; j < this->_num_genes; j++)
                elites[i](j) = genes[j];
        }

        //Compute new mean
        _mean_old = _mean;
        _mean = elites[0] * _weights(0);
        for(unsigned i = 1; i < _mu; i++)
            _mean += elites[i] * _weights(i);
        
        //Update sigma evolutionary path
        _p_sigma = (1. - _c_sig) * _p_sigma + std::sqrt(_c_sig * (2. - _c_sig) * _mu_eff) *
                   _invsqrtC * (_mean - _mean_old) / _sigma;

        if(_adapt_C)
        {
            //Update C evolutionary path
            double h_sig = 0.;
            h_sig = (_p_sigma.squaredNorm() / _N / 
                    (1. - std::pow((1. - _c_sig), (2. * (double)_count_eval / _lambda))))
                    < (2. + 4. / (_N + 1.));
            _p_c = (1. - _c_c) * _p_c + h_sig * std::sqrt(_c_c * (2. - _c_c) * _mu_eff) 
                    * (_mean - _mean_old) / _sigma;
            
            //Compute new C
            _C_old = _C;
            _C = (elites[0] - _mean_old) * (elites[0] - _mean_old).transpose() * _weights(0);
            for(unsigned i = 1; i < _mu; i++)
                _C += (elites[i] - _mean_old) * 
                      (elites[i] - _mean_old).transpose() * _weights(i);
            _C /= _sigma * _sigma;
            _C = (1. - _c_1 - _c_mu) * _C_old + _c_mu * _C + 
                _c_1 * ((_p_c * _p_c.transpose()) + (1. - h_sig) * _c_c * (2. - _c_c) * _C_old);

        }
        
        //Compute new sigma
        _sigma *= std::exp(std::min(0.6, (_c_sig / _d_sig) * (_p_sigma.norm() / _chiN - 1.)));

        //std::cout << "Mean: " << std::endl << _mean << std::endl;
        //std::cout << "C: " << std::endl << _C << std::endl;
        //std::cout << "invsqrtC: " << std::endl << _invsqrtC << std::endl;
        //std::cout << "sigma: " << std::endl << _sigma << std::endl;

        if(_adapt_C)
            perform_eigendecompostion();

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

        for(unsigned i = 0; i < this->_pop_size; i++)
        {
            Eigen::VectorXd norm_rands = Eigen::VectorXd::Zero(this->_num_genes);
            for(unsigned j = 0; j < this->_num_genes; j++)
                norm_rands(j) = _gauss_distr.next();

            //Sample from multivariate normal
            Eigen::VectorXd genes;
            if(_adapt_C)
                genes = _mean + (_sigma * _B * _D * norm_rands);
            else
                genes = _mean + (_sigma * norm_rands);

            std::vector<double> genes_vec(genes.data(), 
                                          genes.data() + genes.rows() * genes.cols());
            genotypes.push_back(Genotype<double>(genes_vec));
        }

        return Population<double, T>(genotypes, this->_gp_map);
    }

    void perform_eigendecompostion() {

        //Enforce O(N^2)
        if(static_cast<double>(_count_eval - _eigen_eval) > 
                               (_lambda / (_c_1 + _c_mu) / _N / 10.))
        {
            _eigen_eval = _count_eval;

            //Spectral decomposition of covariance matrix _C in order to sample from the
            //multivariate normal distribution
            _C = (_C + _C.transpose()) / 2.;   //Enforce symmetry
            _es.compute(_C);
            if(_es.info() == Eigen::Success)
            {

                _B = _es.eigenvectors();
                _D = _es.eigenvalues().asDiagonal();
                for(unsigned i = 0; i < this->_num_genes; i++)
                    _D(i, i) = std::sqrt(std::max(1e-20, _D(i, i)));

                //Calculate some aditional matrices for use later
                for(unsigned i = 0; i < this->_num_genes; i++)
                    _invD(i, i) = 1. / _D(i, i);
                _invsqrtC = _B * _invD * _B.transpose();

            } 
            /*
            else {
                std::cerr << "Eigen decomposition of _C failed!" << std::endl;
                exit(0);
            }
            */
        } 

    }

    Eigen::VectorXd calculate_weights() const
    {
        Eigen::VectorXd weights = Eigen::VectorXd::Zero(_mu);

        /*
        //All weights are equal
        for(unsigned i = 0; i < _mu; i++)
            weights(i) = 1 / (double)_mu;
        */

        //Weight creation procedure in pagmo
        for(unsigned i = 0; i < _mu; i++)
            weights(i) = std::log((double)_mu + 0.5) - std::log((double)i + 1);
        weights /= weights.sum();

        return weights;

    }


    CMAES* clone_impl() const override
    {
        return new CMAES(*this);
    }

    void reset() override 
    {
        if(this->_seed.has_value())
            _gauss_distr.set_seed(this->_seed.value());
        else
            _gauss_distr.randomly_seed();
    }

    Eigen::VectorXd _mean;
    Eigen::VectorXd _mean_old;
    double _sigma;
    Eigen::MatrixXd _B;
    Eigen::MatrixXd _D;
    Eigen::MatrixXd _invD;
    Eigen::MatrixXd _C;
    Eigen::MatrixXd _C_old;
    Eigen::MatrixXd _invsqrtC;
    
    double _N;
    double _lambda;

    unsigned _mu;
    Eigen::VectorXd _weights;

    double _mu_eff;
    double _c_mu;
    double _c_c;
    double _c_sig;
    double _c_1;
    double _d_sig;
    double _chiN;

    Eigen::VectorXd _p_c;
    Eigen::VectorXd _p_sigma;

    unsigned _count_eval;
    unsigned _eigen_eval;

    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> _es;

    GaussianDistribution _gauss_distr;

    const bool _adapt_C;

};

} // namespace NeuroEvo

#endif
