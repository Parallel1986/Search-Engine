#pragma once
#include <vector>
#include <map>
#include <string>
#include <mutex>
#include <QList>
#include <QMap>
#include <QObject>

struct Entry
{
    int doc_id, count;

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
    void CreateFrequencyDictionary();
    QList<QString> docs;                            // список содержимого документов
    QMap<QString, QList<Entry>> freq_dictionary;    // частотный словарь
};
