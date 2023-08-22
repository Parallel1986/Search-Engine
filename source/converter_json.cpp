#ifndef CONVERTER_JSON
#define CONVERTER_JSON
#include "..\\include\converter_json.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <exception>
#include <iostream>

std::vector<std::string> ConverterJSON::GetTextDocuments()
{
	if (!initialized)		//Проверка на инициализированность //Initialization checking
		initialize();
	
	return fileList;		//Возвращаем список содержимого файлов //Returning content of files for searching
}

int ConverterJSON::GetResponsesLimit()
{
	if (!initialized)		//Проверка на инициализированность //Initialization checking
		initialize();
	
	return maxResponses;	//Возвращаем максимальное количество запросов //Returning maximum requests` count
}

std::vector<std::string> ConverterJSON::GetRequests()
{
	if (!initialized)		//Проверка на инициализированность //Checking initialization
		initialize();

	return requests;		//Возвращаем содержимое запросов //Returning content of requests
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers)
{
	nlohmann::json answersTemplate;				//Создаём шаблон для заполнения ответа //Creating template for filling answer
	std::ofstream answerFile("answer.json");	//Открываем файл для записи ответа //Opening file for recording
	if (answerFile.is_open())					
		answerFile.clear();						//Очищаем содержимое файла //Clearing file
	for (int i = 0; i < answers.size(); i++)	//Проходим по вектору ответов и заполнгяем шаблон //Going through answer vector and filling template
	{		
		if (answers[i].size() == 0)				//Если ответ пустой, значит ничего не нашлось //if answer is empty
			answersTemplate["answers"][getRequestNumber(i)]["result"] = false;
		else									//Заполняем поля ответа // Filling the template`s fields
		{
			answersTemplate["answers"][getRequestNumber(i)]["result"] = true;		//Результат // Result
			std::vector< std::pair<std::pair<std::string, int>, std::pair<std::string, float>>> outVector;	// Вспомогательный вектор для записи результатов// Auxiliary vector for result writing
			for (auto& result : answers[i])
				outVector.push_back({ {"docid", result.first},{"rank", result.second}});
			answersTemplate["answers"][getRequestNumber(i)]["relevance"] = outVector;
		}		
	}
	
	answerFile << answersTemplate;	//Записываем шаблон в файл //Writing the template to the file
	answerFile.close();				//Закрываем файл // Closing the file
}

void ConverterJSON::initialize()
{
	std::ifstream configFile("config.json");	//Открываем файл с настройками //Opening configuration file
	if (!configFile.is_open())
        throw(std::exception("config.json is missing"));	//Если файл не открылся, выбрасыфваем исклюение //If file isn`t opened throwing exception
	else		//Иначе, считываем поля
	{
        nlohmann::json config;	//Объект для чтения JSON файла //Objeect fo reading JSON file
         configFile >> config;	//Записываем содержимое файла в объект //Writing file content to the object
		configFile.close();		//Закрываем файл настроек //Closing the file

		if (config.find("config") == config.end())	
            throw(std::exception("Config file is empty"));	//Если отсутствует поле "config", то выбрасывается исключение //Throwing exception if "config" field is missing
		else if (config.find("files") == config.end() || config["files"].size() == 0)
            throw(std::exception("File list is missing"));	//Если отсутствует список файлов, то выбрасываем исключение // Throwing exception if the file list is missing
		else
		{	//Заполняем поля конфигурации //Filling configuration fields
			engineName = config["config"]["name"];				//Записываем имя поискового двигателя //Filling search engine`s namee
			engineVersion = config["config"]["version"];		//Записываем версию поискового двигателя //Filling search engine`s version
			maxResponses = config["config"]["max_responses"];	//Записываем максимавльное число запросов //Filling number of maximum responses
			config.at("files").get_to(fileList);		//Заполняем список содержимого файлов по которым будет производиться поиск
																//Filling list of files` content for search
		}
	}
	
	std::ifstream requestFile("requests.json");		//Открываем файл с поисковыми запросами //Opening requests file
	if (!requestFile.is_open())
        throw(std::exception("requests.json is missing"));	//Выбрасыванм исключение если файл не открывается
															//Throwing exception if file isn`t opened
    else
	{
        nlohmann::json requestsContainer;	//Создаём объект для чтения из файла запроса //Creating an object for reading request file
        requestFile >> requestsContainer;	//Заполняем объект содержимым файла запросов //Filling the object with reequests` file content
		requestFile.close();				//Закрываем файл запросов //Closing requests file	

		requestsContainer.at("requests").get_to(requests);	//Заполняем список запросов //Filling requests` list
	}
	initialized = true;		//Переключаем флаг инициализации //Setting on initialization flag
}

std::string ConverterJSON::getRequestNumber(int number)
{	//Преобразуем номер запроса в строку //Transforming numbre of requeest to a string
	if (number < 10)
		return ("request00" + std::to_string(number));
	else if (number < 100)
		return("request0" + std::to_string(number));
	else
		return("request" + std::to_string(number));
}
#endif
