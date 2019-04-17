#ifndef LEVELFORMAT_H
#define LEVELFORMAT_H

#include "levelitem.h"

#include <QLinkedList>
#include <QSet>

class QPoint;

struct LevelState
{
    QSet<QPoint> movables;
    QPoint player;
    LevelState *previousState;
    int cost;
};

struct LimitedZone
{
    int line;
    int start;
    int end;
    bool horizontal;
    int maxMovablesAllowed;
};

/*
 * The layout of a level is stored as a set of points representing the walls
 * and a set of points representing the targets. Forbidden zones are areas
 * such that if any box occupies that area, the puzzle is unsolvable. Limited
 * zones are regions such that if a certain number of boxes are in that region,
 * the puzzle is unsolvable (e.g. multiple targets along a wall). Level states
 * describe the positions of the players and boxes in a state, and should only
 * be used with the level layouts they were generated from. LevelFormat is not
 * responsible for deleting the states it generates.
 */

class LevelFormat
{
public:
    LevelFormat(int h, int w);
    void setRoleAt(QPoint pos, LevelItem::Role role);
    void buildZones(); // only use after all walls have been set
    LevelState *getInitialState() const;

    QSet<LevelState*> *nextStatesFor(LevelState *state) const;
    bool goalReached(LevelState *state) const;
    bool similarTo(LevelState *a, LevelState *b) const;
    bool similarTo(LevelState *a, LevelState *b, int tolerance) const;
    int getHeuristic(LevelState *state) const; // -1 if unsolvable
    int distanceForPlayerToMoveTo(LevelState *state, const QPoint &destination) const;

    void log(LevelState *state) const;
private:
    QHash<QPoint, int> *getReachablePointsWithCosts(LevelState *state) const;
    bool isValid(const QPoint &pos) const; // in domain and not at wall
    bool isValid(LevelState *state, const QPoint &pos) const; // also not at a box
    bool blockExists(LevelState *state) const; // if blocks are stuck somewhere
    bool blockExistsForCode(int code) const; // helper, see implementation for explanation

    QSet<QPoint> goals;
    QSet<QPoint> walls;
    LevelState *initialState;

    QList<LimitedZone> limitedZones;
    QSet<QPoint> forbiddenZones;

    int height;
    int width;
};

inline uint qHash (const QPoint &key)
{
    return qHash (QPair<int,int>(key.x(), key.y()));
}

#endif // LEVELFORMAT_H
