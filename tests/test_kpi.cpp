#include <gtest/gtest.h>

#include <vector>

#include "ran/metrics/Kpi.hpp"

using ran::metrics::jainsFairnessIndex;
using ran::metrics::sum;

TEST(Kpi, EmptyFairnessIsZero) {
    EXPECT_DOUBLE_EQ(jainsFairnessIndex({}), 0.0);
}

TEST(Kpi, AllZeroFairnessIsZero) {
    EXPECT_DOUBLE_EQ(jainsFairnessIndex({0.0, 0.0, 0.0}), 0.0);
}

TEST(Kpi, EqualAllocationIsPerfectlyFair) {
    EXPECT_DOUBLE_EQ(jainsFairnessIndex({5.0, 5.0, 5.0, 5.0}), 1.0);
}

TEST(Kpi, SingleUserIsPerfectlyFair) {
    EXPECT_DOUBLE_EQ(jainsFairnessIndex({7.0}), 1.0);
}

TEST(Kpi, MaximallyUnfairApproachesOneOverN) {
    // One user has everything: fairness = 1/n.
    const double f = jainsFairnessIndex({10.0, 0.0, 0.0, 0.0});
    EXPECT_NEAR(f, 0.25, 1e-9);
}

TEST(Kpi, FairnessBoundedBetweenZeroAndOne) {
    const double f = jainsFairnessIndex({1.0, 2.0, 3.0, 4.0, 5.0});
    EXPECT_GT(f, 0.0);
    EXPECT_LE(f, 1.0);
}

TEST(Kpi, SumAddsValues) {
    EXPECT_DOUBLE_EQ(sum({1.5, 2.5, 3.0}), 7.0);
    EXPECT_DOUBLE_EQ(sum({}), 0.0);
}
