#pragma once

#include <string>
#include <vector>
#include <map>

//struct Answer
//{
//	std::string request;
//	bool result;
//	std::map<float, int> relevantTable;
//};

class ConverterJSON {
public:
	ConverterJSON() = default;
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
	/**
	* Положить в файл answers.json результаты поисковых запросов
	*/
	void putAnswers(std::vector<std::vector<std::pair<int, float>>>
		answers);
	/*
	* Инициализация конвертера файлом конфигурации config.json
	*/
	void initialize();
private:
	/*
	* Получает строку с номером запроса
	*/
	std::string getRequestNumber(int number);

	std::string engineName;						// Имя поискового двигателя //Search engine name
	std::string engineVersion;					// Версия поискового двигателя //Search eengine version
	int maxResponses = 5;						// Максимальное количество ответов на запрос //Maximum number of responsesж
	std::vector<std::string> requests;			// Список поисковых запросов //List of requests
	std::vector<std::string> fileList;			// Список содержимого файлов, в которых производится поиск //List of files for search
	bool initialized = false;					// Указывает на инициализацию конвертера // Flag of converter initialization
//	std::vector<std::map<float,int>> answers;	//List of answers
};