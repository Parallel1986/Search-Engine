#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->no_config_mark->show();
    ui->no_requests_mark->show();
    converter = new ConverterJSON();
    conf_ui = new ConfigWindow(this,converter);
    index = new InvertedIndex();
    index->UpdateDocumentBase(converter->GetTextDocuments());
    server = new SearchServer(*index);
    server->setMaxResponse(converter->GetResponsesLimit());    
    connect(conf_ui,&ConfigWindow::openConfigRequest, this,&MainWindow::openConfig);
    connect(conf_ui, &ConfigWindow::noConfigChanges, this, &MainWindow::checkConfigPath);
    connect(conf_ui, &ConfigWindow::configPathChanged, this, &MainWindow::setConfigPath);
    connect(converter, &ConverterJSON::configUpdated,this,&MainWindow::setConfigPath);
    connect(converter,&ConverterJSON::requestsUpdated,this,&MainWindow::setRequestsPath);
    connect(converter,&ConverterJSON::answersUpdated,this,&MainWindow::setAnswersPath);    
    conf_ui->close();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
    delete converter;
}

//!!!
void MainWindow::checkConfigPath()
{
    if (converter->getEngineStatus()&ConverterStatus::CONFIG_MISSED)
    {
        ui->config_path_edit->clear();
        ui->no_config_mark->show();
    }
    else if ((converter->getEngineStatus()&ConverterStatus::CONFIG_FIELD_MISSED)
            ||(converter->getEngineStatus()&ConverterStatus::SEARCH_FILES_MISSED))
        ui->no_config_mark->show();
}

void MainWindow::initializeSearchEngine()
{
    if (!converter->isConfigInitialized())
        configError();
    if (!converter->isRequestsInitialized())
        requestsError();
}

void MainWindow::openConfig()
{
    QFileDialog* dlg = new QFileDialog();
    dlg->setNameFilter(tr("JSON file (*.json)"));
    if (dlg->exec())
    {
        auto config_file = dlg->selectedFiles();        
        conf_ui->config_path = config_file[0];
        converter->setConfigPath(config_file[0]);
//        ui->config_path_edit->setText(conf_ui->config_path);
    }
    dlg->close();
    delete dlg;
    conf_ui->fillSettings();
    conf_ui->fillFields();
    conf_ui->hide();
    return checkConfigPath();
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
        converter->setRequestsPath(conf_ui->requests_path);
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
    converter->setAnswersPath(conf_ui->answers_path);
}
void MainWindow::addRequests()
{}
void MainWindow::deleteRequests()
{}
void MainWindow::modifyConfig()
{
    conf_ui->fillSettings();
    conf_ui->fillFields();
    conf_ui->show();
}
void MainWindow::modifyRequeests()
{}

void MainWindow::modifyAnswers()
{}

void MainWindow::setConfigPath()
{
    ui->config_path_edit->setText(conf_ui->config_path);
    if (converter->isConfigInitialized())
        ui->no_config_mark->hide();
    else
        ui->no_config_mark->show();
}

void MainWindow::setRequestsPath()
{ui->requests_path_edit->setText(conf_ui->requests_path);}

void MainWindow::setAnswersPath()
{ui->answers_path_edit->setText(conf_ui->answers_path);}

//void MainWindow::setConfigWarning()
//{ui->no_config_mark->show();}

//void MainWindow::unsetConfigWarning()
//{ui->no_config_mark->hide();}

//void MainWindow::setRequestsWarning()
//{ui->no_requests_mark->show();}

//void MainWindow::unsetRequestsWarning()
//{ui->no_requests_mark->hide();}

void MainWindow::configError()
{
    ui->no_config_mark->show();
    char status = converter->getEngineStatus();
    switch (status)
    {
        case ConverterStatus::CONFIG_MISSED:
        case ConverterStatus::CONFIG_FIELD_MISSED:
        {
            QMessageBox* error_message = new QMessageBox();
            if (status&ConverterStatus::CONFIG_MISSED)
            {
                error_message->setText("Missing config.json");
                error_message->setInformativeText("Couldn`t find config.json, do you wish to configure new one or to open existing?");
            }
            else
            {
                error_message->setText("Corrupted config.json");
                error_message->setInformativeText("config.json is corrupted, do you wish to configure new one or to open existing?");
            }
            error_message->setStandardButtons(QMessageBox::Yes|QMessageBox::Open|QMessageBox::Close);
            error_message->setDefaultButton(QMessageBox::Yes);
            int result = error_message->exec();
            delete error_message;
            switch (result)
            {
            case QMessageBox::Close:
            {
                this->close();
                QMessageBox* critical_message = new QMessageBox();
                critical_message->setText("Missing config.json");
                critical_message->setInformativeText("Application will be terminated");
                critical_message->setStandardButtons(QMessageBox::Ok);
                critical_message->setDefaultButton(QMessageBox::Ok);
                critical_message->exec();
                delete critical_message;
                conf_ui->close();
                this->close();
                break;
            }
            case QMessageBox::Open:
            {
                openConfig();
                break;
            }
            case QMessageBox::Yes:
            {
                modifyConfig();
                break;
            }
            default:
                break;
            }
            break;
        }

        case ConverterStatus::SEARCH_FILES_MISSED:
        {
            QMessageBox* error_message = new QMessageBox();
            error_message->setText("config.json is corrupted!");
            error_message->setInformativeText("Couldn`t find files for search! Add at least one file");
            error_message->setStandardButtons(QMessageBox::Ok);
            error_message->setDefaultButton(QMessageBox::Ok);
            error_message->exec();
            delete error_message;
            modifyConfig();
            break;
        }
    }
}

void MainWindow::requestsError()
{

}
