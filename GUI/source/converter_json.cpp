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
//ConverterJSON& ConverterJSON::getInstance()
//{
//    if (!instance)
//        instance = new ConverterJSON();
//    return *instance;
//}

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
//    if (instance)
//    {
//        delete instance;
//        instance = nullptr;
//    }
}

//Change path to a configurations' file
void ConverterJSON::changeConfigPath(QString new_path)
{
    if (config_file_path != new_path)
    {
        QFileInfo new_target(new_path);
        if (new_target.exists())
        {
            if (new_target.isFile())
            {
                if (new_target.isReadable())
                {
                    config_file_path = new_path;
                    config_loaded = false;
                    emit configPathChanged(config_file_path);
                    emit configUpdated(configCorrectionCheck());
                }
                else
                    emit fileOpenFailure(new_path);
            }
            else if (new_target.isDir())
            {
                if (new_target.isReadable())
                {
                    config_file_path = new_path + "/config.json";
                    config_loaded = false;
                    emit configPathChanged(config_file_path);
                    emit configUpdated(configCorrectionCheck());
                }
            }
            else
                emit fileOpenFailure(new_path);
        }
        else
            emit fileOpenFailure(new_path);
    }
}

//Change path to a requests' file
void ConverterJSON::changeRequestsPath(QString new_path)
{
    if (requests_file_path != new_path)
    {
        QFileInfo new_target(new_path);
                if (new_target.exists())
        {
            if (new_target.isFile())
            {
                if (new_target.isReadable())
                {
                    requests_file_path = new_path;
                    requests_loaded = false;
                    emit requestsPathChanged(requests_file_path);
                    emit requestsUpdated(requestsCorrectionCheck());
                }
                else
                    emit fileOpenFailure(new_path);
            }
            else if (new_target.isDir())
            {
                if (new_target.isReadable())
                {
                    requests_file_path = new_path + "/requests.json";
                    requests_loaded = false;
                    emit requestsPathChanged(requests_file_path);
                    emit requestsUpdated(requestsCorrectionCheck());
                }
            }
            else
                emit fileOpenFailure(new_path);
        }
        else
            emit fileOpenFailure(new_path);
    }
}

//Change path to a answers' file
void ConverterJSON::changeAnswersPath(QString new_path)
{
    if (answers_file_path != new_path)
    {
        QFileInfo new_target(new_path);
        if (new_target.exists())
        {
            if (new_target.isFile())
            {
                if (new_target.isWritable())
                {
                    answers_file_path = new_path;
                    emit answersPathChanged(answers_file_path);
                }
                else
                    emit fileOpenFailure(new_path);
            }
            else if (new_target.isDir())
            {
                if (new_target.isReadable())
                {
                    answers_file_path = new_path + "/answers.json";
                    void answersPathChanged(QString);
                }
            }
            else
                emit fileOpenFailure(new_path);
        }
        else if (new_target.isWritable())
        {
            if (new_target.isDir())
            {
                answers_file_path = new_path + "/answers.json";
                void answersPathChanged(QString);
            }
            else if (new_target.isFile())
            {
                answers_file_path = new_path;
                void answersPathChanged(QString);
            }
        }
    }
}

//Get documents' content
QStringList ConverterJSON::getTextDocuments()
{
    if (config_loaded
        || (!config_loaded && loadConfigs()))
    {
        QStringList documents_text;
        Loader::LoadFileContent(documents_text,getFilesPaths());
        return documents_text;
    }
    return QList<QString>();
}

//Get responses' limit
int ConverterJSON::getResponsesLimit()
{
    if (config_loaded
        || (!config_loaded && loadConfigs()))
    {
        auto json_config_field = configuration->object()["config"];
        return json_config_field.toObject()["max_responses"].toInt();
    }
    return MIN_RESPONSE;
}

//Get list of requests
QStringList ConverterJSON::getRequests()
{
    if (requests_loaded
    ||(!requests_loaded && loadRequests()))
    {
        QList<QString> req;
        auto json_requests_field = requests->object()["requests"].toArray();
        if (!json_requests_field.isEmpty())
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
void ConverterJSON::putAnswers(QList<QList<RelativeIndex>> answers)
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
                answer.insert(makeRequestNumber(counter),result);
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
                answer.insert(makeRequestNumber(counter),res_array);
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
void ConverterJSON::putConfig(const ConfigList settings, QString conf_file)
{
    QFileInfo target(conf_file);
    if (target.isWritable())
    {
        if ((target.exists()&&target.isDir())
            ||target.isDir())
        {
            conf_file = conf_file + "/config.json";
        }
        else if ((target.exists()&&!target.isFile())
                ||!target.isFile())
            {
                emit fileOpenFailure(conf_file);
                return;
            }
        emit configPathChanged(conf_file);
    }
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
        emit configUpdated(ConverterStatus::NO_ERRORS);
    }
    else
        emit fileOpenFailure(conf_file);
}

//Save requests to a requests' file
void ConverterJSON::putRequests(const QList<QString> in_requests)
{
    QFile requests_file(requests_file_path);
    if (requests_file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        if (!requests)
            requests = new QJsonDocument();
        QJsonObject output;
        QJsonArray requests_list;

        for (auto& request:in_requests)
        {
            requests_list.append(request);
        }        
        output.insert("requests",requests_list);
        requests->setObject(output);
        QByteArray out_stream(requests->toJson(QJsonDocument::Indented));
        requests_file.write(out_stream);
        requests_file.close();
        emit requestsUpdated(ConverterStatus::NO_ERRORS);
        requests_loaded = true;
    }
    else
        emit fileOpenFailure(requests_file_path);
}

//Get search engine's name
QString ConverterJSON::getEngineName()
{
    if (config_loaded
        || (!config_loaded && loadConfigs()))
    {
        auto json_config_field = configuration->object()["config"];
        return json_config_field.toObject()["name"].toString();
    }
    else
        return "Unknown";
}

//Get search engine's version
QString ConverterJSON::getEngineVersion()
{
    if (config_loaded
        || (!config_loaded && loadConfigs()))
    {
        auto json_config_field = configuration->object()["config"];
        return json_config_field.toObject()["version"].toString();
    }
    else
        return "Unknown";
}

//Get pathes to files for search
QStringList ConverterJSON::getFilesPaths()
{
    if (config_loaded
        || (!config_loaded && loadConfigs()))
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
QString ConverterJSON::getRequestsPath()
{
    return requests_file_path;
}

//Make request number as a string
QString ConverterJSON::makeRequestNumber(std::size_t number)
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
char ConverterJSON::configCorrectionCheck()
{
    char result = ConverterStatus::NO_ERRORS;
    QFile config(config_file_path);
    if (config.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QJsonDocument configuration(QJsonDocument::fromJson(QByteArray(config.readAll())));
        config.close();

        if (!(configuration.isNull()||configuration.isEmpty()))
        {
            if (!configuration.object().isEmpty())
            {
                if (!configuration.object().contains("files")
                    || configuration.object()["files"].toArray().isEmpty())
                {
                    result |= ConverterStatus::SEARCH_FILES_MISSED;
                }

                if (configuration.object().contains("config"))
                {
                    auto json_config_field = configuration.object()["config"];
                    if (!json_config_field.toObject().contains("name"))
                        result |= ConverterStatus::ENGINE_NAME_MISSED;

                    if (!json_config_field.toObject().contains("version"))
                        result |= ConverterStatus::ENGINE_VERSION_MISSED;

                    if (!json_config_field.toObject().contains("max_responses"))
                        result |= ConverterStatus::MAX_RESPONSES_MISSED;
                }
                else result = ConverterStatus::CONFIG_FIELD_MISSED;
            }
            else
                result |= ConverterStatus::SEARCH_FILES_MISSED|ConverterStatus::CONFIG_FIELD_MISSED;
        }
        else
            result |= ConverterStatus::SEARCH_FILES_MISSED|ConverterStatus::CONFIG_FIELD_MISSED;
    }
    else
        result |= ConverterStatus::CONFIG_MISSED;

    return result;
}

//Check the requests' file for errors
char ConverterJSON::requestsCorrectionCheck()
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
bool ConverterJSON::loadConfigs()
{
    if (!config_loaded
        && !(ConverterStatus::NO_CONFIG_ERRORS & configCorrectionCheck()))
    {
        QFile config_file(config_file_path);
        if (config_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            configuration = new QJsonDocument(QJsonDocument::fromJson(QByteArray(config_file.readAll())));
            config_file.close();
            config_loaded = true;
        }
        else
        {
            emit fileOpenFailure(config_file_path);
            config_loaded = false;
            return false;
        }
    }
    else if (!config_loaded)
        return false;
    return true;
}

//Loading requests' file
bool ConverterJSON::loadRequests()
{
    if (!requests_loaded
        && !(ConverterStatus::NO_REQUESTS_ERRORS & requestsCorrectionCheck()))
    {
        QFile requests_file(requests_file_path);
        if (requests_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            requests = new QJsonDocument(QJsonDocument::fromJson(QByteArray(requests_file.readAll().toLower())));
            requests_file.close();
            requests_loaded = true;
        }
        else
        {
            emit fileOpenFailure(requests_file_path);
            requests_loaded = false;
            return false;
        }
    }
    else if (!requests_loaded)
        return false;
    return true;
}

void Loader::LoadFileContent(QStringList& result, const QStringList& source)
{
//    destination.clear();
    QList<QFuture<QString>> open_file_threads;
    for (auto& file : source)
    {
        open_file_threads.append(QtConcurrent::run([file, result]()
        {
            QFile document_file(file);
            if (document_file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                QString content = document_file.readAll();
                document_file.close();
                content = content.toLower();
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
        result.append(thread.result());
    }
}

FileErrors Loader::checkFilePath(QString& file_path)
{
    QFileInfo file(file_path);
    if (!file.isFile())
        return FileErrors::NOT_A_FILE;
    if (file.isReadable())
        return FileErrors::READ_ERR;
    if (!file.isWritable())
        return FileErrors::WRITE_ERR;
    return FileErrors::NO_ERR;
}


QString ConverterJSON::getConfigsPath() const
{
    return QFileInfo(config_file_path).filePath();
}
QString ConverterJSON::getRequestsPath() const
{
    return QFileInfo(requests_file_path).filePath();
}
QString ConverterJSON::getAnswersPath() const
{
    return QFileInfo(answers_file_path).filePath();
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
