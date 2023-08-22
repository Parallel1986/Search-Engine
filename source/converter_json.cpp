#ifndef CONVERTER_JSON
#define CONVERTER_JSON
#include "..\\include\converter_json.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <exception>
#include <iostream>

std::vector<std::string> ConverterJSON::GetTextDocuments()
{
	if (!initialized)		//�������� �� �������������������� //Initialization checking
		initialize();
	
	return fileList;		//���������� ������ ����������� ������ //Returning content of files for searching
}

int ConverterJSON::GetResponsesLimit()
{
	if (!initialized)		//�������� �� �������������������� //Initialization checking
		initialize();
	
	return maxResponses;	//���������� ������������ ���������� �������� //Returning maximum requests` count
}

std::vector<std::string> ConverterJSON::GetRequests()
{
	if (!initialized)		//�������� �� �������������������� //Checking initialization
		initialize();

	return requests;		//���������� ���������� �������� //Returning content of requests
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers)
{
	nlohmann::json answersTemplate;				//������ ������ ��� ���������� ������ //Creating template for filling answer
	std::ofstream answerFile("answer.json");	//��������� ���� ��� ������ ������ //Opening file for recording
	if (answerFile.is_open())					
		answerFile.clear();						//������� ���������� ����� //Clearing file
	for (int i = 0; i < answers.size(); i++)	//�������� �� ������� ������� � ���������� ������ //Going through answer vector and filling template
	{		
		if (answers[i].size() == 0)				//���� ����� ������, ������ ������ �� ������� //if answer is empty
			answersTemplate["answers"][getRequestNumber(i)]["result"] = false;
		else									//��������� ���� ������ // Filling the template`s fields
		{
			answersTemplate["answers"][getRequestNumber(i)]["result"] = true;		//��������� // Result
			std::vector< std::pair<std::pair<std::string, int>, std::pair<std::string, float>>> outVector;	// ��������������� ������ ��� ������ �����������// Auxiliary vector for result writing
			for (auto& result : answers[i])
				outVector.push_back({ {"docid", result.first},{"rank", result.second}});
			answersTemplate["answers"][getRequestNumber(i)]["relevance"] = outVector;
		}		
	}
	
	answerFile << answersTemplate;	//���������� ������ � ���� //Writing the template to the file
	answerFile.close();				//��������� ���� // Closing the file
}

void ConverterJSON::initialize()
{
	std::ifstream configFile("config.json");	//��������� ���� � ����������� //Opening configuration file
	if (!configFile.is_open())
        throw(std::exception("config.json is missing"));	//���� ���� �� ��������, ������������ ��������� //If file isn`t opened throwing exception
	else		//�����, ��������� ����
	{
        nlohmann::json config;	//������ ��� ������ JSON ����� //Objeect fo reading JSON file
         configFile >> config;	//���������� ���������� ����� � ������ //Writing file content to the object
		configFile.close();		//��������� ���� �������� //Closing the file

		if (config.find("config") == config.end())	
            throw(std::exception("Config file is empty"));	//���� ����������� ���� "config", �� ������������� ���������� //Throwing exception if "config" field is missing
		else if (config.find("files") == config.end() || config["files"].size() == 0)
            throw(std::exception("File list is missing"));	//���� ����������� ������ ������, �� ����������� ���������� // Throwing exception if the file list is missing
		else
		{	//��������� ���� ������������ //Filling configuration fields
			engineName = config["config"]["name"];				//���������� ��� ���������� ��������� //Filling search engine`s namee
			engineVersion = config["config"]["version"];		//���������� ������ ���������� ��������� //Filling search engine`s version
			maxResponses = config["config"]["max_responses"];	//���������� ������������� ����� �������� //Filling number of maximum responses
			config.at("files").get_to(fileList);		//��������� ������ ����������� ������ �� ������� ����� ������������� �����
																//Filling list of files` content for search
		}
	}
	
	std::ifstream requestFile("requests.json");		//��������� ���� � ���������� ��������� //Opening requests file
	if (!requestFile.is_open())
        throw(std::exception("requests.json is missing"));	//����������� ���������� ���� ���� �� �����������
															//Throwing exception if file isn`t opened
    else
	{
        nlohmann::json requestsContainer;	//������ ������ ��� ������ �� ����� ������� //Creating an object for reading request file
        requestFile >> requestsContainer;	//��������� ������ ���������� ����� �������� //Filling the object with reequests` file content
		requestFile.close();				//��������� ���� �������� //Closing requests file	

		requestsContainer.at("requests").get_to(requests);	//��������� ������ �������� //Filling requests` list
	}
	initialized = true;		//����������� ���� ������������� //Setting on initialization flag
}

std::string ConverterJSON::getRequestNumber(int number)
{	//����������� ����� ������� � ������ //Transforming numbre of requeest to a string
	if (number < 10)
		return ("request00" + std::to_string(number));
	else if (number < 100)
		return("request0" + std::to_string(number));
	else
		return("request" + std::to_string(number));
}
#endif
