#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pushButton_clicked()
{
  std::string dir("/local2/scratch/maxilar/2/");

  SurfaceReconstruction sr;
  sr.readDICOMDir(dir);
  sr.update();
  sr.show(ui->qvtkWidget->GetRenderWindow());

}
