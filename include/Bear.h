#pragma once
#include "npc.h"

class Bear : public NPC {
public:
    Bear(int x, int y, const std::string& name);
    Bear(std::istream &is);
    
    bool accept(std::shared_ptr<IVisitor> visitor) override;
    void print() override;
    void save(std::ostream &os) override;
};