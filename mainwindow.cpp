#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::openConfig()
//{}
//void MainWindow::openRequests()
//{}
//void MainWindow::openAnswers()
//{}
//void MainWindow::addRequests()
//{}
//void MainWindow::deleteRequests()
//{}
//void MainWindow::modifyConfig()
//{}
//void MainWindow::modifyRequeests()
//{}
//void MainWindow::modifyAnswers()
//{}
