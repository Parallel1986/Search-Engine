#pragma once

#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>
#include <QObject>
#include <QList>
#include <QDir>

struct ConfigList
{    
    QString enegine_name,
            engine_ver;
    int max_responses;
    QList<QString> files;
};

enum ConverterStatus
{
    CONFIG_MISSED = 1,
    CONFIG_FIELD_MISSED =2,
    SEARCH_FILES_MISSED = 4,
    ENGINE_NAME_MISSED = 8,
    ENGINE_VERSION_MISSED = 16,
    MAX_RESPONSES_MISSED = 32,
    REQUESTS_MISSED = 64,
    REQUESTS_EMPTY = 128
};

class ConverterJSON : public QObject
{
    Q_OBJECT
public:
	ConverterJSON() = default;
    ~ConverterJSON();

    /**
	* Метод получения содержимого файлов
	* @return Возвращает список с содержимым файлов перечисленных
	* в config.json
    */
    QList<QString> GetTextDocuments();

	/**
	* Метод считывает поле max_responses для определения предельного
	* количества ответов на один запрос
	* @return
	*/
	int GetResponsesLimit();

	/**
	* Метод получения запросов из файла requests.json
	* @return возвращает список запросов из файла requests.json
	*/
    QList<QString> GetRequests();

	/**
	* Положить в файл answers.json результаты поисковых запросов
	*/
    void putAnswers(QList<QList<std::pair<int, float>>>
		answers);

    /**
	* Инициализация конвертера файлом конфигурации config.json
    */
    void putConfig(ConfigList settings,QString path);


    QString getEngineName();
    QString getEngineVersion();
    QList<QString> getFilesPaths();
    char getEngineStatus();
    bool isInitialized();
    bool isConfigInitialized();
    bool isRequestsInitialized();

public slots:
    void setConfigPath(QString new_path);
    void setRequestsPath(QString new_path);
    void setAnswersPath(QString new_path);
    void initialize();
    void setMaxRequests(int new_max);
signals:
    void configUpdated();
    void requestsUpdated();
    void answersUpdated();
//    void configMissed();
//    void configFieldMissed();
//    void configFilesMissed();
//    void requestsMissed();
//    void requestsFieldMissed();
    void fileOpenFailure(QString filename);
private:
    void initializeConfig();                        //Загружает данные из config.json //Loading data from config.json
    void initializeRequests();                      //Загружает данные из requests.json //Loading data from requests.json
    void initializeCheck();                         //Проверяет инициализацию //Checking initialization
    QString makeRequestNumber(std::size_t number); //Генерирует строку запроса для записи в answers.json //Generates string of a request for writing to answers.json
    QString config_file_path = QDir::currentPath() + "/config.json";       //Путь до файла конфигурации //Configuration file`s path
    QString requests_file_path = QDir::currentPath() + "/requests.json";   //Путь до файла запросов //Requests file`s path
    QString answers_file_path = QDir::currentPath() + "/answers.json";     //Путь до файла ответов //Answers file`s path
    QString engine_name;                            //Имя поискового двигателя //Search engine name
    QString engine_version;                         //Версия поискового двигателя //Search eengine version
    int max_responses = 5;                          //Максимальное количество ответов на запрос //Maximum number of responsesж
    QList<QString> requests;                        //Список поисковых запросов //List of requests
    QList<QString> fileList;                        //Список содержимого файлов, в которых производится поиск //List of files for search
    QList<QString> file_paths;                      //Список путей файлов по которым производится поиск //List of files` paths for search
//  bool initialized = false;               		//Указывает на инициализацию конвертера //Flag of converter initialization
    char engine_status = 0;                         //Состояние поискового движка //Search engine statee
};
