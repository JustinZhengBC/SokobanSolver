#ifndef BFSSOLVER_H
#define BFSSOLVER_H

#include "abstractsolver.h"

class BFSSolver : public AbstractSolver
{
public:
    BFSSolver(LevelFormat *format);
    bool solve() override;
};

#endif // BFSSOLVER_H
