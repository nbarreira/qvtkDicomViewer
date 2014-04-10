#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "surfacereconstruction.h"

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
  void on_actionOpen_directory_activated();

private:
  Ui::MainWindow *ui;
  SurfaceReconstruction sr;
};

#endif // MAINWINDOW_H
