#include "Werewolf.h"
#include "Visitor.h"
#include <iostream>

Werewolf::Werewolf(int x, int y, const std::string& name) 
    : NPC(Type::Werewolf, x, y, name) {}

Werewolf::Werewolf(std::istream &is) : NPC(Type::Werewolf, is) {}

bool Werewolf::accept(std::shared_ptr<IVisitor> visitor) {
    return visitor->visit(*this);
}

void Werewolf::print() {
    std::cout << " Оборотень '" << name << "' [" << x << "," << y << "]" << std::endl;
}

void Werewolf::save(std::ostream &os) {
    os << static_cast<int>(Type::Werewolf) << " ";
    NPC::save(os);
}