#include "mainwindow.h"
#include "./ui/ui_main_window.cpp"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);                              //Настройка пользовательского интерфейса
    ui->save_button->setEnabled(false);             //Выключаем кнопку Save
    r_dialog = new RequestAddDialog(this);          //Создаём диалог добавления запросов
    converter = new ConverterJSON();                //Создаём конвертер
    conf_ui = new ConfigWindow(this,converter);     //Создаём пользовательский интерфейс для настроек
    index = new InvertedIndex();                    //Создаём инвертированый индекс
    request_list_model = new QStringListModel;      //Создаём модель для для вывода запросов
    //Соединяем сигналы и слоты
    connect(r_dialog,&RequestAddDialog::newRequest, this, &MainWindow::modifyRequests);     //Добавление запроса
    connect(conf_ui,&ConfigWindow::openConfigRequest, this,&MainWindow::openConfig);        //Открытие настроек
    connect(conf_ui, &ConfigWindow::noConfigChanges, this, &MainWindow::checkConfigPath);   //Отсутствие изменений настроек
    connect(conf_ui, &ConfigWindow::configPathChanged, this, &MainWindow::setConfigPath);   //Изменение пути до файла config.json
    connect(converter, &ConverterJSON::configUpdated,this,&MainWindow::setConfigPath);      //Обновление пути до файла config.json
    connect(converter,&ConverterJSON::requestsUpdated,this,&MainWindow::setRequestsPath);   //Обновление пути до файла requests.json
    connect(converter,&ConverterJSON::answersUpdated,this,&MainWindow::setAnswersPath);     //Обновление пути до файла answers.json
    if (!(converter->getEngineStatus()&ConverterStatus::CONFIG_MISSED))     //Проверка наличия файла config.json
        ui->config_path_edit->setText(conf_ui->config_path);                //Выводим путь config.json в главном окне
    checkConfigPath();                              //Проверяем путь до файла config.json
    conf_ui->close();                               //Закрываем окно настроек
    ui->results_tree_widget->setColumnCount(1);     //Указываем количество столбцов в модели запросов
    loadRequests();                                 //Проверка загрузки запросов
    readynessCheck();                               //Проверка готовности
}

MainWindow::~MainWindow()
{
    if (request_list_model)
        delete request_list_model;      //Удаляем модель запросов
    if (ui)
        delete ui;                      //Удаляем пользовательский интерфейс
    if (server)
        delete server;                  //Удаляем поисковый сервер
    if (converter)
        delete converter;               //Удаляем конвертер
    if (r_dialog)
        delete r_dialog;                //Удаляем диалог добавления запросов
}

//Проверяем конфигурацию
void MainWindow::checkConfigPath()
{
    if (converter->getEngineStatus()&ConverterStatus::CONFIG_MISSED)    //Проверка наличия файла config.json
    {
        ui->config_path_edit->clear();      //Очищаем путь до файла настроек в главном окне
        ui->no_config_mark->show();         //Включаем иконку ошибки для файла настроек
        config_ready = false;               //Сбрасываем флаг готовности настроек
    }
    else if ((converter->getEngineStatus()&ConverterStatus::CONFIG_FIELD_MISSED)    //Проверяем наличие полей настроек и список файлов
            ||(converter->getEngineStatus()&ConverterStatus::SEARCH_FILES_MISSED)
            )
    {
        ui->no_config_mark->show();         //Включаем иконку ошибки для файла настроек
        config_ready = false;               //Сбрасываем флаг готовности настроек
        configError();                      //Открываем диалог ошибки файла config.json
    }
    readynessCheck();                       //Проверка готовногсти
}

//Инициализируем поисковый двигатель
void MainWindow::initializeSearchEngine()
{
    if (!converter->isConfigInitialized())  //Проверка инициализации настроек
    {
        configError();                      //Открываем диалог ошибки файла config.json
        config_ready = false;               //Сбрасываем флаг готовности настроек
    }
    else
    {
        ui->no_config_mark->hide();         //Выключаем иконку ошибки для файла настроек
        config_ready = true;                //Устанавливаем флаг готовности настроек
    }

    if (!converter->isRequestsInitialized())    //Проверка инициализации запросов
    {
        requestsError();                    //Открываем сообщение об ошибке запросов
        requests_ready = false;             //Сбрасываем флаг готовности запросов
    }
    else
    {
        ui->no_requests_mark->hide();       //Выключаем иконку ошибки для файла запросов
        requests_ready = true;              //Устанавливаем флаг готовности запросов
    }

    readynessCheck();                       //Проверка готовности
}

//Открыть файл конфигурации
void MainWindow::openConfig()
{
    QFileDialog* dlg = new QFileDialog();           //Создаём диалоговое окно для открытия файла
    dlg->setNameFilter(tr("JSON file (*.json)"));   //Устанавливаем фильтр
    if (dlg->exec())
    {
        auto config_file = dlg->selectedFiles();    //Получаем список выбранныфх файлов
        conf_ui->config_path = config_file[0];      //Устанавливаем в главном окне путь до файла
        converter->setConfigPath(config_file[0]);   //Задаём путь до файла для конвертера
        conf_ui->fillSettings();                    //Заполняем настройки
        conf_ui->fillFields();                      //Заполняем поля окна настроек
        conf_ui->hide();                            //Скрываем окно настроек
    }
    dlg->close();                                   //Закрываем диалоговое окно
    delete dlg;                                     //Удаляем диалоговое окно
    return checkConfigPath();                       //Выход с проверкой готовности
}

//Открыть файл запросов
void MainWindow::openRequests()
{
    QFileDialog* dlg = new QFileDialog(this);       //Создаём диалоговое окно для открытия файла
    dlg->setNameFilter(tr("JSON file (*.json)"));   //Устанавливаем фильтр
    if (dlg->exec())
    {
        auto requests_file = dlg->selectedFiles();                  //Получаем список выбранныфх файлов
        conf_ui->requests_path = requests_file[0];                  //
        ui->requests_path_edit->setText(conf_ui->requests_path);    //Устанавливаем в главном окне путь до файла
        converter->setRequestsPath(conf_ui->requests_path);         //Задаём путь до файла для конвертера
    }
    delete dlg;                                                     //Закрываем диалоговое окно
    if (converter->isRequestsInitialized()                          //Проверка готовности запросов
        &&!converter->GetRequests().empty())
    {
        requests_ready = true;                                      //Устанавливаем флаг готовности запросов
        loadRequests();                                             //Загружаем запросы
    }
    else
    {
        requests_list.clear();                                      //Очищаем список запросов
        request_list_model->setStringList(requests_list);           //Устанавливаем пустой список для модели запросов
        ui->requests_line_list->setModel(request_list_model);       //Стираем список в главном окне
        ui->no_requests_mark->show();                               //Устанавливаем иконку ошибки файла запросов
        requests_ready = false;                                     //Сбрасываем флаг готовности запросов
    }
}

//Открыть файл ответов
void MainWindow::openAnswers()
{
    QFileDialog* dlg = new QFileDialog(this);               //Создаём диалоговое окно для открытия файла
    dlg->setNameFilter(tr("JSON file (*.json)"));           //Устанавливаем фильтр
    if (dlg->exec())
    {
        auto answers_file = dlg->selectedFiles();           //Получаем список выбранныфх файлов
        conf_ui->answers_path = answers_file[0];            //Устанавливаем в окне настроек путь до файла
    }
    delete dlg;                                             //Удаляем диалоговое окно
    ui->answers_path_edit->setText(conf_ui->answers_path);  //Устанавливаем в главном окне путь до файла
    converter->setAnswersPath(conf_ui->answers_path);       //Задаём новый путь в конвертере
}

//Добавить запрос
void MainWindow::addRequests()
{
    r_dialog->show();           //Показать окно добавления запроса
}

//Удалить запрос
void MainWindow::deleteRequests()
{
    QItemSelectionModel* selection = ui->requests_line_list->selectionModel();   //Выделенный элемент
    if (!selection->selectedIndexes().empty())                  //Проверяем что выделение не пустое
    {
        QModelIndexList index = selection->selectedIndexes();   //Список выбраных строк
        int row = index[0].row();                               //Первая выбранная строка
        request_list_model->removeRows(row,1,QModelIndex());    //Удаляем строку row
        ui->requests_line_list->setModel(request_list_model);   //Переназначаем список строк
    }
    requests_list = request_list_model->stringList();           //Загружаем в список запросов оставшиеся запросы
    converter->putRequests(requests_list);                      //Сохраняем запросы в файл запросов
    readynessCheck();                                           //Проверка готовности
    loadRequests();                                             //Загружаем запросы
}

//Изменить настройки
void MainWindow::modifyConfig()
{
    conf_ui->fillSettings();        //Заполняем настройки
    conf_ui->fillFields();          //Заполняем поля окна настроек
    conf_ui->show();                //Показываем окно настроек
}

//Изменить запросы
void MainWindow::modifyRequests(QString new_request)
{
    requests_list.append(new_request);          //Добавляем запрос в список
    converter->putRequests(requests_list);      //Сохраняем спписок запросов в файле запросов
    readynessCheck();                           //Проверка готовности
    loadRequests();                             //Загружаем настройки
}

//Установить путь к файлу конфигурвации
void MainWindow::setConfigPath()
{
    ui->config_path_edit->setText(conf_ui->config_path);    //Выводим путь к файлу настроек в главном окне
    if (converter->isConfigInitialized())                   //Проверяем инициализацию настроек
        {
            ui->no_config_mark->hide();                     //Скрываем иконку ошибки
            config_ready = true;                            //Выставляем флаг готовности настроек
            readynessCheck();                               //Проверка готовности
        }
    else
        {
            ui->no_config_mark->show();                     //Показываем иконку ошибки настроек
            config_ready = false;                           //Сбрасываем флаг готовности настроек
        }
}

//Установить поле пути к файлу запросов
void MainWindow::setRequestsPath()
{
    ui->requests_path_edit->setText(conf_ui->requests_path);    //Устанавливаем путь к файлу запросов в главном окне
    loadRequests();                                             //Загружаем запросы
}

//Загружаем запросы из requests.json
void MainWindow::loadRequests()
{
    if (!converter->GetRequests().empty())                              //Проверяем на отсутствие запросов
    {
        ui->requests_path_edit->setText(converter->getRequestsPath());  //Устанавливаем путь до файла запросов в главном окне
        ui->no_requests_mark->hide();                                   //Убираем отметку об ошибке запросов
        requests_list.clear();                                          //Очищаем список запросов
        //Заполняем запросы
        for (auto& request:converter->GetRequests())
        {
            requests_list.append(request);
        }
        request_list_model->setStringList(requests_list);               //Устанавливаем список запросов для модели
        ui->requests_line_list->setModel(request_list_model);           //Выводим список запросов в главном окне
        requests_ready = true;                                          //Устанавливаем флаг готовности запросов
        readynessCheck();                                               //Проверка готовности
    }
}

//Установить поле пути к файлу ответов
void MainWindow::setAnswersPath()
{
    ui->answers_path_edit->setText(conf_ui->answers_path);                //Выводим путь к файлу ответов в главном окне
}

//Проверяем готовность
void MainWindow::readynessCheck()
{
    ui->search_button->setEnabled(requests_ready&&config_ready);    //Включаем кнопку Search при готовности настроек и запросов
}

//Вызов ошибки файла конфигурации
void MainWindow::configError()
{
    ui->no_config_mark->show();                     //Показываем иконку ошибки настроек
    char status = converter->getEngineStatus();     //Запрашиваем состояние конвертера
    switch (status)
    {
        case ConverterStatus::CONFIG_MISSED:        //Отсутствует файл настроек
        case ConverterStatus::CONFIG_FIELD_MISSED:  //Отсутствует поле настроек
        {
            QMessageBox* error_message = new QMessageBox();     //Создаём сообщение об ошибке
            error_message->setText(status&ConverterStatus::CONFIG_MISSED ? "Missing config.json"    //Текст ошибки - Отсутствует файл настроек
                                                                    :"Corrupted config.json");      //Текст ошибки - Повреждён файл настроек
            error_message->setInformativeText("Couldn`t find config.json, do you wish to"           //Сообщение об ошибке
                                              " configure new one or to open existing?");
            //Задаём кнопки для окна ошибки
            error_message->setStandardButtons(QMessageBox::Yes|QMessageBox::Open|QMessageBox::Close);
            error_message->setDefaultButton(QMessageBox::Yes);      //Устанавливаем кнопку по умолчанию
            int result = error_message->exec();                     //Получаем выбор пользователя
            delete error_message;                                   //Удаляем сообщение об ошибке
            switch (result)                                         //Проверяем выбор пользователя
            {
            case QMessageBox::Close:                                //Выбрано Close
            {
                this->close();                                                          //Закрываем окно
                QMessageBox* critical_message = new QMessageBox();                      //Создаём сообщение об ошибке
                critical_message->setText("Missing config.json");                       //Текст ошибки
                critical_message->setInformativeText("Application will be terminated"); //Дополнительная информация
                critical_message->setStandardButtons(QMessageBox::Ok);                  //Устанавливаем кнопку
                critical_message->setDefaultButton(QMessageBox::Ok);                    //Устанавливаем кнопку по умолчанию
                critical_message->exec();                                               //Выводим окно ошибки
                delete critical_message;                                                //Удаляем окно ошибки
                conf_ui->close();                                                       //Закрываем окно настроек
                break;                                                                  //Выходим и з функции
            }
            case QMessageBox::Open:     //Выбрано Open
            {
                openConfig();           //Открыть окно выбора файла настроек
                break;
            }
            case QMessageBox::Yes:      //Выбрано Yes
            {
                modifyConfig();         //Открываем окно для изменения настроек
                break;
            }
            default:                    //По умолчанию - не должно поадать сюда
                break;
            }            
        }

        case ConverterStatus::SEARCH_FILES_MISSED:                  //Отсутствуют файлы для поиска
        {
            QMessageBox* error_message = new QMessageBox();         //Создаём сообщение об ошибке
            error_message->setText("config.json is corrupted!");    //Текст ошибки
            error_message->setInformativeText("Couldn`t find files for search! Add at least one file"); //Дополнительная информация
            error_message->setStandardButtons(QMessageBox::Ok);     //Устанавливаем кнопку
            error_message->setDefaultButton(QMessageBox::Ok);       //Устанавливаем кнопку по умолчанию
            error_message->exec();                                  //Выводим окно ошибки
            delete error_message;                                   //Удаляем окно ошибки
            modifyConfig();                                         //Открываем окно настроек для изменений
            break;                                                  //Выход из функции
        }
    }
}

//Выполнить поиск
void MainWindow::search()
{
//Removed to engine_core.cpp
//    index->UpdateDocumentBase(converter->GetTextDocuments());                   //Обновляем список документов в индексе
//    server = new SearchServer(*index);                                          //Создаём поисковый сервер
//    server->setMaxResponse(converter->GetResponsesLimit());                     //Устанавливаем максимальное количество результатов
//    search_result = (server->search(converter->GetRequests()));                 //Проводим поиск и записываем результат

    int rec_id = 0;                                                             //Создаём счётчик номеров запросов
    ui->results_tree_widget->clear();                                           //Очищаем список ответов
    QTreeWidgetItem* main_item= new QTreeWidgetItem(ui->results_tree_widget);   //Создаём виджет для запросов
    main_item->setText(0,"Search results");                                     //Добавляем поле Search result
    //Проходим по результатам поиска и заносим результат в список ответов
    for (auto& result : search_result)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(main_item);                 //Создаём поле для ответа
        item->setText(0,QString("request " + QString::number(rec_id)));         //Устанавливаем номер ответа
        QTreeWidgetItem* is_result = new QTreeWidgetItem(item);                 //Создаём поле результата
        is_result->setText(0,result.empty()? "result: false":"result: true");   //Вводим результат
        if (!result.empty())                                                    //Проверка результата на отсутствие документов
        {
            for (auto& line : result)                                           //Проходим по сиску документов и добавляем в список ответа
            {
                QTreeWidgetItem* record = new QTreeWidgetItem(is_result);       //Создаём поле документа
                record->setSizeHint(0,QSize(40,10));                            //Устанавливаем минимальный размер поля
                //Записываем результат
                record->setText(0,"doc_id: "+QString::number(line.doc_id) +", rank: " + QString::number(line.rank));
            }
        }
        ++rec_id;   //увеличиваем счётчик номера запроса
    }
    delete server;                                                  //Удаляем поисковый сервер
    server = nullptr;                                               //Убираем адресс из указзателя
    ui->results_tree_widget->setItemWidget(main_item,0,nullptr);    //Выводим результат в главном окне
    ui->save_button->setEnabled(true);                              //Активируем кнопку сохранения результата
}

//Сохранить результат
void MainWindow::saveResult()
{
    converter->putAnswers(search_result);
}

//Вызов ошибки файлов запросов
void MainWindow::requestsError()
{
    ui->no_requests_mark->show();                       //Показываем иконку ошибки запросов
    char status = converter->getEngineStatus();         //Полуаем статус конвертера
    QMessageBox* error_message = new QMessageBox();     //Создаём сообщение об ошибке
    error_message->setText(status&ConverterStatus::REQUESTS_MISSED?"requesets.json is missing!" //Текст ошибки - отсутствует файл запросов
                                                            :"requests.json is corrupted!");    //Текст ошибки - повреждён файл запросов
    error_message->setInformativeText("Please add at least one request");       //Дополнительная информация - добавьте хотябы один запрос
    error_message->setStandardButtons(QMessageBox::Ok); //Устанавливаем кнопку
    error_message->setDefaultButton(QMessageBox::Ok);   //Устанавливаем кнопку по умолчанию
    error_message->exec();                              //Выводим окно об ошибке
    delete error_message;                               //Удаляем сообщение об ошибке
}
