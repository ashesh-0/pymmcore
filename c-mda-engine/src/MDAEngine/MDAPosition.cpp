#include "MDAPosition.h"
#include <iostream>

using namespace std;

Position::Position():
    x_(UnsetPosition),
    y_(UnsetPosition),
    z_(UnsetPosition),
    xSet_(false),
    ySet_(false),
    zSet_(false)
{
; // empty constructor
}
void Position::setX(double x)
{
    x_ = x;
    xSet_ = true;
}
void Position::setY(double y)
{
    y_ = y;
    ySet_ = true;
}
void Position::setZ(double z)
{
    z_ = z;
    zSet_ = true;
}
void Position::setName(string name)
{
    name_ = name;
}
double Position::getX()
{
    return x_;
}
double Position::getY()
{
    return y_;
}
double Position::getZ()
{
    return z_;
}
string Position::getName()
{
    return name_;
}

// int main()
// {
// // initialize the position class with a dummy example
//     Position pos;
//     pos.setX(1.0);
//     pos.setY(2.0);
//     pos.setZ(3.0);
//     char name[MaxNameLength] = "test";
//     pos.setName(name);
//     std::cout << "Position name: " << pos.getName() << std::endl;
//     std::cout << "Position x: " << pos.getX() << std::endl;
//     std::cout << "Position y: " << pos.getY() << std::endl;
//     std::cout << "Position z: " << pos.getZ() << std::endl;
//     return 0;
// }
