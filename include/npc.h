#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <cmath>
#include <set>
#include <mutex>
#include <shared_mutex>
#include <atomic>

class Bear;
class Werewolf;
class Rogue;

// Observer interface
class IFightObserver {
public:
    virtual void on_fight(const std::shared_ptr<class NPC> attacker, 
                         const std::shared_ptr<class NPC> defender, 
                         bool win) = 0;
    virtual ~IFightObserver() = default;
};

// Visitor interface
class IVisitor {
public:
    virtual bool visit(Bear& bear) = 0;
    virtual bool visit(Werewolf& werewolf) = 0;
    virtual bool visit(Rogue& rogue) = 0;
    virtual ~IVisitor() = default;
};

class NPC : public std::enable_shared_from_this<NPC> {
public:
    enum class Type {
        Unknown = 0,
        Bear = 1,
        Werewolf = 2,
        Rogue = 3
    };

protected:
    Type type;
    int x{0};
    int y{0};
    std::string name;
    std::vector<std::shared_ptr<IFightObserver>> observers;
    std::atomic<bool> alive{true};
    mutable std::shared_mutex mutex;
    
public:
    NPC(Type t, int _x, int _y, const std::string& _name);
    NPC(Type t, std::istream &is);
    virtual ~NPC() = default;
    
    void move(int dx, int dy);
    bool is_alive() const { return alive; }
    void die() { alive = false; }
    
    void subscribe(std::shared_ptr<IFightObserver> observer);
    void fight_notify(const std::shared_ptr<NPC> defender, bool win);
    
    virtual bool accept(std::shared_ptr<IVisitor> visitor) = 0;
    
    bool is_close(const std::shared_ptr<NPC> &other, size_t distance) const;
    
    virtual void print() = 0;
    virtual void save(std::ostream &os);
    
    Type get_type() const { return type; }
    int get_x() const;
    int get_y() const;
    std::string get_name() const { return name; }
    
    friend class NPCFactory;
};

using set_t = std::set<std::shared_ptr<NPC>>;