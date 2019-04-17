#ifndef ALGORITHMDIALOG_H
#define ALGORITHMDIALOG_H

#include "mainwindow.h"

#include <QDialog>

class QRadioButton;

class AlgorithmDialog : public QDialog
{
public:
    AlgorithmDialog(MainWindow::Algorithm currentAlgorithm);
    MainWindow::Algorithm getAlgorithm();
private:
    QRadioButton *aStarButton;
    QRadioButton *lcfsButton;
    QRadioButton *dfsButton;
    QRadioButton *bfsButton;
};

#endif // ALGORITHMDIALOG_H
