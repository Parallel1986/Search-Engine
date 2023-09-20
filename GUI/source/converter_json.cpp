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
#include <QtConcurrent/QtConcurrent>

//Get instance of the converter
ConverterJSON& ConverterJSON::GetInstance()
{
    if (!instance)
        instance = new ConverterJSON();
    return *instance;
}

//Destructor
ConverterJSON::~ConverterJSON()
{
    if (configuration)
    {
        delete configuration;
        configuration = nullptr;
    }
    if (requests)
    {
        delete requests;
        requests = nullptr;
    }
    if (instance)
    {
        delete instance;
        instance = nullptr;
    }
}

//Change path to a configurations' file
void ConverterJSON::ChangeConfigPath(QString new_path)
{
    if (config_file_path != new_path)
    {
      config_file_path = new_path;
      emit ConfigUpdated(ConfigCorrectionCheck());
    }
}

//Change path to a requests' file
void ConverterJSON::ChangeRequestsPath(QString new_path)
{
    if (requests_file_path != new_path)
    {
      requests_file_path = new_path;
      emit RequestsUpdated(RequestsCorrectionCheck());
    }
}

//Change path to a answers' file
void ConverterJSON::ChangeAnswersPath(QString new_path)
{
    if (answers_file_path != new_path)
    {
        answers_file_path = new_path;
        emit AnswersUpdated();
    }
}

//Get documents' content
QList<QString> ConverterJSON::GetTextDocuments()
{
    if (config_loaded
        || (!config_loaded && LoadConfigs()))
    {
        QList<QString> documents_text;
        Loader::LoadFileContent(documents_text,GetFilesPaths());
        return documents_text;
    }
    return QList<QString>();
}

//Get responses' limit
int ConverterJSON::GetResponsesLimit()
{
    if (config_loaded
        || (!config_loaded && LoadConfigs()))
    {
        auto json_config_field = configuration->object()["config"];
        return json_config_field.toObject()["max_responses"].toInt();
    }
}

//Get list of requests
QList<QString> ConverterJSON::GetRequests()
{
    if (requests_loaded
    ||(!requests_loaded && LoadRequests()))
    {
        QList<QString> req;
        auto json_requests_field = requests->object()["requests"].toArray();
        if (json_requests_field.isEmpty())              //Проверяем на наличие записей
        {
            for (auto it = json_requests_field.begin();
            it != json_requests_field.end();
            it++)
            {
                req.append(it->toString());
            }
        }
        return req;
    }
    return QList<QString>();
}

//Save answers to a answers' file
void ConverterJSON::PutAnswers(QList<QList<RelativeIndex>> answers)
{
    QJsonDocument answers_template;
    QFile answer_file(answers_file_path);

    if (answer_file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        QJsonArray answer_array;
        int counter = 0;

        for (auto it = answers.begin(); it != answers.end(); it++, counter++)
        {
            if (it->size() == 0)
            {
                QJsonObject answer, result;
                result.insert("result", false);
                answer.insert(MakeRequestNumber(counter),result);
                answer_array.append(answer);
            }
            else
            {
                QJsonObject answer, res;
                QJsonArray res_array;
                for(auto& pair : *it)
                {
                    res.insert("docid",pair.doc_id);
                    res.insert("rank",pair.rank);
                    res_array.append(res);
                }
                answer.insert(MakeRequestNumber(counter),res_array);
                answer_array.append(answer);
            }
        }
        QJsonObject output;
        output.insert("answers",answer_array);
        answers_template.setObject(output);
        QByteArray out_stream;
        out_stream = answers_template.toJson(QJsonDocument::Indented);
        answer_file.write(out_stream);
        answer_file.close();
    }
}

//Save cofniguration to a configurations' file
void ConverterJSON::PutConfig(const ConfigList settings, QString conf_file)
{
    QFile config_file(conf_file);
    if (config_file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        if (!configuration)
            configuration = new QJsonDocument();
        QJsonObject configs,output;
        QJsonArray json_files;

        configs.insert("name",settings.enegine_name);
        configs.insert("version",settings.engine_ver);
        configs.insert("max_responses", settings.max_responses);

        for(auto& file : settings.files)
        {
            json_files.append(file);
        }

        output.insert("config",configs);
        output.insert("files", json_files);

        configuration->setObject(output);
        QByteArray out_stream;
        out_stream = configuration->toJson(QJsonDocument::Indented);
        config_file.write(out_stream);
        config_file.close();

        config_loaded = true;
        config_file_path = conf_file;
        emit ConfigUpdated(ConverterStatus::NO_ERRORS);
    }
}

//Save requests to a requests' file
void ConverterJSON::PutRequests(const QList<QString> in_requests)
{
    QFile requests_file(requests_file_path);
    if (requests_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (requests)
            requests = new QJsonDocument();
        QJsonObject output;
        QJsonArray requests_list;

        for (auto& request:in_requests)
        {
            requests_list.append(request);
        }        
        output.insert("requests",requests_list);
        this->requests->setObject(output);
        QByteArray out_stream(requests->toJson(QJsonDocument::Indented));
        requests_file.write(out_stream);
        requests_file.close();
        emit RequestsUpdated(ConverterStatus::NO_ERRORS);
        requests_loaded = true;
    }
}

//Get search engine's name
QString ConverterJSON::GetEngineName()
{
    if (config_loaded
        || (!config_loaded && LoadConfigs()))
    {
        auto json_config_field = configuration->object()["config"];
        return json_config_field.toObject()["name"].toString();
    }
    else
        return "Unknown";
}

//Get search engine's version
QString ConverterJSON::GetEngineVersion()
{
    if (config_loaded
        || (!config_loaded && LoadConfigs()))
    {
        auto json_config_field = configuration->object()["config"];
        return json_config_field.toObject()["version"].toString();
    }
    else
        return "Unknown";
}

//Get pathes to files for search
QList<QString> ConverterJSON::GetFilesPaths()
{
    if (config_loaded
        || (!config_loaded && LoadConfigs()))
    {
        auto json_fiels_field = configuration->object()["files"].toArray();
        QList<QString> file_paths;
        for (auto it = json_fiels_field.begin();it != json_fiels_field.end();it++)
            file_paths.append(it->toString());
        return file_paths;
    }
    else
        return QList<QString>();
}

//Get path to requests file
QString ConverterJSON::GetRequestsPath()
{
    return requests_file_path;
}

//Make request number as a string
QString ConverterJSON::MakeRequestNumber(std::size_t number)
{
    QString num;
    num.setNum(number);

    if (number < 10)
        return QString("request00" + num);
    else if (number < 100)
        return QString("request0" + num);
    else
        return QString("request" + num);
}

//Check the configurations' file for errors
char ConverterJSON::ConfigCorrectionCheck()
{
    char result = ConverterStatus::NO_ERRORS;
    QFile config(config_file_path);
    if (config.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QJsonDocument configuration(QJsonDocument::fromJson(QByteArray(config.readAll())));
        config.close();

        auto json_files_field = configuration.object()["files"].toArray();
        auto json_config_field = configuration.object()["config"];

        if (!json_config_field.toObject().contains("name"))
            result |= ConverterStatus::ENGINE_NAME_MISSED;

        if (!json_config_field.toObject().contains("version"))
            result |= ConverterStatus::ENGINE_VERSION_MISSED;

        if (!json_config_field.toObject().contains("max_responses"))
            result |= ConverterStatus::MAX_RESPONSES_MISSED;

        if (json_files_field.isEmpty())
            result |= ConverterStatus::SEARCH_FILES_MISSED;
    }
    else
        result |= ConverterStatus::CONFIG_MISSED;

    return result;
}

//Check the requests' file for errors
char ConverterJSON::RequestsCorrectionCheck()
{
    char result = ConverterStatus::NO_ERRORS;
    QFile requests_file(requests_file_path);
    if (requests_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray in_stream;
        in_stream = requests_file.readAll();
        requests_file.close();

        QJsonDocument json_requests(QJsonDocument::fromJson(in_stream));

        if (json_requests.isNull())
            result |= ConverterStatus::REQUESTS_EMPTY;
        else if (!json_requests.isObject()
                 || !json_requests.object().contains("requests"))
            result |= ConverterStatus::REQUESTS_EMPTY;
    }
    else
        result |= ConverterStatus::REQUESTS_MISSED;

    return result;
}

//Loading configurations' file
bool ConverterJSON::LoadConfigs()
{
    if (!config_loaded
        && (ConverterStatus::NO_CONFIG_ERRORS & ConfigCorrectionCheck()))
    {
        QFile config_file(config_file_path);
        if (config_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            *configuration = QJsonDocument::fromJson(QByteArray(config_file.readAll()));
            config_file.close();
            config_loaded = true;
        }
        else
        {
            emit FileOpenFailure(config_file_path);
            config_loaded = false;
            return false;
        }
    }
    return true;
}

//Loading requests' file
bool ConverterJSON::LoadRequests()
{
    if (!requests_loaded
        && (ConverterStatus::NO_REQUESTS_ERRORS & RequestsCorrectionCheck()))
    {
        QFile requests_file(requests_file_path);
        if (requests_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            *requests = QJsonDocument::fromJson(QByteArray(requests_file.readAll()));
            requests_file.close();
            requests_loaded = true;
        }
        else
        {
            emit FileOpenFailure(requests_file_path);
            requests_loaded = false;
            return false;
        }
    }
    return true;
}

void Loader::LoadFileContent(QList<QString>& destination,const QList<QString>& source_pathes)
{
    destination.clear();
    QList<QFuture<QString>> open_file_threads;
    for (auto& file : source_pathes)
    {
        open_file_threads.append(QtConcurrent::run([file, destination]()
        {
            QFile document_file(file);
            if (document_file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QString content = document_file.readAll();
                return content;
            }
            else
            {
                return QString();
            }
        }));
    }
    for (auto& thread : open_file_threads)
    {
        thread.waitForFinished();
    }
    for (auto& thread : open_file_threads)
    {
        destination.append(thread.result());
    }
}

//Removed to engine_core.cpp
////Проверка на инициализацию
//bool ConverterJSON::IsInitialized()
//{
//    return IsConfigInitialized() && IsRequestsInitialized();
//}

////Проверка инициализации файла config.json
//bool ConverterJSON::IsConfigInitialized()
//{
//    return (!(engine_status&ConverterStatus::CONFIG_FIELD_MISSED)
//        &&!(engine_status&ConverterStatus::CONFIG_MISSED)
//        &&!(engine_status&ConverterStatus::SEARCH_FILES_MISSED));
//}

////Проверка инициализации файла requests.json
//bool ConverterJSON::IsRequestsInitialized()
//{
//    return (!(engine_status&ConverterStatus::REQUESTS_MISSED)
//        &&!(engine_status&ConverterStatus::REQUESTS_EMPTY));
//}

////Указать максимальное колиество ответов
//void ConverterJSON::SetMaxRequests(int new_max)
//{
//    if (max_responses != new_max)
//        {max_responses = new_max;}
//}

////Загрузить поля config из config.json
//void ConverterJSON::LoadConfigs()
//{
//    if (!(engine_status&ConverterStatus::CONFIG_MISSED)             //Проверка статуса
//        &&!(engine_status&ConverterStatus::CONFIG_FIELD_MISSED))
//    {
//        QFile config(config_file_path);
//        if (config.open(QIODevice::ReadOnly | QIODevice::Text))     //Проверка открытия
//        {
//            //Читаем файл конфигурации
//            QJsonDocument configuration(QJsonDocument::fromJson(QByteArray(config.readAll())));
//            config.close();                 //Закрываем файл
//            LoadConfigs(configuration);     //Загружаем настройки
//        }
//    }
//}

////Загрузить поля из JSON документа
//void ConverterJSON::LoadConfigs(QJsonDocument& configuration)
//{
//    auto json_config_field = configuration.object()["config"];      //Получаем объект из JSON документа
//    if (json_config_field.toObject().contains("name"))
//    {
//        engine_name = json_config_field.toObject()["name"].toString();  //Записываем имя поискового двигателя
//        engine_status &= ~ConverterStatus::ENGINE_NAME_MISSED;      //Выставляем статус
//    }
//    else
//        engine_status |= ConverterStatus::ENGINE_NAME_MISSED;       //Выставляем статус

//    if (json_config_field.toObject().contains("version"))           //Проверяем наличие поля "version"
//    {
//        engine_version = json_config_field.toObject()["version"].toString();    //Записываем версию поискового двигателя
//        engine_status &= ~ConverterStatus::ENGINE_VERSION_MISSED;   //Выставляем статус
//    }
//    else
//        engine_status |= ConverterStatus::ENGINE_VERSION_MISSED;    //Выставляем статус

//    if (json_config_field.toObject().contains("max_responses"))
//    {
//        max_responses = json_config_field.toObject()["max_responses"].toInt();  //Записываем максимавльное число запросов //Filling number of maximum responses
//        engine_status &= ~ConverterStatus::MAX_RESPONSES_MISSED;    //Выставляем статус
//    }
//    else
//        engine_status |= ConverterStatus::MAX_RESPONSES_MISSED;     //Выставляем статус
//}

////Загрузить поля files из config.json
//void ConverterJSON::LoadSearchFiles()
//{
//    if (!(engine_status&ConverterStatus::CONFIG_MISSED)             //Проверяем статус
//        &&!(engine_status&ConverterStatus::SEARCH_FILES_MISSED))
//    {
//        QFile config(config_file_path);
//        if (config.open(QIODevice::ReadOnly | QIODevice::Text))     //Проверка на открытие
//        {
//            //Загружаем JSON документ из файла
//            QJsonDocument configuration(QJsonDocument::fromJson(QByteArray(config.readAll())));
//            config.close();                     //Закрываем файл
//            LoadSearchFiles(configuration);     //Загружаем список фвайлов
//        }
//    }
//}

////Загрузить поля files из JSON документа
//void ConverterJSON::LoadSearchFiles(QJsonDocument& configuration)
//{
//    auto json_fiels_field = configuration.object()["files"].toArray();
//    file_paths.clear();                 //Очищаем список путей
//    if (!json_fiels_field.isEmpty())    //Проверка на наличие записей
//    {
//        //Заполняем список
//        for (auto it = json_fiels_field.begin();it != json_fiels_field.end();it++)
//            file_paths.append(it->toString());
//    }
//    else
//        engine_status |= ConverterStatus::SEARCH_FILES_MISSED;  //Выставляем статус
//}

////Инициализировать конвертер из файла config.json
//void ConverterJSON::InitializeConfig()
//{
//    //Загружаем данные из config.json
//    {
//        QFile config_file(config_file_path);
//        if (!config_file.open(QIODevice::ReadOnly | QIODevice::Text))   //Проверка на открытие
//        {
//            engine_status |= ConverterStatus::CONFIG_MISSED;        //Выставляем статус
//        }
//        else
//        {
//            QByteArray in_stream;   //Для чтения из файла
//            engine_status &= ~ConverterStatus::CONFIG_MISSED;       //Выставляем статус
//            in_stream = config_file.readAll();                      //Читаем файл
//            config_file.close();                                    //Закрываем файл
//            QJsonDocument configuration (QJsonDocument::fromJson(in_stream));   //Создаём JSON документ
//            if (configuration.isNull())                             //Проверка на валидность
//            {
//                engine_status |= ConverterStatus::CONFIG_FIELD_MISSED   //Выставляем статус
//                                |ConverterStatus::SEARCH_FILES_MISSED;
//            }
//            else if (!configuration.isObject()                      //Проверяем наличие поля "config"
//                    ||!configuration.object().contains("config"))
//            {
//                engine_status |= ConverterStatus::CONFIG_FIELD_MISSED;  //Выставляем статус
//            }
//            else
//            {  //Заполняем поля конфигурации
//                engine_status &= ~ConverterStatus::CONFIG_FIELD_MISSED; //Выставляем статус
//                LoadConfigs(configuration);                             //Загружаем конфигурации
//            }

//            if (!configuration.object().contains("files")           //Проверяем наличие поля "files"
//                ||!configuration.object()["files"].isArray())
//            {
//                engine_status |= ConverterStatus::SEARCH_FILES_MISSED;  //Выставляем статус
//            }
//            else
//            {
//                engine_status &= ~ConverterStatus::SEARCH_FILES_MISSED; //Выставляем статус
//                LoadSearchFiles(configuration);                         //Загружаем список файлов для поиска
//            }
//            //Заполняем список содержимого файлов по которым будет производиться поиск
//            for (auto& file : file_paths)
//            {
//                QFile file_to_search(file);
//                if (!file_to_search.open(QIODevice::ReadOnly | QIODevice::Text)) //Проверка на открытие
//                {
//                    emit FileOpenFailure(file);     //Отправляем сигнал об отсуутствии
//                    file_to_search.close();         //Закрываем файл
//                }
//                else
//                {
//                    QTextStream file_stream(&file_to_search);

//                    QString content = file_stream.readAll();   //Строка с содержимым файла
//                    file_to_search.close();                    //Закрываем файл
//                    fileList.append(content.toLower());        //Добавляем содержимое документа в список содержимого
//                }
//            }
//        }
//    }
//}

////Инициализировать конвертер из файла requests.json
//void ConverterJSON::InitializeRequests()
//{
//    //Загружаем данные из requests.json
//    QFile requests_file(requests_file_path);    //Открываем файл с поисковыми запросами
//    if (!requests_file.open(QIODevice::ReadOnly | QIODevice::Text))
//    {
//        requests_file.close(); // Закрываем файл запросов
//        engine_status |= ConverterStatus::REQUESTS_MISSED;  //Выставляем статус
//    }
//    else
//    {
//        engine_status &=~ConverterStatus::REQUESTS_MISSED;  //Выставляем статус
//        QByteArray in_stream;                               //Для чтения из файла
//        in_stream = requests_file.readAll();                //Читаем файл
//        requests_file.close();                              //Закрываем файл запросов
//        QJsonDocument json_requests(QJsonDocument::fromJson(in_stream));  //Создаём объект для чтения из файла запроса //Creating an object for reading request file
//        // Заполняем объект содержимым файла запросов
//        if (json_requests.isNull())                         //Проверка на валидность
//            engine_status |= ConverterStatus::REQUESTS_EMPTY;
//        else if (!json_requests.isObject()                  //Проверяем поле "requests"
//                 || !json_requests.object().contains("requests"))
//            engine_status |= ConverterStatus::REQUESTS_EMPTY;   //Выставляем статус
//        else
//        {
//            auto json_requests_field = json_requests.object()["requests"].toArray();
//            if (json_requests_field.isEmpty())              //Проверяем на наличие записей
//            {
//                engine_status |= ConverterStatus::REQUESTS_EMPTY;   //Выставляем статус
//            }
//            else
//            {
//                engine_status &=~ConverterStatus::REQUESTS_EMPTY;   //Выставляем статус
//                requests.clear();                                   //Очищаем список запросов
//                // Заполняем список запросов
//                for (auto it = json_requests_field.begin();
//                it != json_requests_field.end();
//                it++)
//                {
//                    requests.append(it->toString());
//                }
//            }
//        }
//    }
//}

////Получить статус конвертера
//char ConverterJSON::GetEngineStatus()
//{
//    return engine_status;
//}

////Инициализировать конвертер
//void ConverterJSON::Initialize()
//{
//    InitializeConfig();
//    InitializeRequests();
//}
#endif
