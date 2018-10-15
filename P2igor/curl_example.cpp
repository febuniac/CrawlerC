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

// Convert curl output into a string
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// writing a file with program info (HTML page)
void write_file(string info){
    ofstream myfile;
    myfile.open ("mycurlfile.txt");
    myfile << info;
    myfile.close();
}

//receives as input an URL and makes the download of the HTMl code of the page
string curl_downloadHTML(std::string url){
    CURL *curl; //new curl handle to restart the connection
    CURLcode res;
    std::string readBuffer;
    std::string header_string;
    curl = curl_easy_init();
    if(curl) {//continue if curl can be initialized
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
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

//Parse tyhe HTML page looking for products and append the products link into a list of product links
void regex_parseHTML_prods(std::string html_pag){
    std::regex linksprod_reg("<a class=\"card-product-url\" href=\"([^\"]+)\"");
    //auto html_pag= curl_downloadHTML(url); //My string in HTML whole page (reasBuffer)
    
    auto words_begin =
    std::sregex_iterator(html_pag.begin(), html_pag.end(), linksprod_reg);
    auto words_end = std::sregex_iterator();
    
    std::cout << "Found PRODUCTS:"
    << std::distance(words_begin, words_end)
    << " links:\n";
     std::string link_com_site_antes_p = "";   
    
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;  
        std::string match_str_prod = match[1].str();
        link_com_site_antes_p = "www.submarino.com.br" + match_str_prod;
        lista_links_produtos.push_back(link_com_site_antes_p);
        //std::cout << match_str_prod << '\n';
    }
    for (int i = 0; i < lista_links_produtos.size(); ++i){
            std::cout <<"Link Produto:"<< i << lista_links_produtos[i] << '\n';
    }
}

//Parse tyhe HTML page looking for the next button page and append the next page link  into a list of next page  links and return the next page of the actual page
std::string regex_parseHTML_next_page(std::string html_pag){
    CURL *curl; 
    std::regex linkspages_reg("<li class=\"\"><a href=\"([^<]+)\"><span aria-label=\"Next\">");
   // auto html_pag= curl_downloadHTML(url); //My string in HTML whole page (reasBuffer)
    auto words_begin =
    std::sregex_iterator(html_pag.begin(), html_pag.end(), linkspages_reg);
    auto words_end = std::sregex_iterator();
    
    std::cout << "Found NEXT: "
    << std::distance(words_begin, words_end)
    << " links:\n";
    std::string match_str_next;
    std::string link_com_site_antes_n = "";   
    for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
        std::smatch match = *i;  
        std::string match_str_next = match[1].str();
        link_com_site_antes_n = "www.submarino.com.br" + match_str_next;
        std::regex amp("amp;");
        link_com_site_antes_n = std::regex_replace(link_com_site_antes_n, amp, "");
        lista_links_paginas.push_back(link_com_site_antes_n);
        //curl_downloadHTML(lista_links_paginas[i]);
        //std::cout << match_str_next << '\n';
    }
        for (int i = 0; i < lista_links_paginas.size(); ++i){
                std::cout <<"Página Next:"<<i<< lista_links_paginas[i] << '\n';
        }
    return link_com_site_antes_n;//return link da pagina de next
}

//Does a whole cycle of wdownloading products and going to the next page
std::vector<string> regex_parseHTML_next_page_loop(std::string url){
    // std::vector< string > next_link = lista_links_paginas;
    // std::string no_next_link = regex_parseHTML_no_next_page();
    std::string vazio ="";
    for (int i = 0; i <=lista_links_paginas.size(); ++i){
        std::cout <<"Página Next:"<<i<< lista_links_paginas[i] << '\n';
        while(lista_links_paginas[i] != vazio){
            std::string html_page = curl_downloadHTML(url);
            regex_parseHTML_prods(html_page);
            url = regex_parseHTML_next_page(html_page);

        }
    }
    return lista_links_paginas;
    //LOOP QUE VAI NA PAGINA, BAIXA TODOS OS PRODUTOS
    //ACHA O NEXT CLICA
    //VAI NA NOVA PAGINA E BAIXA TODOS PRODUTOS
    // FAZ ISSO ATÉ O NEXT NÃO DAR MAIS MATCH (DISABLED==ACABOU TODAS AS NEXT PAGES)
}

void regex_download_prod_page_loop(){
     for (int i = 0; i < lista_links_produtos.size(); ++i){
        std::string link_baixado= lista_links_produtos[i];
        std::cout<< link_baixado<< "\n";
        curl_downloadHTML(link_baixado);
        regex_parseHTML_prods(link_baixado);
        //entra em cada produto
        //pega as infos com regex
        // joga para um arq json
    }
}

int main(void)
{
    regex_parseHTML_next_page_loop("https://www.submarino.com.br/categoria/bebes/brinquedos-para-bebe");
    // std::string html_page = curl_downloadHTML("https://www.submarino.com.br/categoria/bebes/brinquedos-para-bebe");
    // regex_parseHTML_prods(html_page);
    // std::string url = regex_parseHTML_next_page(html_page);
    // html_page = curl_downloadHTML(url);
    // regex_parseHTML_prods(html_page);
    // std::cout<<url<<"\n";
    // url = regex_parseHTML_next_page(html_page);
    // html_page = curl_downloadHTML(url);
    // std::cout<<"cade"<<url<<"\n";
    //regex_parseHTML_next_page("https://www.submarino.com.br/categoria/bebes/brinquedos-para-bebe");
    // for (int i = 0; i <= lista_links_paginas.size(); ++i){
    //     std::cout <<"Página Next:"<<i<< lista_links_paginas[i] << '\n';
    //     while(lista_links_paginas[i] != ""){
    //         std::string html_page = curl_downloadHTML("https://www.submarino.com.br/categoria/bebes/brinquedos-para-bebe");
    //         regex_parseHTML_prods(html_page);
    //         std::string url = regex_parseHTML_next_page(html_page);
    //     }
    // }
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
