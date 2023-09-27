#ifndef FILE_INDEX
#define FILE_INDEX
#include "../include/file_index.h"
#include <set>
#include <sstream>
#include <functional>

#include <QFile>

#include <QSet>
#include <QRegularExpression>

//Sets maximal count of answers
void SearchServer::setMaxResponse(int max_response)
{
	this->max_response = max_response; 
}

//Make search
QList<QList<RelativeIndex>> SearchServer::search(const QList<QString>& queries_input)
{
    QList<QList<RelativeIndex>> result;

	for (auto& request_line : queries_input)
    {
        //Spliting requests to separate words
        QSet<QString> words_list;
        QRegExp words("\\W+");
        auto word_list = request_line.split(words, Qt::SkipEmptyParts);
        for (auto request = word_list.begin()
            ; request != word_list.end()
            ; request++)
        {
            words_list.insert(*request);
        }

        QList<RelativeIndex> request_result;
        QMultiMap<int, QString> sorted_words;

        auto iterator = words_list.begin();
        auto iterator_end = words_list.end();

        //Counting count of word's instances
        while (iterator != iterator_end)
		{
            QList<Entry> helper(_index.getWordCount(*iterator));
            if (!helper.empty())
			{
                int counter = 0;
                for (auto& entry : helper)
				{
					counter += entry.count;
				}
                sorted_words.insert( counter, (*iterator));
			}
            iterator++;
        }

        if (sorted_words.empty())
		{
            QList<RelativeIndex> empty_vector;
            result.append(empty_vector);
		}
        else
        {            
			auto it = sorted_words.begin();
            auto it_end = sorted_words.end();
            QMap<int/*doc_id*/, int/*relevance*/> doc_absolute_relevance;
            QList<Entry> helper(_index.getWordCount(it.value()));
            //Counting absolute relevance
            while (doc_absolute_relevance.size() < max_response && it != it_end)
			{
				for (auto& entry : helper)
				{
                    if (doc_absolute_relevance.contains(entry.doc_id))
                        doc_absolute_relevance[entry.doc_id] += entry.count;
					else if (doc_absolute_relevance.size() < max_response)
                        doc_absolute_relevance.insert( entry.doc_id, entry.count);
				}
				if (doc_absolute_relevance.size() < max_response)
				{
                    ++it;
					if (it != it_end)
                        helper = _index.getWordCount(it.value());
				}
			}

			if (it != it_end)
			{
				it++;
            }
            while (it != it_end)
			{
                helper = _index.getWordCount(it.value());
				for (auto& entry : helper)
				{
                    if (doc_absolute_relevance.contains(entry.doc_id))
                        doc_absolute_relevance[entry.doc_id] += entry.count;
				}
				it++;
            }

            //Counting maximal absolute relevance
            int max_count = 0;
            for (auto& count:doc_absolute_relevance)
            {
                if (max_count < count)
                    max_count = count;
            }

            //Counting relative relevance
            std::multimap<float, size_t, std::greater<float>> relevance_map;
            for (auto document = doc_absolute_relevance.begin(); document != doc_absolute_relevance.end(); document++)
			{
                relevance_map.insert({ ((float)((float)document.value() / (float)max_count)), document.key() });
            }

            //Filling relevance list
			for (auto& rel : relevance_map)
			{
				RelativeIndex ri;
				ri.rank = rel.first;
				ri.doc_id = rel.second;
                request_result.append(ri);
			}            
            result.append(request_result);
		}		
	}
	return result;
}
#endif
