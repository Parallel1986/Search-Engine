#ifndef INVERTED_INDEX
#define INVERTED_INDEX

#include <QSet>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include "../include/inverted_index.h"
#include <fstream>
#include <iostream>
#include <sstream>

//Updates documens' base
void InvertedIndex::UpdateDocumentBase(QList<QString> input_docs)
{
    docs.clear();
    for (auto& document : input_docs)
	{
        docs.append(document);
	}
    CreateFrequencyDictionary();
}

//Gets word's count in frequency dictionary
QList<Entry> InvertedIndex::GetWordCount(const QString& word)
{
    if (freq_dictionary.empty())
		CreateFrequencyDictionary();
	if (freq_dictionary.find(word) == freq_dictionary.end())
    {
        QList<Entry> list;
		return list;
	}
	else
    {
        QList<Entry> list = freq_dictionary.value(word);
		return list;
	}
}

//Creates frequency dictionary
void InvertedIndex::CreateFrequencyDictionary()
{
    freq_dictionary.clear();
    size_t doc_id = 0;

    QList<QFuture<void>> index_threads;
	for (auto& document : docs)
    {
        index_threads.append(QtConcurrent::run([&document, doc_id,/*dictionaries,*/ this]()
			{                
                QMap<QString,Entry> dictionary;
                //Spliting document to separate words
                for (auto& word : document.split(QRegExp("\\W+"), Qt::SkipEmptyParts))
                {
                    if (dictionary.contains(word))
                        dictionary[word].count +=1;
                    else
                    {
                        dictionary[word] = Entry(doc_id,1);
                    }
                }
                //Inserting entries to the dictionary
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

	for (auto& thread : index_threads)
	{
		thread.waitForFinished();		
    }
}


#endif
