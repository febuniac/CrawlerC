#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
using namespace std;
#include <stdio.h>
#include <regex>
#include <vector>
#include <iterator>
std::vector< string > lista_links_produtos;
std::vector< string >lista_links_paginas;
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// reading the file with program info
void write_file(string info){
    ofstream myfile;
    myfile.open ("mycurlfile.txt");
    myfile << info;
    myfile.close();
}


string curl_downloadHTML(char *url){
    CURL *curl; //new curl handle to restart the connection
    CURLcode res;
    std::string readBuffer;
    std::string header_string;
    curl = curl_easy_init();
    if(curl) {//continue if curl can be initialized
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);//setup curl connection with preferences
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        
        write_file(readBuffer);
        // std::cout << readBuffer << std::endl;
    }
    return readBuffer;
}

void regex_parseHTML_prods(char *url){
    std::regex linksprod_reg("<a class=\"card-product-url\" href=\"([^\"]+)\"");
    auto html_pag= curl_downloadHTML(url); //My string in HTML whole page (reasBuffer)
    
    auto words_begin =
    std::sregex_iterator(html_pag.begin(), html_pag.end(), linksprod_reg);
    auto words_end = std::sregex_iterator();
    
    std::cout << "Found PRODUCTS:"
    << std::distance(words_begin, words_end)
    << " links:\n";
    
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        std::string match_str_prod = match.str();
        lista_links_produtos.push_back(match_str_prod);
        std::cout << match_str_prod << '\n';
    }
}

vector<std::string> regex_parseHTML_next_page(char *url){
    std::regex linkspages_reg("<li class=\"\"><a href=\"([^<]+)\"><span aria-label=\"Next\">");
    auto html_pag= curl_downloadHTML(url); //My string in HTML whole page (reasBuffer)
    
    auto words_begin =
    std::sregex_iterator(html_pag.begin(), html_pag.end(), linkspages_reg);
    auto words_end = std::sregex_iterator();
    
    std::cout << "Found NEXT: "
    << std::distance(words_begin, words_end)
    << " links:\n";
    std::string match_str_next;
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;
        match_str_next = match.str();
        lista_links_paginas.push_back(match_str_next);
        std::cout << match_str_next << '\n';
    }
    return lista_links_paginas;
}

std::vector<string> regex_parseHTML_next_page_loop(char *url){
    std::vector< string > next_link = regex_parseHTML_next_page(url);
    // std::string no_next_link = regex_parseHTML_no_next_page();
    std::string vazio ="";
    for (int i = 0; i < lista_links_paginas.size(); ++i){
        while(next_link[i] != vazio){
        curl_downloadHTML(url);
        regex_parseHTML_prods(url);
        regex_parseHTML_next_page(url);

        }
        for (int i = 0; i < lista_links_paginas.size(); ++i){
            std::cout << lista_links_paginas[i] << '\n';
        }
    }

    return lista_links_paginas;
    //LOOP QUE VAI NA PAGINA, BAIXA TODOS OS PRODUTOS
    //ACHA O NEXT CLICA
    //VAI NA NOVA PAGINA E BAIXA TODOS PRODUTOS
    // FAZ ISSO ATÉ O NEXT NÃO DAR MAIS MATCH (DISABLED==ACABOU TODAS AS NEXT PAGES)
}

// void regex_download_next_page_loop(){
//     CURL *curl;
//     CURLcode res;
//     std::string pageHTML;
//      for (int i = 0; i < lista_links_paginas.size(); ++i){
//         curl = curl_easy_init();
//         if(curl) {
//             curl_easy_setopt(curl, CURLOPT_URL, lista_links_paginas[i]);
//             curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//             curl_easy_setopt(curl, CURLOPT_WRITEDATA, &pageHTML);
//             res = curl_easy_perform(curl);
//             curl_easy_cleanup(curl);
//             write_file(pageHTML);
//             // std::cout << readBuffer << std::endl;
//         }

//     }
// }
// void regex_parse_next_page_loop(){ 
// }

int main(void)
{
    regex_parseHTML_next_page_loop("https://www.submarino.com.br/categoria/bebes/brinquedos-para-bebe");
    // curl_downloadHTML();
    // regex_parseHTML_prods();
    // regex_parseHTML_next_page();
    return 0;
}



// //GET_________________________________________________
        // curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
        // /* Perform the request */
        // curl_easy_perform(curl); 
        // //_____________________________________________________
// string regex_parseHTML_no_next_page(){
//     std::regex no_next_reg("<li class=\"disabled\"><a href=\"([^<]+)\"><span aria-label=\"Next\">");
//     auto html_pag= curl_downloadHTML(); //My string in HTML whole page (reasBuffer)
    
//     auto words_begin =
//     std::sregex_iterator(html_pag.begin(), html_pag.end(), no_next_reg);
//     auto words_end = std::sregex_iterator();
    
//     std::cout << "Found NO_next: "
//     << std::distance(words_begin, words_end)
//     << " links:\n";
//     std::string match_str_next;
//     for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
//         std::smatch match = *i;
//         match_str_next = match.str();
//         std::cout << match_str_next << '\n';
//     }
//     return match_str_next;
// }
//https://www.experts-exchange.com/questions/26903182/Using-cURL-to-download-an-entire-webpage-HTML-images-css-js-etc.html


//COMPILE:
// g++ curl_example.cpp -o curl_example -lcurl  
