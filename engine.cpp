/*Search engine main file*/
#include "include/converter_json.h"
#include "include/inverted_index.h"
#include "include/file_index.h"

#include <iostream>
//using namespace std;
int main(int argC, char* argV[])
{
    ConverterJSON converter;
    converter.initialize();

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

    std::cout << "Ready!\n";
}
