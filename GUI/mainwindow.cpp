#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    converter = new ConverterJSON();
    conf_ui = new ConfigWindow(this,converter);
    conf_ui->close();
    //index = new InvertedIndex();
    //index->UpdateDocumentBase(converter->GetTextDocuments());
    //server = new SearchServer(*index);
    //server->setMaxResponse(converter->GetResponsesLimit());
    ui->config_path_edit->setText(QDir::currentPath() + "\\config.json");
    connect(conf_ui,&ConfigWindow::openConfigRequest, this,&MainWindow::openConfig);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
}

void MainWindow::initializeSearchEngine()
{

}

void MainWindow::openConfig()
{
    QFileDialog* dlg = new QFileDialog();
    dlg->setNameFilter(tr("JSON file (*.json)"));
    if (dlg->exec())
    {
        auto config_file = dlg->selectedFiles();
        conf_ui->config_path = config_file[0];
        ui->config_path_edit->setText(conf_ui->config_path);
    }
    dlg->close();
    delete dlg;
    conf_ui->hide();
    return initializeSearchEngine();
}
void MainWindow::openRequests()
{
    QFileDialog* dlg = new QFileDialog(this);
    dlg->setNameFilter(tr("JSON file (*.json)"));
    if (dlg->exec())
    {
        auto requests_file = dlg->selectedFiles();
        conf_ui->requests_path = requests_file[0];
        ui->requests_path_edit->setText(conf_ui->requests_path);
        converter->setRequestsPath(conf_ui->requests_path.toStdString());
    }
    delete dlg;

}
void MainWindow::openAnswers()
{
    QFileDialog* dlg = new QFileDialog(this);
    dlg->setNameFilter(tr("JSON file (*.json)"));
    if (dlg->exec())
    {
        auto answers_file = dlg->selectedFiles();
        conf_ui->answers_path = answers_file[0];
    }
    delete dlg;
    ui->answers_path_edit->setText(conf_ui->answers_path);
    converter->setAnswersPath(conf_ui->answers_path.toStdString());
}
void MainWindow::addRequests()
{}
void MainWindow::deleteRequests()
{}
void MainWindow::modifyConfig()
{
    conf_ui->fillFields();
    conf_ui->show();
}
void MainWindow::modifyRequeests()
{}
void MainWindow::modifyAnswers()
{}
void MainWindow::setConfigPath(QString new_path)
{
    ui->config_path_edit->setText(new_path);
}
