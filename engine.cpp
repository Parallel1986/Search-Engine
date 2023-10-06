/*Search engine main file*/
#include "include/converter_json.h"
#include "include/inverted_index.h"
#include "include/file_index.h"

#include <iostream>
#define GUI

#ifdef GUI
#include "mainwindow.h"
#include <QApplication>
#endif // GUI

int main(int argC, char* argV[])
{
#ifdef GUI
    QApplication a(argC, argV);
    MainWindow w;
#endif // GUI
    
    ConverterJSON converter;
    try
    {
        converter.initialize();
    }
    catch (const std::exception& ex)
    {
        std::cerr << ex.what();
    }

    InvertedIndex index;
    index.UpdateDocumentBase(converter.GetTextDocuments());
    
    SearchServer server(index);
    server.setMaxResponse(converter.GetResponsesLimit());

    std::vector<std::vector<RelativeIndex>> search_results{ server.search(converter.GetRequests()) };
    std::vector < std::vector<std::pair<int, float>>> out_vector;
    for (auto& result : search_results)
    {
        std::vector<std::pair<int, float>> out;
        for (auto& rel_ind : result)
        {
            out.push_back({ rel_ind.doc_id,rel_ind.rank });
        }
        out_vector.push_back(out);
    }
    converter.putAnswers(out_vector);

    //std::cout << "Ready!\n";
#ifdef GUI
    w.show();
    return a.exec();
#endif // GUI    
}
