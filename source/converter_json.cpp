#ifndef CONVERTER_JSON
#define CONVERTER_JSON
#include "..\\include\converter_json.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <exception>
#include <iostream>
#include <sstream>

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
	std::ofstream answerFile("answers.json");	//��������� ���� ��� ������ ������ //Opening file for recording
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
	std::stringstream unformat_stream;			//��������� ����� ��� �������������������� ������ //String stream for unformated output
	std::string unformat_line, format_line;		//������ � ������������������ � ���������������� ������������ //String for unformated and formated result respeectively

	unformat_stream << answersTemplate;		//������ ������������������ ��������� //Creating unformated result
	std::getline(unformat_stream, unformat_line); //���������� ���������������� ��������� � ������ //writing unformated result to the string
	size_t tabs = 0;		//������� ��������� //tab counter
	for (size_t i = 0; i < unformat_line.length(); i++)
	{
		if (unformat_line[i] == '{'			//����� ����������� ������ ��������� �� ����� ������
			|| unformat_line[i] == '['		//After opening brackets going to new line
			|| unformat_line[i] == '(')
		{
			format_line += unformat_line[i];
			tabs++;							//������� ������� ��������� �� +1 //Shifting tab counter to +1
			format_line += '\n';
			for (size_t j = 0; j < tabs; j++)
			{
				format_line += '\t';		//��������� ������ ���������� ��������� //Adding required tabs number
			}
		}
		else if (unformat_line[i] == '}'	//����� ����������� ������ ��������� �� ����� ������
			|| unformat_line[i] == ']'		//After closening brackets going to new line
			|| unformat_line[i] == ')')
		{
			tabs--;							//������� ������� ��������� �� -1 //Shifting tab counter to -1
			format_line += '\n';
			for (size_t j = 0; j < tabs; j++)
			{
				format_line += '\t';		//��������� ������ ���������� ��������� //Adding required tabs number
			}
			format_line += unformat_line[i];
		}		
		else if (unformat_line[i] == ',')	//����� ������� ��������� �� ����� ������
		{									//After comma going to new line
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
	answerFile << format_line;	//���������� ������ � ���� //Writing the template to the file

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
			
			std::vector<std::string> file_paths;
			config.at("files").get_to(file_paths);		//��������� ������ ����������� ������ �� ������� ����� ������������� �����
																//Filling list of files` content for search
			for (auto& file : file_paths)
			{
				std::ifstream document(file);
				if (!document.is_open())
				{	//���� ���� �� ������� �������, �� ����������� ����������
					//Throwing the exception if can not open the file
					throw (std::exception(("Could not open " + file).c_str()));
				}
				else
				{
					std::string content;	//������ � ���������� ����� //String for file content
					//char buffer[20];		//������ ��� ������ //Buffer for reading of the efile
					std::getline(document, content);
					//document >> content;
					//do
					//{	//������ ���� �� ��������� ����� ��������� //Reading while reaching end of file
					//	document.read(buffer,20);
					//	document.
					//	content += buffer;	//��������� ���������� ������ � ������ //Adding content of the buffer to string
					//} while (!document.eof());
					fileList.push_back(content);	//��������� ���������� ��������� � ������ �����������
														//Adding content of file to content list
				}
			}
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
