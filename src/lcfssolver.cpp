#include "lcfssolver.h"
#include "levelformat.h"

#include <queue>

LCFSSolver::LCFSSolver(LevelFormat *format):
    AbstractSolver (format)
{
    solved = solve();
}

bool LCFSSolver::solve()
{
    QSet<LevelState *> seen;
    QSet<LevelState *> pruned;
    auto cmp = [](LevelState *a, LevelState *b) { return a->cost > b->cost; };
    std::priority_queue<LevelState*, std::vector<LevelState*>, decltype (cmp)> frontier(cmp);
    frontier.push(level->getInitialState());
    while (!frontier.empty()) {
        LevelState *state = frontier.top();
        frontier.pop();
        if (pruned.contains(state))
            continue;
        else if (level->goalReached(state)) {
            while (state->previousState) {
                solution.prepend(state);
                seen.remove(state);
                state = state->previousState;
            }
            solution.prepend(level->getInitialState());
            seen.remove(level->getInitialState());
            for (LevelState *seenState : seen)
                delete seenState;
            while (!frontier.empty()) {
                delete frontier.top();
                frontier.pop();
            }
            solved = true;
            return true;
        } else {
            bool isNewState = true;
            for (LevelState *seenState : seen) {
                if (level->similarTo(state, seenState, qAbs(state->cost - seenState->cost))) {
                    if (state->cost >= seenState->cost) {
                        isNewState = false;
                        break;
                    } else {
                        pruned.insert(seenState);
                    }
                }
            }
            seen.insert(state);
            if (isNewState) {
                QSet<LevelState*> *nextStates = level->nextStatesFor(state);
                if (nextStates) {
                    for (LevelState *nextState : *nextStates) {
                        frontier.push(nextState);
                    }
                }
            }
        }
    }
    for (LevelState *seenState : seen)
        delete seenState;
    while (!frontier.empty()) {
        delete frontier.top();
        frontier.pop();
    }
    return false;
}
