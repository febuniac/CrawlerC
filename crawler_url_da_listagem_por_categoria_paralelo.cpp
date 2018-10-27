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
#include "Semaphore.cpp"



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

void download_products_links_LOOP(std::string &url,Semaphore& mutex_access_List_link_products,Semaphore& pc_available_List_link_products, bool &download_products_linksDone,std::list< string > &list_link_products){
    std::string vazio ="";
    // std::list< string > list_link_products;
    while(url != vazio){
        std::string html_page = curl_downloadHTML(url);
        //download_prods_links_________________________________________________________________________
            std::regex linksprod_reg("<a class=\"card-product-url\" href=\"([^\"]+)\"");
            auto words_begin = std::sregex_iterator(html_page.begin(), html_page.end(), linksprod_reg);
            auto words_end = std::sregex_iterator();
            std::string link_com_site_antes_p = "";  
            mutex_access_List_link_products.acquire();
            std::cout<< "mutex lista produto -1"<<'\n'; 
            for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
                std::smatch match = *i;  
                std::string match_str_prod = match[1].str();
                link_com_site_antes_p = "https://www.submarino.com.br" + match_str_prod;
                list_link_products.push_back(link_com_site_antes_p);
                pc_available_List_link_products.release();
                std::cout<< "UP na lista de produtos"<<'\n';     
                    
                std::cout<<list_link_products.front()<<'\n'<<'\n';
            }
            mutex_access_List_link_products.release();
            std::cout<< "mutex lista produto +1"<<'\n';
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
                list_link_nexts.push_back(link_com_site_antes_n);
            }
            url = link_com_site_antes_n;
     }
    std::cout<< "Cheguei no fim da T1.1"<<'\n'; 
    download_products_linksDone = true;
}


void download_HTMLpages_products_LOOP(std::string &url,Semaphore& mutex_access_List_HTML_products,Semaphore& pc_available_List_HTML_products, Semaphore&  pc_available_List_link_products,Semaphore& mutex_access_List_link_products,bool &download_products_linksDone, bool &download_HTMLpages_productsDone,std::list< string > &list_link_products,std::list< string > &list_HTML_products){
    // std::list< string > list_HTML_products;  
    // std::list< string > list_link_products = download_products_links_LOOP(url, mutex_access_List_link_products ,pc_available_List_link_products,download_products_linksDone); 
    
    std::cout<< "Cheguei no while true Thread2"<<'\n';   
     
     while (true){
        if(download_products_linksDone && list_link_products.empty())
        {
                std::cout<< "FIM THREAD";
                break;
        }
        else{
        pc_available_List_link_products.acquire();
        std::cout<< "DOWN na lista de Produtos"<<'\n';
         mutex_access_List_link_products.acquire();
         std::cout<< "mutex lista de Produtos -1"<<'\n';
         
        std::string link_baixado= list_link_products.front();
        list_link_products.pop_front();
        mutex_access_List_link_products.release();
        std::cout<< "mutex lista de Produtos +1"<<'\n';

        std::string html_page_prod = curl_downloadHTML(link_baixado);
         mutex_access_List_HTML_products.acquire();
        std::cout<< "mutex lista de HTMNL -1"<<'\n';
        list_HTML_products.push_back(html_page_prod);
        pc_available_List_HTML_products.release();
        std::cout<< "UP na lista de HTMLs"<<'\n';
        mutex_access_List_HTML_products.release();
        std::cout<< "mutex lista de HTMNL +1"<<'\n';
     }
     }
    download_HTMLpages_productsDone = true;

}
     

std::string smatch_regex(std::string link_produto,std::regex reg){
    smatch match; 
    if (regex_search(link_produto, match, reg) == true) { 
        // cout << "INFOS : " << match[1].str() << endl; 
    }   
    return match[1].str();
}

void get_infos_productHTML_LOOP(std::string &url,Semaphore& mutex_access_list_Jsons,Semaphore& pc_available_List_HTML_products,Semaphore& mutex_access_List_link_products,Semaphore& pc_available_List_link_products,Semaphore& mutex_access_List_HTML_products,bool &download_products_linksDone, bool &download_HTMLpages_productsDone,std::list< string > &list_link_products,std::list< string > &list_HTML_products){
    // std::list< string > list_link_products = download_products_links_LOOP(url,mutex_access_List_link_products ,pc_available_List_link_products,download_products_linksDone);
    // std::list< string > list_HTML_products = download_HTMLpages_products_LOOP(url,mutex_access_List_HTML_products, pc_available_List_HTML_products,  pc_available_List_link_products, mutex_access_List_link_products,download_products_linksDone,download_HTMLpages_productsDone);
    std::list< string > list_Jsons;
        while (true){
            //GET PRODUCT INFO
            if(download_HTMLpages_productsDone && list_HTML_products.empty())
            {
                    std::cout<< "FIM THREAD";
                    break;
            }
            else{
            pc_available_List_HTML_products.acquire();
            std::cout<< "DOWN na lista de HTMLs"<<'\n';
            mutex_access_List_HTML_products.acquire();
            std::cout<< "mutex lista de HTMNL -1"<<'\n';
            std::string HTMLprod = list_HTML_products.front();
            list_HTML_products.pop_front();
            mutex_access_List_HTML_products.release();
            std::cout<< "mutex lista de HTMNL +1"<<'\n'; 
            std::regex nome_prod_reg ("<h1 class=\"product-name\">([^<]+)</h1>");
            std::regex descricao_prod_reg ("<div><noframes>((.|\n)+)</noframes><iframe");
            std::regex foto_prod_reg ("<img class=\"swiper-slide-img\" alt=\"(.+)\" src=\"([^\"]+)\"");
            std::regex preco_a_vista_prod_reg ("<p class=\"sales-price\">([^<]+)</p>");
            std::regex preco_parcelado_prod_reg ("<p class=\"payment-option payment-option-rate\">([^<]+)</p>");
            std::regex categoria_prod_reg ("<span class=\"TextUI-iw976r-5 grSSAT TextUI-sc-1hrwx40-0 jIxNod\">([^<]+)</span>");
            
            // std::string url =list_link_products.front();
            // list_link_products.pop_front();
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
             "    \"url\" : \"" + url +"\",\n"
            "  },\n";
            mutex_access_list_Jsons.acquire();
            std::cout<< "mutex Json -1"<<'\n';
           list_Jsons.push_back(saida);
           mutex_access_list_Jsons.release();
            std::cout<< "mutex Json +1"<<'\n';
            cout<< saida; 
            //  mutex_access_list_Jsons.acquire();
            // std::cout<< "mutex Json -1"<<'\n';
            // cout<< list_Jsons.front();  
            // list_Jsons.pop_front();
            // mutex_access_list_Jsons.release();
            // std::cout<< "mutex Json +1"<<'\n';
        }
        }


    // }
     
}


int main(void)
{

    std::chrono::  high_resolution_clock::time_point t1, t2, t3,t4,t5,t6,t7,t8;
    std::chrono:: duration<double> tempoProduto;
    std::chrono:: duration<double> tempoTotalOcioso;
    std::chrono:: duration<double> tempoTotalCrawler;
    double tempoProduto1;
    double tempoTotalCrawler1;
    double tempoTotalOcioso1;
    
    std::list< string > list_link_products;
    std::list< string > list_HTML_products;  
    bool download_products_linksDone = false;
    bool download_HTMLpages_productsDone = false;
    Semaphore mutex_access_List_link_products(1);
    Semaphore pc_available_List_link_products(0);
    Semaphore mutex_access_List_HTML_products(1);
    Semaphore pc_available_List_HTML_products(0);
    Semaphore mutex_access_list_Jsons(1);
    std:: string url = "https://www.submarino.com.br/busca/jogo-de-panelas-aluminio?conteudo=jogo%20de%20panelas%20aluminio&filtro=%5B%7B%22id%22:%22wit%22,%22value%22:%22Panela%22,%22fixed%22:false%7D%5D&ordenacao=relevance&origem=nanook&suggestion=true";
    // download_products_links_LOOP(url,mutex_access_List_link_products,pc_available_List_link_products,download_products_linksDone);
    // download_HTMLpages_products_LOOP(url,mutex_access_List_HTML_products,pc_available_List_HTML_products, pc_available_List_link_products,mutex_access_List_link_products,download_products_linksDone,download_HTMLpages_productsDone);
    // get_infos_productHTML_LOOP(url,mutex_access_list_Jsons, pc_available_List_HTML_products, mutex_access_List_link_products, pc_available_List_link_products,mutex_access_List_HTML_products,download_products_linksDone,download_HTMLpages_productsDone);
    
    //Criando Threads

    std::thread Thread1(download_products_links_LOOP,std::ref(url),std::ref(mutex_access_List_link_products),std::ref(pc_available_List_link_products),std::ref(download_products_linksDone),std::ref(list_link_products));//, std::ref(url),std::ref(),std::ref()
    std::thread Thread2(download_HTMLpages_products_LOOP, std::ref(url),std::ref(mutex_access_List_HTML_products),std::ref(pc_available_List_HTML_products),std::ref(pc_available_List_link_products),std::ref(mutex_access_List_link_products),std::ref(download_products_linksDone),std::ref(download_HTMLpages_productsDone),std::ref(list_link_products),std::ref(list_HTML_products));
    std::thread Thread22(download_HTMLpages_products_LOOP, std::ref(url),std::ref(mutex_access_List_HTML_products),std::ref(pc_available_List_HTML_products),std::ref(pc_available_List_link_products),std::ref(mutex_access_List_link_products),std::ref(download_products_linksDone),std::ref(download_HTMLpages_productsDone),std::ref(list_link_products),std::ref(list_HTML_products));
    std::thread Thread222(download_HTMLpages_products_LOOP, std::ref(url),std::ref(mutex_access_List_HTML_products),std::ref(pc_available_List_HTML_products),std::ref(pc_available_List_link_products),std::ref(mutex_access_List_link_products),std::ref(download_products_linksDone),std::ref(download_HTMLpages_productsDone),std::ref(list_link_products),std::ref(list_HTML_products));
    std::thread Thread3(get_infos_productHTML_LOOP,std::ref(url),std::ref(mutex_access_list_Jsons),std::ref(pc_available_List_HTML_products),std::ref(mutex_access_List_link_products),std::ref(pc_available_List_link_products),std::ref(mutex_access_List_HTML_products),std::ref(download_products_linksDone),std::ref(download_HTMLpages_productsDone),std::ref(list_link_products),std::ref(list_HTML_products));
    std::thread Thread33(get_infos_productHTML_LOOP,std::ref(url),std::ref(mutex_access_list_Jsons),std::ref(pc_available_List_HTML_products),std::ref(mutex_access_List_link_products),std::ref(pc_available_List_link_products),std::ref(mutex_access_List_HTML_products),std::ref(download_products_linksDone),std::ref(download_HTMLpages_productsDone),std::ref(list_link_products),std::ref(list_HTML_products));
    std::thread Thread333(get_infos_productHTML_LOOP,std::ref(url),std::ref(mutex_access_list_Jsons),std::ref(pc_available_List_HTML_products),std::ref(mutex_access_List_link_products),std::ref(pc_available_List_link_products),std::ref(mutex_access_List_HTML_products),std::ref(download_products_linksDone),std::ref(download_HTMLpages_productsDone),std::ref(list_link_products),std::ref(list_HTML_products));

   t1 = std::chrono::high_resolution_clock::now();
    Thread1.join();
    Thread2.join();
    Thread22.join();
    //Thread222.join();
    Thread3.join();
    Thread33.join();
    //Thread333.join();
    t2= std::chrono::high_resolution_clock::now();

 
    tempoTotalCrawler = std::chrono::duration_cast<std::chrono::duration<double> >(t2 - t1);
    tempoTotalCrawler1 = tempoTotalCrawler.count();
    cout << "Tempo total de Execucão do Crawler: " << tempoTotalCrawler1 << '\n';
//   Threads e condicao de parada
    return 0;
}





//+ thread ocupa mais memoria (thread 3 dá mais impacto)






















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
