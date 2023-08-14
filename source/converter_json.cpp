#pragma once
#include "..\\include\converter_json.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <exception>
#include <iostream>

ConverterJSON::ConverterJSON()
{
	initialize();
}

std::vector<std::string> ConverterJSON::GetTextDocuments()
{
	return fileList;
}

int ConverterJSON::GetResponsesLimit()
{
	return maxResponses;
}

std::vector<std::string> ConverterJSON::GetRequests()
{
	return requests;
}

void ConverterJSON::putAnswers(std::vector<std::vector<std::pair<int, float>>> answers)
{
	nlohmann::json answersTemplate;
	std::ofstream answerFile("answer.json");
	if (answerFile.is_open())
		answerFile.clear();	
	for (int i = 0; i < answers.size(); i++)
	{		
		if (answers[i].size() == 0)
		{
			answersTemplate["answers"][getRequestNumber(i)]["result"] = false;
		}
		else
		{
			answersTemplate["answers"][getRequestNumber(i)]["result"] = true;
			std::vector< std::pair<std::pair<std::string, int>, std::pair<std::string, float>>> outVector;
			for (auto& result : answers[i])
			{
				outVector.push_back({ {"docid", result.first},{"rank", result.second}});
//				answersTemplate["answers"][getRequestNumber(i)]["relevance"]["docid"] = result.first;
//				answersTemplate["answers"][getRequestNumber(i)]["relevance"]["rank"] = result.second;
			}
			answersTemplate["answers"][getRequestNumber(i)]["relevance"] = outVector;
		}		
	}
	
	answerFile << answersTemplate;
	answerFile.close();
}

void ConverterJSON::initialize()
{
	std::ifstream configFile("config.json");
	if (!configFile.is_open())
	{		
		throw(std::exception("config.json is missing"));
	}
	else
	{
		nlohmann::json config;
		config << configFile;
		configFile.close();

		if (config.find("config") == config.end())
			throw(std::exception("Config file is empty"));
		else if (config.find("files") == config.end() || config["files"].size() == 0)
		{
			throw(std::exception("File list is missing"));
		}
		else
		{
			engineName = config["config"]["name"];
			engineVersion = config["config"]["version"];
			maxResponses = config["config"]["max_responses"];
			config.at("files").get_to(fileList);
		}
	}
	
	std::ifstream requestFile("requests.json");
	if (!requestFile.is_open())
	{
		throw(std::exception("requests.json is missing"));
	}
	else
	{
		nlohmann::json requestsContainer;		
		requestsContainer << requestFile;
		requestFile.close();		

		requestsContainer.at("requests").get_to(requests);		
	}
}

std::string ConverterJSON::getRequestNumber(int number)
{
	if (number < 10)
		return ("request00" + std::to_string(number));
	else if (number < 100)
		return("request0" + std::to_string(number));
	else
		return("request" + std::to_string(number));
}