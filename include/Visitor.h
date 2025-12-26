#pragma once
#include "npc.h"
#include "Bear.h"
#include "Werewolf.h"
#include "Rogue.h"

class FightVisitor : public IVisitor {
private:
    std::shared_ptr<NPC> attacker;
    
public:
    FightVisitor(std::shared_ptr<NPC> att) : attacker(att) {}
    
    bool visit(Bear& bear) override {
        if (attacker->get_type() == NPC::Type::Rogue) {
            bear.fight_notify(attacker, false);
            return true;
        }
        return false;
    }
    
    bool visit(Werewolf& werewolf) override {
        if (attacker->get_type() == NPC::Type::Bear) {
            werewolf.fight_notify(attacker, false);
            return true;
        }
        return false;
    }
    
    bool visit(Rogue& rogue) override {
        if (attacker->get_type() == NPC::Type::Werewolf) {
            rogue.fight_notify(attacker, false);
            return true;
        }
        return false;
    }
};