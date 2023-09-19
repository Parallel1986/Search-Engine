#pragma once

#include <string>
#include <vector>
#include <map>

enum CHANGES
{
    PATH_TO_CONFIG = 1,
    PATH_TO_REQUESTS =2,
    PATH_TO_ANSWERS = 4,
    RESPONSES = 8,
    REQUESTS_ADDED = 16,
    FILES_TO_SEARCH_ADDED = 32,
    NEW_CONFIG = 64
};

struct Preference
{
    unsigned char changes = 0;
    std::string config_path, requests_path, answers_path;
    int max_response = -1;
    std::vector<std::string> file_list_adds, requests_adds;
};

class ConverterJSON {
public:
    //Получить ссылку на экземпляр
    static ConverterJSON& GetInstance();
    ~ConverterJSON();
    /**
	* Метод получения содержимого файлов
	* @return Возвращает список с содержимым файлов перечисленных
	* в config.json
	*/
    std::vector<std::string> GetTextDocuments();

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
    std::vector<std::string> GetRequests();

    //Положить в файл answers.json результаты поисковых запросов
    void PutAnswers(std::vector<std::vector<std::pair<int, float>>>& answers);

    //Инициализация конвертера файлом конфигурации config.json
    void Initialize();

    //Изменить настройки
    void SetPreferences(Preference& pref);
private:
    static ConverterJSON* instance;
    void GenerateConfig(Preference& pref);
    ConverterJSON() = default;                  //Убираем конструктор, тобы нельзя было инстанциировать класс
    std::string GetRequestNumber(int number);   //Получает строку с номером запроса
    void operator=(ConverterJSON&);             //Убираем оператор присваивания
    std::string engine_name;                    //Имя поискового двигателя
    std::string engine_version;                 //Версия поискового двигателя
    int max_responses = 5;                      //Максимальное количество ответов на запрос
    std::vector<std::string> requests;          //Список поисковых запросов
    std::vector<std::string> file_list;         //Список содержимого файлов, в которых производится поиск
    bool initialized = false;                   //Указывает на инициализацию конвертера
    std::string config_path = "config.json";    //Путь до config.json
    std::string requests_path= "requests.json"; //Путь до requests.json
    std::string answers_path= "answers.json";   //Путь до answers.json
};
