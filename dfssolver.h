#ifndef DFSSOLVER_H
#define DFSSOLVER_H

#include "abstractsolver.h"

class DFSSolver : public AbstractSolver
{
public:
    DFSSolver(LevelFormat *format);
    bool solve() override;
};

#endif // DFSSOLVER_H
