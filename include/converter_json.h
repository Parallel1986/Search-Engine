#pragma once

#include <string>
#include <vector>
#include <map>

struct Answer
{
	std::string request;
	bool reesult;
	std::map<float, int> relevantTable;
};

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

	void initialize();
private:
	std::string getRequestNumber(int number);

	std::string engineName;						//Search engine name
	std::string engineVersion;					//Search eengine version
	int maxResponses = 5;						//Maximum number of responsesж
	std::vector<std::string> requests;			//List of requests
	std::vector<std::string> fileList;			//List of files for search
//	std::vector<std::map<float,int>> answers;	//List of answers
};