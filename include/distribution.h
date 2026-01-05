#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include <random>

namespace tt_int {

/**
 * @brief Abstract base class for probability distributions
 * 
 * Distributions can be sampled to generate random values according to
 * their probability density function.
 */
class Distribution {
public:
    virtual ~Distribution() = default;
    
    /**
     * @brief Sample a value from this distribution
     * @param rng Random number generator to use for sampling
     * @return A random sample from the distribution
     */
    virtual double sample(std::mt19937& rng) const = 0;
};

/**
 * @brief Normal (Gaussian) distribution
 * 
 * Generates samples from a normal distribution with specified mean and
 * standard deviation.
 */
class NormalDistribution : public Distribution {
public:
    /**
     * @brief Construct a normal distribution
     * @param mean The mean (μ) of the distribution
     * @param stddev The standard deviation (σ) of the distribution
     */
    NormalDistribution(double mean, double stddev);
    
    double sample(std::mt19937& rng) const override;
    
    double getMean() const { return mean_; }
    double getStddev() const { return stddev_; }
    
private:
    double mean_;
    double stddev_;
    mutable std::normal_distribution<double> dist_;
};

/**
 * @brief Uniform distribution over a continuous range
 * 
 * Generates samples uniformly distributed between min and max (inclusive).
 */
class UniformDistribution : public Distribution {
public:
    /**
     * @brief Construct a uniform distribution
     * @param min The minimum value of the range (inclusive)
     * @param max The maximum value of the range (inclusive)
     */
    UniformDistribution(double min, double max);
    
    double sample(std::mt19937& rng) const override;
    
    double getMin() const { return min_; }
    double getMax() const { return max_; }
    
private:
    double min_;
    double max_;
    mutable std::uniform_real_distribution<double> dist_;
};

} // namespace tt_int

#endif // DISTRIBUTION_H
