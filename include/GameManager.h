#pragma once

#include "npc.h"
#include "Factory.h"
#include "Table.h"
#include "constants.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <random>
#include <memory>
#include <chrono>

struct FightTask {
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

class GameManager {
private:
    set_t npcs;
    NPCStatsTable stats_table;
    NPCFactory factory;
    
    std::thread movement_thread;
    std::thread fight_thread;
    
    mutable std::mutex npcs_mutex;
    mutable std::mutex cout_mutex;
    std::mutex fight_queue_mutex;
    std::condition_variable fight_cv;
    std::queue<FightTask> fight_queue;
    
    std::atomic<bool> game_running{false};
    std::atomic<bool> stop_requested{false};
    
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dice{1, 6};
    
    void movement_worker();
    void fight_worker();
    void print_status();
    
public:
    GameManager();
    ~GameManager();
    
    void initialize_game();
    void start_game();
    void stop_game();
    void print_survivors();
};