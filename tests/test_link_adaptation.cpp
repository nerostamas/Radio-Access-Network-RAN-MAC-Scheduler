#include <gtest/gtest.h>

#include "ran/phy/LinkAdaptation.hpp"

using ran::phy::LinkAdaptation;

TEST(LinkAdaptation, ZeroCqiCarriesNoData) {
    EXPECT_EQ(LinkAdaptation::transportBlockSize(0, 50), 0u);
}

TEST(LinkAdaptation, ZeroResourceBlocksCarryNoData) {
    EXPECT_EQ(LinkAdaptation::transportBlockSize(10, 0), 0u);
    EXPECT_EQ(LinkAdaptation::transportBlockSize(10, -5), 0u);
}

TEST(LinkAdaptation, TbsMonotonicInCqi) {
    ran::Bits prev = 0;
    for (ran::Cqi cqi = 1; cqi <= ran::kMaxCqi; ++cqi) {
        const ran::Bits tbs = LinkAdaptation::transportBlockSize(cqi, 50);
        EXPECT_GE(tbs, prev) << "TBS must not shrink as CQI grows (cqi=" << int{cqi} << ")";
        prev = tbs;
    }
}

TEST(LinkAdaptation, TbsMonotonicInResourceBlocks) {
    ran::Bits prev = 0;
    for (int rbs = 1; rbs <= 100; ++rbs) {
        const ran::Bits tbs = LinkAdaptation::transportBlockSize(10, rbs);
        EXPECT_GE(tbs, prev);
        prev = tbs;
    }
}

TEST(LinkAdaptation, CqiAboveMaxIsClampedNotCrashing) {
    const ran::Bits atMax = LinkAdaptation::transportBlockSize(ran::kMaxCqi, 50);
    const ran::Bits aboveMax = LinkAdaptation::transportBlockSize(200, 50);
    EXPECT_EQ(atMax, aboveMax);
}

TEST(LinkAdaptation, ModulationOrderIncreasesWithCqi) {
    EXPECT_EQ(LinkAdaptation::mcsForCqi(3).modulationOrder, 2);   // QPSK
    EXPECT_EQ(LinkAdaptation::mcsForCqi(8).modulationOrder, 4);   // 16QAM
    EXPECT_EQ(LinkAdaptation::mcsForCqi(13).modulationOrder, 6);  // 64QAM
}
