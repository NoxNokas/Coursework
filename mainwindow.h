#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include "integrator.h"
#include "custom.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void showResult(TEnzymaticModel* model);
    void statCharacteristics();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
