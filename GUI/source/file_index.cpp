#ifndef FILE_INDEX
#define FILE_INDEX
#include "../include/file_index.h"
#include <set>
#include <sstream>
#include <functional>

#include <QSet>
#include <QRegularExpression>

void SearchServer::setMaxResponse(int max_response)
{	//Устанавливаем максимальное число ответов //Setting maximum number of responses
	this->max_response = max_response; 
}

QList<QList<RelativeIndex>> SearchServer::search(const QList<QString>& queries_input)
{
    QList<QList<RelativeIndex>> result;				//итоговый вектор для вывода

	for (auto& request_line : queries_input)
    {
        QSet<QString> words_list;
        for (auto request : request_line.split(QRegularExpression("\\W+"), Qt::SkipEmptyParts))
        {
            words_list.insert(request);
        }

		/**************/
        QList<RelativeIndex> request_result;		//результат запроса
        QMap<int, QString> sorted_words;			//Отсортированный список слов по частоте встречаемости

        auto iterator = words_list.begin();         //начало списка слов
        auto iterator_end = words_list.end();       //конец списка слов
        QMap<int, int> doc_absolute_relevance;		//<doc_id, relevance> список документов для подсчёта релевантности

		while (iterator != iterator_end)			//проходим по списку и считаем количество совпадений для каждого слова
		{
            QList<Entry> helper(_index.GetWordCount(*iterator));	//получаем список документов содержащих слово и его количество
            if (!helper.empty())									//если список пуст, то меняем флаг not_matched на true и выходим из цикла
			{
                int counter = 0;					//количество вхождений слова
                for (auto& entry : helper)			//проходим по списку документов и считаем количество
				{
					counter += entry.count;
				}
                sorted_words.insert( counter, (*iterator));	//добавляем в список слов отсортированный по количеству вхождений
			}
            iterator++;		//смещаем итератор на следующее слово
		}	//на выходе получаем список слов отсортированных по частоте вхождений		

        if (sorted_words.empty())	//если отсутствует слово во всех документах выводим пустой вектор
		{
            QList<RelativeIndex> empty_vector;
            result.append(empty_vector);
		}
        else			//иначе считаем абсолютную и относительную релевантность
        {               //получаем список документов самого редкого слова для подсчёта релевантности
			//получаем список документов содержащих самое редкое слово и колчичество его вхождений
			auto it = sorted_words.begin();
			auto it_end = sorted_words.end();

            QList<Entry> helper(_index.GetWordCount(it.value()));

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
					it++;
					if (it != it_end)
                        helper = _index.GetWordCount(it.value());
				}
			}

			//проходим по оставшимся словам и добавляем их количество вхождений к имеющемуся, попутно вычисляя максимальное количество вхождений
			if (it != it_end)
			{
				it++;
			}
            int max_count = 0;
            while (it != it_end)
			{
                helper = _index.GetWordCount(it.value());
				for (auto& entry : helper)
				{
                    if (doc_absolute_relevance.contains(entry.doc_id))
					{
                        doc_absolute_relevance[entry.doc_id] += entry.count;
					}
                    if (max_count < doc_absolute_relevance[entry.doc_id])
                    {
                        max_count = doc_absolute_relevance[entry.doc_id];
                    }
				}
				it++;
            }		// на выходе получаем заполненый список документов с абсолютными релевантностями и максимальную релевантность

			//проходим по списку и считаем относительную релевантность, добавляя в новый список релевантности
			std::multimap<float, size_t, std::greater<float>> relevance_map;
            for (auto document = doc_absolute_relevance.begin(); document != doc_absolute_relevance.end(); document++)
			{
                relevance_map.insert({ (float)((float)document.value() / (float)max_count), document.key() });
			}		//на выходе получаем отсортированный список релевантности документов

			//заполняем вектор релевантностей
			for (auto& rel : relevance_map)
			{
				RelativeIndex ri;
				ri.rank = rel.first;
				ri.doc_id = rel.second;
                request_result.append(ri);
			}
			//добавляем вектор результата в главный вектор
            result.append(request_result);
		}
		/**************/
	}
	return result;
}
#endif
