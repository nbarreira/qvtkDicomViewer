/**
 * @file mainwindow.cpp
 * @brief Implementation of the Qt interface to the surface reconstruction algorithm.
 *
 *
 * @author Noelia Barreira (nbarreira@udc.es)
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>



MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  this->setWindowTitle("qvtkDicomViewer");
  ui->pushButton->setDisabled(true);
  ui->progressBar->setVisible(false);
  sr.Show(ui->qvtkWidget->GetRenderWindow()); // Reset the framebuffer with a dark background
}

MainWindow::~MainWindow()
{
  delete ui;
}

extern void Update(void);

void MainWindow::on_pushButton_clicked()
{

  ui->pushButton->setDisabled(true);
  ui->progressBar->setVisible(true);
  ui->progressBar->setMaximum(0);
  ui->progressBar->setMinimum(0);
  sr.setThreshold(ui->spinBox->text().toDouble());

  connect(&watcher, SIGNAL(finished()), this, SLOT(handleFinished()));

  QFuture<void> future = QtConcurrent::run(&sr, &SurfaceReconstruction::Update);
  watcher.setFuture(future);

}

void MainWindow::handleFinished() {
  sr.Show(ui->qvtkWidget->GetRenderWindow());
  ui->pushButton->setDisabled(false);
  ui->progressBar->setVisible(false);
}

void MainWindow::on_actionOpen_directory_activated() {
  QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                  QDir::home().absolutePath(),
                                                  QFileDialog::ShowDirsOnly);

  int files = sr.ReadDICOMSeries(dir.toStdString());

  QString message = QString::number(files) + " files read from " + dir;
  ui->statusBar->showMessage(message);
  ui->pushButton->setDisabled(false);

}

void MainWindow::on_actionAbout_activated() {
  QMessageBox about;
  about.setText("A simple DICOM viewer using vtk + qt");
  about.setWindowTitle("qvtkDicomViewer");
  about.exec();
}
