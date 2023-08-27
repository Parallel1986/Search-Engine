#ifndef FILE_INDEX
#define FILE_INDEX
#include "../include/file_index.h"
#include "../include/converter_json.h"
#include <set>
#include <sstream>
#include <functional>

void SearchServer::setMaxResponse(int max_response)
{	//Устанавливаем максимальное число ответов //Setting maximum number of responses
	this->max_response = max_response; 
}

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input)
{
	std::vector<std::vector<RelativeIndex>> result;				//итоговый вектор для вывода

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
		std::vector<RelativeIndex> request_result;					//результат запроса
		std::multimap<size_t, std::string> sorted_words;			//Отсортированный список слов по частоте встречаемости

		auto iterator = word_list.begin();	//начало списка слов
		auto iterator_end = word_list.end();	//конец списка слов
		std::map<size_t, size_t> doc_absolute_relevance;			//<doc_id, relevance> список документов для подсчёта релевантности 

		while (iterator != iterator_end)			//проходим по списку и считаем количество совпадений для каждого слова
		{
			std::vector<Entry> helper(_index.GetWordCount(*iterator));	//получаем список документов содержащих слово и его количество
			if (!helper.empty())										//если список пуст, то меняем флаг not_matched на true и выходим из цикла
			{
				size_t counter = 0;									//количество вхождений слова
				for (auto& entry : helper)					//проходим по списку документов и считаем количество
				{
					counter += entry.count;
				}
				sorted_words.insert({ counter, (*iterator) });	//добавляем в список слов отсортированный по количеству вхождений
			}
			iterator++;											//смещаем итератор на следующее слово
		}	//на выходе получаем список слов отсортированных по частоте вхождений		

		if (sorted_words.empty())											//если отсутствует слово во всех документах выводим пустой вектор
		{
			std::vector<RelativeIndex> empty_vector;
			result.push_back(empty_vector);
		}
		else														//иначе считаем абсолютную и относительную релевантность
		{															//получаем список документов самого редкого слова для подсчёта релевантности	
			//получаем список документов содержащих самое редкое слово и колчичество его вхождений
			auto it = sorted_words.begin();
			auto it_end = sorted_words.end();

			std::vector<Entry> helper(_index.GetWordCount((*it).second));

			while (doc_absolute_relevance.size() < max_response && it != it_end)
			{
				for (auto& entry : helper)
				{
					if (doc_absolute_relevance.find(entry.doc_id) != doc_absolute_relevance.end())
						doc_absolute_relevance.at(entry.doc_id) += entry.count;
					else if (doc_absolute_relevance.size() < max_response)
						doc_absolute_relevance.insert({ entry.doc_id, entry.count });
				}
				if (doc_absolute_relevance.size() < max_response)
				{
					it++;
					if (it != it_end)
						helper = _index.GetWordCount((*it).second);
				}
			}

			//проходим по оставшимся словам и добавляем их количество вхождений к имеющемуся, попутно вычисляя максимальное количество вхождений
			if (it != it_end)
			{
				it++;
			}
			while (it != it_end)
			{
				helper = _index.GetWordCount((*it).second);
				for (auto& entry : helper)
				{
					if (doc_absolute_relevance.find(entry.doc_id) != doc_absolute_relevance.end())
					{
						doc_absolute_relevance.at(entry.doc_id) += entry.count;
					}
				}
				it++;
			}		// на выходе получаем заполненый список документов с абсолютными релевантностями

			size_t max_count = 0;
			for (auto& doc : doc_absolute_relevance)
			{
				if (max_count < doc.second)
				{
					max_count = doc.second;
				}
			}

			//проходим по списку и считаем относительную релевантность, добавляя в новый список релевантности
			std::multimap<float, size_t, std::greater<float>> relevance_map;
			for (auto& document : doc_absolute_relevance)
			{
				relevance_map.insert({ (float)((float)document.second / (float)max_count), document.first });
			}		//на выходе получаем отсортированный список релевантности документов

			//заполняем вектор релевантностей
			for (auto& rel : relevance_map)
			{
				RelativeIndex ri;
				ri.rank = rel.first;
				ri.doc_id = rel.second;
				request_result.push_back(ri);
			}
			//добавляем вектор результата в главный вектор
			result.push_back(request_result);
		}
		/**************/
	}
	return result;
}
#endif
