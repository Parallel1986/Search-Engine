#ifndef FILE_INDEX
#define FILE_INDEX
#include "../include/file_index.h"
#include "../include/converter_json.h"
#include <set>
#include <sstream>


std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string>& queries_input)
{
	std::vector<std::vector<RelativeIndex>> result;
	std::vector<std::set<std::string>> words_sets;
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
		words_sets.push_back(word_list);
	}
	std::vector<std::multimap<size_t, std::string>> word_ordered_list; // нужен ли?

	for (auto& w_set : words_sets)
	{
		std::multimap<size_t, std::string> ordered_words;
		for (auto& word : w_set)
		{
			size_t count = 0;
			std::vector<Entry> word_count(_index.GetWordCount(word));
			for (auto& entry : word_count)
			{
				count += entry.count;
			}
			ordered_words.insert({ count, word });
		}

		auto begin = ordered_words.begin();
		bool first_loop = true;
		std::vector<size_t> doc_list; // список документов, в которых встречаются все слова
		while (begin != ordered_words.end())
		{
			auto end = ordered_words.upper_bound((*begin).first);
			std::vector<Entry> word_location(_index.GetWordCount((*begin).second));
			while (begin != end)
			{
				if (first_loop)
				{
					for (auto& element : word_location)
					{
						doc_list.push_back(element.doc_id);
					}
					first_loop = false;
				}
				else
				{
					std::vector<size_t> dont_match;
					for (auto& doc : doc_list)
					{
						bool has_match = false;
						for (auto& element : word_location)
						{
							if (element.doc_id == doc)
							{
								has_match = true;
							}
						}
						if (!has_match)
						{
							dont_match.push_back(doc);
						}
					}
					if (!dont_match.empty())
					{
						for (auto& doc_to_del : dont_match)
						{
							auto to_del = doc_list.end();
							for (auto it = doc_list.begin(); it != doc_list.end();it++)
							{
								if ((*it) == doc_to_del)
								{
									to_del = it;
								}
							}
							if (to_del != doc_list.end())
							{
								doc_list.erase(to_del);								
							}
						}
					}
				}
				begin++;
			}
		}// Получаем список документов, в которых встречаются все слова из запроса
		
		 //Опрелеояем релевантность
		if (!doc_list.empty())
		{
			std::vector<std::pair<size_t, size_t>> absolute_relevance;
			for (auto& doc : doc_list)
			{
				size_t count = 0;
				for (auto& word : w_set)
				{
					std::vector<Entry> word_instances(_index.GetWordCount(word));
					/*for (auto& entry : _index.GetWordCount(word);)
					{
						word_instances.push_back(entry);
					}*/
					auto record = word_instances.begin();
					while ((*record).doc_id != doc)
					{
						record++;
					}
					count += (*record).count;
				}
				absolute_relevance.emplace_back(std::make_pair(doc, count));
			}
			size_t max_relevance = 0;
			for (auto& relevance : absolute_relevance)
			{
				if (max_relevance < relevance.second)
				{
					max_relevance = relevance.second;
				}
			}
			std::map<float, size_t, std::greater<float>> request_answer;
			for (auto& relevance : absolute_relevance)
			{
				request_answer.insert({ (float)(relevance.second / max_relevance), relevance.second });
			}
			std::vector<RelativeIndex> answer;
			for (auto it = request_answer.begin(); it != request_answer.end(); it++)
			{
				RelativeIndex index;
				index.doc_id = (*it).second;
				index.rank = (*it).first;
				answer.push_back(index);
			}
			result.push_back(answer);
		}		
	} // Получаем на выходе частотный список для каждого слова


	return result;
}
#endif
