#ifndef ABSTRACTSOLVER_H
#define ABSTRACTSOLVER_H

#include <QList>

class LevelFormat;
struct LevelState;

/*
 * Solver classes are responsible for finding a solution and supplying
 * pointers to states when requested for rendering. They are also responsible
 * for deleting the states they request from the level format, but not the
 * format itself (that is done by the level editor).
 */

class AbstractSolver
{
public:
    AbstractSolver(LevelFormat *format);
    virtual ~AbstractSolver();
    bool isSolved() const;
    LevelState *stepForward();
    LevelState *fastForward();
    LevelState *stepBackward();
    LevelState *fastBackward();
protected:
    virtual bool solve();

    LevelFormat *level;
    QList<LevelState *> solution;
    bool solved;
    int solutionIndex;
};

#endif // ABSTRACTSOLVER_H
