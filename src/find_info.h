#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>
#include "Parser.h"
#include "DBLPManager.h"
#include "ArticleInfo.h"
#include "Biblio_exception.h"

bool greater (const ArticleInfo& info_1, const ArticleInfo& info_2);
std::vector <ArticleInfo> find_info(std::string filename, bool offline);
std::vector <ArticleInfo> search_dblp (DBLPManager dblp, std::string query);
int levenshtein_distance(std::string s, std::string t);
void print_info(std::vector <ArticleInfo> result);
void printf_info(std::string filename, std::vector <ArticleInfo> result);
