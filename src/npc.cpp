#include "npc.h"
#include "constants.h"
#include <shared_mutex>

NPC::NPC(Type t, int _x, int _y, const std::string& _name) 
    : type(t), x(_x), y(_y), name(_name) {}

NPC::NPC(Type t, std::istream &is) : type(t) {
    is >> x >> y >> name;
}

void NPC::subscribe(std::shared_ptr<IFightObserver> observer) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    observers.push_back(observer);
}

void NPC::fight_notify(const std::shared_ptr<NPC> defender, bool win) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    for (auto &o : observers) {
        o->on_fight(shared_from_this(), defender, win);
    }
}

bool NPC::is_close(const std::shared_ptr<NPC> &other, size_t distance) const {
    std::shared_lock<std::shared_mutex> lock1(mutex, std::defer_lock);
    std::shared_lock<std::shared_mutex> lock2(other->mutex, std::defer_lock);
    std::lock(lock1, lock2);
    
    double dx = x - other->x;
    double dy = y - other->y;
    return (dx * dx + dy * dy) <= (distance * distance);
}

void NPC::save(std::ostream &os) {
    std::shared_lock<std::shared_mutex> lock(mutex);
    os << x << " " << y << " " << name;
}

void NPC::move(int dx, int dy) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    x += dx;
    y += dy;
    
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= constants::MAP_WIDTH) x = constants::MAP_WIDTH - 1;
    if (y >= constants::MAP_HEIGHT) y = constants::MAP_HEIGHT - 1;
}

int NPC::get_x() const {
    std::shared_lock<std::shared_mutex> lock(mutex);
    return x;
}

int NPC::get_y() const {
    std::shared_lock<std::shared_mutex> lock(mutex);
    return y;
}