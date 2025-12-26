#pragma once
#include "npc.h"
#include <map>
#include <iostream>

struct NPCStats {
    int move_distance;
    int kill_distance;
};

class NPCStatsTable {
private:
    std::map<NPC::Type, NPCStats> stats;
    
public:
    NPCStatsTable() {
        stats[NPC::Type::Bear] = {5, 10};
        stats[NPC::Type::Werewolf] = {40, 5};
        stats[NPC::Type::Rogue] = {10, 10};
    }
    
    NPCStats get_stats(NPC::Type type) const {
        auto it = stats.find(type);
        if (it != stats.end()) {
            return it->second;
        }
        return {5, 10};
    }
    
    int get_move_distance(NPC::Type type) const {
        return get_stats(type).move_distance;
    }
    
    int get_kill_distance(NPC::Type type) const {
        return get_stats(type).kill_distance;
    }
    
    void print_table() const {
        std::cout << "Таблица характеристик NPC:\n";
        for (const auto& [type, stat] : stats) {
            std::string type_name;
            switch(type) {
                case NPC::Type::Bear: type_name = "Медведь"; break;
                case NPC::Type::Werewolf: type_name = "Оборотень"; break;
                case NPC::Type::Rogue: type_name = "Разбойник"; break;
                default: type_name = "Неизвестный";
            }
            std::cout << type_name << ": ход=" << stat.move_distance 
                     << ", убийство=" << stat.kill_distance << std::endl;
        }
    }
};