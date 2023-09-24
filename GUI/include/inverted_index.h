#ifndef INVERTED_INDEX_H
#define INVERTED_INDEX_H
#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <QList>
#include <QMap>
#include <QObject>

//Structure of word's entry
struct Entry
{
    int doc_id, count;
    Entry(){}
    Entry(int in_doc_id, int in_count) : doc_id(in_doc_id), count(in_count){}

    // Данный оператор необходим для проведения тестовых сценариев
    bool operator ==(const Entry& other) const {
        return (doc_id == other.doc_id &&
                count == other.count);
    }
};

class InvertedIndex
{

public:
    InvertedIndex() = default;
/*
* Обновить или заполнить базу документов, по которой будем совершать
поиск
* @param texts_input содержимое документов
*/
    void UpdateDocumentBase(QList<QString> input_docs);

/*
* Метод определяет количество вхождений слова word в загруженной базе
документов
* @param word слово, частоту вхождений которого необходимо определить
* @return возвращает подготовленный список с частотой слов
*/
    QList<Entry> GetWordCount(const QString& word);

private:
    void CreateFrequencyDictionary();               //Creqtes frequency dictionary
    QList<QString> docs;                            //Contains list of documens' content
    QMap<QString, QList<Entry>> freq_dictionary;    //Frequency dictionary
};
#endif
