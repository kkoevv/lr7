#include <gtest/gtest.h>
#include <memory>
#include "npc.h"
#include "Bear.h"
#include "Factory.h"
#include "Table.h"
#include "constants.h"

// нпс
TEST(NPCTest, Creation) {
    auto bear = std::make_shared<Bear>(10, 20, "TestBear");
    EXPECT_EQ(bear->get_type(), NPC::Type::Bear);
    EXPECT_EQ(bear->get_name(), "TestBear");
    EXPECT_TRUE(bear->is_alive());
}

TEST(NPCTest, Movement) {
    auto npc = std::make_shared<Bear>(0, 0, "Movable");
    npc->move(5, 10);
    EXPECT_EQ(npc->get_x(), 5);
    EXPECT_EQ(npc->get_y(), 10);
}

TEST(NPCTest, Death) {
    auto npc = std::make_shared<Bear>(0, 0, "Victim");
    EXPECT_TRUE(npc->is_alive());
    npc->die();
    EXPECT_FALSE(npc->is_alive());
}

TEST(NPCTest, DistanceCalculation) {
    auto npc1 = std::make_shared<Bear>(0, 0, "N1");
    auto npc2 = std::make_shared<Bear>(3, 4, "N2");
    
    EXPECT_TRUE(npc1->is_close(npc2, 5));
    EXPECT_TRUE(npc1->is_close(npc2, 6));
    EXPECT_FALSE(npc1->is_close(npc2, 4));
}

//фабр
TEST(FactoryTest, Creation) {
    NPCFactory factory;
    auto npc = factory.create(NPC::Type::Bear, 50, 50, "FactoryBear");
    EXPECT_NE(npc, nullptr);
    EXPECT_EQ(npc->get_type(), NPC::Type::Bear);
}

TEST(FactoryTest, InvalidCoordinates) {
    NPCFactory factory;
    auto invalid = factory.create(NPC::Type::Bear, 600, 600, "Invalid");
    EXPECT_EQ(invalid, nullptr);
}

// табл
TEST(TableTest, Stats) {
    NPCStatsTable table;
    auto stats = table.get_stats(NPC::Type::Bear);
    EXPECT_EQ(stats.move_distance, 5);
    EXPECT_EQ(stats.kill_distance, 10);
}

// гр карт
TEST(MapBoundsTest, StayWithinBounds) {
    auto npc = std::make_shared<Bear>(50, 50, "Bounder");
    
    npc->move(-100, 0);
    EXPECT_GE(npc->get_x(), 0);
    
    npc->move(200, 0);
    EXPECT_LE(npc->get_x(), constants::MAP_WIDTH);
    
    npc->move(0, -100);
    EXPECT_GE(npc->get_y(), 0);
    
    npc->move(0, 200);
    EXPECT_LE(npc->get_y(), constants::MAP_HEIGHT);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}