#pragma once
#include "npc.h"

class Rogue : public NPC {
public:
    Rogue(int x, int y, const std::string& name);
    Rogue(std::istream &is);
    
    bool accept(std::shared_ptr<IVisitor> visitor) override;
    void print() override;
    void save(std::ostream &os) override;
};