#ifndef CONVERTER_JSON
#define CONVERTER_JSON
#include "..\\include\\converter_json.h"
#include "..\\include\\file_index.h"

#include <fstream>
#include <exception>
#include <iostream>
#include <sstream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

ConverterJSON::~ConverterJSON(){}

//Указать новый файл конфигурации
void ConverterJSON::setConfigPath(QString new_path)
{
    if (config_file_path != new_path)
    {
      config_file_path = new_path;
      initializeConfig();
      emit configUpdated();
    }
}

//Указать новый файл запросов
void ConverterJSON::setRequestsPath(QString new_path)
{
    if (requests_file_path != new_path)
    {
      requests_file_path = new_path;
      initializeRequests();
      emit requestsUpdated();
    }
}

//Указать новый файл ответов
void ConverterJSON::setAnswersPath(QString new_path)
{
    if (answers_file_path != new_path)
    {
        answers_file_path = new_path;
        emit answersUpdated();
    }
}

//Проверка на инициализацию
bool ConverterJSON::isInitialized()
{
    return isConfigInitialized() && isRequestsInitialized();
}

//Проверка инициализации файла config.json
bool ConverterJSON::isConfigInitialized()
{
    return (!(engine_status&ConverterStatus::CONFIG_FIELD_MISSED)
        &&!(engine_status&ConverterStatus::CONFIG_MISSED)
        &&!(engine_status&ConverterStatus::SEARCH_FILES_MISSED));
}

//Проверка инициализации файла requests.json
bool ConverterJSON::isRequestsInitialized()
{
    return (!(engine_status&ConverterStatus::REQUESTS_MISSED)
        &&!(engine_status&ConverterStatus::REQUESTS_EMPTY));
}

//Указать максимальное колиество ответов
void ConverterJSON::setMaxRequests(int new_max)
{
    if (max_responses != new_max)
        {max_responses = new_max;}
}

//Получить тексты документов
QList<QString> ConverterJSON::GetTextDocuments()
{
    if (!isConfigInitialized())		//Проверка на инициализированность
        {initializeConfig();}
    return fileList;                //Возвращаем список содержимого файлов
}

//Получить количество ответов
int ConverterJSON::GetResponsesLimit()
{
    if (!isConfigInitialized())		//Проверка на инициализированность
        {initializeConfig();}
    return max_responses;           //Возвращаем максимальное количество запросов
}

//Полуить список запросов
QList<QString> ConverterJSON::GetRequests()
{
    initializeRequests();
    return requests;		//Возвращаем содержимое запросов
}

//Сохранить ответ в answers.json
void ConverterJSON::putAnswers(QList<QList<RelativeIndex>> answers)
{
    QJsonDocument answers_template;				//Создаём шаблон для заполнения ответа
    QFile answer_file(answers_file_path);       //Открываем файл для записи ответа

    if (answer_file.open(QIODevice::WriteOnly|QIODevice::Text))     //Проверка на открытые
    {
        QJsonArray answer_array;
        int counter = 0;
        // Проходим по вектору ответов и заполнгяем шаблон
        for (auto it = answers.begin(); it != answers.end(); it++, counter++)
        {
            if (it->size() == 0) // Если ответ пустой, значит ничего не нашлось
            {
                QJsonObject answer, result;
                result.insert("result", false);
                answer.insert(makeRequestNumber(counter),result);
                answer_array.append(answer);
            }
            else // Заполняем поля ответа
            {
                QJsonObject answer, res;
                QJsonArray res_array;
                for(auto& pair : *it)
                {
                    res.insert("docid",pair.doc_id);
                    res.insert("rank",pair.rank);
                    res_array.append(res);
                }
                answer.insert(makeRequestNumber(counter),res_array);
                answer_array.append(answer);
            }
        }
        QJsonObject output;
        output.insert("answers",answer_array);
        answers_template.setObject(output);
        QByteArray out_stream;                  //Для записи в файл
        out_stream = answers_template.toJson(QJsonDocument::Indented);
        answer_file.write(out_stream);          //Записываем шаблон в файл
        answer_file.close();                    //Закрываем файл
    }
}

//Сохранить конфигурацию в файл config.json
void ConverterJSON::putConfig(const ConfigList settings, QString conf_file)
{
    QJsonDocument config_template;      //Создаём шаблон JSON документа
    QFile config_file(conf_file);
    if (config_file.open(QIODevice::WriteOnly|QIODevice::Text)) //Проверка на открытие
    {
        QJsonObject configs,output;     //Создаём объекты для вставки
        QJsonArray json_files;          //Создаём массив для вставки
        //Заполняем поля
        configs.insert("name",settings.enegine_name);
        configs.insert("version",settings.engine_ver);
        configs.insert("max_responses", settings.max_responses);
        //Заполняем массив файлов
        for(auto& file : settings.files)
        {
            json_files.append(file);
        }
        //Вставляем в итоговый объект конфигурации и список файлов
        output.insert("config",configs);
        output.insert("files", json_files);
        config_template.setObject(output);  //Назначаем объект в шаблоне
        QByteArray out_stream;              //Для записи в файл
        out_stream = config_template.toJson(QJsonDocument::Indented);
        config_file.write(out_stream);      //Записываем в файл JSON документ
        config_file.close();                //Закрываем файл
        emit configUpdated();               //Отправляем сигнал об обновлении файла config.json
    }
}

//Сохранить запросы в requests.json
void ConverterJSON::putRequests(const QList<QString> requests)
{
    QFile requests_file(requests_file_path);
    if (requests_file.open(QIODevice::WriteOnly | QIODevice::Text)) //Проверка на открытие
    {
        QJsonDocument requests_template;    //Создаём шаблон JSON документа
        QJsonObject output;                 //Создаём выходной объект
        QJsonArray requests_list;           //Создаём массив запросов
        //Заполняем массив запросов
        for (auto& request:requests)
        {
            requests_list.append(request);
        }        
        output.insert("requests",requests_list);    //Вставляем массив в объект
        requests_template.setObject(output);        //Назанчаем объект для JSON документа
        QByteArray out_stream(requests_template.toJson(QJsonDocument::Indented));
        requests_file.write(out_stream);    //Записываем в файл
        requests_file.close();              //Закрываем файл
        emit requestsUpdated();             //Отправляем сигнал об изменении файла requests.json
    }
}

//Инициализировать конвертер из файла config.json
void ConverterJSON::initializeConfig()
{
    //Загружаем данные из config.json    
    {        
        QFile config_file(config_file_path);        
        if (!config_file.open(QIODevice::ReadOnly | QIODevice::Text))   //Проверка на открытие
        {
            engine_status |= ConverterStatus::CONFIG_MISSED;        //Выставляем статус
        }
        else
        {
            QByteArray in_stream;   //Для чтения из файла
            engine_status &= ~ConverterStatus::CONFIG_MISSED;       //Выставляем статус
            in_stream = config_file.readAll();                      //Читаем файл
            config_file.close();                                    //Закрываем файл
            QJsonDocument configuration (QJsonDocument::fromJson(in_stream));   //Создаём JSON документ
            if (configuration.isNull())                             //Проверка на валидность
            {
                engine_status |= ConverterStatus::CONFIG_FIELD_MISSED   //Выставляем статус
                                |ConverterStatus::SEARCH_FILES_MISSED;
            }
            else if (!configuration.isObject()                      //Проверяем наличие поля "config"
                    ||!configuration.object().contains("config"))
            {
                engine_status |= ConverterStatus::CONFIG_FIELD_MISSED;  //Выставляем статус
            }
            else
            {  //Заполняем поля конфигурации
                engine_status &= ~ConverterStatus::CONFIG_FIELD_MISSED; //Выставляем статус
                loadConfigs(configuration);                             //Загружаем конфигурации
            }

            if (!configuration.object().contains("files")           //Проверяем наличие поля "files"
                ||!configuration.object()["files"].isArray())
            {
                engine_status |= ConverterStatus::SEARCH_FILES_MISSED;  //Выставляем статус
            }
            else
            {
                engine_status &= ~ConverterStatus::SEARCH_FILES_MISSED; //Выставляем статус
                loadSearchFiles(configuration);                         //Загружаем список файлов для поиска
            }
            //Заполняем список содержимого файлов по которым будет производиться поиск
            for (auto& file : file_paths)
            {
                QFile file_to_search(file);
                if (!file_to_search.open(QIODevice::ReadOnly | QIODevice::Text)) //Проверка на открытие
                {
                    emit fileOpenFailure(file);     //Отправляем сигнал об отсуутствии
                    file_to_search.close();         //Закрываем файл
                }
                else
                {
                    QTextStream file_stream(&file_to_search);

                    QString content = file_stream.readAll();   //Строка с содержимым файла
                    file_to_search.close();                    //Закрываем файл
                    fileList.append(content.toLower());        //Добавляем содержимое документа в список содержимого
                }
            }
        }
    }
}

//Инициализировать конвертер из файла requests.json
void ConverterJSON::initializeRequests()
{
    //Загружаем данные из requests.json
    QFile requests_file(requests_file_path);    //Открываем файл с поисковыми запросами
    if (!requests_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        requests_file.close(); // Закрываем файл запросов
        engine_status |= ConverterStatus::REQUESTS_MISSED;  //Выставляем статус
    }
    else
    {
        engine_status &=~ConverterStatus::REQUESTS_MISSED;  //Выставляем статус
        QByteArray in_stream;                               //Для чтения из файла
        in_stream = requests_file.readAll();                //Читаем файл
        requests_file.close();                              //Закрываем файл запросов
        QJsonDocument json_requests(QJsonDocument::fromJson(in_stream));  //Создаём объект для чтения из файла запроса //Creating an object for reading request file
        // Заполняем объект содержимым файла запросов
        if (json_requests.isNull())                         //Проверка на валидность
            engine_status |= ConverterStatus::REQUESTS_EMPTY;
        else if (!json_requests.isObject()                  //Проверяем поле "requests"
                 || !json_requests.object().contains("requests"))
            engine_status |= ConverterStatus::REQUESTS_EMPTY;   //Выставляем статус
        else
        {
            auto json_requests_field = json_requests.object()["requests"].toArray();
            if (json_requests_field.isEmpty())              //Проверяем на наличие записей
            {
                engine_status |= ConverterStatus::REQUESTS_EMPTY;   //Выставляем статус
            }
            else
            {                
                engine_status &=~ConverterStatus::REQUESTS_EMPTY;   //Выставляем статус
                requests.clear();                                   //Очищаем список запросов
                // Заполняем список запросов
                for (auto it = json_requests_field.begin();
                it != json_requests_field.end();
                it++)
                {
                    requests.append(it->toString());
                }
            }
        }
    }
}

//Получить статус конвертера
char ConverterJSON::getEngineStatus()
{return engine_status;}

//Инициализировать конвертер
void ConverterJSON::initialize()
{
    initializeConfig();
    initializeRequests();
}

//Получить имя поискового двигателя
QString ConverterJSON::getEngineName()
{
    if (!isConfigInitialized())		//Проверка на инициализированность //Initialization checking
        {initializeConfig();}
    if (engine_status&ConverterStatus::ENGINE_NAME_MISSED
        ||engine_name =="")         //Проверка на наличие названия
        {return "Unknown";}
    return engine_name;
}

//Получить версию поискового двигателя
QString ConverterJSON::getEngineVersion()
{
    if (!isConfigInitialized())		//Проверка на инициализированность //Initialization checking
        {initializeConfig();}
    if (engine_status&ConverterStatus::ENGINE_VERSION_MISSED
        ||engine_version == "")     //Проверка на наличие названия
        {return "Unknown";}
    return engine_version;
}

//Получить пути к файлам для поиска
QList<QString> ConverterJSON::getFilesPaths()
{
    loadSearchFiles();  //Загружаем список файлов
    return file_paths;
}

//Загрузить поля config из config.json
void ConverterJSON::loadConfigs()
{
    if (!(engine_status&ConverterStatus::CONFIG_MISSED)             //Проверка статуса
        &&!(engine_status&ConverterStatus::CONFIG_FIELD_MISSED))
    {
        QFile config(config_file_path);
        if (config.open(QIODevice::ReadOnly | QIODevice::Text))     //Проверка открытия
        {
            //Читаем файл конфигурации
            QJsonDocument configuration(QJsonDocument::fromJson(QByteArray(config.readAll())));
            config.close();                 //Закрываем файл
            loadConfigs(configuration);     //Загружаем настройки
        }
    }
}

//Загрузить поля из JSON документа
void ConverterJSON::loadConfigs(QJsonDocument& configuration)
{
    auto json_config_field = configuration.object()["config"];      //Получаем объект из JSON документа
    if (json_config_field.toObject().contains("name"))
    {
        engine_name = json_config_field.toObject()["name"].toString();  //Записываем имя поискового двигателя
        engine_status &= ~ConverterStatus::ENGINE_NAME_MISSED;      //Выставляем статус
    }
    else
        engine_status |= ConverterStatus::ENGINE_NAME_MISSED;       //Выставляем статус

    if (json_config_field.toObject().contains("version"))           //Проверяем наличие поля "version"
    {
        engine_version = json_config_field.toObject()["version"].toString();    //Записываем версию поискового двигателя
        engine_status &= ~ConverterStatus::ENGINE_VERSION_MISSED;   //Выставляем статус
    }
    else
        engine_status |= ConverterStatus::ENGINE_VERSION_MISSED;    //Выставляем статус

    if (json_config_field.toObject().contains("max_responses"))
    {
        max_responses = json_config_field.toObject()["max_responses"].toInt();  //Записываем максимавльное число запросов //Filling number of maximum responses
        engine_status &= ~ConverterStatus::MAX_RESPONSES_MISSED;    //Выставляем статус
    }
    else
        engine_status |= ConverterStatus::MAX_RESPONSES_MISSED;     //Выставляем статус
}

//Загрузить поля files из config.json
void ConverterJSON::loadSearchFiles()
{
    if (!(engine_status&ConverterStatus::CONFIG_MISSED)             //Проверяем статус
        &&!(engine_status&ConverterStatus::SEARCH_FILES_MISSED))
    {
        QFile config(config_file_path);
        if (config.open(QIODevice::ReadOnly | QIODevice::Text))     //Проверка на открытие
        {
            //Загружаем JSON документ из файла
            QJsonDocument configuration(QJsonDocument::fromJson(QByteArray(config.readAll())));
            config.close();                     //Закрываем файл
            loadSearchFiles(configuration);     //Загружаем список фвайлов
        }
    }
}

//Загрузить поля files из JSON документа
void ConverterJSON::loadSearchFiles(QJsonDocument& configuration)
{
    auto json_fiels_field = configuration.object()["files"].toArray();
    file_paths.clear();                 //Очищаем список путей
    if (!json_fiels_field.isEmpty())    //Проверка на наличие записей
    {
        //Заполняем список
        for (auto it = json_fiels_field.begin();it != json_fiels_field.end();it++)
            file_paths.append(it->toString());
    }
    else
        engine_status |= ConverterStatus::SEARCH_FILES_MISSED;  //Выставляем статус
}

//Установить путь к файлу запросов
QString ConverterJSON::getRequestsPath()
{
    return requests_file_path;
}

//Создать строковый номер запроса
QString ConverterJSON::makeRequestNumber(std::size_t number)
{
    QString num;
    num.setNum(number);                     //Переводим число в строку
    //Добавляем нли при необходимости
    if (number < 10)
        return QString("request00" + num);
    else if (number < 100)
        return QString("request0" + num);
    else
        return QString("request" + num);
}
#endif
