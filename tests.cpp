#include <gtest/gtest.h>
#include "include/inverted_index.h"
#include "include/file_index.h"
#include <QRegularExpression>

using namespace std;

#define INVERTED_INDEX_READY
#define SEARCH_SERVER_READY
#define QT_SINGLE_THREAD

#ifdef QT_SINGLE_THREAD
TEST(TestQtTextSplitMethod, UniqeWords) {
    const QString doc = "london is the capital of great britain";
    QMap<QString,int> words_expected;
    words_expected.insert("london",1);
    words_expected.insert("is",1);
    words_expected.insert("the",1);
    words_expected.insert("capital",1);
    words_expected.insert("of",1);
    words_expected.insert("great",1);
    words_expected.insert("britain",1);

    QMap<QString,int> words_list;

    QStringList words_from_text = IndexParser::splitTextToWords(doc);

    for (auto& word : words_from_text)
    {
        if (words_list.contains(word))
        words_list[word]+=1;
        else
        {
            words_list[word] = 1;
        }
    }

    auto it_is = words_list.begin();
    auto it_expected = words_expected.begin();
    for (;
         it_is != words_list.end() && it_expected != words_expected.end();
         ++it_is, ++it_expected)
    {
        EXPECT_EQ(it_is.key(), it_expected.key()) << "Expected key: " << it_expected.key().toStdString()
                                                  << "\nGiven key: " << it_is.key().toStdString();
        EXPECT_EQ(it_is.value(), it_expected.value()) << "Expected value: " << it_expected.value()
                                                      << "\nGiven value: " << it_is.value();
    }
}

TEST(TestQtTextSplitMethod, RepeatedWords){
    const QString doc = "milk milk milk milk water water water";
    QMap<QString,int> words_list;

    QMap<QString,int> words_expected;
    words_expected.insert("milk",4);
    words_expected.insert("water",3);

    QStringList words_from_text = IndexParser::splitTextToWords(doc);
    for (auto& word : words_from_text)
    {
        if (words_list.contains(word))
        words_list[word]+=1;
        else
        {
            words_list[word] = 1;
        }
    }

    auto it_is = words_list.begin();
    auto it_expected = words_expected.begin();
    for (;it_is != words_list.end() && it_expected != words_expected.end();
         ++it_is, ++it_expected)
    {
        EXPECT_EQ(it_is.key(), it_expected.key()) << "Expected key: " << it_expected.key().toStdString()
                                                  << "\nGiven key: " << it_is.key().toStdString();
        EXPECT_EQ(it_is.value(), it_expected.value()) << "Expected value: " << it_expected.value()
                                                      << "\nGiven value: " << it_is.value();
    }
}
#endif


#ifdef INVERTED_INDEX_READY
void TestInvertedIndexFunctionality(
    const QList<QString>& docs,
    const QList<QString>& requests,
    const QList<QList<Entry>>& expected
) {
    QList<QList<Entry>> result;
    InvertedIndex idx;
    idx.updateDocumentBase(docs);
    for (auto& request : requests) {
        QList<Entry> word_count = idx.getWordCount(request);
        result.append(word_count);
    }
    EXPECT_EQ(result, expected);
}

TEST(TestCaseInvertedIndex, TestBasic) {
    const QList<QString> docs = {
    "london is the capital of great britain",
    "big ben is the nickname for the Great bell of the striking clock"
    };
    const QList<QString> requests = { "london", "the" };
    const QList<QList<Entry>> expected = {
    {
    {0, 1}
    }, {
    {0, 1}, {1, 3}
    }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestBasic2) {
    const QList<QString> docs = {
    "milk milk milk milk water water water",
    "milk water water",
    "milk milk milk milk milk water water water water water",
    "americano cappuchino"
    };
    const QList<QString> requests = { "milk", "water", "cappuchino" };
    const QList<QList<Entry>> expected = {
    {
    {0, 4}, {1, 1}, {2, 5}
    }, {
    {0, 3}, {1, 2}, {2, 5}
    }, {
    {3, 1}
    }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestInvertedIndexMissingWord) {
    const QList<QString> docs = {
    "a b c d e f g h i j k l",
    "statement"
    };
    const QList<QString> requests = { "m", "statement" };
    const QList<QList<Entry>> expected = {
    {
    }, {
    {1, 1}
    }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}
#endif // INVERTED_INDEX_READY

#ifdef SEARCH_SERVER_READY
TEST(TestCaseSearchServer, TestSimple) 
{
    const QList<QString> docs =
    {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    const QList<QString> request = { "milk water", "sugar" };
    const QList<QList<RelativeIndex>> expected =
    {
        {
            {2, 1.0f},
            {0, 0.7f},
            {1, 0.3f}
        },
        {}
    };
    InvertedIndex* idx = new InvertedIndex();
    idx->updateDocumentBase(docs);
    SearchServer srv(idx);
    QList<QList<RelativeIndex>> result = srv.search(request);
    EXPECT_EQ(result, expected);
    delete idx;
}

TEST(TestCaseSearchServer, TestTop5) 
{
    const QList<QString> docs =
    {
        "london is the capital of great britain",
        "paris is the capital of france",
        "berlin is the capital of germany",
        "rome is the capital of italy",
        "madrid is the capital of spain",
        "lisboa is the capital of portugal",
        "bern is the capital of switzerland",
        "moscow is the capital of russia",
        "kiev is the capital of ukraine",
        "minsk is the capital of belarus",
        "astana is the capital of kazakhstan",
        "beijing is the capital of china",
        "tokyo is the capital of japan",
        "bangkok is the capital of thailand",
        "welcome to moscow the capital of russia the third rome",
        "amsterdam is the capital of netherlands",
        "helsinki is the capital of finland",
        "oslo is the capital of norway",
        "stockholm is the capital of sweden",
        "riga is the capital of latvia",
        "tallinn is the capital of estonia",
        "warsaw is the capital of poland",
    };
    const QList<QString> request = { "moscow is the capital of russia" };
    const QList<QList<RelativeIndex>> expected =
    {
        {
            {7, 1},
            {14, 1},
            {0, 0.666666687},
            {1, 0.666666687},
            {2, 0.666666687}
        }
    };
    InvertedIndex* idx = new InvertedIndex();
    idx->updateDocumentBase(docs);
    SearchServer srv(idx);
    QList<QList<RelativeIndex>> result = srv.search(request);
    EXPECT_EQ(result, expected);
    delete idx;
}
#endif // SEARCH_SERVER_READY
