#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "levelitem.h"

#include <QMainWindow>

class AbstractSolver;
class LevelEditor;
class QGraphicsView;
class QGroupBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    enum Algorithm { DFS, BFS, LCFS, AStar };
private slots:
    void roleChanged(LevelItem::Role role);
    void clearRequested();
    void solveRequested();
    void solveOptionsRequested();
    void nextStepRequested();
    void fastForwardRequested();
    void prevStepRequested();
    void fastBackwardRequested();
private:
    void createActions();
    void createGroupBoxes();

    LevelEditor *editorScene;
    QGraphicsView *view;
    AbstractSolver *solver;
    Algorithm algorithm;

    QGroupBox *tileEditGroup;
    QAction *playerItemAction;
    QAction *wallItemAction;
    QAction *movableItemAction;
    QAction *movableOnGoalItemAction;
    QAction *goalItemAction;
    QAction *eraseItemAction;
    QAction *clearAction;

    QGroupBox *solveGroup;
    QAction *solveAction;
    QAction *solveOptionsAction;

    QGroupBox *navigateGroup;
    QAction *nextStepAction;
    QAction *fastForwardAction;
    QAction *prevStepAction;
    QAction *fastBackwardAction;
};
#endif // MAINWINDOW_H
