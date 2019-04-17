#include "leveleditor.h"
#include "levelformat.h"

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QHoverEvent>
#include <QPen>
#include <QtDebug>
#include <QtMath>

LevelEditor::LevelEditor(int iconLength, int windowLength, QObject *parent):
    QGraphicsScene (parent),
    currentRole(LevelItem::Player),
    iconSize(iconLength),
    windowSize(windowLength),
    snapCursor(new QGraphicsRectItem),
    currentLevel(nullptr)
{
    setSceneRect(0, 0, iconLength * windowLength, iconLength * windowLength);
    setBackgroundBrush(Qt::black);
    playerImg = new QPixmap(":/tiles/Player.png");
    playerImg->save("/Users/admin/desktop/Player.png");
    wallImg = new QPixmap(":/tiles/Wall.png");
    movableImg = new QPixmap(":/tiles/Movable.png");
    movableOnGoalImg = new QPixmap(":/tiles/MovableOnGoal.png");
    goalImg = new QPixmap(":/tiles/Goal.png");
    snapCursor->setPen(QPen(Qt::white));
    snapCursor->setVisible(true);
    snapCursor->setAcceptHoverEvents(true);
    snapCursor->setZValue(100);
    addItem(snapCursor);
}

LevelItem::Role LevelEditor::getRole() const
{
    return currentRole;
}

void LevelEditor::setRole(LevelItem::Role role)
{
    currentRole = role;
    if (currentLevel) {
        delete currentLevel;
        currentLevel = nullptr;
    }
}

QPixmap* LevelEditor::getPixmapForRole(LevelItem::Role role) const
{
    switch (role) {
    case LevelItem::Player: return playerImg;
    case LevelItem::Wall: return wallImg;
    case LevelItem::Movable: return movableImg;
    case LevelItem::MovableOnGoal: return movableOnGoalImg;
    case LevelItem::Goal: return goalImg;
    default: return nullptr;
    }
}

LevelFormat* LevelEditor::getLevelFormat()
{
    removeItem(snapCursor);
    QRect bounds = itemsBoundingRect().toRect();
    formatOffset = bounds.topLeft();
    bounds = QRect(bounds.topLeft() / iconSize, bounds.bottomRight() / iconSize);
    LevelFormat *format = new LevelFormat(bounds.height(), bounds.width());
    bool playerFound = false;
    int goalsAdded = 0;
    int movablesAdded = 0;
    for (QGraphicsItem *sceneItem : items()) {
        if (LevelItem *levelItem = static_cast<LevelItem*>(sceneItem)) {
            QPoint itemPos = getAlignedTopLeftPointAt(levelItem->scenePos()) - bounds.topLeft();
            if (levelItem->getRole() == LevelItem::Player) {
                if (!playerFound) {
                    format->setRoleAt(itemPos, LevelItem::Player);
                    playerFound = true;
                } else {
                    addItem(snapCursor);
                    return nullptr;
                }
            } else {
                format->setRoleAt(itemPos, levelItem->getRole());
                if (levelItem->getRole() == LevelItem::Movable) {
                    ++movablesAdded;
                }
                else if (levelItem->getRole() == LevelItem::Goal) {
                    ++goalsAdded;
                }
                else if (levelItem->getRole() == LevelItem::MovableOnGoal) {
                    ++movablesAdded;
                    ++goalsAdded;
                }
            }
        }
    }
    addItem(snapCursor);
    if (goalsAdded != movablesAdded || !playerFound)
        return nullptr;
    format->buildZones();
    currentLevel = format;
    return format;
}

void LevelEditor::renderState(LevelState *state)
{
    removeItem(snapCursor);
    for (QGraphicsItem *sceneItem : items()) {
        if (LevelItem *levelItem = static_cast<LevelItem*>(sceneItem)) {
            if (levelItem->getRole() == LevelItem::Player || levelItem->getRole() == LevelItem::Movable)
                removeItem(levelItem);
            else if (levelItem->getRole() == LevelItem::MovableOnGoal)
                levelItem->setRole(LevelItem::Goal);
        }
    }
    LevelItem *newItem = new LevelItem(this);
    addItem(newItem);
    newItem->setRole(LevelItem::Player);
    newItem->setPos(state->player * iconSize + formatOffset);
    for (QPoint pos : state->movables) {
        QList<QGraphicsItem*> maybeMovableHere = items(pos * iconSize + formatOffset, Qt::IntersectsItemShape, Qt::AscendingOrder);
        if (maybeMovableHere.size() == 1) {
            QGraphicsItem *sceneItem = maybeMovableHere.first();
            if (LevelItem *levelItem = static_cast<LevelItem*>(sceneItem))
                levelItem->setRole(LevelItem::MovableOnGoal);
        } else {
            LevelItem *newMovable = new LevelItem(this);
            addItem(newMovable);
            newMovable->setRole(LevelItem::Movable);
            newMovable->setPos(pos * iconSize + formatOffset);
        }
    }
    addItem(snapCursor);
    update();
}

void LevelEditor::requestClear()
{
    removeItem(snapCursor); // we remove this because clear() deletes all items
    clear();
    addItem(snapCursor);
}

const QPoint LevelEditor::getAlignedTopLeftPointAt(const QPointF &pos) const
{
    qreal x = pos.x() / iconSize;
    qreal y = pos.y() / iconSize;
    return QPointF(qFloor(x), qFloor(y)).toPoint();
}

const QRect LevelEditor::getAlignedRectAt(const QPointF &pos) const
{
    return QRect(getAlignedTopLeftPointAt(pos) * iconSize, QSize(iconSize, iconSize));
}

void LevelEditor::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->buttons() & (Qt::LeftButton | Qt::RightButton) && sceneRect().contains(mouseEvent->scenePos())) {
        if (currentLevel) {
            setRole(currentRole); // triggers deletion of active level format
            emit solveInterrupted();
        }
        QList<QGraphicsItem *> sceneItems = items(mouseEvent->scenePos(), Qt::IntersectsItemShape, Qt::AscendingOrder);
        if (sceneItems.size() == 1 && mouseEvent->buttons() & Qt::LeftButton) {
            if (currentRole == LevelItem::Erase) return;
            LevelItem *newItem = new LevelItem(this);
            addItem(newItem);
            newItem->setPos(getAlignedTopLeftPointAt(mouseEvent->scenePos()) * iconSize);
        } else {
            QGraphicsItem *sceneItem = sceneItems.first();
            if (LevelItem *levelItem = qgraphicsitem_cast<LevelItem*>(sceneItem)) {
                if (currentRole == LevelItem::Erase || mouseEvent->buttons() & Qt::RightButton)
                    removeItem(levelItem);
                else
                    levelItem->setRole(currentRole);
            }
        }
        update();
    }
}

bool LevelEditor::event(QEvent *event)
{
    if (event->type() == QEvent::GraphicsSceneMouseMove) {
        if (QGraphicsSceneMouseEvent *moveEvent = static_cast<QGraphicsSceneMouseEvent*>(event)) {
            snapCursor->setVisible(true);
            snapCursor->setRect(getAlignedRectAt(moveEvent->scenePos()));
            mousePressEvent(moveEvent);
            return true;
        }
    } else if (event->type() == QEvent::Leave)
        snapCursor->setVisible(false);
    return QGraphicsScene::event(event);
}
