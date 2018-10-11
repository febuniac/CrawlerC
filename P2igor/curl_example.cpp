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



string curl_downloadHTML(){
 CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.submarino.com.br/categoria/bebes/brinquedos-para-bebe");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    write_file(readBuffer);
    std::cout << readBuffer << std::endl;
  }
  return readBuffer;
}

void regex_parseHTML_prods(){
    std::regex linksprod_reg("<a class=\"card-product-url\" href=\"([^\"]+)\"");
    auto html_pag= curl_downloadHTML(); //My string in HTML whole page (reasBuffer)

    auto words_begin = 
        std::sregex_iterator(html_pag.begin(), html_pag.end(), linksprod_reg);
    auto words_end = std::sregex_iterator();
    
    std::cout << "Found " 
          << std::distance(words_begin, words_end) 
          << " links:\n";
 
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;                                                 
        std::string match_str = match.str();
        lista_links_produtos.push_back(match_str);
        std::cout << match_str << '\n';
    }   
}

void regex_parseHTML_next_page(){
    std::regex linkspages_reg("<li class=\"\"><a href=\"([^<]+)\"><span aria-label=\"Next\">");
    auto html_pag= curl_downloadHTML(); //My string in HTML whole page (reasBuffer)

    auto words_begin = 
        std::sregex_iterator(html_pag.begin(), html_pag.end(), linkspages_reg);
    auto words_end = std::sregex_iterator();
    
    std::cout << "Found " 
          << std::distance(words_begin, words_end) 
          << " links:\n";
 
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;                                                 
        std::string match_str = match.str();
        lista_links_paginas.push_back(match_str);
        std::cout << match_str << '\n';
    }   
}
void regex_parseHTML_next_page_loop(){
  //LOOP QUE VAI NA PAGINA, BAIXA TODOS OS PRODUTOS 
  //ACHA O NEXT CLICA
  //VAI NA NOVA PAGINA E BAIXA TODOS PRODUTOS
  // FAZ ISSO ATÉ O NEXT NÃO DAR MAIS MATCH (DISABLED==ACABOU TODAS AS NEXT PAGES)
}

int main(void)
{
 curl_downloadHTML();
 regex_parseHTML_prods();
 regex_parseHTML_next_page();
  return 0;
}




//https://www.experts-exchange.com/questions/26903182/Using-cURL-to-download-an-entire-webpage-HTML-images-css-js-etc.html
