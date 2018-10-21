#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
using namespace std;
#include <stdio.h>
#include <regex>
#include <vector>
#include <iterator>
#include <thread>
#include <chrono>
#include <list>



// Convert curl output into a string
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
} 

// writing a file with program info (HTML page)
void write_file(string info, string filename){
    ofstream myfile;
    myfile.open (filename);
    myfile << info;
    myfile.close();
}

//receives as input an URL and makes the download of the HTMl code of the page
string curl_downloadHTML(std::string url){
    CURL *curl; 
    CURLcode res;
    std::string readBuffer;
    std::string header_string;
    curl = curl_easy_init();

    if(curl) {
        
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);
        
        res = curl_easy_perform(curl);
        
        curl_easy_cleanup(curl);
     
        write_file(readBuffer,"mycurlfile_parallel.txt");
    }
    
    return readBuffer;
}

std::list< string > download_products_links_LOOP(std::string url){
    std::string vazio ="";
    std::list< string > list_link_products;
    while(url != vazio){
        std::string html_page = curl_downloadHTML(url);

    //download_prods_links_________________________________________________________________________
        std::regex linksprod_reg("<a class=\"card-product-url\" href=\"([^\"]+)\"");
        auto words_begin = std::sregex_iterator(html_page.begin(), html_page.end(), linksprod_reg);
        auto words_end = std::sregex_iterator();
        std::string link_com_site_antes_p = "";   
        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;  
            std::string match_str_prod = match[1].str();
            link_com_site_antes_p = "https://www.submarino.com.br" + match_str_prod;
            list_link_products.push_front(link_com_site_antes_p);
            std::cout<<list_link_products.front()<<'\n'<<'\n';
        }
    //_____________________________________________________________________________________________
    
    //download_next_page_________________________________________________________________________  
        std::list< string > list_link_nexts;
        CURL *curl; 
        std::regex linkspages_reg("<li class=\"\"><a href=\"([^<]+)\"><span aria-label=\"Next\">");
        auto words_begin2 = std::sregex_iterator(html_page.begin(), html_page.end(), linkspages_reg);
        auto words_end2 = std::sregex_iterator();
        std::string match_str_next;
        std::string link_com_site_antes_n = "";   
        for (std::sregex_iterator i = words_begin2; i != words_end2; ++i) {
            std::smatch match = *i;  
            std::string match_str_next = match[1].str();
            link_com_site_antes_n = "https://www.submarino.com.br" + match_str_next;
            std::regex amp("amp;");
            link_com_site_antes_n = std::regex_replace(link_com_site_antes_n, amp, "");
            list_link_nexts.push_front(link_com_site_antes_n);
        }
        url = link_com_site_antes_n;
        
    //_____________________________________________________________________________________________
    }
    return list_link_products;
}

// while (True){

// }

// ler e ecrever aquire antes e release depois

// pegar o link e dar release

//quando a thread 1 acabar break (bool compartilhada na main comeca na mais como false )
//Quando T1 acabar flag ==True

std::list< string >  download_HTMLpages_products_LOOP(std::string url){
    std::list< string > list_HTML_products;
    std::list< string > list_link_products = download_products_links_LOOP(url);
    
    //  for (int i = 0; i <= list_link_products.size(); ++i){
     while (true){
        std::string link_baixado= list_link_products.front();
        std::string html_page_prod = curl_downloadHTML(link_baixado);
        list_HTML_products.push_front(html_page_prod);
     }
    return list_HTML_products;
}

std::string smatch_regex(std::string link_produto,std::regex reg){
    smatch match; 
    if (regex_search(link_produto, match, reg) == true) { 
        // cout << "INFOS : " << match[1].str() << endl; 
    }   
    return match[1].str();
}

void get_infos_productHTML_LOOP(std::string url){
    std::list< string > list_HTML_products = download_HTMLpages_products_LOOP(url);
    // for (int i = 0; i <= list_HTML_products.size(); ++i){
        while (true){
            //GET PRODUCT INFO
            std::string HTMLprod = download_HTMLpages_products_LOOP(url).front(); 
            std::regex nome_prod_reg ("<h1 class=\"product-name\">([^<]+)</h1>");
            std::regex descricao_prod_reg ("<div><noframes>((.|\n)+)</noframes><iframe");
            std::regex foto_prod_reg ("<img class=\"swiper-slide-img\" alt=\"(.+)\" src=\"([^\"]+)\"");
            std::regex preco_a_vista_prod_reg ("<p class=\"sales-price\">([^<]+)</p>");
            std::regex preco_parcelado_prod_reg ("<p class=\"payment-option payment-option-rate\">([^<]+)</p>");
            std::regex categoria_prod_reg ("<span class=\"TextUI-iw976r-5 grSSAT TextUI-sc-1hrwx40-0 jIxNod\">([^<]+)</span>");

            auto nome =smatch_regex(HTMLprod,nome_prod_reg);
            auto descricao =smatch_regex(HTMLprod,descricao_prod_reg);
            auto foto =smatch_regex(HTMLprod,foto_prod_reg);
            auto p_vista =smatch_regex(HTMLprod,preco_a_vista_prod_reg);
            auto p_parcelado =smatch_regex(HTMLprod,preco_parcelado_prod_reg);
            auto categoria =smatch_regex(HTMLprod,categoria_prod_reg);
            std::string saida = 
            "  {\n"
            "    \"nome\" : \"" + nome +"\",\n"
            "    \"descricao\" : \"" + descricao +"\",\n"
            "    \"foto\" : \"" + foto +"\",\n"
            "    \"preco\" : \"" + p_vista +"\",\n"
            "    \"preco_parcelado\" : \"" + p_parcelado +"\",\n"
            "    \"categoria\" : \"" + categoria +"\",\n"
            // "    \"url\" : \"" + url +"\",\n"
            "  },\n";  
            cout<< saida;  
        }


    // }
     
}


int main(void)
{
    std:: string url = "https://www.submarino.com.br/busca/carrinho-de-bebe-cosco?conteudo=carrinho%20de%20bebe%20cosco&filtro=%5B%7B%22id%22%3A%22wit%22%2C%22value%22%3A%22Cesta%22%2C%22fixed%22%3Afalse%7D%5D&ordenacao=relevance&origem=nanook&suggestion=true";
    download_products_links_LOOP(url);
    download_HTMLpages_products_LOOP(url);
    get_infos_productHTML_LOOP(url);
    //std::string html_page = curl_downloadHTML(url);
    // regex_parseHTML_next_page_loop("https://www.submarino.com.br/busca/controle-remoto-fisher-price?pfm_carac=controle%20remoto%20fisher%20price&pfm_index=8&pfm_page=search&pfm_type=spectreSuggestions");
    // regex_download_prod_page_loop();
    //Criando Threads
    // std::thread Thread1(download_products_links_LOOP,std::ref(url),std::ref(url));//, std::ref(url),std::ref(),std::ref()
    // std::thread Thread2(download_HTMLpages_products_LOOP);
    // std::thread Thread3(get_infos_productHTML_LOOP);
    return 0;
}




























    //https://www.submarino.com.br/categoria/bebes/brinquedos-para-bebe
    //https://www.submarino.com.br/busca/controle-remoto-fisher-price?pfm_carac=controle%20remoto%20fisher%20price&pfm_index=8&pfm_page=search&pfm_type=spectreSuggestions
    // std::string html_page = curl_downloadHTML("https://www.submarino.com.br/categoria/bebes/brinquedos-para-bebe");
    //https://www.submarino.com.br/busca/fogao-brastemp-clean?conteudo=fogao%20brastemp%20clean&filtro=%5B%7B%22id%22%3A%22wit%22%2C%22value%22%3A%22Capa%22%2C%22fixed%22%3Afalse%7D%2C%7B%22id%22%3A%22wit%22%2C%22value%22%3A%22Câmara%22%2C%22fixed%22%3Afalse%7D%5D&ordenacao=relevance&origem=nanook&suggestion=true
    // regex_parseHTML_prods(html_page);
    // std::string url = regex_parseHTML_next_page(html_page);
    // std::cout<<"c"<<url<<"\n";

    // html_page = curl_downloadHTML(url);
    // std::cout<< "s"<<html_page<<"\n";
    // regex_parseHTML_prods(html_page);
    // //std::cout<<url<<"\n";
    // url = regex_parseHTML_next_page(html_page);
    // html_page = curl_downloadHTML(url);
    // regex_parseHTML_prods(html_page);
    // //std::cout<<url<<"\n";
    // url = regex_parseHTML_next_page(html_page);

    //regex_parseHTML_next_page("https://www.submarino.com.br/categoria/bebes/brinquedos-para-bebe");
    // for (int i = 0; i <= lista_links_paginas.size(); ++i){
    //     std::cout <<"Página Next:"<<i<< lista_links_paginas[i] << '\n';
    //     while(lista_links_paginas[i] != ""){
    //         std::string html_page = curl_downloadHTML("https://www.submarino.com.br/categoria/bebes/brinquedos-para-bebe");
    //         regex_parseHTML_prods(html_page);
    //         std::string url = regex_parseHTML_next_page(html_page);
    //     }
    // }

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
// <h1 class="product-name">Cubo De Atividades  Fisher-Price Laranja</h1>
// <span class="TextUI-iw976r-5 grSSAT TextUI-sc-1hrwx40-0 jIxNod">Outros Brinquedos</span>

//COMPILE:
// g++ curl_example.cpp -o curl_example -lcurl  
