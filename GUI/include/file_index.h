#pragma once
#include "inverted_index.h"

struct RelativeIndex {
    int doc_id;
	float rank;
	bool operator ==(const RelativeIndex& other) const {
		return (doc_id == other.doc_id && rank == other.rank);
	}
};
class SearchServer {
public:
	/**
	* @param idx в конструктор класса передаётся ссылка на класс
	InvertedIndex,
	* чтобы SearchServer мог узнать частоту слов встречаемых в
	запросе
    */
	SearchServer(InvertedIndex& idx) : _index(idx) { };

    /**
	* Метод обработки поисковых запросов
	* @param queries_input поисковые запросы взятые из файла
	requests.json
	* @return возвращает отсортированный список релевантных ответов для
	заданных запросов
	*/
    QList<QList<RelativeIndex>> search(const
        QList<QString>& queries_input);
	void setMaxResponse(int max_response);
private:
    InvertedIndex _index;
	int max_response = 5;
};

