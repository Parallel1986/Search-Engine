#include "mainwindow.h"
#include "./ui/ui_main_window.cpp"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent, EngineCore* search_engine)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , core(search_engine)
{
    ui->setupUi(this);                              //Setting up users interface
    ui->save_button->setEnabled(false);             //Turning off save button
    r_dialog = new RequestAddDialog(this);          //Creating add request dialog
    conf_ui = new ConfigWindow(this,search_engine); //Creatign window for settings
    request_list_model = new QStringListModel;      //Creating model for results view
    //Connecting slots and signals
    connect(r_dialog,&RequestAddDialog::newRequest, this, &MainWindow::modifyRequests);     //Добавление запроса
    connect(conf_ui,&ConfigWindow::openConfigRequest, this,&MainWindow::openConfig);        //Открытие настроек
    connect(conf_ui, &ConfigWindow::noConfigChanges, this, &MainWindow::checkConfigPath);   //Отсутствие изменений настроек
    connect(conf_ui, &ConfigWindow::configPathChanged, this, &MainWindow::setConfigPath);   //Изменение пути до файла config.json
    connect(core, &EngineCore::ConfigPathChanged,this,&MainWindow::setConfigPath);          //Updating path to configurations' file
    connect(core,&EngineCore::RequestsPathChanged,this,&MainWindow::setRequestsPath);       //Updating path to requests' file
    connect(core,&EngineCore::AnswersPathChanged,this,&MainWindow::setAnswersPath);         //Updating path to answers' file
//    if (!(converter->getEngineStatus()&ConverterStatus::CONFIG_MISSED))     //Проверка наличия файла config.json
//        ui->config_path_edit->setText(conf_ui->config_path);                //Выводим путь config.json в главном окне
//    checkConfigPath();                              //Проверяем путь до файла config.json
    conf_ui->close();                               //Closing settings window
    ui->results_tree_widget->setColumnCount(1);     //Setting number of columns in result's model
    loadRequests();                                 //Checking requests
    readinessCheck();                               //Checking readiness
}

MainWindow::~MainWindow()
{
    if (request_list_model)
        delete request_list_model;  //Deleting requests model
    if (ui)
        delete ui;                  //Deleting users interface
    if (r_dialog)
        delete r_dialog;            //Deleting add requests dialog
}

//Checking configuration
void MainWindow::checkConfigPath()
{
    if (core->GetEngineStatus()&ConverterStatus::CONFIG_MISSED)
    {
        ui->config_path_edit->clear();
        ui->no_config_mark->show();
        config_ready = false;
    }
    else if ((core->GetEngineStatus()&ConverterStatus::CONFIG_FIELD_MISSED)
            ||(core->GetEngineStatus()&ConverterStatus::SEARCH_FILES_MISSED)
            )
    {
        ui->no_config_mark->show();
        config_ready = false;
        configError();
    }
    readinessCheck();
}

//Initialising search engine
void MainWindow::initializeSearchEngine()
{
    if (!core->IsConfigInitialized())
    {
        configError();
        config_ready = false;
    }
    else
    {
        ui->no_config_mark->hide();
        config_ready = true;
    }

    if (!core->IsRequestsInitialized())
    {
        requestsError();
        requests_ready = false;
    }
    else
    {
        ui->no_requests_mark->hide();
        requests_ready = true;
    }

    readinessCheck();
}

//Открыть файл конфигурации
void MainWindow::openConfig()
{
    QFileDialog* dlg = new QFileDialog();
    dlg->setNameFilter(tr("JSON file (*.json)"));
    if (dlg->exec())
    {
        auto config_file = dlg->selectedFiles();
        conf_ui->config_path = config_file[0];
        core->SetConfigPath(config_file[0]);
        conf_ui->fillSettings();
        conf_ui->fillFields();
        conf_ui->hide();
    }
    dlg->close();
    delete dlg;
    return checkConfigPath();
}

//Открыть файл запросов
void MainWindow::openRequests()
{
    QFileDialog* dlg = new QFileDialog(this);
    dlg->setNameFilter(tr("JSON file (*.json)"));
    if (dlg->exec())
    {
        auto requests_file = dlg->selectedFiles();
        conf_ui->requests_path = requests_file[0];
        ui->requests_path_edit->setText(conf_ui->requests_path);
        core->setRequestsPath(conf_ui->requests_path);
    }
    delete dlg;
/**/if (core->isRequestsInitialized()
        &&!core->getRequests().empty())
    {
        requests_ready = true;
        loadRequests();
    }
    else
    {
        request_list_model->setStringList(QStringList());
        ui->requests_line_list->setModel(request_list_model);
        ui->no_requests_mark->show();
        requests_ready = false;
    }
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
    core->setAnswersPath(conf_ui->answers_path);
}

void MainWindow::addRequests()
{
    r_dialog->show();
}

void MainWindow::deleteRequests()
{
    QItemSelectionModel* selection = ui->requests_line_list->selectionModel();
    if (!selection->selectedIndexes().empty())
    {
        QModelIndexList index = selection->selectedIndexes();
        int row = index[0].row();
        request_list_model->removeRows(row,1,QModelIndex());
        ui->requests_line_list->setModel(request_list_model);
    }
    requests_list = request_list_model->stringList();
    core->putRequests(requests_list);
    readinessCheck();
    loadRequests();
}

//Modifying configuration
void MainWindow::modifyConfig()
{
    conf_ui->fillSettings();
    conf_ui->fillFields();
    conf_ui->show();
}

//Changing requests
void MainWindow::modifyRequests(QString new_request)
{
    requests_list.append(new_request);
    converter->putRequests(requests_list);
    readynessCheck();
    loadRequests();
}

//Set configurations' path
void MainWindow::setConfigPath()
{
    ui->config_path_edit->setText(conf_ui->config_path);
    if (core->isConfigInitialized())
        {
            ui->no_config_mark->hide();
            config_ready = true;
            readinessCheck();
        }
    else
        {
            ui->no_config_mark->show();
            config_ready = false;
        }
}

//Setting text to requests' path field
void MainWindow::setRequestsPath()
{
    ui->requests_path_edit->setText(conf_ui->requests_path);
    loadRequests();
}

//Loading requests
void MainWindow::loadRequests()
{
    if (!core->getRequests().empty())
    {
        ui->requests_path_edit->setText(core->getRequestsPath());
        ui->no_requests_mark->hide();
        requests_list.clear();

        for (auto& request:core->getRequests())
        {
            requests_list.append(request);
        }
        request_list_model->setStringList(requests_list);
        ui->requests_line_list->setModel(request_list_model);
        requests_ready = true;
        readinessCheck();
    }
}

//Setting text to answers' path field
void MainWindow::setAnswersPath()
{
    ui->answers_path_edit->setText(conf_ui->answers_path);
}

//Checking readiness
void MainWindow::readinessCheck()
{
    ui->search_button->setEnabled(requests_ready&&config_ready);
}

//Calling error window of configurations
void MainWindow::configError()
{
    ui->no_config_mark->show();
    char status = core->getEngineStatus();
    switch (status)
    {
        case ConverterStatus::CONFIG_MISSED:
        case ConverterStatus::CONFIG_FIELD_MISSED:
        {
            QMessageBox* error_message = new QMessageBox();
            error_message->setText(status&ConverterStatus::CONFIG_MISSED ? "Missing config.json"    //Текст ошибки - Отсутствует файл настроек
                                                                    :"Corrupted config.json");      //Текст ошибки - Повреждён файл настроек
            error_message->setInformativeText("Couldn`t find config.json, do you wish to"           //Сообщение об ошибке
                                              " configure new one or to open existing?");

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

//Make search
void MainWindow::search()
{
//Removed to engine_core.cpp
//    index->UpdateDocumentBase(converter->GetTextDocuments());                   //Обновляем список документов в индексе
//    server = new SearchServer(*index);                                          //Создаём поисковый сервер
//    server->setMaxResponse(converter->GetResponsesLimit());                     //Устанавливаем максимальное количество результатов
//    search_result = (server->search(converter->GetRequests()));                 //Проводим поиск и записываем результат

    int rec_id = 0;
    ui->results_tree_widget->clear();
    QTreeWidgetItem* main_item= new QTreeWidgetItem(ui->results_tree_widget);
    main_item->setText(0,"Search results");

    for (auto& result : search_result)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(main_item);
        item->setText(0,QString("request " + QString::number(rec_id)));
        QTreeWidgetItem* is_result = new QTreeWidgetItem(item);
        is_result->setText(0,result.empty()? "result: false":"result: true");
        if (!result.empty())
        {
            for (auto& line : result)
            {
                QTreeWidgetItem* record = new QTreeWidgetItem(is_result);
                record->setSizeHint(0,QSize(40,10));

                record->setText(0,"doc_id: "+QString::number(line.doc_id) +", rank: " + QString::number(line.rank));
            }
        }
        ++rec_id;
    }
    ui->results_tree_widget->setItemWidget(main_item,0,nullptr);
    ui->save_button->setEnabled(true);
}

//Saving result
void MainWindow::saveResult()
{
    core->putAnswers(search_result);
}

//Calling requests error
void MainWindow::requestsError()
{
    ui->no_requests_mark->show();
    char status = core->getEngineStatus();
    QMessageBox* error_message = new QMessageBox();
    error_message->setText(status&ConverterStatus::REQUESTS_MISSED?"requesets.json is missing!"
                                                            :"requests.json is corrupted!");
    error_message->setInformativeText("Please add at least one request");
    error_message->setStandardButtons(QMessageBox::Ok);
    error_message->setDefaultButton(QMessageBox::Ok);
    error_message->exec();
    delete error_message;
}

