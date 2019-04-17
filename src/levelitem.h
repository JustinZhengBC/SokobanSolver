#ifndef LEVELITEM_H
#define LEVELITEM_H

#include <QGraphicsPixmapItem>

class LevelEditor;

class LevelItem : public QGraphicsPixmapItem
{
public:
    LevelItem(LevelEditor *levelEditor, QGraphicsItem *parent = nullptr);
    enum Role { Player, Wall, Movable, MovableOnGoal, Goal, Erase, Empty };
    void setRole(Role newType);
    LevelItem::Role getRole();
private:
    Role role;
    LevelEditor *editor;
};

#endif // LEVELITEM_H
