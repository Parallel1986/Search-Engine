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
{	//Устанавливаем максимальное число ответов
	this->max_response = max_response; 
}

QList<QList<RelativeIndex>> SearchServer::search(const QList<QString>& queries_input)
{
    QList<QList<RelativeIndex>> result;				//Итоговый список для вывода

	for (auto& request_line : queries_input)
    {
        QSet<QString> words_list;
        QRegExp words("\\W+");
        auto word_list = request_line.split(words, Qt::SkipEmptyParts);
        for (auto request = word_list.begin()
            ; request != word_list.end()
            ; request++)
        {
            words_list.insert(*request);
        }

        QList<RelativeIndex> request_result;		//Результат запроса
        QMultiMap<int, QString> sorted_words;		//Отсортированный список слов по частоте встречаемости

        auto iterator = words_list.begin();         //Начало списка слов
        auto iterator_end = words_list.end();       //Конец списка слов


        while (iterator != iterator_end)			//Проходим по списку и считаем количество совпадений для каждого слова
		{
            QList<Entry> helper(_index.GetWordCount(*iterator));	//Получаем список документов содержащих слово и его количество
            if (!helper.empty())									//Если список пуст, то меняем флаг not_matched на true и выходим из цикла
			{
                int counter = 0;					//Количество вхождений слова
                for (auto& entry : helper)			//Проходим по списку документов и считаем количество
				{
					counter += entry.count;
				}
                sorted_words.insert( counter, (*iterator));	//Добавляем в список слов отсортированный по количеству вхождений
			}
            iterator++;		//Смещаем итератор на следующее слово
        }	//На выходе получаем список слов отсортированных по частоте вхождений

        if (sorted_words.empty())	//Если отсутствует слово во всех документах выводим пустой вектор
		{
            QList<RelativeIndex> empty_vector;
            result.append(empty_vector);
		}
        else	//Иначе считаем абсолютную и относительную релевантность
        {
            //Получаем список документов содержащих самое редкое слово и колчичество его вхождений
			auto it = sorted_words.begin();
            auto it_end = sorted_words.end();
            QMap<int/*doc_id*/, int/*relevance*/> doc_absolute_relevance;		//Список документов для подсчёта релевантности
            QList<Entry> helper(_index.GetWordCount(it.value()));               //Вспомогательный список
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
                        helper = _index.GetWordCount(it.value());
				}
			}

            //Проходим по оставшимся словам и добавляем их количество вхождений к имеющемуся,
            //попутно вычисляя максимальное количество вхождений
			if (it != it_end)
			{
				it++;
            }
            while (it != it_end)
			{
                helper = _index.GetWordCount(it.value());
				for (auto& entry : helper)
				{
                    if (doc_absolute_relevance.contains(entry.doc_id))
                        doc_absolute_relevance[entry.doc_id] += entry.count;
				}
				it++;
            }	//На выходе получаем заполненый список документов с абсолютными релевантностями
            int max_count = 0;
            for (auto& count:doc_absolute_relevance)
            {
                if (max_count < count)
                    max_count = count;
            }   //На выходе получаем значение максимальной абсолютной релевантности

            //Проходим по списку и считаем относительную релевантность, добавляя в новый список релевантности
            std::multimap<float, size_t, std::greater<float>> relevance_map;    //для сортировки по убыванию релевантности
            for (auto document = doc_absolute_relevance.begin(); document != doc_absolute_relevance.end(); document++)
			{
                relevance_map.insert({ ((float)((float)document.value() / (float)max_count)), document.key() });
            }	//На выходе получаем отсортированный список релевантности документов

            //Заполняем список релевантностей
			for (auto& rel : relevance_map)
			{
				RelativeIndex ri;
				ri.rank = rel.first;
				ri.doc_id = rel.second;
                request_result.append(ri);
			}
            //Добавляем список результата в главный список
            result.append(request_result);
		}		
	}
	return result;
}
#endif
