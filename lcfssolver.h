#ifndef LCFSSOLVER_H
#define LCFSSOLVER_H

#include "abstractsolver.h"

class LCFSSolver : public AbstractSolver
{
public:
    LCFSSolver(LevelFormat *format);
    bool solve() override;
};

#endif // LCFSSOLVER_H
