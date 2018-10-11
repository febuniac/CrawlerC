#include <stdio.h>
#include <curl/curl.h>
#include <regex>
#include <string>
#include <fstream>
#include <vector>
#include <iostream>

std::vector< int > lista_links_produtos;
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}  

void curl_downloadHTML(){
  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://www.amazon.com/home-garden-kitchen-furniture-bedding/b/ref=sd_allcat_home_storefront?ie=UTF8&node=1055398");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    std::cout << readBuffer << std::endl;
}
// void regex_parseHTML_prods(){
//     std::regex linksprod_reg("<a class=\"a-link-normal s-access-detail-page  s-color-twister-title-link a-text-normal\" title=\"([^\"]+)\" href=\"([^ \"]+)\">");
//     if(regex_match(curl_downloadHTML().fp,linksprod_reg)){
//         lista_links_produtos.push_back(linksprod_reg);


// }
int main(void)
{
curl_downloadHTML();
    return 0;
}
