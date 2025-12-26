#pragma once
#include "npc.h"
#include <fstream>

class ConsoleObserver : public IFightObserver {
public:
    void on_fight(const std::shared_ptr<NPC> attacker, 
                 const std::shared_ptr<NPC> defender, 
                 bool win) override {
        if (win) {
            std::cout << attacker->get_name() 
                      << " убил " << defender->get_name() << std::endl;
        }
    }
};

class FileObserver : public IFightObserver {
    std::ofstream log_file;
public:
    FileObserver() { 
        log_file.open("log.txt", std::ios::app);
        if (log_file.is_open()) {
            log_file << "Начало сессии " << std::endl;
        }
    }
    ~FileObserver() { 
        if (log_file.is_open()) {
            log_file << "Конец сессии " << std::endl;
            log_file.close();
        }
    }
    
    void on_fight(const std::shared_ptr<NPC> attacker, 
                 const std::shared_ptr<NPC> defender, 
                 bool win) override {
        if (win && log_file.is_open()) {
            log_file << attacker->get_name() 
                     << " убил " << defender->get_name() << std::endl;
        }
    }
};