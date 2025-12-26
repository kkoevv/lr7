#include "GameManager.h"
#include "Visitor.h"
#include "constants.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std::chrono_literals;

GameManager::GameManager() : gen(rd()) {
    std::cout << "GameManager инициализирован\n";
}

GameManager::~GameManager() {
    stop_game();
}

void GameManager::initialize_game() {
    std::cout << "Инициализация игры...\n";
    stats_table.print_table();
    
    std::uniform_int_distribution<> coord(0, constants::MAP_WIDTH - 1);
    std::uniform_int_distribution<> type_dist(1, 3);
    
    int created = 0;
    for (int i = 0; i < constants::INITIAL_NPC_COUNT; ++i) {
        int x = coord(gen);
        int y = coord(gen);
        auto type = static_cast<NPC::Type>(type_dist(gen));
        
        if (auto npc = factory.create(type, x, y, "")) {
            std::lock_guard<std::mutex> lock(npcs_mutex);
            npcs.insert(npc);
            created++;
        }
    }
    
    std::cout << "Создано " << created << " NPC\n";
}

void GameManager::movement_worker() {
    std::uniform_int_distribution<> dir_dist(-1, 1);
    
    while (game_running && !stop_requested) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(constants::MOVEMENT_INTERVAL_MS)
        );
        
        std::vector<std::shared_ptr<NPC>> npc_list;
        {
            std::lock_guard<std::mutex> lock(npcs_mutex);
            for (auto& npc : npcs) {
                if (npc->is_alive()) {
                    npc_list.push_back(npc);
                }
            }
        }
        
        for (auto& npc : npc_list) {
            if (!npc->is_alive()) continue;
            
            int move_dist = stats_table.get_move_distance(npc->get_type());
            int dx = dir_dist(gen) * move_dist;
            int dy = dir_dist(gen) * move_dist;
            
            npc->move(dx, dy);
            
            for (auto& other : npc_list) {
                if (other == npc || !other->is_alive()) continue;
                
                int kill_dist = stats_table.get_kill_distance(npc->get_type());
                if (npc->is_close(other, kill_dist)) {
                    std::lock_guard<std::mutex> lock(fight_queue_mutex);
                    fight_queue.push({npc, other});
                    fight_cv.notify_one();
                }
            }
        }
    }
}

void GameManager::fight_worker() {
    while (game_running && !stop_requested) {
        FightTask task;
        {
            std::unique_lock<std::mutex> lock(fight_queue_mutex);
            fight_cv.wait_for(lock, 100ms, [this]() {
                return !fight_queue.empty() || stop_requested;
            });
            
            if (stop_requested) break;
            if (fight_queue.empty()) continue;
            
            task = fight_queue.front();
            fight_queue.pop();
        }
        
        if (!task.attacker->is_alive() || !task.defender->is_alive()) {
            continue;
        }
        
        int attack_power = dice(gen);
        int defense_power = dice(gen);
        
        auto visitor = std::make_shared<FightVisitor>(task.attacker);
        bool can_kill = task.defender->accept(visitor);
        
        if (can_kill && attack_power > defense_power) {
            task.defender->die();
            task.attacker->fight_notify(task.defender, true);
            
            {
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << task.attacker->get_name() 
                          << " [" << attack_power << "] убил " 
                          << task.defender->get_name() 
                          << " [" << defense_power << "]" << std::endl;
            }
        }
    }
}

void GameManager::print_status() {
    std::lock_guard<std::mutex> lock(npcs_mutex);
    
    int alive_count = 0;
    int bear_count = 0;
    int werewolf_count = 0;
    int rogue_count = 0;
    
    for (auto& npc : npcs) {
        if (npc->is_alive()) {
            alive_count++;
            switch (npc->get_type()) {
                case NPC::Type::Bear: bear_count++; break;
                case NPC::Type::Werewolf: werewolf_count++; break;
                case NPC::Type::Rogue: rogue_count++; break;
                case NPC::Type::Unknown: break;
            }
        }
    }
    
    std::lock_guard<std::mutex> cout_lock(cout_mutex);
    std::cout << "\n[Статус] Живых: " << alive_count 
              << " (Медведей: " << bear_count
              << ", Оборотней: " << werewolf_count
              << ", Разбойников: " << rogue_count << ")\n";
}

void GameManager::start_game() {
    if (game_running) return;
    
    game_running = true;
    stop_requested = false;
    
    // Запускаем рабочие потоки
    movement_thread = std::thread(&GameManager::movement_worker, this);
    fight_thread = std::thread(&GameManager::fight_worker, this);
    
    std::cout << "Игра началась! Длительность: " 
              << constants::GAME_DURATION_SECONDS << " секунд\n";
    
    // Основной поток управляет временем игры
    auto start_time = std::chrono::steady_clock::now();
    while (game_running && !stop_requested) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
        
        // Проверяем не истекло ли время
        if (elapsed.count() >= constants::GAME_DURATION_SECONDS) {
            std::cout << "Время вышло (" << elapsed.count() << " сек)! Завершение игры...\n";
            stop_requested = true;
            break;
        }
        
        print_status();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(constants::PRINT_INTERVAL_MS)
        );
    }
    
    // Останавливаем игру
    stop_game();
}

void GameManager::stop_game() {
    if (!game_running && stop_requested) return;
    
    stop_requested = true;
    game_running = false;
    
    // Будим все потоки
    {
        std::lock_guard<std::mutex> lock(fight_queue_mutex);
        fight_cv.notify_all();
    }
    
    // Принудительно завершаем потоки через detach если join не срабатывает
    bool force_stop = false;
    
    if (movement_thread.joinable()) {
        auto start = std::chrono::steady_clock::now();
        movement_thread.join();
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
    }
    
    if (fight_thread.joinable()) {
        auto start = std::chrono::steady_clock::now();
        fight_thread.join();
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
    }
    
    if (force_stop) {
        // Принудительно отсоединяем потоки
        movement_thread.detach();
        fight_thread.detach();
        std::cout << " Потоки отсоединены принудительно\n";
    }
    
    std::cout << " Игра остановлена\n";
}

void GameManager::print_survivors() {
    std::lock_guard<std::mutex> lock(npcs_mutex);
    
    int alive_count = 0;
    for (auto& npc : npcs) {
        if (npc->is_alive()) alive_count++;
    }
    
    
    std::cout << "Выжило NPC: " << alive_count << " из " << npcs.size() << "\n";
    std::cout << "\nСписок выживших:\n";
    
    for (auto& npc : npcs) {
        if (npc->is_alive()) {
            std::cout << "   ";
            npc->print();
        }
    }
    
    if (alive_count == 0) {
        std::cout << "  (никто не выжил)\n";
    }
    std::cout << "\nПрограмма завершена. Выход...\n";
    std::exit(0);
}