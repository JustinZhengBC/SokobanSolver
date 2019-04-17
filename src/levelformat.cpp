#include "levelformat.h"

#include <QQueue>
#include <QtDebug>

LevelFormat::LevelFormat(int h, int w) :
    height(h - 1),
    width(w - 1)
{
    initialState = new LevelState;
    initialState->previousState = nullptr;
    initialState->cost = 0;
}

void LevelFormat::setRoleAt(QPoint pos, LevelItem::Role role)
{
    switch(role) {
    case LevelItem::Player:
        initialState->player = pos;
        break;
    case LevelItem::Wall:
        walls.insert(pos);
        break;
    case LevelItem::Movable:
        initialState->movables.insert(pos);
        break;
    case LevelItem::MovableOnGoal:
        initialState->movables.insert(pos);
        goals.insert(pos);
        break;
    case LevelItem::Goal:
        goals.insert(pos);
        break;
    default: return;
    }
}

/*
 * After all walls have been added to the level, this function detects
 * edges that boxes cannot escape from and builds appropriate zones.
 *
 * Algorithm:
 *
 * Input: a grid of either wall or empty
 * Output: a set of regions, each with a number describing max boxes allowed
 *
 * Start with an empty set
 * For each row/column in grid:
 *   Split the row/column into 1xn or nx1 regions of empty, separated by walls
 *   For each region:
 *     If that region is completely blocked on at least one of its wide sides:
 *       The number of targets in the region is the max allowed boxes; add to set
 *
 * Return the set
 *
 */
void LevelFormat::buildZones()
{
    int lastStart = 0;
    bool upBlocked = true;
    bool downBlocked = true;
    int goalsSeen = 0;
    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            if (!upBlocked && !downBlocked) {
                while (isValid(QPoint(i, j)) && !walls.contains(QPoint(i, j)))
                    ++j;
            } else if (walls.contains(QPoint(i, j))) {
                if (lastStart != j && (upBlocked || downBlocked)) {
                    if (goalsSeen == 0) {
                        for (int k = lastStart; k < j; ++k)
                            forbiddenZones << QPoint(i, k);
                    } else {
                        LimitedZone zone;
                        zone.line = i;
                        zone.start = lastStart;
                        zone.end = j;
                        zone.maxMovablesAllowed = goalsSeen;
                        zone.horizontal = true;
                        limitedZones.append(zone);
                    }
                }
                lastStart = j + 1;
                upBlocked = true;
                downBlocked = true;
                goalsSeen = 0;
            } else {
                if (goals.contains(QPoint(i, j)))
                    ++goalsSeen;
                upBlocked = upBlocked && !isValid(QPoint(i - 1, j));
                downBlocked = downBlocked && !isValid(QPoint(i + 1, j));
            }
        }
        if (lastStart != height && (upBlocked || downBlocked)) {
            if (goalsSeen == 0) {
                for (int k = lastStart; k < height; ++k)
                    forbiddenZones << QPoint(i, k);
            } else {
                LimitedZone zone;
                zone.line = i;
                zone.start = lastStart;
                zone.end = height;
                zone.maxMovablesAllowed = goalsSeen;
                zone.horizontal = true;
                limitedZones.append(zone);
            }
        }
        lastStart = 0;
        upBlocked = true;
        downBlocked = true;
        goalsSeen = 0;
    }
    bool leftBlocked = true;
    bool rightBlocked = true;
    for (int j = 0; j < height; ++j) {
        for (int i = 0; i < width; ++i) {
            if (!leftBlocked && !rightBlocked) {
                while (isValid(QPoint(i, j)) && !walls.contains(QPoint(i, j)))
                    ++i;
            } else if (walls.contains(QPoint(i, j))) {
                if (lastStart != i && (leftBlocked || rightBlocked)) {
                    if (goalsSeen == 0) {
                        for (int k = lastStart; k < i; ++k)
                            forbiddenZones << QPoint(k, j);
                    } else {
                        LimitedZone zone;
                        zone.line = j;
                        zone.start = lastStart;
                        zone.end = i;
                        zone.maxMovablesAllowed = goalsSeen;
                        zone.horizontal = false;
                        limitedZones.append(zone);
                    }
                }
                lastStart = i + 1;
                leftBlocked = true;
                rightBlocked = true;
                goalsSeen = 0;
            } else {
                if (goals.contains(QPoint(i, j)))
                    ++goalsSeen;
                leftBlocked = leftBlocked && !isValid(QPoint(i, j - 1));
                rightBlocked = rightBlocked && !isValid(QPoint(i, j + 1));
            }
        }
        if (lastStart != width && (leftBlocked || rightBlocked)) {
            if (goalsSeen == 0) {
                for (int k = lastStart; k < width; ++k)
                    forbiddenZones << QPoint(k, j);
            } else {
                LimitedZone zone;
                zone.line = j;
                zone.start = lastStart;
                zone.end = width;
                zone.maxMovablesAllowed = goalsSeen;
                zone.horizontal = false;
                limitedZones.append(zone);
            }
        }
        lastStart = 0;
        leftBlocked = true;
        rightBlocked = true;
        goalsSeen = 0;
    }
}

LevelState* LevelFormat::getInitialState() const
{
    return initialState;
}

/*
 * Finds the next possible states for a given state. Note that the next
 * states are the possible ways boxes can be moved, and not the possible
 * ways the player can move.
 */
QSet<LevelState*> *LevelFormat::nextStatesFor(LevelState *state) const
{
    QSet<LevelState*> *nextStates = new QSet<LevelState*>;
    QHash<QPoint, int> *reachablePoints = getReachablePointsWithCosts(state);
    for (QPoint movable : state->movables) {
        QPoint leftPoint = QPoint(movable.x() - 1, movable.y());
        QPoint rightPoint = QPoint(movable.x() + 1, movable.y());
        QPoint upPoint = QPoint(movable.x(), movable.y() - 1);
        QPoint downPoint = QPoint(movable.x(), movable.y() + 1);
        if (reachablePoints->contains(leftPoint) && isValid(state, rightPoint)) {
            LevelState *newState = new LevelState(*state);
            newState->movables.remove(movable);
            newState->movables.insert(rightPoint);
            newState->player = movable;
            newState->cost = state->cost + reachablePoints->value(leftPoint) + 1;
            newState->previousState = state;
            nextStates->insert(newState);
        }
        if (reachablePoints->contains(rightPoint) && isValid(state, leftPoint)) {
            LevelState *newState = new LevelState(*state);
            newState->movables.remove(movable);
            newState->movables.insert(leftPoint);
            newState->player = movable;
            newState->cost = state->cost + reachablePoints->value(rightPoint) + 1;
            newState->previousState = state;
            nextStates->insert(newState);
        }
        if (reachablePoints->contains(upPoint) && isValid(state, downPoint)) {
            LevelState *newState = new LevelState(*state);
            newState->movables.remove(movable);
            newState->movables.insert(downPoint);
            newState->player = movable;
            newState->cost = state->cost + reachablePoints->value(upPoint) + 1;
            newState->previousState = state;
            nextStates->insert(newState);
        }
        if (reachablePoints->contains(downPoint) && isValid(state, upPoint)) {
            LevelState *newState = new LevelState(*state);
            newState->movables.remove(movable);
            newState->movables.insert(upPoint);
            newState->player = movable;
            newState->cost = state->cost + reachablePoints->value(downPoint) + 1;
            newState->previousState = state;
            nextStates->insert(newState);
        }
    }
    delete reachablePoints;
    return nextStates;
}

/*
 * Returns true if all boxes are on targets.
 */
bool LevelFormat::goalReached(LevelState *state) const
{
    for (QPoint movable : state->movables) {
        if (!goals.contains(movable))
            return false;
    }
    return true;
}

/*
 * Returns true if in both states, boxes are in the same positions
 * and both states' player positions are mutually reachable without
 * having to move any boxes.
 */
bool LevelFormat::similarTo(LevelState *a, LevelState *b) const
{
    for (QPoint movable : a->movables) {
        if (!b->movables.contains(movable))
            return false;
    }
    int playerDistance = distanceForPlayerToMoveTo(a, b->player);
    return playerDistance != -1;
}

/*
 * Same as above, but with the added condition that the player positions
 * must be mutually reachable within a certain number of moves.
 */
bool LevelFormat::similarTo(LevelState *a, LevelState *b, int tolerance) const
{
    if (tolerance < 0)
        return false;
    for (QPoint movable : a->movables) {
        if (!b->movables.contains(movable))
            return false;
    }
    int playerDistance = distanceForPlayerToMoveTo(a, b->player);
    return playerDistance != -1 && playerDistance <= tolerance;
}

/*
 * Returns the sum of the manhattan distances from each box to its nearest
 * goal if the puzzle is solvable at this state, otherwise returns -1.
 */
int LevelFormat::getHeuristic(LevelState *state) const
{
    // Forbidden zones are zones where any box being present makes the
    // puzzle unsolvable (e.g. a concave wall without a target)
    for (QPoint movable : state->movables) {
        if (forbiddenZones.contains(movable))
            return -1;
    }

    // Limited zones are zones where a certain number of boxes being present
    // makes the puzzle unsolvable (e.g. a concave wall without some targets)
    for (LimitedZone limitedZone : limitedZones) {
        int movablesInZone = 0;
        if (limitedZone.horizontal) {
            for (int i = limitedZone.start; i < limitedZone.end; ++i) {
                if (state->movables.contains(QPoint(limitedZone.line, i)))
                    ++movablesInZone;
            }
        }
        else {
            for (int i = limitedZone.start; i < limitedZone.end; ++i) {
                if (state->movables.contains(QPoint(i, limitedZone.line)))
                    ++movablesInZone;
            }
        }
        if (movablesInZone > limitedZone.maxMovablesAllowed)
            return -1;
    }

    // Check if boxes are arranged in a way that block each other
    if (blockExists(state))
        return -1;

    int sumOfDistances = 0;
    for (QPoint movable : state->movables) {
        int pathToClosestGoal = INT_MAX;
        for (QPoint goal : goals)
            pathToClosestGoal = qMin(pathToClosestGoal, (movable - goal).manhattanLength());
        sumOfDistances += pathToClosestGoal;
    }
    return sumOfDistances;
}

/*
 * Uses BFS to determine the number of moves needed for the player in
 * a given state to move to a given position, returns -1 if impossible.
 */
int LevelFormat::distanceForPlayerToMoveTo(LevelState *state, const QPoint &destination) const
{
    QSet<QPoint> seen;
    QQueue<QPoint> pointsQueue;
    QQueue<int> costsQueue;
    pointsQueue.enqueue(state->player);
    costsQueue.enqueue(0);
    while (!pointsQueue.isEmpty()) {
        QPoint point = pointsQueue.dequeue();
        int cost = costsQueue.dequeue();
        if (point == destination) {
            return cost;
        }
        else if (!seen.contains(point)) {
            seen.insert(point);
            QList<QPoint> nextPoints;
            nextPoints << QPoint(point.x() - 1, point.y());
            nextPoints << QPoint(point.x() + 1, point.y());
            nextPoints << QPoint(point.x(), point.y() - 1);
            nextPoints << QPoint(point.x(), point.y() + 1);
            for (QPoint nextPoint : nextPoints) {
                if (isValid(state, nextPoint)) {
                    pointsQueue.enqueue(nextPoint);
                    costsQueue.enqueue(cost + 1);
                }
            }
        }
    }
    return -1;
}

/*
 * Prints a representation of the level at this state to the console.
 */
void LevelFormat::log(LevelState *state) const
{
    for (int i = 0; i < width; i++) {
        QString row;
        for (int j = 0; j < height; j++) {
            QPoint pos(i, j);
            if (walls.contains(pos))
                row += "X";
            else if (state->player == pos)
                row += "P";
            else if (state->movables.contains(pos)) {
                if (goals.contains(pos))
                    row += "#";
                else
                    row += "+";
            } else if (goals.contains(pos))
                row += "O";
            else
                row += " ";
        }
        qDebug() << row;
    }
}

/*
 * Uses BFS to return a set of points the player can access in a given
 * state, as well as their costs.
 */
QHash<QPoint, int> *LevelFormat::getReachablePointsWithCosts(LevelState *state) const
{
    QHash<QPoint, int> *hash = new QHash<QPoint, int>;
    QQueue<QPoint> pointsQueue;
    QQueue<int> costsQueue;
    pointsQueue.enqueue(state->player);
    costsQueue.enqueue(0);
    while (!pointsQueue.isEmpty()) {
        QPoint point = pointsQueue.dequeue();
        int cost = costsQueue.dequeue();
        if (!hash->contains(point)) {
            hash->insert(point, cost);
            QList<QPoint> nextPoints;
            nextPoints << QPoint(point.x() - 1, point.y());
            nextPoints << QPoint(point.x() + 1, point.y());
            nextPoints << QPoint(point.x(), point.y() - 1);
            nextPoints << QPoint(point.x(), point.y() + 1);
            for (QPoint nextPoint : nextPoints) {
                if (isValid(state, nextPoint)) {
                    pointsQueue.enqueue(nextPoint);
                    costsQueue.enqueue(cost + 1);
                }
            }
        }
    }
    return hash;
}

/*
 * Returns true if pos is in the domain of the level and not at a wall.
 */
bool LevelFormat::isValid(const QPoint &pos) const
{
    return pos.x() >= 0 && pos.x() < width && pos.y() >= 0 && pos.y() < height &&
            !walls.contains(pos);
}

/*
 * Same as above but also checks if there is a box at pos.
 */
bool LevelFormat::isValid(LevelState *state, const QPoint &pos) const
{
    return isValid(pos) && !state->movables.contains(pos);
}

/*
 * Checks if boxes have been placed in a way such that they
 * cannot be moved anymore (and they are not already at goals).
 *
 * Algorithm:
 *
 * Input: a list of boxes positions
 * Output: whether the boxes are blocking each other in a way
 * that makes the problem unsolvable
 *
 * Construct a queue containing the positions of every box can
 * be pushed in at least one direction
 *
 * Construct an empty set
 *
 * While the queue is not empty:
 *   Take a position from the queue
 *   Add it to the set
 *   If there are any boxes
 *     - adjacent to the current position
 *     - have not been added to the set
 *     - cannot be pushed in any direction in the current state
 *     - could be pushed in at least one direction IF the box at
 *       the current position was moved away
 *   then add those adjacent boxes to the queue
 *
 * If there are any boxes that have not been added to the set and
 * are not currently at a target, the problem is unsolvable and
 * return true, otherwise return false
 *
 */
bool LevelFormat::blockExists(LevelState *state) const
{
    // in the blockedMovables hash, int is a number from 0 to 15
    // flagging which adjacent positions are blocked
    // 0b0001 - left side blocked
    // 0b0010 - right side blocked
    // 0b0100 - up side blocked
    // 0b1000 - down side blocked
    QHash<QPoint, qint8> blockedMovables;
    for (QPoint movable : state->movables) {
        qint8 blockCode = 0;
        if (!isValid(state, QPoint(movable.x() - 1, movable.y())))
            blockCode |= 1;
        if (!isValid(state, QPoint(movable.x() + 1, movable.y())))
            blockCode |= 2;
        if (!isValid(state, QPoint(movable.x(), movable.y() - 1)))
            blockCode |= 4;
        if (!isValid(state, QPoint(movable.x(), movable.y() + 1)))
            blockCode |= 8;
        blockedMovables.insert(movable, blockCode);
    }

    QQueue<QPoint> queue;
    QSet<QPoint> movableLater;
    for (QPoint movable : blockedMovables.keys()) {
        if (!blockExistsForCode(blockedMovables.value(movable)))
            queue.enqueue(movable);
    }
    while (!queue.isEmpty()) {
        QPoint movable = queue.dequeue();
        movableLater.insert(movable);
        QPoint leftPoint = QPoint(movable.x() - 1, movable.y());
        QPoint rightPoint = QPoint(movable.x() + 1, movable.y());
        QPoint upPoint = QPoint(movable.x(), movable.y() - 1);
        QPoint downPoint = QPoint(movable.x(), movable.y() + 1);
        if (blockedMovables.contains(leftPoint) && !movableLater.contains(leftPoint) &&
                blockExistsForCode(blockedMovables.value(leftPoint)) &&
                !blockExistsForCode(blockedMovables.value(leftPoint) & ~2))
            queue.enqueue(leftPoint);
        if (blockedMovables.contains(rightPoint) && !movableLater.contains(rightPoint) &&
                blockExistsForCode(blockedMovables.value(rightPoint)) &&
                !blockExistsForCode(blockedMovables.value(rightPoint) & ~1))
            queue.enqueue(rightPoint);
        if (blockedMovables.contains(upPoint) && !movableLater.contains(upPoint) &&
                blockExistsForCode(blockedMovables.value(upPoint)) &&
                !blockExistsForCode(blockedMovables.value(upPoint) & ~8))
            queue.enqueue(upPoint);
        if (blockedMovables.contains(downPoint) && !movableLater.contains(downPoint) &&
                blockExistsForCode(blockedMovables.value(downPoint)) &&
                !blockExistsForCode(blockedMovables.value(downPoint) & ~4))
            queue.enqueue(downPoint);
    }

    for (QPoint movable : blockedMovables.keys()) {
        if (!movableLater.contains(movable) && !goals.contains(movable))
            return true;
    }
    return false;
}

/*
 * Helper for above function.
 */
bool LevelFormat::blockExistsForCode(int code) const
{
    // blocked if cannot be pushed horizontally (either blocked
    // on left or right side) and cannot be pushed vertically
    // (either blocked up or down)
    return (code & 3) && (code & 12);
}
