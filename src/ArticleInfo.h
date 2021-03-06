#pragma once

#include <vector>
#include <cstring>
#include <string>

#include "../lib/json/json.h"
#include "../lib/json/value.h"

class ArticleInfo {

private:
    std::string filename;
    std::string title;
    std::vector<std::string> authors;
    std::string venue;
    std::string volume;
    std::string number;
    std::string pages;
    std::string year;
    std::string type;
    std::string url;
    int precision;

public:
    ArticleInfo(std::string const &title, std::vector<std::string> const &authors,
                std::string const &venue, std::string const &volume, std::string const &number,
                std::string const &pages, std::string const &year, std::string const &type,
                std::string const &url);
    ArticleInfo(std::string const &title, std::vector<std::string> const &authors, std::string const &filename);
    ArticleInfo(std::string const &title, std::string const &filename);
    std::string const &get_filename() const;
    std::string const &get_title() const;
    std::vector<std::string> const &get_authors() const;
    std::string const &get_venue() const;
    std::string const &get_volume() const;
    std::string const &get_number() const;
    std::string const &get_pages() const;
    std::string const &get_year() const;
    std::string const &get_type() const;
    std::string const &get_url() const;
    int get_precision() const;
    void set_precision(int precision);
    void set_filename(std::string const& filename);
    std::string to_string();
};

