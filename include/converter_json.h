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
	* ����� ��������� ����������� ������
	* @return ���������� ������ � ���������� ������ �������������
	* � config.json
	*/
	std::vector<std::string> GetTextDocuments();
	/**
	* ����� ��������� ���� max_responses ��� ����������� �����������
	* ���������� ������� �� ���� ������
	* @return
	*/
	int GetResponsesLimit();
	/**
	* ����� ��������� �������� �� ����� requests.json
	* @return ���������� ������ �������� �� ����� requests.json
	*/
	std::vector<std::string> GetRequests();
	/**
	* �������� � ���� answers.json ���������� ��������� ��������
	*/
	void putAnswers(std::vector<std::vector<std::pair<int, float>>>
		answers);

	void initialize();
private:
	std::string getRequestNumber(int number);

	std::string engineName;						//Search engine name
	std::string engineVersion;					//Search eengine version
	int maxResponses = 5;						//Maximum number of responses�
	std::vector<std::string> requests;			//List of requests
	std::vector<std::string> fileList;			//List of files for search
//	std::vector<std::map<float,int>> answers;	//List of answers
};