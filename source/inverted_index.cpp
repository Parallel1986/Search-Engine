#ifndef INVERTED_INDEX
#define INVERTED_INDEX


//#define QT_THREADS
//#define STD_THREADS

#include "../include/inverted_index.h"
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef STD_THREADS
#include <thread>
#define THREADS
#endif // STD_THREADS


#ifdef QT_THREADS
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#define THREADS
#endif // QT_THREADS

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
#ifdef QT_THREADS
	QList<QFuture<void>> index_threads;	
	for (auto& document : docs)
    {
		index_threads.append(QtConcurrent::run(InvertedIndex::MakeIndexDocument, ((void*)this), document, doc_id));
		//MakeIndexDocument(document, doc_id, *this);
		doc_id++;
	}
	for (auto& thread : index_threads)
	{
		thread.waitForFinished();
	}
#endif // QT_THREADS

#ifdef STD_THREADS
	std::vector<std::thread> index_threads;
	for (auto& document : docs)
    {
        index_threads.emplace_back(std::thread(MakeIndexDocument, document, doc_id, (*this)));
		//MakeIndexDocument(document, doc_id, *this);
		doc_id++;
	}
	for (auto& thread : index_threads)
	{
		thread.join();
	}
#endif // STD_THREADS

#ifndef THREADS
	for (auto& document : docs)
	{
		MakeIndexDocument(this, document, doc_id);
		doc_id++;
	}
#endif // !THREADS

}

void InvertedIndex::MakeIndexDocument(void* object, std::string& document, size_t doc_id)
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
	((InvertedIndex*)object)->freq_dictionary_access.lock();
	for (auto& word : wordList)
	{
		Entry wordFrequency;
		wordFrequency.doc_id = doc_id;
		wordFrequency.count = word.second;
		((InvertedIndex*)object)->freq_dictionary[word.first].push_back(wordFrequency);
	}
	((InvertedIndex*)object)->freq_dictionary_access.unlock();
}

#endif
