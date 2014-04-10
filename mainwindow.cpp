#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QFileDialog>
#include <QDir>


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->pushButton->setDisabled(true);
  sr.Show(ui->qvtkWidget->GetRenderWindow()); // Reset the framebuffer with a dark background
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pushButton_clicked()
{
  sr.setThreshold(ui->spinBox->text().toDouble());
  sr.Update();
  sr.Show(ui->qvtkWidget->GetRenderWindow());
}

void MainWindow::on_actionOpen_directory_activated() {
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                  QDir::home().absolutePath(),
                                                  QFileDialog::ShowDirsOnly);

  int files = sr.ReadDICOMSeries(dir.toStdString());

  QString message = "Read " + QString::number(files) + " files from " + dir;
  ui->statusBar->showMessage(message);
  ui->pushButton->setDisabled(false);

}

