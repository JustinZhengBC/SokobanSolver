#include "bfssolver.h"
#include "levelformat.h"

#include <QQueue>
#include <QtDebug>

BFSSolver::BFSSolver(LevelFormat *format):
    AbstractSolver (format)
{
    solved = solve();
}

bool BFSSolver::solve()
{
    QSet<LevelState *> seen;
    QQueue<LevelState*> frontier;
    frontier.enqueue(level->getInitialState());
    while (!frontier.isEmpty()) {
        LevelState *state = frontier.dequeue();
        if (level->goalReached(state)) {
            while (state->previousState) {
                solution.prepend(state);
                seen.remove(state);
                state = state->previousState;
            }
            solution.prepend(level->getInitialState());
            seen.remove(level->getInitialState());
            for (LevelState *seenState : seen)
                delete seenState;
            for (LevelState *queuedState : frontier)
                delete queuedState;
            solved = true;
            return true;
        } else {
            bool isNewState = true;
            for (LevelState *seenState : seen) {
                if (level->similarTo(state, seenState)) {
                    isNewState = false;
                    break;
                }
            }
            seen.insert(state);
            if (isNewState) {
                QSet<LevelState*> *nextStates = level->nextStatesFor(state);
                if (nextStates) {
                    for (LevelState *nextState : *nextStates) {
                        frontier.enqueue(nextState);
                    }
                }
            }
        }
    }
    for (LevelState *seenState : seen)
        delete seenState;
    for (LevelState *queuedState : frontier)
        delete queuedState;
    return false;
}
