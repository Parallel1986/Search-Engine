#ifndef FILE_INDEX
#define FILE_INDEX
#include "../include/file_index.h"
#include <set>
#include <sstream>
#include <functional>

#include <QFile>

#include <QSet>
#include <QRegularExpression>

void SearchServer::setMaxResponse(int max_response)
{
	this->max_response = max_response; 
}

QList<QList<RelativeIndex>> SearchServer::search(const QList<QString>& queries_input)
{
    QList<QList<RelativeIndex>> result;

	for (auto& request_line : queries_input)
    {
        QSet<QString> set_of_words;
        auto word_list = IndexParser::splitTextToWords(request_line);
        for (auto request = word_list.begin()
            ; request != word_list.end()
            ; request++)
        {
            set_of_words.insert(*request);
        }

        QList<RelativeIndex> result_of_request;
        QMultiMap<int, QString> sorted_words;

        auto iterator = set_of_words.begin();
        auto iterator_end = set_of_words.end();

        while (iterator != iterator_end)
		{
            QList<Entry> helper(_index->getWordCount(*iterator));
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

            QMap<int/*doc_id*/, int/*relevance*/> document_absolute_relevance;

            QList<Entry> helper(_index->getWordCount(it.value()));

            while (document_absolute_relevance.size() < max_response && it != it_end)
			{
				for (auto& entry : helper)
				{
                    if (document_absolute_relevance.contains(entry.doc_id))
                        document_absolute_relevance[entry.doc_id] += entry.count;
                    else if (document_absolute_relevance.size() < max_response)
                        document_absolute_relevance.insert( entry.doc_id, entry.count);
				}
                if (document_absolute_relevance.size() < max_response)
				{
                    ++it;
					if (it != it_end)
                        helper = _index->getWordCount(it.value());
				}
			}

			if (it != it_end)
				it++;

            while (it != it_end)
			{
                helper = _index->getWordCount(it.value());
				for (auto& entry : helper)
				{
                    if (document_absolute_relevance.contains(entry.doc_id))
                        document_absolute_relevance[entry.doc_id] += entry.count;
				}
				it++;
            }

            int max_count = 0;
            for (auto& count:document_absolute_relevance)
            {
                if (max_count < count)
                    max_count = count;
            }

            std::multimap<float, unsigned int, std::greater<float>> sorted_relevance_map;
            for (auto document = document_absolute_relevance.begin();
                      document != document_absolute_relevance.end();
                      document++)
                sorted_relevance_map.insert({ ((float)((float)document.value() / (float)max_count)), document.key() });

            for (auto& rel : sorted_relevance_map)
			{
                result_of_request.append({rel.second,rel.first});
            }
            result.append(result_of_request);
        }
	}
	return result;
}
#endif
