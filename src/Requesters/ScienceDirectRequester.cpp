#include <regex>

#include "ScienceDirectRequester.h"

using namespace std;

ScienceDirectRequester::ScienceDirectRequester(const string &url, const string &apikey): Requester() {
    this->url = url;
    this->apikey = apikey;
    chunk = NULL;
    chunk = curl_slist_append(chunk, "Accept: application/json");
    string api = "X-ELS-APIKey: " + apikey;
    chunk = curl_slist_append(chunk, api.c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, chunk);
}

string ScienceDirectRequester::make_request(const string &query){
    regex re_frmt(" ");
	string new_query = regex_replace(query, re_frmt, "%20");
	return this->url + "{" + new_query + "}&count=10&sort=relevancy";
}

std::vector<ArticleInfo> ScienceDirectRequester::parse_response() {
    Json::Reader reader;
    Json::Value root;

    bool parsingSuccessful = reader.parse(buffer, root);
    vector<ArticleInfo> articles;

    if (!parsingSuccessful) {
        string what = "ScienceDirect: Parser error: " + reader.getFormattedErrorMessages();
        cout << what << endl;
		return articles;
    }

    Json::Value entry = root["search-results"]["entry"];

    for (unsigned int i = 0; i < entry.size(); i++) {
        string title = entry[i]["dc:title"].asString();

        Json::Value authorsList = entry[i]["authors"]["author"];
        vector<string> authors = {};

        if (authorsList.size() > 0) {
            for (unsigned int j = 0; j < authorsList.size(); j++) {
                string surname = authorsList[j].get("surname", "").asString();
                string given_name = authorsList[j].get("given-name", "").asString();
                authors.push_back(given_name + " " + surname);
            }
        } else {
                string surname = authorsList.get("surname", "").asString();
                string given_name = authorsList.get("given-name", "").asString();
                authors.push_back(given_name + " " + surname);
        }


        string venue = entry[i].get("prism:publicationName", "").asString();
        string volume = entry[i].get("prism:volume", "").asString();
        string number = entry[i].get("prism:issueIdentifier", "").asString();
        string start_page = entry[i].get("prism:startingPage","").asString();
        string end_page = entry[i].get("prism:endingPage","").asString();
        string pages = start_page +"-" + end_page;

        string year = entry[i]["prism:coverDate"][0].get("$", "").asString();
        year = year.substr(0,4);
        string type = entry[i].get("prism:aggregationType","").asString();
        string art_url = entry[i].get("prism:url","").asString();

        articles.push_back(ArticleInfo(title, authors, venue, volume, 
                    number, pages, year, type, art_url));
    }

    return articles;
}


