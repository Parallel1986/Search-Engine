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
    if (!initialized)		//�������� �� ��������������������
        Initialize();
	
    return file_list;		//���������� ������ ����������� ������
}

int ConverterJSON::GetResponsesLimit()
{
    if (!initialized)		//�������� �� ��������������������
        Initialize();
	
    return max_responses;	//���������� ������������ ���������� ��������
}

std::vector<std::string> ConverterJSON::GetRequests()
{
    if (!initialized)		//�������� �� ��������������������
        Initialize();

    return requests;		//���������� ���������� ��������
}

void ConverterJSON::PutAnswers(std::vector<std::vector<std::pair<int, float>>>& answers)
{
    nlohmann::json answers_template;				//������ ������ ��� ���������� ������
    std::ofstream answer_file(answers_path);	//��������� ���� ��� ������ ������
    if (answer_file.is_open())
        answer_file.clear();						//������� ���������� �����
    for (int i = 0; i < answers.size(); i++)	//�������� �� ������� ������� � ���������� ������
	{		
        if (answers[i].size() == 0)				//���� ����� ������, ������ ������ �� �������
            answers_template["answers"][GetRequestNumber(i)]["result"] = false;
        else									//��������� ���� ������
		{
            answers_template["answers"][GetRequestNumber(i)]["result"] = true;		//���������
            std::vector< std::pair<std::pair<std::string, int>, std::pair<std::string, float>>> outVector;	// ��������������� ������ ��� ������ �����������
			for (auto& result : answers[i])
				outVector.push_back({ {"docid", result.first},{"rank", result.second}});
            answers_template["answers"][GetRequestNumber(i)]["relevance"] = outVector;
		}		
	}
    std::stringstream unformat_stream;			//��������� ����� ��� �������������������� ������
    std::string unformat_line, format_line;		//������ � ������������������ � ���������������� ������������

    unformat_stream << answers_template;		//������ ������������������ ���������
    std::getline(unformat_stream, unformat_line); //���������� ���������������� ��������� � ������
    size_t tabs = 0;		//������� ���������
	for (size_t i = 0; i < unformat_line.length(); i++)
	{
		if (unformat_line[i] == '{'			//����� ����������� ������ ��������� �� ����� ������
            || unformat_line[i] == '['
			|| unformat_line[i] == '(')
		{
			format_line += unformat_line[i];
            tabs++;							//������� ������� ��������� �� +1
			format_line += '\n';
			for (size_t j = 0; j < tabs; j++)
			{
                format_line += '\t';		//��������� ������ ���������� ���������
			}
		}
		else if (unformat_line[i] == '}'	//����� ����������� ������ ��������� �� ����� ������
            || unformat_line[i] == ']'
			|| unformat_line[i] == ')')
		{
            tabs--;							//������� ������� ��������� �� -1
			format_line += '\n';
			for (size_t j = 0; j < tabs; j++)
			{
                format_line += '\t';		//��������� ������ ���������� ���������
			}
			format_line += unformat_line[i];
		}		
		else if (unformat_line[i] == ',')	//����� ������� ��������� �� ����� ������
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
    answer_file << format_line;         //���������� ������ � ����
    answer_file.close();				//��������� ����
}

void ConverterJSON::Initialize()
{
    std::ifstream config_file(config_path);	//��������� ���� � �����������
    if (!config_file.is_open())
        throw(std::exception("config.json is missing\n"));	//���� ���� �� ��������, ������������ ���������
	else		//�����, ��������� ����
	{
        nlohmann::json config;	//������ ��� ������ JSON �����
         config_file >> config;	//���������� ���������� ����� � ������
        config_file.close();		//��������� ���� ��������

		if (config.find("config") == config.end())	
            throw(std::exception("Config file is empty\n"));	//���� ����������� ���� "config", �� ������������� ����������
		else if (config.find("files") == config.end() || config["files"].size() == 0)
            throw(std::exception("File list is missing\n"));	//���� ����������� ������ ������, �� ����������� ����������
		else
		{	//��������� ���� ������������ //Filling configuration fields
            engine_name = config["config"]["name"];				//���������� ��� ���������� ���������
            engine_version = config["config"]["version"];		//���������� ������ ���������� ���������
            max_responses = config["config"]["max_responses"];	//���������� ������������� ����� ��������
			
			std::vector<std::string> file_paths;
			config.at("files").get_to(file_paths);		//��������� ������ ����������� ������ �� ������� ����� ������������� �����
			for (auto& file : file_paths)
			{
				std::ifstream document(file);
				if (!document.is_open())
				{	//���� ���� �� ������� �������, �� ����������� ����������
					//Throwing the exception if can not open the file
                    throw (std::exception(("Could not open " + file + "\n").c_str()));
				}
				else
				{
                    std::string content;	//������ � ���������� �����
					std::getline(document, content);
                    file_list.push_back(content);	//��������� ���������� ��������� � ������ �����������
				}
			}
		}
    }
	
    std::ifstream request_file(requests_path);		//��������� ���� � ���������� ���������
    if (!request_file.is_open())
        throw(std::exception("requests.json is missing\n"));	//����������� ���������� ���� ���� �� �����������

    else
	{
        nlohmann::json requests_container;	//������ ������ ��� ������ �� ����� �������
        request_file >> requests_container;	//��������� ������ ���������� ����� ��������
        request_file.close();				//��������� ���� ��������

        requests_container.at("requests").get_to(requests);	//��������� ������ ��������
	}
    initialized = true;		//���������� ���� �������������
}

//������ ��������� � ���������
void ConverterJSON::SetPreferences(Preference& pref)
{
    while (pref.changes != 0)                       //��������� ��� ���������
    {
        switch (pref.changes)
        {
        case CHANGES::NEW_CONFIG:                   //������������� ���� config.json
            GenerateConfig(pref);
            pref.changes &=~CHANGES::NEW_CONFIG;    //������ ������� �� ���������
            break;
        case CHANGES::PATH_TO_CONFIG:               //�������� ���� �� config.json
            (!std::filesystem::is_regular_file(pref.config_path))
                ? config_path =pref.config_path + "config.json"
                : config_path =pref.config_path;
            pref.changes &=~CHANGES::PATH_TO_CONFIG;    //������ ������� �� ���������
            break;
        case CHANGES::PATH_TO_REQUESTS:             //�������� ���� ��� requests.json
            (!std::filesystem::is_regular_file(pref.requests_path))
                ? requests_path =pref.requests_path + "requests.json"
                : requests_path =pref.requests_path;
                pref.changes &=~CHANGES::PATH_TO_REQUESTS;  //������ ������� �� ���������
            break;
        case CHANGES::PATH_TO_ANSWERS:              //�������� ���� �� answers.json
            (!std::filesystem::is_regular_file(pref.answers_path))
                ? requests_path =pref.answers_path + "requests.json"
                : answers_path =pref.answers_path;
                pref.changes &=~CHANGES::PATH_TO_ANSWERS;   //������ ������� �� ���������
            break;
        case CHANGES::RESPONSES:                    //�������� ����� �������
            max_responses = pref.max_response;
            pref.changes &=~CHANGES::RESPONSES;     //������ ������� �� ���������
            break;
        case CHANGES::REQUESTS_ADDED:               //�������� �������
            for (auto& request:pref.requests_adds)
            {
                requests.push_back(request);
            }
            pref.changes &=~CHANGES::REQUESTS_ADDED;    //������ ������� �� ���������
            break;
        case CHANGES::FILES_TO_SEARCH_ADDED:        //�������� ����� ��� ��������������
            for (auto& file:pref.file_list_adds)
            {
                file_list.push_back(file);
            }
            pref.changes &=~CHANGES::FILES_TO_SEARCH_ADDED; //������ ������� �� ���������
            break;
        default:    //�� ������ ������� ����
            break;
        }
    }
}

//���������� ���� config.json
void ConverterJSON::GenerateConfig(Preference& pref)
{
    nlohmann::json config_template;				//������ ������ ��� ���������� ��������
    std::ofstream config_file(config_path);     //��������� ���� ��� ������ ��������
    if (config_file.is_open())
        config_file.clear();					//������� ���������� �����
    config_template["config"]["name"] = "SearchEngine - autogen";	//���������� ��� ���������� ���������
    config_template["config"]["version"] = "1.0 - autogen";         //���������� ������ ���������� ���������
    config_template["config"]["max_responses"] = pref.max_response;	//���������� ������������� ����� ��������

    for (auto& file : pref.file_list_adds)          //��������� ����� ��� ��������������
    {
        config_template["files"].push_back(file);
    }
    std::stringstream unformat_stream;              //��������� ����� ��� �������������������� ������
    std::string unformat_line, format_line;         //������ � ������������������ � ���������������� ������������
    unformat_stream << config_template;             //������ ������������������ ���������
    std::getline(unformat_stream, unformat_line);   //���������� ���������������� ��������� � ������
    size_t tabs = 0;                                //������� ���������
    for (size_t i = 0; i < unformat_line.length(); i++)
    {
        if (unformat_line[i] == '{'			//����� ����������� ������ ��������� �� ����� ������
            || unformat_line[i] == '['
            || unformat_line[i] == '(')
        {
            format_line += unformat_line[i];
            tabs++;							//������� ������� ��������� �� +1
            format_line += '\n';
            for (size_t j = 0; j < tabs; j++)
            {
                format_line += '\t';		//��������� ������ ���������� ���������
            }
        }
        else if (unformat_line[i] == '}'	//����� ����������� ������ ��������� �� ����� ������
            || unformat_line[i] == ']'
            || unformat_line[i] == ')')
        {
            tabs--;							//������� ������� ��������� �� -1
            format_line += '\n';
            for (size_t j = 0; j < tabs; j++)
            {
                format_line += '\t';		//��������� ������ ���������� ���������
            }
            format_line += unformat_line[i];
        }
        else if (unformat_line[i] == ',')	//����� ������� ��������� �� ����� ������
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
    config_file << format_line;             //���������� ������ � ����
    config_file.close();                    //��������� ����
}

//����������� ����� ������� � ������
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
