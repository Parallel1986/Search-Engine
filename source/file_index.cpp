#ifndef FILE_INDEX
#define FILE_INDEX
#include "../include/file_index.h"
#include "../include/converter_json.h"
#include <set>
#include <sstream>
#include <functional>


std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input)
{
	std::vector<std::vector<RelativeIndex>> result;				//�������� ������ ��� ������

	for (auto& request_line : queries_input)
	{
		std::stringstream line_buffer;
		line_buffer << request_line;
		std::string word;
		std::set<std::string> word_list;
		while (std::getline(line_buffer, word))
		{
			std::size_t prev = 0, pos;
			while ((pos = word.find_first_of(" ';", prev)) != std::string::npos)
			{
				if (pos > prev)
				{
					std::string subWord = word.substr(prev, pos - prev);
					word_list.insert(subWord);
				}
				prev = pos + 1;
			}
			if (prev < word.length())
			{
				std::string subWord = word.substr(prev, std::string::npos);
				word_list.insert(subWord);
			}
		}
		/**************/
		std::vector<RelativeIndex> request_result;					//��������� �������
		std::multimap<size_t, std::string> sorted_words;			//��������������� ������ ���� �� ������� �������������

		auto iterator = word_list.begin();	//������ ������ ����
		auto iterator_end = word_list.end();	//����� ������ ����
		bool not_matched = false;									//��� ���������� ����� � ������� ����������
		std::map<size_t, size_t> doc_absolute_relevance;			//<doc_id, relevance> ������ ���������� ��� �������� ������������� 

		while (iterator != iterator_end && !not_matched)			//�������� �� ������ � ������� ���������� ���������� ��� ������� �����
		{
			std::vector<Entry> helper(_index.GetWordCount(*iterator));	//�������� ������ ���������� ���������� ����� � ��� ����������
			if (helper.empty())										//���� ������ ����, �� ������ ���� not_matched �� true � ������� �� �����
				not_matched = true;
			else													//����� ������� ���������� ��������� ����� � ����������
			{
				size_t counter = 0;									//���������� ��������� �����
				for (auto& entry : helper)					//�������� �� ������ ���������� � ������� ����������
				{
					counter += entry.count;
				}
				sorted_words.insert({ counter, (*iterator) });	//��������� � ������ ���� ��������������� �� ���������� ���������
				iterator++;											//������� �������� �� ��������� �����
			}
		}															//�� ������ �������� ������ ���� ��������������� �� ������� ���������		

		if (not_matched)											//���� ����������� ����� �� ���� ���������� ������� ������ ������
		{
			std::vector<RelativeIndex> empty_vector;
			result.push_back(empty_vector);
		}
		else														//����� ������� ���������� � ������������� �������������
		{															//�������� ������ ���������� ������ ������� ����� ��� �������� �������������	
			//�������� ������ ���������� ���������� ����� ������ ����� � ����������� ��� ���������
			auto it = sorted_words.begin();
			std::vector<Entry> helper(_index.GetWordCount((*it).second));
			for (auto& entry : helper)
			{
				doc_absolute_relevance.insert({ entry.doc_id, entry.count });
			}

			//�������� �� ���������� ������ � ��������� �� ���������� ��������� � ����������, ������� �������� ������������ ���������� ���������
			it++;
			size_t max_count = 0;
			auto it_end = sorted_words.end();
			while (it != it_end)
			{
				helper = _index.GetWordCount((*it).second);
				for (auto& entry : helper)
				{
					if (doc_absolute_relevance.find(entry.doc_id) != doc_absolute_relevance.end())
					{
						doc_absolute_relevance.at(entry.doc_id) += entry.count;		
						if (max_count < doc_absolute_relevance.at(entry.doc_id))
						{
							max_count = doc_absolute_relevance.at(entry.doc_id);
						}
					}
				}				
				it++;
			}		// �� ������ �������� ���������� ������ ���������� � ����������� ���������������

			//�������� �� ������ � ������� ������������� �������������, �������� � ����� ������ �������������
			std::multimap<float, size_t, std::greater<float>> relevance_map;
			for (auto& document : doc_absolute_relevance)
			{
				relevance_map.insert({ (float)((float)document.second / (float)max_count), document.first });
			}		//�� ������ �������� ��������������� ������ ������������� ����������

			//��������� ������ ��������������
			for (auto& rel : relevance_map)
			{
				RelativeIndex ri;
				ri.rank = rel.first;
				ri.doc_id = rel.second;
				request_result.push_back(ri);
			}
			//��������� ������ ���������� � ������� ������
			result.push_back(request_result);
		}
		/**************/
	}	
	return result;
}
#endif
