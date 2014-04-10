/**
 * @file mainwindow.h
 * @brief Qt interface to the surface reconstruction algorithm.
 *
 *
 * @author Noelia Barreira (nbarreira@udc.es)
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFutureWatcher>
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
  void on_actionAbout_activated();
  void handleFinished();

private:
  Ui::MainWindow *ui;
  SurfaceReconstruction sr;
  QFutureWatcher<void> watcher;

};

#endif // MAINWINDOW_H
