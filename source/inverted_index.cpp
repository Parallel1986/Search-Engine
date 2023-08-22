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
{	//Проходим по входящим документам и вставляем их в список //Going through incoming documents and filling document list
	for (auto& document : input_docs)
	{
		docs.push_back(document);
	}
	CreateFrequencyDictionary();	//Создаём частотный словарь //Creating frequency dictionary
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word)
{
	if (freq_dictionary.empty())	//Если частотный словарь пустой, то создаёт его//Creates frequency dictionari if it is empty
		CreateFrequencyDictionary();
	if (freq_dictionary.find(word) == freq_dictionary.end())
	{	//Если слово отсутствует, то возвращает пустой список //Returns empty list if word is missing
		std::vector<Entry> list;
		return list;
	}
	else
	{	//Инае возвращает список документов и количество вхождений //Othervise, returns list of document numbers and entries number
		std::vector<Entry> list = (*freq_dictionary.find(word)).second;
		return list;
	}
}

void InvertedIndex::CreateFrequencyDictionary()
{
	size_t doc_id = 0;		//Для генерации номеров документов //For document`s number generation
#ifdef QT_THREADS	//Параллельные потоки на основе Qt //Multithreading using Qt
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

#ifdef STD_THREADS	//Паралельные потоки на основе std::thread //Multithreading by using std::thread
	std::vector<std::thread> index_threads;		//Список потоков //List of threads
	std::vector< std::map<std::string, int>*> word_vector;	//Список слов //List of words
	for (auto& document : docs)
    {	//Проходим по документам и создаём частотный словарь для каждого из них
		//Going through documents` list and creating frequency dictionary for each one
		auto new_map = new std::map<std::string, int>;	//Частотный словарь //Frequency dictionary
		word_vector.emplace_back(new_map);	//Вставляем словарь в общий спписок //Inserting dictionary to general list
        index_threads.emplace_back(std::thread([&document, new_map]()	//Запускаем потоки //Launching threads
			{
				std::stringstream buffer;	//Строковый буфер для считывания строки //String buffer for string reading
				buffer << document;			//Заполняем буфер содержимым документа //Filling the buffer by document content
				std::string word;			//Отдельное слово //Single word
				//std::map<std::string, int> wordList;	//Словарь слов с количеством вхождений //Dictionary of words and word`s entries
				while (std::getline(buffer, word))
				{	//Выделяем из строк отдельные слова, используя разделители //Extracting words by using dividers
					std::size_t prev = 0, pos;		//Начальные положения указателей в строке //Initial pointers` positions
					while ((pos = word.find_first_of(" ';", prev)) != std::string::npos)
					{	//Ищем первое вхождение разделителя и проверяем выход за границы //Searching first entry of divider and checking reaching out of borders
						if (pos > prev)
						{	//Если разделитель дальше предыдуущей позиции, то выделяем слово
							//If divider far than initial position then extracting word
							std::string subWord = word.substr(prev, pos - prev);
							if (new_map->find(subWord) != new_map->end())
								new_map->at(subWord) += 1;	//Если слово ууже в словаре, то добавляем к количеству единицу
																	//If word already in the list adding 1 to its count
							else	//Иначе добавляем слово в словарь и указываем количество как 1
									//Otherewise inserting word to the dictionary and setting cout as 1
								new_map->insert({ subWord, 1 });
						}
						prev = pos + 1; //смещаем указатель за раздклитель
					}
					if (prev < word.length())
					{	//Если указатель уходит за границы то выделяем последнее слово
						//If pointer going after line`s end substracting last word
						std::string subWord = word.substr(prev, std::string::npos);
						if (new_map->find(subWord) != new_map->end())
							new_map->at(subWord) += 1;		//Если слово ууже в словаре, то добавляем к количеству единицу
																	//If word already in the list adding 1 to its count
						else	//Иначе добавляем слово в словарь и указываем количество как 1
								//Otherewise inserting word to the dictionary and setting cout as 1
							new_map->insert({ subWord, 1 });
					}
				}
			}));
	}
	for (auto& thread : index_threads)
	{	//Присоединяем потоки //Joining threads
		thread.join();
	}
	for (auto& wordList : word_vector)
	{	//Заполняем частотный словарь для каждого слова
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
	{	//Освобождаем память //Releasing the memory
		delete vector;
	}
#endif // STD_THREADS

#ifndef THREADS		//Если многопоточность выключена //If multithreading is turned off
	for (auto& document : docs)
	{
		//MakeIndexDocument(this, document, doc_id); //Функция удалена //The function is deleted
		doc_id++;
	}
#endif // !THREADS

}


#endif
