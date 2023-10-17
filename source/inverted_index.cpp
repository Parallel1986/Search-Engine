#ifndef INVERTED_INDEX
#define INVERTED_INDEX

#include <QSet>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

#include "../include/inverted_index.h"
#include <fstream>
#include <iostream>
#include <sstream>
QStringList IndexParser::splitTextToWords(QString text)
{
    return text.split(QRegExp("\\W+"), Qt::SkipEmptyParts);
}

QMap<QString,Entry> IndexParser::fillEntryDictionary(const QString& text_of_document, int doc_id)
    {
        QMap<QString,Entry> dictionary;
        QStringList words_in_text = IndexParser::splitTextToWords(text_of_document);
        for (auto& word : words_in_text)
        {
            if (dictionary.contains(word))
                dictionary[word].count +=1;
            else
                dictionary[word] = Entry(doc_id,1);
        }
        return dictionary;
    }

//Updates documens' base
void InvertedIndex::updateDocumentBase(QList<QString> input_docs)
{
    docs.clear();
    for (auto& document : input_docs)
	{
        docs.append(document);
	}
    createFrequencyDictionary();
}

//Gets word's count in frequency dictionary
QList<Entry> InvertedIndex::getWordCount(const QString& word)
{
    if (freq_dictionary.isEmpty())
        createFrequencyDictionary();

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
void InvertedIndex::createFrequencyDictionary()
{
    freq_dictionary.clear();
    size_t doc_id = 0;

    QList<QFuture<QMap<QString,Entry>>> index_threads;

    for (auto& document : docs)
    {
        index_threads.append(QtConcurrent::run(IndexParser::fillEntryDictionary,document, doc_id));
            ++doc_id;
    }

    for (auto& thread : index_threads)
    {
        thread.waitForFinished();
    }
    for (auto& thread : index_threads)
    {
        auto dictionary = thread.result();
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
    }
}


#endif
