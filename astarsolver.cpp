#include "astarsolver.h"
#include "levelformat.h"

#include <queue>
#include <QtDebug>

AStarSolver::AStarSolver(LevelFormat *format):
    AbstractSolver (format)
{
    solved = solve();
}

bool AStarSolver::solve()
{
    QHash<LevelState*, int> fValues;
    QSet<LevelState*> pruned;
    auto cmp = [&fValues](LevelState *a, LevelState *b) {
        if (!fValues.contains(a)) {
            return true; }
        if (!fValues.contains(b)) {
            return false;
    }
        return fValues.value(a) > fValues.value(b);
    };
    std::priority_queue<LevelState*, std::vector<LevelState*>, decltype (cmp)> frontier(cmp);
    frontier.push(level->getInitialState());
    fValues.insert(level->getInitialState(), level->getHeuristic(level->getInitialState()));
    if (fValues.value(level->getInitialState()) == -1)
        return false;
    while (!frontier.empty()) {
        LevelState *state = frontier.top();
        frontier.pop();
        if (pruned.contains(state))
            continue;
        else if (level->goalReached(state)) {
            while (state->previousState) {
                solution.prepend(state);
                fValues.remove(state);
                state = state->previousState;
            }
            solution.prepend(level->getInitialState());
            fValues.remove(level->getInitialState());
            for (LevelState *seenState : fValues.keys())
                delete seenState;
            solved = true;
            return true;
        } else {
            bool isNewState = true;
            int fValueForState = fValues.value(state);
            for (LevelState *seenState : fValues.keys()) {
                int fValueForSeenState = fValues.value(seenState);
                if (level->similarTo(state, seenState, qAbs(fValueForState - fValueForSeenState)) && state != seenState) {
                    if (fValueForState >= fValueForSeenState) {
                        isNewState = false;
                        break;
                    } else
                        pruned.insert(seenState);
                }
            }
            if (isNewState) {
                QSet<LevelState*> *nextStates = level->nextStatesFor(state);
                QSet<LevelState*> admissibleNextStates;
                if (nextStates) {
                    for (LevelState *nextState : *nextStates) {
                        int heuristic = level->getHeuristic(nextState);
                        if (heuristic != -1) {
                            fValues.insert(nextState, nextState->cost + heuristic);
                            admissibleNextStates.insert(nextState);
                        }
                    }
                    for (LevelState *nextState : admissibleNextStates)
                        frontier.push(nextState);
                }
            }
        }
    }
    for (LevelState *seenState : fValues.keys())
        delete seenState;
    return false;
}
