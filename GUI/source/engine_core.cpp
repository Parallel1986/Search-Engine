#include "../include/engine_core.h"

EngineCore::EngineCore()
{
    converter = &ConverterJSON::GetInstance();
    index = new InvertedIndex();
    server = new SearchServer(*index);
}

EngineCore::~EngineCore()
{
    if (converter)
    {
        delete converter;
        converter = nullptr;
    }
    if (index)
    {
        delete index;
        index = nullptr;
    }
    if (server)
    {
        delete server;
        server = nullptr;
    }
}

//Set new configurations' file
void EngineCore::SetConfigPath(QString new_path)
{
    converter->ChangeConfigPath(new_path);
}

//Set new requests' file
void EngineCore::SetRequestsPath(QString new_path)
{
    converter->ChangeRequestsPath(new_path);
}

//Sewt new answers' file
void EngineCore::SetAnswersPath(QString new_path)
{
    converter->ChangeAnswersPath(new_path);
}

//Check of initialization
bool EngineCore::IsInitialized()
{
    return IsConfigInitialized() && IsRequestsInitialized();
}

//Check if configurations are initialized
bool EngineCore::IsConfigInitialized()
{
    engine_status &= ConverterStatusReset::RESET_CONFIG_STATUS;
    return (!(engine_status&ConverterStatus::CONFIG_FIELD_MISSED)
        &&!(engine_status&ConverterStatus::CONFIG_MISSED)
        &&!(engine_status&ConverterStatus::SEARCH_FILES_MISSED));
}

//Check if requests are initialized
bool EngineCore::IsRequestsInitialized()
{
    engine_status &= ConverterStatusReset::RESET_REQUESTS_STATUS;
    return (!(engine_status&ConverterStatus::REQUESTS_MISSED)
        &&!(engine_status&ConverterStatus::REQUESTS_EMPTY));
}

//Set maximal limit of responses
void EngineCore::SetMaxRequests(int new_max)
{
    if (max_responses != new_max)
        max_responses = new_max;
}

//Initialize configurations from the configurations' file
void EngineCore::InitializeConfig()
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
                LoadConfigs(configuration);                             //Загружаем конфигурации
            }

            if (!configuration.object().contains("files")           //Проверяем наличие поля "files"
                ||!configuration.object()["files"].isArray())
            {
                engine_status |= ConverterStatus::SEARCH_FILES_MISSED;  //Выставляем статус
            }
            else
            {
                engine_status &= ~ConverterStatus::SEARCH_FILES_MISSED; //Выставляем статус
                LoadSearchFiles(configuration);                         //Загружаем список файлов для поиска
            }
            //Заполняем список содержимого файлов по которым будет производиться поиск
            for (auto& file : file_paths)
            {
                QFile file_to_search(file);
                if (!file_to_search.open(QIODevice::ReadOnly | QIODevice::Text)) //Проверка на открытие
                {
                    emit FileOpenFailure(file);     //Отправляем сигнал об отсуутствии
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
void EngineCore::InitializeRequests()
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
char EngineCore::GetEngineStatus()
{
    return engine_status;
}

//Инициализировать конвертер
void EngineCore::Initialize()
{
    InitializeConfig();
    InitializeRequests();
}

//Загрузить поля config из config.json
void EngineCore::LoadConfigs()
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
            LoadConfigs(configuration);     //Загружаем настройки
        }
    }
}

//Загрузить поля из JSON документа
void EngineCore::LoadConfigs(QJsonDocument& configuration)
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
void EngineCore::LoadSearchFiles()
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
            LoadSearchFiles(configuration);     //Загружаем список фвайлов
        }
    }
}

//Загрузить поля files из JSON документа
void EngineCore::LoadSearchFiles(QJsonDocument& configuration)
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

void EngineCore::Search()
{
    index->UpdateDocumentBase(converter->GetTextDocuments());                   //Обновляем список документов в индексе
    server = new SearchServer(*index);                                          //Создаём поисковый сервер
    server->setMaxResponse(converter->GetResponsesLimit());                     //Устанавливаем максимальное количество результатов
    search_result = (server->search(converter->GetRequests()));                 //Проводим поиск и записываем результат
}
