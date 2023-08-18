#ifndef INVERTED_INDEX
#define INVERTED_INDEX

#include "../include/inverted_index.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

//#include <QtConcurrent/QtConcurrent>

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs)
{
	//for (auto& document : input_docs)
	//{
	//	std::ifstream file(document);
	//	if (file.is_open())
	//	{
	//		std::string content;
	//		file >> content;
	//		file.close();
	//		docs.push_back(content);
	//	}
	//	else
	//	{
	//		std::cerr << "Can not find file: " << document << std::endl;
	//		docs.push_back("");
	//	}
	//}

	for (auto& document : input_docs)
	{
		docs.push_back(document);
	}
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word)
{
	if (freq_dictionary.empty())
	{
		CreateFrequencyDictionary();
	}
	if (freq_dictionary.find(word) == freq_dictionary.end())
	{
		std::vector<Entry> list;
		return list;
	}
	else
	{
		std::vector<Entry> list = (*freq_dictionary.find(word)).second;
		return list;
	}	
}

//	!!!распараллелить
void InvertedIndex::CreateFrequencyDictionary()
{
	size_t doc_id = 0;
	
	for (auto& document : docs)
	{
		MakeIndexDocument(document, doc_id, *this);
		doc_id++;
	}
}

void MakeIndexDocument(std::string& document, size_t doc_id, InvertedIndex& caller)
{
	std::stringstream buffer;
	buffer << document;
	std::string word;
	std::map<std::string, int> wordList;
	while (std::getline(buffer, word))
	{
		std::size_t prev = 0, pos;
		while ((pos = word.find_first_of(" ';", prev)) != std::string::npos)
		{
			if (pos > prev)
			{
				std::string subWord = word.substr(prev, pos - prev);
				if (wordList.find(subWord) != wordList.end())
				{
					wordList.at(subWord) += 1;
				}
				else
					wordList.insert({ subWord, 1 });
			}
			prev = pos + 1;
		}
		if (prev < word.length())
		{
			std::string subWord = word.substr(prev, std::string::npos);
			if (wordList.find(subWord) != wordList.end())
			{
				wordList.at(subWord) += 1;
			}
			else
				wordList.insert({ subWord, 1 });
		}
	}
	caller.freq_dictionary_access.lock();	
	for (auto& word : wordList)
	{
		Entry wordFrequency;
		wordFrequency.doc_id = doc_id;
		wordFrequency.count = word.second;
		caller.freq_dictionary[word.first].push_back(wordFrequency);
	}
	caller.freq_dictionary_access.unlock();	
}

#endif
