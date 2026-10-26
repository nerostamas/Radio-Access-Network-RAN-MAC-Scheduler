#include <gtest/gtest.h>

#include "ran/Simulator.hpp"

using ran::SimConfig;
using ran::Simulator;
using ran::mac::SchedulerType;

namespace {
SimConfig baseConfig() {
    SimConfig c;
    c.numUes = 10;
    c.resourceBlocks = 50;
    c.numTtis = 2000;
    c.seed = 7;
    return c;
}
}  // namespace

TEST(Simulator, ProducesPositiveThroughput) {
    auto cfg = baseConfig();
    cfg.scheduler = SchedulerType::ProportionalFair;
    Simulator sim(cfg);
    const auto r = sim.run();
    EXPECT_GT(r.cellThroughputMbps, 0.0);
    EXPECT_EQ(r.perUeThroughputMbps.size(), 10u);
}

TEST(Simulator, IsDeterministicForFixedSeed) {
    auto cfg = baseConfig();
    const auto r1 = Simulator(cfg).run();
    const auto r2 = Simulator(cfg).run();
    EXPECT_DOUBLE_EQ(r1.cellThroughputMbps, r2.cellThroughputMbps);
    EXPECT_DOUBLE_EQ(r1.jainsFairness, r2.jainsFairness);
}

TEST(Simulator, MaxCiBeatsRoundRobinOnThroughput) {
    auto rr = baseConfig();
    rr.scheduler = SchedulerType::RoundRobin;
    auto maxci = baseConfig();
    maxci.scheduler = SchedulerType::MaxCI;

    const auto rrResult = Simulator(rr).run();
    const auto maxciResult = Simulator(maxci).run();

    // Max C/I is throughput-greedy; it should not do worse than Round Robin.
    EXPECT_GE(maxciResult.cellThroughputMbps, rrResult.cellThroughputMbps);
}

TEST(Simulator, RoundRobinIsFairerThanMaxCi) {
    auto rr = baseConfig();
    rr.scheduler = SchedulerType::RoundRobin;
    auto maxci = baseConfig();
    maxci.scheduler = SchedulerType::MaxCI;

    const auto rrResult = Simulator(rr).run();
    const auto maxciResult = Simulator(maxci).run();

    // Channel-agnostic RR should be at least as fair as throughput-greedy Max C/I.
    EXPECT_GE(rrResult.jainsFairness, maxciResult.jainsFairness);
}

TEST(Simulator, FairnessWithinValidRange) {
    auto cfg = baseConfig();
    cfg.scheduler = SchedulerType::ProportionalFair;
    const auto r = Simulator(cfg).run();
    EXPECT_GT(r.jainsFairness, 0.0);
    EXPECT_LE(r.jainsFairness, 1.0);
}
