#pragma once
#include "npc.h"

class Werewolf : public NPC {
public:
    Werewolf(int x, int y, const std::string& name);
    Werewolf(std::istream &is);
    
    bool accept(std::shared_ptr<IVisitor> visitor) override;
    void print() override;
    void save(std::ostream &os) override;
};