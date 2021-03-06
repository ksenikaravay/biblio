#include <regex>

#include "SpringerRequester.h"

using namespace std;

SpringerRequester::SpringerRequester(const string &url, const string &apikey): Requester() {
    this->url = url;
    this->apikey = apikey;
}

string SpringerRequester::make_request(const string &query){
    regex re_frmt(" ");
    string new_query = regex_replace(query, re_frmt, "%20");
    return this->url + "\"" + new_query + "\"&api_key=" + this->apikey;
}

std::vector<ArticleInfo> SpringerRequester::parse_response() {
    Json::Reader reader;
    Json::Value root;

    bool parsingSuccessful = reader.parse(buffer, root);
    vector<ArticleInfo> articles;

    if (!parsingSuccessful) {
        string what = "Springer: Parser error: " + reader.getFormattedErrorMessages();
        cout << what << endl;
		return articles;
    }

    int size = stoi(root["result"][0].get("total", "").asString());

    Json::Value records = root["records"];

    for (int i = 0; i < size; i++) {
        Json::Value info = records[i];
	    string title = info["title"].asString();
	    Json::Value authorsList = info.get("creators","");
		vector<string> authors = {};
		if (authorsList.size() > 0) {
	        for (unsigned int j = 0; j < authorsList.size(); j++) {
	            authors.push_back(authorsList[j].get("creator", "").asString());
	        }
	    } else {
	        authors.push_back("");
	    }

	    string venue = info.get("publicationName", "").asString();
	    string volume = info.get("volume", "").asString();
	    string number = info.get("number", "").asString();
	    string start_page = info.get("startingPage","").asString();
		string end_page = info.get("endingPage","").asString();
		string pages = start_page +"-" + end_page;
		string year = info.get("publicationDate", "").asString();
	    year = year.substr(0,4);
	    string type = info.get("issuetype","").asString();
	    string art_url = info["url"][0].get("value","").asString();
        articles.push_back(ArticleInfo(title, authors, venue, volume, 
                    number, pages, year, type, art_url));
    }

    return articles;
}


