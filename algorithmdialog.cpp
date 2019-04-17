#include "algorithmdialog.h"
#include "mainwindow.h"

#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>

AlgorithmDialog::AlgorithmDialog(MainWindow::Algorithm currentAlgorithm)
{
    QVBoxLayout *centralLayout = new QVBoxLayout;

    QGroupBox *groupBox = new QGroupBox;
    QVBoxLayout *algorithmsLayout = new QVBoxLayout;
    aStarButton = new QRadioButton(tr("A* (recommended)"));
    lcfsButton = new QRadioButton(tr("LCFS (not as recommended)"));
    dfsButton = new QRadioButton(tr("DFS (not recommended)"));
    bfsButton = new QRadioButton(tr("BFS (REALLY not recommended)"));
    algorithmsLayout->addWidget(aStarButton);
    algorithmsLayout->addWidget(lcfsButton);
    algorithmsLayout->addWidget(dfsButton);
    algorithmsLayout->addWidget(bfsButton);
    switch (currentAlgorithm) {
    case MainWindow::AStar:
        aStarButton->setChecked(true);
        break;
    case MainWindow::LCFS:
        lcfsButton->setChecked(true);
        break;
    case MainWindow::DFS:
        dfsButton->setChecked(true);
        break;
    case MainWindow::BFS:
        bfsButton->setChecked(true);
        break;
    }
    groupBox->setLayout(algorithmsLayout);

    QHBoxLayout *choicesLayout = new QHBoxLayout;
    QPushButton *okButton = new QPushButton(tr("OK"));
    connect(okButton, SIGNAL(released()),
            this, SLOT(accept()));
    QPushButton *cancelButton = new QPushButton(tr("Cancel"));
    connect(cancelButton, SIGNAL(released()),
            this, SLOT(reject()));
    choicesLayout->addWidget(okButton);
    choicesLayout->addWidget(cancelButton);

    centralLayout->addWidget(groupBox);
    centralLayout->addLayout(choicesLayout);
    setLayout(centralLayout);
}

MainWindow::Algorithm AlgorithmDialog::getAlgorithm()
{
    if (lcfsButton->isChecked())
        return MainWindow::LCFS;
    else if (dfsButton->isChecked())
        return MainWindow::DFS;
    else if (bfsButton->isChecked())
        return MainWindow::BFS;
    return MainWindow::AStar; // the recommended default
}
