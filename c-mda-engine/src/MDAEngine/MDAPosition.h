#ifndef _MDAPosition_H_
#define _MDAPosition_H_

#ifdef _MSC_VER
// We use exception specifications to instruct SWIG to generate the correct
// exception specifications for Java. Turn off the warnings that VC++ issues by
// the mere use of exception specifications (which VC++ does not implement).
#pragma warning(disable : 4290)
#endif

#include <climits>
#include <string>

#define MaxNameLength  256
#define MaxConfigLength 2560
#define UnsetPosition  (double)INT_MIN

// enum Indices {TIME, POSITION, GRID, CHANNEL, Z};
// enum GridPlans {GridFromEdges, GridRelative, NoGrid};


class Position {
public:
    Position();
    void setX(double x);
    void setY(double y);
    void setZ(double z);
    void setName(std::string name);
    double getX();
    double getY();
    double getZ();
    bool isXSet() {return xSet_;};
    bool isYSet() {return ySet_;};
    bool isZSet() {return zSet_;};

    std::string getName();

private:
    double x_;
    double y_;
    double z_;
    bool xSet_;
    bool ySet_;
    bool zSet_;

    std::string name_;
    // TODO: sequence?

};


#endif //_MDAPosition_H_
