#ifndef CONVERTER_JSON
#define CONVERTER_JSON
#include "..\\include\converter_json.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <exception>
#include <iostream>
#include <sstream>
#include <filesystem>

ConverterJSON* ConverterJSON::instance = nullptr;

ConverterJSON& ConverterJSON::GetInstance()
{
    if (!instance)
        instance = new ConverterJSON();
    return *instance;
}

ConverterJSON::~ConverterJSON(){}

std::vector<std::string> ConverterJSON::GetTextDocuments()
{
    if (!initialized)		//Проверка на инициализированность
        Initialize();
	
    return file_list;		//Возвращаем список содержимого файлов
}

int ConverterJSON::GetResponsesLimit()
{
    if (!initialized)		//Проверка на инициализированность
        Initialize();
	
    return max_responses;	//Возвращаем максимальное количество запросов
}

std::vector<std::string> ConverterJSON::GetRequests()
{
    if (!initialized)		//Проверка на инициализированность
        Initialize();

    return requests;		//Возвращаем содержимое запросов
}

void ConverterJSON::PutAnswers(std::vector<std::vector<std::pair<int, float>>>& answers)
{
    nlohmann::json answers_template;				//Создаём шаблон для заполнения ответа
    std::ofstream answer_file(answers_path);	//Открываем файл для записи ответа
    if (answer_file.is_open())
        answer_file.clear();						//Очищаем содержимое файла
    for (int i = 0; i < answers.size(); i++)	//Проходим по вектору ответов и заполнгяем шаблон
	{		
        if (answers[i].size() == 0)				//Если ответ пустой, значит ничего не нашлось
            answers_template["answers"][GetRequestNumber(i)]["result"] = false;
        else									//Заполняем поля ответа
		{
            answers_template["answers"][GetRequestNumber(i)]["result"] = true;		//Результат
            std::vector< std::pair<std::pair<std::string, int>, std::pair<std::string, float>>> outVector;	// Вспомогательный вектор для записи результатов
			for (auto& result : answers[i])
				outVector.push_back({ {"docid", result.first},{"rank", result.second}});
            answers_template["answers"][GetRequestNumber(i)]["relevance"] = outVector;
		}		
	}
    std::stringstream unformat_stream;			//Строковый поток для неотформатированного вывода
    std::string unformat_line, format_line;		//Строки с неотформатированым и отформатированым результатами

    unformat_stream << answers_template;		//Создаём неотформатированый результат
    std::getline(unformat_stream, unformat_line); //Записываем неформатированый результат в строку
    size_t tabs = 0;		//Счётчик табуляций
	for (size_t i = 0; i < unformat_line.length(); i++)
	{
		if (unformat_line[i] == '{'			//После открывающих скобок переходим на новую строку
            || unformat_line[i] == '['
			|| unformat_line[i] == '(')
		{
			format_line += unformat_line[i];
            tabs++;							//Смещаем счётчик табуляций на +1
			format_line += '\n';
			for (size_t j = 0; j < tabs; j++)
			{
                format_line += '\t';		//Добавляем нужное количество табуляций
			}
		}
		else if (unformat_line[i] == '}'	//После закрывающих скобок переходим на новую строку
            || unformat_line[i] == ']'
			|| unformat_line[i] == ')')
		{
            tabs--;							//Смещаем счётчик табуляции на -1
			format_line += '\n';
			for (size_t j = 0; j < tabs; j++)
			{
                format_line += '\t';		//Добавляем нужное количество табуляций
			}
			format_line += unformat_line[i];
		}		
		else if (unformat_line[i] == ',')	//После запятой переходим на новую строку
        {
            format_line += unformat_line[i];
			format_line += '\n';
			for (size_t j = 0; j < tabs; j++)
			{
				format_line += '\t';
			}
		}
		else
			format_line += unformat_line[i];
	}
    answer_file << format_line;         //Записываем шаблон в файл
    answer_file.close();				//Закрываем файл
}

void ConverterJSON::Initialize()
{
    std::ifstream config_file(config_path);	//Открываем файл с настройками
    if (!config_file.is_open())
        throw(std::exception("config.json is missing\n"));	//Если файл не открылся, выбрасыфваем исклюение
	else		//Иначе, считываем поля
	{
        nlohmann::json config;	//Объект для чтения JSON файла
         config_file >> config;	//Записываем содержимое файла в объект
        config_file.close();		//Закрываем файл настроек

		if (config.find("config") == config.end())	
            throw(std::exception("Config file is empty\n"));	//Если отсутствует поле "config", то выбрасывается исключение
		else if (config.find("files") == config.end() || config["files"].size() == 0)
            throw(std::exception("File list is missing\n"));	//Если отсутствует список файлов, то выбрасываем исключение
		else
		{	//Заполняем поля конфигурации //Filling configuration fields
            engine_name = config["config"]["name"];				//Записываем имя поискового двигателя
            engine_version = config["config"]["version"];		//Записываем версию поискового двигателя
            max_responses = config["config"]["max_responses"];	//Записываем максимавльное число запросов
			
			std::vector<std::string> file_paths;
			config.at("files").get_to(file_paths);		//Заполняем список содержимого файлов по которым будет производиться поиск
			for (auto& file : file_paths)
			{
				std::ifstream document(file);
				if (!document.is_open())
				{	//Если файл не удалось открыть, то выбрасываем исключение
					//Throwing the exception if can not open the file
                    throw (std::exception(("Could not open " + file + "\n").c_str()));
				}
				else
				{
                    std::string content;	//Строка с содержимым файла
					std::getline(document, content);
                    file_list.push_back(content);	//Добавляем содержимое документа в список содержимого
				}
			}
		}
    }
	
    std::ifstream request_file(requests_path);		//Открываем файл с поисковыми запросами
    if (!request_file.is_open())
        throw(std::exception("requests.json is missing\n"));	//Выбрасыванм исключение если файл не открывается

    else
	{
        nlohmann::json requests_container;	//Создаём объект для чтения из файла запроса
        request_file >> requests_container;	//Заполняем объект содержимым файла запросов
        request_file.close();				//Закрываем файл запросов

        requests_container.at("requests").get_to(requests);	//Заполняем список запросов
	}
    initialized = true;		//Выставляем флаг инициализации
}

//Вносим изменения в настройки
void ConverterJSON::SetPreferences(Preference& pref)
{
    while (pref.changes != 0)                       //Проверяем все изменения
    {
        switch (pref.changes)
        {
        case CHANGES::NEW_CONFIG:                   //Сгенерировать файл config.json
            GenerateConfig(pref);
            pref.changes &=~CHANGES::NEW_CONFIG;    //Убрать отметку об изменении
            break;
        case CHANGES::PATH_TO_CONFIG:               //Изменить путь до config.json
            (!std::filesystem::is_regular_file(pref.config_path))
                ? config_path =pref.config_path + "config.json"
                : config_path =pref.config_path;
            pref.changes &=~CHANGES::PATH_TO_CONFIG;    //Убрать отметку об изменении
            break;
        case CHANGES::PATH_TO_REQUESTS:             //Изменить путь доа requests.json
            (!std::filesystem::is_regular_file(pref.requests_path))
                ? requests_path =pref.requests_path + "requests.json"
                : requests_path =pref.requests_path;
                pref.changes &=~CHANGES::PATH_TO_REQUESTS;  //Убрать отметку об изменении
            break;
        case CHANGES::PATH_TO_ANSWERS:              //Изменить путь до answers.json
            (!std::filesystem::is_regular_file(pref.answers_path))
                ? requests_path =pref.answers_path + "requests.json"
                : answers_path =pref.answers_path;
                pref.changes &=~CHANGES::PATH_TO_ANSWERS;   //Убрать отметку об изменении
            break;
        case CHANGES::RESPONSES:                    //Изменить число ответов
            max_responses = pref.max_response;
            pref.changes &=~CHANGES::RESPONSES;     //Убрать отметку об изменении
            break;
        case CHANGES::REQUESTS_ADDED:               //Добавить запросы
            for (auto& request:pref.requests_adds)
            {
                requests.push_back(request);
            }
            pref.changes &=~CHANGES::REQUESTS_ADDED;    //Убрать отметку об изменении
            break;
        case CHANGES::FILES_TO_SEARCH_ADDED:        //Добавить файлы для индексирования
            for (auto& file:pref.file_list_adds)
            {
                file_list.push_back(file);
            }
            pref.changes &=~CHANGES::FILES_TO_SEARCH_ADDED; //Убрать отметку об изменении
            break;
        default:    //Не должно попасть сюда
            break;
        }
    }
}

//Генерируем файл config.json
void ConverterJSON::GenerateConfig(Preference& pref)
{
    nlohmann::json config_template;				//Создаём шаблон для заполнения настроек
    std::ofstream config_file(config_path);     //Открываем файл для записи настроек
    if (config_file.is_open())
        config_file.clear();					//Очищаем содержимое файла
    config_template["config"]["name"] = "SearchEngine - autogen";	//Записываем имя поискового двигателя
    config_template["config"]["version"] = "1.0 - autogen";         //Записываем версию поискового двигателя
    config_template["config"]["max_responses"] = pref.max_response;	//Записываем максимавльное число запросов

    for (auto& file : pref.file_list_adds)          //Добавляем файлы для индексирования
    {
        config_template["files"].push_back(file);
    }
    std::stringstream unformat_stream;              //Строковый поток для неотформатированного вывода
    std::string unformat_line, format_line;         //Строки с неотформатированым и отформатированым результатами
    unformat_stream << config_template;             //Создаём неотформатированый результат
    std::getline(unformat_stream, unformat_line);   //Записываем неформатированый результат в строку
    size_t tabs = 0;                                //Счётчик табуляций
    for (size_t i = 0; i < unformat_line.length(); i++)
    {
        if (unformat_line[i] == '{'			//После открывающих скобок переходим на новую строку
            || unformat_line[i] == '['
            || unformat_line[i] == '(')
        {
            format_line += unformat_line[i];
            tabs++;							//Смещаем счётчик табуляций на +1
            format_line += '\n';
            for (size_t j = 0; j < tabs; j++)
            {
                format_line += '\t';		//Добавляем нужное количество табуляций
            }
        }
        else if (unformat_line[i] == '}'	//После закрывающих скобок переходим на новую строку
            || unformat_line[i] == ']'
            || unformat_line[i] == ')')
        {
            tabs--;							//Смещаем счётчик табуляции на -1
            format_line += '\n';
            for (size_t j = 0; j < tabs; j++)
            {
                format_line += '\t';		//Добавляем нужное количество табуляций
            }
            format_line += unformat_line[i];
        }
        else if (unformat_line[i] == ',')	//После запятой переходим на новую строку
        {
            format_line += unformat_line[i];
            format_line += '\n';
            for (size_t j = 0; j < tabs; j++)
            {
                format_line += '\t';
            }
        }
        else
            format_line += unformat_line[i];
    }
    config_file << format_line;             //Записываем шаблон в файл
    config_file.close();                    //Закрываем файл
}

//Преобразуем номер запроса в строку
std::string ConverterJSON::GetRequestNumber(int number)
{
	if (number < 10)
		return ("request00" + std::to_string(number));
	else if (number < 100)
		return("request0" + std::to_string(number));
	else
		return("request" + std::to_string(number));
}
#endif
