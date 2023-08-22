#ifndef INVERTED_INDEX
#define INVERTED_INDEX


//#define QT_THREADS
#define STD_THREADS

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
{	//�������� �� �������� ���������� � ��������� �� � ������ //Going through incoming documents and filling document list
	for (auto& document : input_docs)
	{
		docs.push_back(document);
	}
	CreateFrequencyDictionary();	//������ ��������� ������� //Creating frequency dictionary
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word)
{
	if (freq_dictionary.empty())	//���� ��������� ������� ������, �� ������ ���//Creates frequency dictionari if it is empty
		CreateFrequencyDictionary();
	if (freq_dictionary.find(word) == freq_dictionary.end())
	{	//���� ����� �����������, �� ���������� ������ ������ //Returns empty list if word is missing
		std::vector<Entry> list;
		return list;
	}
	else
	{	//���� ���������� ������ ���������� � ���������� ��������� //Othervise, returns list of document numbers and entries number
		std::vector<Entry> list = (*freq_dictionary.find(word)).second;
		return list;
	}
}

void InvertedIndex::CreateFrequencyDictionary()
{
	size_t doc_id = 0;		//��� ��������� ������� ���������� //For document`s number generation
#ifdef QT_THREADS	//������������ ������ �� ������ Qt //Multithreading using Qt
	QList<QFuture<std::map<std::string, int>>> index_threads;
	for (auto& document : docs)
    {
		index_threads.append(QtConcurrent::run([&document]()
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
				return wordList;
			}));		
	}
	for (auto& thread : index_threads)
	{
		thread.waitForFinished();
	}
	for (auto& result : index_threads)
	{
		/*for (auto& word : )
		{
			Entry wordFrequency;
			wordFrequency.doc_id = doc_id;
			wordFrequency.count = word.second;
			((InvertedIndex*)object)->freq_dictionary[word.first].push_back(wordFrequency);
		}*/
	}  
#endif // QT_THREADS

#ifdef STD_THREADS	//����������� ������ �� ������ std::thread //Multithreading by using std::thread
	std::vector<std::thread> index_threads;		//������ ������� //List of threads
	std::vector< std::map<std::string, int>*> word_vector;	//������ ���� //List of words
	for (auto& document : docs)
    {	//�������� �� ���������� � ������ ��������� ������� ��� ������� �� ���
		//Going through documents` list and creating frequency dictionary for each one
		auto new_map = new std::map<std::string, int>;	//��������� ������� //Frequency dictionary
		word_vector.emplace_back(new_map);	//��������� ������� � ����� ������� //Inserting dictionary to general list
        index_threads.emplace_back(std::thread([&document, new_map]()	//��������� ������ //Launching threads
			{
				std::stringstream buffer;	//��������� ����� ��� ���������� ������ //String buffer for string reading
				buffer << document;			//��������� ����� ���������� ��������� //Filling the buffer by document content
				std::string word;			//��������� ����� //Single word
				//std::map<std::string, int> wordList;	//������� ���� � ����������� ��������� //Dictionary of words and word`s entries
				while (std::getline(buffer, word))
				{	//�������� �� ����� ��������� �����, ��������� ����������� //Extracting words by using dividers
					std::size_t prev = 0, pos;		//��������� ��������� ���������� � ������ //Initial pointers` positions
					while ((pos = word.find_first_of(" ';", prev)) != std::string::npos)
					{	//���� ������ ��������� ����������� � ��������� ����� �� ������� //Searching first entry of divider and checking reaching out of borders
						if (pos > prev)
						{	//���� ����������� ������ ����������� �������, �� �������� �����
							//If divider far than initial position then extracting word
							std::string subWord = word.substr(prev, pos - prev);
							if (new_map->find(subWord) != new_map->end())
								new_map->at(subWord) += 1;	//���� ����� ���� � �������, �� ��������� � ���������� �������
																	//If word already in the list adding 1 to its count
							else	//����� ��������� ����� � ������� � ��������� ���������� ��� 1
									//Otherewise inserting word to the dictionary and setting cout as 1
								new_map->insert({ subWord, 1 });
						}
						prev = pos + 1; //������� ��������� �� �����������
					}
					if (prev < word.length())
					{	//���� ��������� ������ �� ������� �� �������� ��������� �����
						//If pointer going after line`s end substracting last word
						std::string subWord = word.substr(prev, std::string::npos);
						if (new_map->find(subWord) != new_map->end())
							new_map->at(subWord) += 1;		//���� ����� ���� � �������, �� ��������� � ���������� �������
																	//If word already in the list adding 1 to its count
						else	//����� ��������� ����� � ������� � ��������� ���������� ��� 1
								//Otherewise inserting word to the dictionary and setting cout as 1
							new_map->insert({ subWord, 1 });
					}
				}
			}));
	}
	for (auto& thread : index_threads)
	{	//������������ ������ //Joining threads
		thread.join();
	}
	for (auto& wordList : word_vector)
	{	//��������� ��������� ������� ��� ������� �����
		//Filling frequeency dictionary for each word
		for (auto& word : *wordList)
		{
			Entry wordFrequency;
			wordFrequency.doc_id = doc_id;
			wordFrequency.count = word.second;
			freq_dictionary[word.first].push_back(wordFrequency);
		}
		doc_id++;
	}
	for (auto& vector : word_vector)
	{	//����������� ������ //Releasing the memory
		delete vector;
	}
#endif // STD_THREADS

#ifndef THREADS		//���� ��������������� ��������� //If multithreading is turned off
	for (auto& document : docs)
	{
		//MakeIndexDocument(this, document, doc_id); //������� ������� //The function is deleted
		doc_id++;
	}
#endif // !THREADS

}


#endif
