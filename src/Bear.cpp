#include "Bear.h"
#include "Visitor.h"
#include <iostream>

Bear::Bear(int x, int y, const std::string& name) 
    : NPC(Type::Bear, x, y, name) {}

Bear::Bear(std::istream &is) : NPC(Type::Bear, is) {}

bool Bear::accept(std::shared_ptr<IVisitor> visitor) {
    return visitor->visit(*this);
}

void Bear::print() {
    std::cout << " Медведь '" << name << "' [" << x << "," << y << "]" << std::endl;
}

void Bear::save(std::ostream &os) {
    os << static_cast<int>(Type::Bear) << " ";
    NPC::save(os);
}