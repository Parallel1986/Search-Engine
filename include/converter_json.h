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
    //�������� ������ �� ���������
    static ConverterJSON& GetInstance();
    ~ConverterJSON();
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

    //�������� � ���� answers.json ���������� ��������� ��������
    void PutAnswers(std::vector<std::vector<std::pair<int, float>>>& answers);

    //������������� ���������� ������ ������������ config.json
    void Initialize();

    //�������� ���������
    void SetPreferences(Preference& pref);
private:
    static ConverterJSON* instance;
    void GenerateConfig(Preference& pref);
    ConverterJSON() = default;                  //������� �����������, ���� ������ ���� ��������������� �����
    std::string GetRequestNumber(int number);   //�������� ������ � ������� �������
    void operator=(ConverterJSON&);             //������� �������� ������������
    std::string engine_name;                    //��� ���������� ���������
    std::string engine_version;                 //������ ���������� ���������
    int max_responses = 5;                      //������������ ���������� ������� �� ������
    std::vector<std::string> requests;          //������ ��������� ��������
    std::vector<std::string> file_list;         //������ ����������� ������, � ������� ������������ �����
    bool initialized = false;                   //��������� �� ������������� ����������
    std::string config_path = "config.json";    //���� �� config.json
    std::string requests_path= "requests.json"; //���� �� requests.json
    std::string answers_path= "answers.json";   //���� �� answers.json
};
