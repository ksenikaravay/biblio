#include <regex>

#include "ScopusRequester.h"

using namespace std;

ScopusRequester::ScopusRequester(const string &url, const string &apikey): Requester() {
    this->url = url;
    this->apikey = apikey;
    chunk = NULL;
    chunk = curl_slist_append(chunk, "Accept: application/json");
    string api = "X-ELS-APIKey: " + apikey;
    chunk = curl_slist_append(chunk, api.c_str());
    curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, chunk);
}

string ScopusRequester::make_request(const string &query){
    regex re_frmt(" ");
    string new_query = regex_replace(query, re_frmt, "%20");
	return this->url + "{" + new_query + "}&sort=relevancy";
}

std::vector<ArticleInfo> ScopusRequester::parse_response() {
    Json::Reader reader;
    Json::Value root;

    bool parsingSuccessful = reader.parse(buffer, root);
    vector<ArticleInfo> articles;

    if (!parsingSuccessful) {
        string what = "Scopus: Parser error: " + reader.getFormattedErrorMessages();
        cout << what << endl;
		return articles;
    }

    Json::Value entry = root["search-results"]["entry"];

    for (unsigned int i = 0; i < entry.size(); i++) {
        string title = entry[i]["dc:title"].asString();

        vector<string> authors = {};
        authors.push_back(entry[i].get("dc:creator", "").asString());


        string venue = entry[i].get("prism:publicationName", "").asString();
        string volume = entry[i].get("prism:volume", "").asString();
        string number = entry[i].get("prism:issueIdentifier", "").asString();
        string pages = entry[i].get("prism:pageRange","").asString();
        string year = entry[i].get("prism:coverDate", "").asString();
        year = year.substr(0,4);
        string type = entry[i].get("prism:aggregationType","").asString();
        string art_url = entry[i].get("prism:url","").asString();

        articles.push_back(ArticleInfo(title, authors, venue, volume, 
                    number, pages, year, type, art_url));
    }

    return articles;
}


