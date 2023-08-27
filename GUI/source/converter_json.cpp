#ifndef CONVERTER_JSON
#define CONVERTER_JSON
#include "..\\include\converter_json.h"

#include <fstream>
#include <exception>
#include <iostream>
#include <sstream>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

void ConverterJSON::setConfigPath(QString new_path)     //Указать новый файл конфигурации //Set new configuration`s file
{
    if (config_file_path != new_path)
    {
      config_file_path = new_path;
      initializeConfig();
      initializeCheck();
      emit configUpdated();
    }
}

ConverterJSON::~ConverterJSON(){}

void ConverterJSON::setRequestsPath(QString new_path)   //Указать новый файл запросов //Set new requests` file
{
    if (requests_file_path != new_path)
    {
      requests_file_path = new_path;
      initializeRequests();
      initializeCheck();
      emit requestsUpdated();
    }
}
void ConverterJSON::setAnswersPath(QString new_path)    //Указать новый файл ответов //Set new answers` file
{
    if (answers_file_path != new_path)
    {
        answers_file_path = new_path;
        emit answersUpdated();
    }
}
bool ConverterJSON::isInitialized()
{
    return initialized;
}

QList<QString> ConverterJSON::GetTextDocuments()
{
	if (!initialized)		//Проверка на инициализированность //Initialization checking
		initialize();	
	return fileList;		//Возвращаем список содержимого файлов //Returning content of files for searching
}

int ConverterJSON::GetResponsesLimit()
{
	if (!initialized)		//Проверка на инициализированность //Initialization checking
		initialize();
	
    return max_responses;	//Возвращаем максимальное количество запросов //Returning maximum requests` count
}

QList<QString> ConverterJSON::GetRequests()
{
	if (!initialized)		//Проверка на инициализированность //Checking initialization
		initialize();

	return requests;		//Возвращаем содержимое запросов //Returning content of requests
}

void ConverterJSON::putAnswers(QList<QList<std::pair<int, float>>> answers)
{
    QJsonDocument answers_template;				//Создаём шаблон для заполнения ответа //Creating template for filling answer
    QFile answer_file(answers_file_path);	//Открываем файл для записи ответа //Opening file for recording
    answer_file.open(QIODevice::WriteOnly|QIODevice::Text);
    if (answer_file.isOpen())
    {
        QJsonArray answer_array;
        int counter = 0;
        for (auto it = answers.begin(); it != answers.end(); it++, counter++)    // Проходим по вектору ответов и заполнгяем шаблон
                                                        // //Going through answer vector and filling template
        {
            if (it->size() == 0) // Если ответ пустой, значит ничего не нашлось //if answer is empty
            {
                QJsonObject answer, result;
                result.insert("result", false);
                answer.insert(makeRequestNumber(counter),result);
                answer_array.append(answer);
            }
            else // Заполняем поля ответа // Filling the template`s fields
            {
                QJsonObject answer, res;
                QJsonArray res_array;
                for(auto& pair : *it)
                {
                    res.insert("docid",pair.first);
                    res.insert("rank",pair.second);
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
        answer_file.write(out_stream);      //Записываем шаблон в файл //Writing the template to the file
    }
    answer_file.close();                //Закрываем файл // Closing the file
}

void ConverterJSON::putConfig(ConfigList settings,QList<QString> files, QString path)
{
    QJsonDocument config_template;
    QFile config_file(path+"\\config.json");
    config_file.open(QIODevice::WriteOnly|QIODevice::Text);
    if (config_file.isOpen())
    {
        QJsonObject configs,output;
        QJsonArray json_files;
        configs.insert("name",settings.enegine_name);
        configs.insert("version",settings.engine_ver);
        configs.insert("max_responses", settings.max_responses);
        for(auto file : files)
        {
            json_files.append(file);
        }
        output.insert("config",configs);
        output.insert("files", json_files);
        config_template.setObject(output);
        QByteArray out_stream;
        out_stream = config_template.toJson(QJsonDocument::Indented);
        config_file.write(out_stream);
    }
    config_file.close();
}

void ConverterJSON::initializeConfig()
{
    //Загружаем данные из config.json
    //Loading data from config.json
    {
        QJsonDocument configuration;
        QByteArray in_stream;
        QFile config_file(config_file_path);
        config_file.open(QIODevice::ReadOnly | QIODevice::Text);
        if (!config_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            emit configMissed();	//Если файл не открылся, отправляем сигнал об этом //If file isn`t opened sending signal about it
            config_file.close();
            engine_status |= ConverterStatus::CONFIG_MISSED;
        }
        else
        {
            while (!config_file.atEnd())
            {
                in_stream += config_file.readLine();
            }
            configuration.fromJson(in_stream);
            config_file.close();
            if (configuration.isNull())
            {
                emit configMissed();
                engine_status |= ConverterStatus::CONFIG_MISSED;
            }
            else if (!configuration.isObject())
            {
                emit configFieldMissed();
                engine_status |= ConverterStatus::CONFIG_FIELD_MISSED;
            }
            else
            {  //Заполняем поля конфигурации //Filling configuration fields
                if (!configuration.object().contains("config"))
                {
                    emit configFieldMissed();
                    engine_status |= ConverterStatus::CONFIG_FIELD_MISSED;
                }
                else
                {
                    auto json_config_field = configuration.object()["config"];
                    if (json_config_field.toObject().contains("name"))
                        engine_name = json_config_field.toObject()["name"].toString();  //Записываем имя поискового двигателя //Filling search engine`s name
                    else
                        engine_status |= ConverterStatus::ENGINE_NAME_MISSED;

                    if (json_config_field.toObject().contains("version"))
                        engine_version = json_config_field.toObject()["version"].toString();    //Записываем версию поискового двигателя //Filling search engine`s version
                    else
                        engine_status |= ConverterStatus::ENGINE_VERSION_MISSED;

                    if (json_config_field.toObject().contains("max_responses"))
                        max_responses = json_config_field.toObject()["max_responses"].toInt();  //Записываем максимавльное число запросов //Filling number of maximum responses
                    else
                        engine_status |= ConverterStatus::MAX_RESPONSES_MISSED;
                }

                if (!configuration.object().contains("files")
                    ||!configuration.object()["files"].isArray())
                {
                    emit configFilesMissed();
                    engine_status |= ConverterStatus::SEARCH_FILES_MISSED;
                }
                else
                {
                    auto json_fiels_field = configuration.object()["files"].toArray();
                    for (auto it = json_fiels_field.begin();it != json_fiels_field.end();it++)
                    {
                        file_paths.append(it->toString());
                    }
                }

                for (auto& file : file_paths)   //Заполняем список содержимого файлов по которым будет производиться поиск
                {                               //Filling list of files` content for search
                    QFile file_to_search(file);
                    file_to_search.open(QIODevice::ReadOnly | QIODevice::Text);
                    if (file_to_search.open(QIODevice::ReadOnly | QIODevice::Text)) //Если файл не удалось открыть, то отправляем сигнал об отсуутствии
                    {                                                               //Sending signal about missing if can not open the file
                        emit fileOpenFailure(file);
                        file_to_search.close();
                    }
                    else
                    {
                        QTextStream file_stream(&file);
                        file_to_search.close();
                        QString content;                            //Строка с содержимым файла //String for file content
                        while (!file_stream.atEnd())
                        {
                            content += file_stream.readLine();
                        }
                        fileList.append(content);                   //Добавляем содержимое документа в список содержимого
                    }                                               //Adding content of file to content list
                }
            }
        }
    }
}

void ConverterJSON::initializeRequests()
{
    //Загружаем данные из requests.json
    //Loading data from requests.json
    QJsonDocument json_requests;                //Создаём объект для чтения из файла запроса //Creating an object for reading request file
    QByteArray in_stream;
    QFile requests_file(requests_file_path);    //Открываем файл с поисковыми запросами //Opening requests file
    requests_file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!requests_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit
        requestsMissed(); // Если файл не открылся, отправляем сигнал об этом
                          // //If file isn`t opened sending signal about it
        requests_file.close(); // Закрываем файл запросов //Closing requests
                               // file
    } else {
        while (!requests_file.atEnd()) {
            in_stream += requests_file.readLine();
        }
        json_requests.fromJson(
            in_stream); // Заполняем объект содержимым файла запросов //Filling
                        // the object with reequests` file content
        requests_file.close(); // Закрываем файл запросов //Closing requests
                               // file
        if (json_requests.isNull())
            emit requestsMissed(); // Отправляем сигнал об отсутстви файла
                                   // //Sending signsal that file is missing
        else if (!json_requests.isObject() // Проверяем поле requests //Checking
                                           // field requests
                 || !json_requests.object().contains("requests"))
            emit requestsFieldMissed(); // Если нету, то отправляем сигнал об
                                        // отсутстви //Sending signal about
                                        // missing if field doesn`t exist
        else {
            auto json_requests_field =
                json_requests.object()["requests"].toArray();
            for (auto it = json_requests_field.begin();
                 it != json_requests_field.end(); it++) {
                requests.append(it->toString()); // Заполняем список запросов
                                                 // //Filling requests` list
            }
        }
    }
}

void ConverterJSON::initializeCheck()
{
    if (!(engine_status & ConverterStatus::CONFIG_FIELD_MISSED) &&
        !(engine_status & ConverterStatus::CONFIG_MISSED) &&
        !(engine_status & ConverterStatus::SEARCH_FILES_MISSED) &&
        !(engine_status & ConverterStatus::REQUESTS_MISSED))
        initialized = true;
}

void ConverterJSON::initialize()
{
    initializeConfig();
    initializeRequests();
    initializeCheck();
}

QString ConverterJSON::getEngineName()
{
    if (!initialized)		//Проверка на инициализированность //Initialization checking
        initialize();
    return engine_name;
}
QString ConverterJSON::getEngineVersion()
{
    if (!initialized)		//Проверка на инициализированность //Initialization checking
        initialize();
    return engine_version;
}
QList<QString> ConverterJSON::getFilesPaths()
{
        return file_paths;
}

QString ConverterJSON::makeRequestNumber(std::size_t number)
{
    QString num = std::to_string(number).c_str();
    if (number < 10)
        return QString("request00" + num);
    else if (number < 100)
        return QString("request0" + num);
    else
        return QString("request" + num);
}
#endif
