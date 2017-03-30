#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <sqlite3.h>

#include "BiblioException.h"
#include "ArticleInfo.h"
#include "Config.h"
#include "tools.h"

class Database {

private:
    std::string db_filename;
    sqlite3 * db;

public:
    Database(const std::string &db_filename);
    ~Database();
    static Database * connect_database();
    int check_status (const char * request, sqlite3_stmt **stmt) const;
    ArticleInfo * get_data(const std::string& filename) const;
    std::vector<ArticleInfo> get_data(const std::vector<std::string>& filenames, std::vector<std::string>* absent = nullptr) const;
    void add_data(const std::vector<ArticleInfo> &data);
    void purge();
};

