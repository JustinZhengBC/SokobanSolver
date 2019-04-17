#ifndef ASTARSOLVER_H
#define ASTARSOLVER_H

#include "abstractsolver.h"

class AStarSolver : public AbstractSolver
{
public:
    AStarSolver(LevelFormat *format);
    bool solve() override;
};

#endif // ASTARSOLVER_H
