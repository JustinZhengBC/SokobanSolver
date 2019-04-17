#include "mainwindow.h"
#include "algorithmdialog.h"
#include "astarsolver.h"
#include "bfssolver.h"
#include "dfssolver.h"
#include "lcfssolver.h"
#include "leveleditor.h"
#include "levelformat.h"

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    solver(nullptr),
    algorithm(AStar)
{
    editorScene = new LevelEditor(32, 16, this);
    view = new QGraphicsView(editorScene);
    view->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    connect(editorScene, &LevelEditor::solveInterrupted,
            this, [this]() { navigateGroup->setEnabled(false); });

    createActions();
    createGroupBoxes();

    QVBoxLayout *groupBoxes = new QVBoxLayout;
    groupBoxes->addWidget(tileEditGroup);
    groupBoxes->addWidget(solveGroup);
    groupBoxes->addWidget(navigateGroup);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(groupBoxes);
    mainLayout->addWidget(view);

    QWidget *mainWidget = new QWidget;
    mainWidget->setLayout(mainLayout);
    setCentralWidget(mainWidget);
    setWindowTitle("Sokoban Solver");
}

MainWindow::~MainWindow()
{
    if (solver)
        delete solver;
}

void MainWindow::createActions()
{
    playerItemAction = new QAction(tr("Player"));
    connect(playerItemAction, &QAction::triggered,
            [this](){ roleChanged(LevelItem::Player); });
    wallItemAction = new QAction(tr("Wall"));
    connect(wallItemAction, &QAction::triggered,
            [this](){ roleChanged(LevelItem::Wall); });
    movableItemAction = new QAction(tr("Box"));
    connect(movableItemAction, &QAction::triggered,
            [this](){ roleChanged(LevelItem::Movable); });
    movableOnGoalItemAction = new QAction(tr("Box On Target"));
    connect(movableOnGoalItemAction, &QAction::triggered,
            [this](){ roleChanged(LevelItem::MovableOnGoal); });
    goalItemAction = new QAction(tr("Target"));
    connect(goalItemAction, &QAction::triggered,
            [this](){ roleChanged(LevelItem::Goal); });
    eraseItemAction = new QAction(tr("Erase"));
    connect(eraseItemAction, &QAction::triggered,
            [this](){ roleChanged(LevelItem::Erase); });
    clearAction = new QAction(tr("Clear"));
    connect(clearAction, SIGNAL(triggered()),
            this, SLOT(clearRequested()));
    solveAction = new QAction(tr("Solve"));
    connect(solveAction, SIGNAL(triggered()),
            this, SLOT(solveRequested()));
    solveOptionsAction = new QAction(tr("Solve Options"));
    connect(solveOptionsAction, SIGNAL(triggered()),
            this, SLOT(solveOptionsRequested()));
    nextStepAction = new QAction(tr("Step Forward"));
    connect(nextStepAction, SIGNAL(triggered()),
            this, SLOT(nextStepRequested()));
    fastForwardAction = new QAction(tr("Forward"));
    connect(nextStepAction, SIGNAL(triggered()),
            this, SLOT(nextStepRequested()));
    prevStepAction = new QAction(tr("Step Backward"));
    connect(nextStepAction, SIGNAL(triggered()),
            this, SLOT(nextStepRequested()));
    fastBackwardAction = new QAction(tr("Backward"));
    connect(nextStepAction, SIGNAL(triggered()),
            this, SLOT(nextStepRequested()));
}

void MainWindow::createGroupBoxes()
{
    tileEditGroup = new QGroupBox(tr("Add"));
    QRadioButton *playerButton = new QRadioButton(tr("Player"));
    connect(playerButton, SIGNAL(clicked()),
            playerItemAction, SLOT(trigger()));
    QRadioButton *wallButton = new QRadioButton(tr("Wall"));
    connect(wallButton, SIGNAL(clicked()),
            wallItemAction, SLOT(trigger()));
    QRadioButton *movableButton = new QRadioButton(tr("Box"));
    connect(movableButton, SIGNAL(clicked()),
            movableItemAction, SLOT(trigger()));
    QRadioButton *movableOnGoalButton = new QRadioButton(tr("Box On Target"));
    connect(movableOnGoalButton, SIGNAL(clicked()),
            movableOnGoalItemAction, SLOT(trigger()));
    QRadioButton *goalButton = new QRadioButton(tr("Target"));
    connect(goalButton, SIGNAL(clicked()),
            goalItemAction, SLOT(trigger()));
    QRadioButton *eraseButton = new QRadioButton(tr("Erase"));
    connect(eraseButton, SIGNAL(clicked()),
            eraseItemAction, SLOT(trigger()));
    QPushButton *clearButton = new QPushButton(tr("Clear"));
    connect(clearButton, SIGNAL(released()),
            clearAction, SLOT(trigger()));

    playerButton->setChecked(true);

    QVBoxLayout *tileLayout = new QVBoxLayout;
    tileLayout->addWidget(playerButton);
    tileLayout->addWidget(wallButton);
    tileLayout->addWidget(movableButton);
    tileLayout->addWidget(movableOnGoalButton);
    tileLayout->addWidget(goalButton);
    tileLayout->addWidget(eraseButton);
    tileLayout->addWidget(clearButton);
    tileEditGroup->setLayout(tileLayout);

    solveGroup = new QGroupBox(tr("Solve"));
    QPushButton *solveButton = new QPushButton(tr("Solve!"));
    connect(solveButton, SIGNAL(released()),
            this, SLOT(solveRequested()));
    QPushButton *solveOptionsButton = new QPushButton(tr("Solve Options"));
    connect(solveOptionsButton, SIGNAL(released()),
            this, SLOT(solveOptionsRequested()));
    QVBoxLayout *solveLayout = new QVBoxLayout;
    solveLayout->addWidget(solveButton);
    solveLayout->addWidget(solveOptionsButton);
    solveGroup->setLayout(solveLayout);

    navigateGroup = new QGroupBox(tr("View Solution"));
    QPushButton *nextButton = new QPushButton(">");
    connect(nextButton, SIGNAL(released()),
            this, SLOT(nextStepRequested()));
    QPushButton *fastForwardButton = new QPushButton(">>");
    connect(fastForwardButton, SIGNAL(released()),
            this, SLOT(fastForwardRequested()));
    QPushButton *prevButton = new QPushButton("<");
    connect(prevButton, SIGNAL(released()),
            this, SLOT(prevStepRequested()));
    QPushButton *fastBackwardButton = new QPushButton("<<");
    connect(fastBackwardButton, SIGNAL(released()),
            this, SLOT(fastBackwardRequested()));
    QHBoxLayout *navigateLayout = new QHBoxLayout;
    navigateLayout->addWidget(fastBackwardButton);
    navigateLayout->addWidget(prevButton);
    navigateLayout->addWidget(nextButton);
    navigateLayout->addWidget(fastForwardButton);
    navigateGroup->setLayout(navigateLayout);
    navigateGroup->setEnabled(false);
}

void MainWindow::roleChanged(LevelItem::Role role)
{
    editorScene->setRole(role);
    navigateGroup->setEnabled(false);
}

void MainWindow::clearRequested()
{
    editorScene->requestClear();
}

void MainWindow::solveRequested()
{
    if (solver) {
        delete solver;
        solver = nullptr;
    }
    LevelFormat *format = editorScene->getLevelFormat();
    QMessageBox messageBox;
    messageBox.setStandardButtons(QMessageBox::Ok);
    if (format) {
        switch (algorithm) {
        case DFS:
            solver = new DFSSolver(format);
            break;
        case BFS:
            solver = new BFSSolver(format);
            break;
        case LCFS:
            solver = new LCFSSolver(format);
            break;
        case AStar:
            solver = new AStarSolver(format);
        }
        bool solved = solver->isSolved();
        if (solved) {
            messageBox.setText(tr("Solution found!"));
            navigateGroup->setEnabled(true);
        } else {
            messageBox.setText(tr("Solution not found because puzzle is impossible!"));
        }
    } else {
        messageBox.setText(tr("Invalid level. Please make sure exactly one player exists and there are exactly as many targets as there are boxes"));
    }
    messageBox.exec();
}

void MainWindow::solveOptionsRequested()
{
    AlgorithmDialog dialog(algorithm);
    if (dialog.exec() == QDialog::Accepted) {
        algorithm = dialog.getAlgorithm();
        navigateGroup->setEnabled(false);
    }
}

void MainWindow::nextStepRequested()
{
    LevelState *newState = solver->stepForward();
    editorScene->renderState(newState);
}

void MainWindow::fastForwardRequested()
{
    LevelState *newState = solver->fastForward();
    editorScene->renderState(newState);
}

void MainWindow::prevStepRequested()
{
    LevelState *newState = solver->stepBackward();
    editorScene->renderState(newState);
}

void MainWindow::fastBackwardRequested()
{
    LevelState *newState = solver->fastBackward();
    editorScene->renderState(newState);
}
