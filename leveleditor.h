#ifndef LEVELEDITOR_H
#define LEVELEDITOR_H

#include "levelitem.h"

#include <QGraphicsScene>

class LevelFormat;
struct LevelState;
class QMouseEvent;

/*
 * Left click to input tiles, right click to erase.
 */

class LevelEditor : public QGraphicsScene
{
    Q_OBJECT

public:
    LevelEditor(int iconLength, int windowLength, QObject *parent = nullptr);
    LevelItem::Role getRole() const;
    void setRole(LevelItem::Role role);
    QPixmap *getPixmapForRole(LevelItem::Role role) const;
    LevelFormat *getLevelFormat();
    void renderState(LevelState *state);
    void requestClear();
signals:
    void solveInterrupted();
private:
    const QPoint getAlignedTopLeftPointAt(const QPointF &pos) const; // in tile coordinates
    const QRect getAlignedRectAt(const QPointF &pos) const; // in pixel coordinates

    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    bool event(QEvent *event) override;

    LevelItem::Role currentRole;
    int iconSize;
    int windowSize;
    QGraphicsRectItem *snapCursor;

    LevelFormat *currentLevel;
    QPoint formatOffset; // format uses translated coordinates

    QPixmap *playerImg;
    QPixmap *wallImg;
    QPixmap *movableImg;
    QPixmap *movableOnGoalImg;
    QPixmap *goalImg;
};

#endif // LEVELEDITOR_H
