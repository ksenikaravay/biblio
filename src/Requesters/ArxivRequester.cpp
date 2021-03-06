#include <regex>

#include "ArxivRequester.h"

using namespace std;
using namespace tinyxml2;

ArxivRequester::ArxivRequester(const string &url): Requester() {
    this->url = url;
}

string ArxivRequester::make_request(const string &query){
    string new_query = regex_replace(query, regex(" "), "%20");
    return this->url + "\"" + new_query + "\"&start=0&max_results=5";
}

std::vector<ArticleInfo> ArxivRequester::parse_response() {
    vector<ArticleInfo> articles = {};
    XMLDocument doc;
    doc.Parse(buffer);

    XMLElement* root = doc.FirstChildElement("feed");

    for(XMLElement* entry = root->FirstChildElement("entry"); entry != NULL;
                entry = entry->NextSiblingElement("entry"))	{

        string title = entry->FirstChildElement("title")->GetText();

        vector<string> authors = {};
        for(XMLElement* author = entry->FirstChildElement("author"); author != NULL;
                author = author->NextSiblingElement("author"))	{
            string next_author = author->FirstChildElement("name")->GetText();
            authors.push_back(next_author);
        }

        string venue = "arXiv.org";
        string art_url = entry->FirstChildElement("id")->GetText();
        string volume = art_url.substr(21);
        string year = entry->FirstChildElement("published")->GetText();
        year = year.substr(0, 4);

        string number = "";
        string pages = "";
        string type = "";

        articles.push_back(ArticleInfo(title, authors, venue, volume,
                    number, pages, year, type, art_url));
    }
    return articles;
}


