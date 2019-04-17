#include "levelitem.h"
#include "leveleditor.h"

LevelItem::LevelItem(LevelEditor *levelEditor, QGraphicsItem *parent):
    QGraphicsPixmapItem (parent),
    editor(levelEditor)
{
    role = editor->getRole();
    setPixmap(*editor->getPixmapForRole(role));
}

void LevelItem::setRole(Role newRole)
{
    role = newRole;
    setPixmap(*editor->getPixmapForRole(role));
}

LevelItem::Role LevelItem::getRole()
{
    return role;
}
