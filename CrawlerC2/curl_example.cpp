#include <iostream>
#include <string>
#include <curl/curl.h>
#include <fstream>
using namespace std;
#include <stdio.h>
#include <regex>
#include <vector>
#include <iterator>
#include <chrono>
// #include <json.hpp>
// // for convenience
// using json = nlohmann::json;



std::vector< string > lista_links_produtos;
std::vector< string >lista_links_paginas;

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
     
        write_file(readBuffer,"mycurlfile.txt");
       
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
        link_com_site_antes_p = "https://www.submarino.com.br" + match_str_prod;
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
        link_com_site_antes_n = "https://www.submarino.com.br" + match_str_next;
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
// std::vector<string> 
void regex_parseHTML_next_page_loop(std::string url){
    // std::vector< string > next_link = lista_links_paginas;
    // std::string no_next_link = regex_parseHTML_no_next_page();
    std::string vazio ="";
   
    // for (int i = 0; i <= lista_links_paginas.size(); i++){
    //     std::cout <<"i:"<<i<< '\n';
    //     std::cout <<"Página Next:"<<i<< lista_links_paginas[i] << '\n';
    while(url != vazio){
        std::string html_page = curl_downloadHTML(url);
        regex_parseHTML_prods(html_page);
        url = regex_parseHTML_next_page(html_page);

    }
    // }
    // return lista_links_paginas;

    //LOOP QUE VAI NA PAGINA, BAIXA TODOS OS PRODUTOS
    //ACHA O NEXT CLICA
    //VAI NA NOVA PAGINA E BAIXA TODOS PRODUTOS
    // FAZ ISSO ATÉ O NEXT NÃO DAR MAIS MATCH (DISABLED==ACABOU TODAS AS NEXT PAGES)
}

std::string smatch_regex(std::string link_produto,std::regex reg){
    smatch match; 
    if (regex_search(link_produto, match, reg) == true) { 
        cout << "Whole match : " << match[1].str() << endl; 
    }
    return match[1].str();
}

void get_prod_info(std::string link_produto ){
    /*nome do produto, descrição do produto, url da foto do produto, 
    preço à vista, preço parcelado, categoria do produto, url da página de exibição*/
    std::regex nome_prod_reg ("<h1 class=\"product-name\">([^<]+)</h1>");
    std::regex descricao_prod_reg ("<div><noframes>((.|\n)+)</noframes><iframe");
    std::regex foto_prod_reg ("<img class=\"swiper-slide-img\" alt=\"(.+)\" src=\"([^\"]+)\"");
    std::regex preco_a_vista_prod_reg ("<p class=\"sales-price\">([^<]+)</p>");
    std::regex preco_parcelado_prod_reg ("<p class=\"payment-option payment-option-rate\">([^<]+)</p>");
    std::regex categoria_prod_reg ("<span class=\"TextUI-iw976r-5 grSSAT TextUI-sc-1hrwx40-0 jIxNod\">([^<]+)</span>");
    // std::regex url_pag_prod_reg ("");


    auto nome =smatch_regex(link_produto,nome_prod_reg);
    auto descricao =smatch_regex(link_produto,descricao_prod_reg);
    auto foto =smatch_regex(link_produto,foto_prod_reg);
    auto p_vista =smatch_regex(link_produto,preco_a_vista_prod_reg);
    auto p_parcelado =smatch_regex(link_produto,preco_parcelado_prod_reg);
    auto categoria =smatch_regex(link_produto,categoria_prod_reg);

        //ofstream arquivo;
        ofstream arquivo("infos_prod.txt", ios::app);
        // arquivo.open ("infos_prod.txt");
        arquivo<< "  {\n"
            "    \"nome\" : \"" + nome +"\",\n"
            "    \"descricao\" : \"" + descricao +"\",\n"
            "    \"foto\" : \"" + foto +"\",\n"
            "    \"preco\" : \"" + p_vista +"\",\n"
            "    \"preco_parcelado\" : \"" + p_parcelado +"\",\n"
            "    \"categoria\" : \"" + categoria +"\",\n"
            // "    \"url\" : \"" + url +"\",\n"
            "  },\n";
        
        // arquivo << "{";

        // arquivo << "    nome:";
        // arquivo << nome;
        // arquivo << "\n";
        
        // arquivo << "    descrição:";
        // arquivo << descricao;
        // arquivo << "\n";

        // arquivo << "    foto:";
        // arquivo <<foto;
        // arquivo << "\n";

        // arquivo << "    preço a vista:";
        // arquivo <<p_vista;
        // arquivo << "\n";

        // arquivo << "    preço parcelado:";
        // arquivo << p_parcelado;
        // arquivo << "\n";

        // arquivo << "    categoria:";
        // arquivo<< categoria;
        // arquivo << "\n";
        
        // arquivo << "}";
        
        arquivo.close();  
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
    // write_file(nome,"infos.txt");
    // write_file(descricao,"infos.txt");
    // write_file(foto,"infos.txt");
    // write_file(p_vista,"infos.txt");
    // write_file(p_parcelado,"infos.txt");
    // write_file(categoria,"infos.txt");

// std::regex_token_iterator<std::string::iterator> rend; 
// std::regex_token_iterator<std::string::iterator> nome ( link_produto.begin(), link_produto.end(), nome_prod_reg,1);
// while (nome!=rend) 
// {
//     std::cout <<"NOME:"<< " [" << *nome++ << "]"<<'\n';
//     //  write_file(*nome++,"infos.txt");
    
// }


// std::regex_token_iterator<std::string::iterator> descricao( link_produto.begin(), link_produto.end(), descricao_prod_reg,1 );
// while (descricao!=rend)
// {
//      std::cout <<"DESCRIÇÃO:"<< " [" << *descricao++ << "]"<<'\n';
//     //   write_file(*descricao++,"infos.txt");
// }

// std::regex_token_iterator<std::string::iterator> foto ( link_produto.begin(), link_produto.end(), foto_prod_reg,0 );
// while (foto!=rend) 
// {
//     std::cout <<"FOTO:"<< " [" << *foto++ << "]"<<'\n';
//     //  write_file(*foto++,"infos.txt");
// }

// std::regex_token_iterator<std::string::iterator> vista ( link_produto.begin(), link_produto.end(), preco_a_vista_prod_reg,1 );
// while (vista!=rend) 
// {
//     std::cout <<"PREÇO À VISTA:"<< " [" << *vista++ << "]"<<'\n';
//     //  write_file(*vista++,"infos.txt");
// }

// std::regex_token_iterator<std::string::iterator> parcelado ( link_produto.begin(), link_produto.end(), preco_parcelado_prod_reg,1 );
// while (parcelado!=rend) 
// {
//     std::cout <<"PREÇO PARCELADO:"<< " [" << *parcelado++ << "]"<<'\n';
//     //  write_file(*parcelado++,"infos.txt");
// }

// std::regex_token_iterator<std::string::iterator> categoria ( link_produto.begin(), link_produto.end(), categoria_prod_reg,1 );
// while (categoria!=rend) 
// {
//     std::cout <<"CATEGORIA:"<< " [" << *categoria++ <<"]"<<'\n';
//     //  write_file(*categoria++,"infos.txt");
}

//https://stackoverflow.com/questions/21667295/how-to-match-multiple-results-using-stdregex


void regex_download_prod_page_loop(){
     for (int i = 0; i <= lista_links_produtos.size(); ++i){
        std::string link_baixado= lista_links_produtos[i];
        //std::cout<<"link produto:"<< link_baixado<< "\n";
        std::string html_page_prod = curl_downloadHTML(link_baixado);
        // std::cout<< "PAGE_____________________:"<<html_page_prod<< "\n";

        // regex_parseHTML_prods(html_page_prod);
        get_prod_info(html_page_prod);
        //entra em cada produto
        //pega as infos com regex
        // joga para um arq json
    }
}

int main(void)
{
    regex_parseHTML_next_page_loop("https://www.submarino.com.br/busca/controle-remoto-fisher-price?pfm_carac=controle%20remoto%20fisher%20price&pfm_index=8&pfm_page=search&pfm_type=spectreSuggestions");
    regex_download_prod_page_loop();

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
