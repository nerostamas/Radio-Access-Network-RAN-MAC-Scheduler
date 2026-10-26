#include <gtest/gtest.h>

#include "ran/mac/HarqProcess.hpp"

using ran::mac::HarqProcess;

TEST(HarqProcess, StartsIdle) {
    HarqProcess h;
    EXPECT_FALSE(h.busy());
    EXPECT_EQ(h.attempt(), 0);
}

TEST(HarqProcess, StartTransmissionMarksBusy) {
    HarqProcess h;
    h.startNewTransmission(1000);
    EXPECT_TRUE(h.busy());
    EXPECT_EQ(h.attempt(), 1);
    EXPECT_EQ(h.pendingBits(), 1000u);
}

TEST(HarqProcess, AckFreesProcess) {
    HarqProcess h;
    h.startNewTransmission(1000);
    h.onAck();
    EXPECT_FALSE(h.busy());
    EXPECT_EQ(h.attempt(), 0);
}

TEST(HarqProcess, NackIncrementsAttemptUntilMax) {
    HarqProcess h(4);
    h.startNewTransmission(1000);
    EXPECT_TRUE(h.onNack());  // attempt -> 2
    EXPECT_EQ(h.attempt(), 2);
    EXPECT_TRUE(h.onNack());  // attempt -> 3
    EXPECT_TRUE(h.onNack());  // attempt -> 4
    EXPECT_FALSE(h.onNack());  // exhausted, dropped
    EXPECT_FALSE(h.busy());
}

TEST(HarqProcess, NackOnIdleReturnsFalse) {
    HarqProcess h;
    EXPECT_FALSE(h.onNack());
}

TEST(HarqProcess, SingleTransmissionMaxDropsImmediatelyOnNack) {
    HarqProcess h(1);
    h.startNewTransmission(500);
    EXPECT_FALSE(h.onNack());  // no retransmission allowed
    EXPECT_FALSE(h.busy());
}
