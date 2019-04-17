#include "abstractsolver.h"
#include "levelformat.h"

#include <QtDebug>

AbstractSolver::AbstractSolver(LevelFormat *format) :
    level(format),
    solved(false),
    solutionIndex(0)
{

}

AbstractSolver::~AbstractSolver()
{
    for (LevelState *state : solution)
        delete state;
}

bool AbstractSolver::isSolved() const
{
    return solved;
}

LevelState *AbstractSolver::stepForward()
{
    if (solved) {
        solutionIndex = qMin(solutionIndex + 1, solution.size() - 1);
        return solution.at(solutionIndex);
    } else {
        return nullptr;
    }
}

LevelState *AbstractSolver::fastForward()
{
    if (solved) {
        solutionIndex = solution.size() - 1;
        return solution.last();
    } else {
        return nullptr;
    }
}

LevelState *AbstractSolver::stepBackward()
{
    if (solved) {
        solutionIndex = qMax(solutionIndex - 1, 0);
        return solution.at(solutionIndex);
    } else {
        return nullptr;
    }
}

LevelState *AbstractSolver::fastBackward()
{
    if (solved) {
        solutionIndex = 0;
        return solution.first();
    } else {
        return nullptr;
    }
}

bool AbstractSolver::solve()
{
    return false;
}
