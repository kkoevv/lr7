#include "Rogue.h"
#include "Visitor.h"
#include <iostream>

Rogue::Rogue(int x, int y, const std::string& name) 
    : NPC(Type::Rogue, x, y, name) {}

Rogue::Rogue(std::istream &is) : NPC(Type::Rogue, is) {}

bool Rogue::accept(std::shared_ptr<IVisitor> visitor) {
    return visitor->visit(*this);
}

void Rogue::print() {
    std::cout << "Разбойник '" << name << "' [" << x << "," << y << "]" << std::endl;
}

void Rogue::save(std::ostream &os) {
    os << static_cast<int>(Type::Rogue) << " ";
    NPC::save(os);
}