#pragma once
#include "npc.h"
#include "Bear.h"
#include "Werewolf.h"
#include "Rogue.h"
#include "Observers.h"
#include <memory>
#include <random>
#include <set>

class NPCFactory {
    std::shared_ptr<ConsoleObserver> console_observer;
    std::shared_ptr<FileObserver> file_observer;
    std::set<std::string> used_names;
    
    std::string generate_name(NPC::Type npc_type) {
        static std::vector<std::string> bear_names = {"Бурый", "Гризли", "Мишка", "Топтыгин"};
        static std::vector<std::string> werewolf_names = {"Волк", "Лунный", "Клык", "Ночной"};
        static std::vector<std::string> rogue_names = {"Вор", "Тень", "Клинок", "Ловкач"};
        
        static std::random_device rd;
        static std::mt19937 gen(rd());
        
        switch(npc_type) {
            case NPC::Type::Bear:
                return bear_names[std::uniform_int_distribution<>(0, bear_names.size()-1)(gen)];
            case NPC::Type::Werewolf:
                return werewolf_names[std::uniform_int_distribution<>(0, werewolf_names.size()-1)(gen)];
            case NPC::Type::Rogue:
                return rogue_names[std::uniform_int_distribution<>(0, rogue_names.size()-1)(gen)];
            default:
                return "Безымянный";
        }
    }
    
    std::string generate_unique_name(NPC::Type type) {
        for (int i = 0; i < 100; ++i) {
            std::string name = generate_name(type);
            if (used_names.find(name) == used_names.end()) return name;
        }
        return generate_name(type) + "_" + std::to_string(used_names.size());
    }
    
public:
    NPCFactory() {
        console_observer = std::make_shared<ConsoleObserver>();
        file_observer = std::make_shared<FileObserver>();
    }
    
    std::shared_ptr<NPC> create(NPC::Type type, int x, int y, const std::string& name = "") {
        if (x < 0 || x > 500 || y < 0 || y > 500) {
            std::cerr << "Координаты должны быть в диапазоне 0-500" << std::endl;
            return nullptr;
        }
        
        std::string npc_name = name.empty() ? generate_unique_name(type) : name;
        if (used_names.find(npc_name) != used_names.end()) {
            std::cerr << "Имя '" << npc_name << "' уже используется" << std::endl;
            return nullptr;
        }
        
        std::shared_ptr<NPC> npc;
        switch(type) {
            case NPC::Type::Bear:
                npc = std::make_shared<Bear>(x, y, npc_name);
                break;
            case NPC::Type::Werewolf:
                npc = std::make_shared<Werewolf>(x, y, npc_name);
                break;
            case NPC::Type::Rogue:
                npc = std::make_shared<Rogue>(x, y, npc_name);
                break;
            default:
                std::cerr << "Неизвестный тип NPC" << std::endl;
                return nullptr;
        }
        
        used_names.insert(npc_name);
        npc->subscribe(console_observer);
        npc->subscribe(file_observer);
        return npc;
    }
    
    std::shared_ptr<NPC> load(std::istream &is) {
        int type;
        if (!(is >> type)) return nullptr;
        
        std::shared_ptr<NPC> npc;
        switch(static_cast<NPC::Type>(type)) {
            case NPC::Type::Bear:
                npc = std::make_shared<Bear>(is);
                break;
            case NPC::Type::Werewolf:
                npc = std::make_shared<Werewolf>(is);
                break;
            case NPC::Type::Rogue:
                npc = std::make_shared<Rogue>(is);
                break;
            default:
                return nullptr;
        }
        
        used_names.insert(npc->get_name());
        npc->subscribe(console_observer);
        npc->subscribe(file_observer);
        return npc;
    }
};