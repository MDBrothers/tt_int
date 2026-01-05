#include <gtest/gtest.h>
#include <numeric>
#include <cmath>
#include <algorithm>
#include "distribution.h"
#include "variable_registry.h"

using namespace tt_int;

// Helper function to calculate mean
double calculateMean(const std::vector<double>& values) {
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

// Helper function to calculate standard deviation
double calculateStdDev(const std::vector<double>& values, double mean) {
    double variance = 0.0;
    for (double val : values) {
        variance += (val - mean) * (val - mean);
    }
    variance /= values.size();
    return std::sqrt(variance);
}

// Test NormalDistribution construction
TEST(DistributionTest, NormalConstruction) {
    NormalDistribution dist(0.0, 1.0);
    EXPECT_EQ(dist.getMean(), 0.0);
    EXPECT_EQ(dist.getStddev(), 1.0);
}

// Test UniformDistribution construction
TEST(DistributionTest, UniformConstruction) {
    UniformDistribution dist(0.0, 1.0);
    EXPECT_EQ(dist.getMin(), 0.0);
    EXPECT_EQ(dist.getMax(), 1.0);
}

// Test NormalDistribution sampling returns doubles
TEST(DistributionTest, NormalSampling) {
    std::mt19937 rng(42);
    NormalDistribution dist(0.0, 1.0);
    
    double sample = dist.sample(rng);
    EXPECT_TRUE(std::isfinite(sample));
}

// Test NormalDistribution produces different values
TEST(DistributionTest, NormalDifferentValues) {
    std::mt19937 rng(42);
    NormalDistribution dist(0.0, 1.0);
    
    std::vector<double> samples;
    for (int i = 0; i < 10; ++i) {
        samples.push_back(dist.sample(rng));
    }
    
    // Check that not all samples are identical (statistical test, very high probability)
    double first = samples[0];
    bool hasVariation = false;
    for (size_t i = 1; i < samples.size(); ++i) {
        if (samples[i] != first) {
            hasVariation = true;
            break;
        }
    }
    EXPECT_TRUE(hasVariation);
}

// Test UniformDistribution samples in range
TEST(DistributionTest, UniformRange) {
    std::mt19937 rng(42);
    UniformDistribution dist(0.0, 1.0);
    
    for (int i = 0; i < 10; ++i) {
        double sample = dist.sample(rng);
        EXPECT_GE(sample, 0.0);
        EXPECT_LE(sample, 1.0);
    }
}

// Test deterministic seeding
TEST(DistributionTest, DeterministicSeeding) {
    // Use separate distribution objects to avoid internal state issues
    NormalDistribution dist1(0.0, 1.0);
    NormalDistribution dist2(0.0, 1.0);
    
    std::mt19937 rng1(42);
    std::vector<double> samples1;
    for (int i = 0; i < 5; ++i) {
        samples1.push_back(dist1.sample(rng1));
    }
    
    std::mt19937 rng2(42);
    std::vector<double> samples2;
    for (int i = 0; i < 5; ++i) {
        samples2.push_back(dist2.sample(rng2));
    }
    
    EXPECT_EQ(samples1, samples2);
}

// Test NormalDistribution mean with large sample size
TEST(DistributionTest, NormalMean) {
    std::mt19937 rng(42);
    NormalDistribution dist(100.0, 15.0);
    
    std::vector<double> samples(10000);
    for (auto& s : samples) {
        s = dist.sample(rng);
    }
    
    double mean = calculateMean(samples);
    EXPECT_NEAR(mean, 100.0, 0.5); // Within ±0.5 of expected
}

// Test NormalDistribution standard deviation with large sample size
TEST(DistributionTest, NormalStdDev) {
    std::mt19937 rng(42);
    NormalDistribution dist(100.0, 15.0);
    
    std::vector<double> samples(10000);
    for (auto& s : samples) {
        s = dist.sample(rng);
    }
    
    double mean = calculateMean(samples);
    double stddev = calculateStdDev(samples, mean);
    EXPECT_NEAR(stddev, 15.0, 0.5); // Within ±0.5 of expected
}

// Test UniformDistribution mean
TEST(DistributionTest, UniformMean) {
    std::mt19937 rng(42);
    UniformDistribution dist(0.0, 10.0);
    
    std::vector<double> samples(10000);
    for (auto& s : samples) {
        s = dist.sample(rng);
    }
    
    double mean = calculateMean(samples);
    EXPECT_NEAR(mean, 5.0, 0.05); // (min+max)/2 = 5.0
}

// Test UniformDistribution all samples in range
TEST(DistributionTest, UniformRangeLarge) {
    std::mt19937 rng(42);
    UniformDistribution dist(-5.0, 5.0);
    
    std::vector<double> samples(10000);
    for (auto& s : samples) {
        s = dist.sample(rng);
    }
    
    auto minmax = std::minmax_element(samples.begin(), samples.end());
    EXPECT_GE(*minmax.first, -5.0);
    EXPECT_LE(*minmax.second, 5.0);
}

// Test VariableRegistry registration
TEST(VariableRegistryTest, RegisterVariable) {
    VariableRegistry registry;
    auto dist = std::make_shared<NormalDistribution>(0.0, 1.0);
    
    registry.registerVariable("x", dist);
    EXPECT_TRUE(registry.hasVariable("x"));
    EXPECT_EQ(registry.getVariableCount(), 1);
}

// Test VariableRegistry hasVariable for unregistered
TEST(VariableRegistryTest, HasVariableUnregistered) {
    VariableRegistry registry;
    EXPECT_FALSE(registry.hasVariable("y"));
}

// Test VariableRegistry sampleAll returns correct size
TEST(VariableRegistryTest, SampleAllSize) {
    std::mt19937 rng(42);
    VariableRegistry registry;
    
    registry.registerVariable("x", std::make_shared<NormalDistribution>(0.0, 1.0));
    registry.registerVariable("y", std::make_shared<UniformDistribution>(0.0, 1.0));
    registry.registerVariable("z", std::make_shared<NormalDistribution>(5.0, 2.0));
    
    auto samples = registry.sampleAll(rng);
    EXPECT_EQ(samples.size(), 3);
}

// Test VariableRegistry sampleAll returns valid values
TEST(VariableRegistryTest, SampleAllValues) {
    std::mt19937 rng(42);
    VariableRegistry registry;
    
    registry.registerVariable("x", std::make_shared<NormalDistribution>(0.0, 1.0));
    registry.registerVariable("y", std::make_shared<UniformDistribution>(0.0, 1.0));
    
    auto samples = registry.sampleAll(rng);
    
    EXPECT_TRUE(samples.find("x") != samples.end());
    EXPECT_TRUE(samples.find("y") != samples.end());
    EXPECT_TRUE(std::isfinite(samples["x"]));
    EXPECT_TRUE(std::isfinite(samples["y"]));
    EXPECT_GE(samples["y"], 0.0);
    EXPECT_LE(samples["y"], 1.0);
}

// Test VariableRegistry multiple samples produce different values
TEST(VariableRegistryTest, MultipleVariables) {
    std::mt19937 rng(42);
    VariableRegistry registry;
    
    registry.registerVariable("a", std::make_shared<NormalDistribution>(10.0, 2.0));
    registry.registerVariable("b", std::make_shared<NormalDistribution>(20.0, 3.0));
    registry.registerVariable("c", std::make_shared<UniformDistribution>(0.0, 5.0));
    
    auto samples1 = registry.sampleAll(rng);
    auto samples2 = registry.sampleAll(rng);
    
    // Samples should be different (probabilistically)
    EXPECT_NE(samples1["a"], samples2["a"]);
    EXPECT_NE(samples1["b"], samples2["b"]);
    EXPECT_NE(samples1["c"], samples2["c"]);
}

// Test VariableRegistry re-registration replaces distribution
TEST(VariableRegistryTest, ReRegistration) {
    std::mt19937 rng(42);
    VariableRegistry registry;
    
    registry.registerVariable("x", std::make_shared<NormalDistribution>(0.0, 1.0));
    EXPECT_EQ(registry.getVariableCount(), 1);
    
    registry.registerVariable("x", std::make_shared<UniformDistribution>(10.0, 20.0));
    EXPECT_EQ(registry.getVariableCount(), 1); // Still 1 variable
    
    auto samples = registry.sampleAll(rng);
    // Should be uniform, so in range [10, 20]
    EXPECT_GE(samples["x"], 10.0);
    EXPECT_LE(samples["x"], 20.0);
}

// Test different seeds produce different sequences
TEST(DistributionTest, DifferentSeeds) {
    NormalDistribution dist(0.0, 1.0);
    
    std::mt19937 rng1(42);
    std::mt19937 rng2(123);
    
    double sample1 = dist.sample(rng1);
    double sample2 = dist.sample(rng2);
    
    EXPECT_NE(sample1, sample2);
}
