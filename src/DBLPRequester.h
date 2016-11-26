#pragma once
#include "Requester.h"

class DBLPRequester: public Requester {

protected:
	std::string url;
	std::string apikey;  

    std::vector<ArticleInfo> parse_response(char * buffer);
	std::string make_request(std::string query);

public:
	DBLPRequester(std::string url);
};
