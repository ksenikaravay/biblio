#pragma once

#include <iostream>
#include <expat.h>
#include <fstream>
#include <cstring>
#include <vector>
#include "ArticleInfo.h"

struct FB2Parser {
private:
    static const int BUFFSIZE = 4096;

    static void start(void *data, const char *el, const char **attr);
    static void end(void *data, const char *el);
    static void text_saver (void * userData, const XML_Char * s, int length);

public:
    ArticleInfo parse(const std::string &filename);
};


