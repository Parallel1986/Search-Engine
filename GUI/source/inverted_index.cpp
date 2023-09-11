#ifndef INVERTED_INDEX
#define INVERTED_INDEX

#include <QSet>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include "../include/inverted_index.h"
#include <fstream>
#include <iostream>
#include <sstream>




void InvertedIndex::UpdateDocumentBase(QList<QString> input_docs)
{	//Проходим по входящим документам и вставляем их в список
    docs.clear();
    for (auto& document : input_docs)
	{
        docs.append(document);
	}
    CreateFrequencyDictionary();	//Создаём частотный словарь
}

QList<Entry> InvertedIndex::GetWordCount(const QString& word)
{
    if (freq_dictionary.empty())	//Если частотный словарь пустой, то создаёт его
		CreateFrequencyDictionary();
	if (freq_dictionary.find(word) == freq_dictionary.end())
    {	//Если слово отсутствует, то возвращает пустой список
        QList<Entry> list;
		return list;
	}
	else
    {	//Инае возвращает список документов и количество вхождений
        QList<Entry> list = freq_dictionary.value(word);
		return list;
	}
}

void InvertedIndex::CreateFrequencyDictionary()
{
    freq_dictionary.clear();
    size_t doc_id = 0;		//Для генерации номеров документов

    QList<QFuture<void>> index_threads;     //Список для потоков
	for (auto& document : docs)
    {
        //Создаём поток для каждого документа
        index_threads.append(QtConcurrent::run([&document, doc_id,/*dictionaries,*/ this]()
			{                
                QMap<QString,Entry> dictionary; //Частотный словарь документа

                //Разбиваем текст документа на отдельные слова и вносим их в словарь
                for (auto& word : document.split(QRegExp("\\W+"), Qt::SkipEmptyParts))
                {
                    if (dictionary.contains(word))
                        dictionary[word].count +=1;
                    else
                    {
                        dictionary[word] = Entry(doc_id,1);
                    }
                }
                for (auto word = dictionary.begin();
                    word != dictionary.end()
                    ; ++word)
                {
                    if (freq_dictionary.contains(word.key()))
                        freq_dictionary[word.key()].append(word.value());
                    else
                        freq_dictionary.insert(word.key(),QList{word.value()});
                }

            }));
            ++doc_id;
	}
    //Ожидаем завершения всех потоков
	for (auto& thread : index_threads)
	{
		thread.waitForFinished();		
    }
}


#endif
