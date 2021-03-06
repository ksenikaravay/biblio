#include "ArticleInfo.h"

using std::vector;
using std::string;

ArticleInfo::ArticleInfo(string const &title, vector<string> const &authors,
                string const &venue, string const &volume, string const &number,
                string const &pages, string const &year, string const &type, string const &url) {
    this->title = title;
    this->authors = authors;
    this->venue = venue;
    this->volume = volume;
    this->number = number;
    this->pages = pages;
    this->year = year;
    this->type = type;
    this->url = url;
    this->precision = 0;
}

ArticleInfo::ArticleInfo(std::string const &title, vector<string> const &authors, std::string const &filename) {
    this->filename = filename;
    this->title = title;
    this->authors = authors;
    this->venue = "";
    this->volume = "";
    this->number = "";
    this->pages = "";
    this->year = "";
    this->type = "";
    this->url = "";
    this->precision = 0;
}

ArticleInfo::ArticleInfo(std::string const &title, std::string const &filename) {
    this->filename = filename;
    this->title = title;
    this->authors = {};
    this->venue = "";
    this->volume = "";
    this->number = "";
    this->pages = "";
    this->year = "";
    this->type = "";
    this->url = "";
    this->precision = 0;
}

string const &ArticleInfo::get_title() const {
    return title;
}

vector<string> const &ArticleInfo::get_authors() const {
    return authors;
}

string const &ArticleInfo::get_venue() const {
    return venue;
}

string const &ArticleInfo::get_volume() const {
    return volume;
}

string const &ArticleInfo::get_number() const {
    return number;
}

string const &ArticleInfo::get_pages() const {
    return pages;
}

string const &ArticleInfo::get_year() const {
    return year;
}

string const &ArticleInfo::get_type() const {
    return type;
}

string const &ArticleInfo::get_url() const {
    return url;
}

int ArticleInfo::get_precision() const {
    return precision;
}

void ArticleInfo::set_precision(int precision) {
    this->precision = precision;
}

string ArticleInfo::to_string() {

    string output = "\n Filename: \t" + this->filename + "\n Title: \t" + this->title;
    output += "\n Authors: ";
    for (unsigned int i = 0; i < authors.size(); i++) {
        output += "\n\t\t" + this->authors[i];
    }
    output += "\n Venue: \t" + this->venue;
    output += "\n Volume: \t" + this->volume;
    output += "\n Number: \t" + this->number;
    output += "\n Pages: \t" + this->pages;
    output += "\n Year: \t\t" + this->year;
    output += "\n Type: \t\t" + this->type;
    output += "\n URL: \t\t" + this->url;
    output += "\n Precision of response: \t" + std::to_string(this->precision) + "% \n";
    return output;
}

void ArticleInfo::set_filename(std::string const &filename) {
    this->filename = filename;
}

std::string const &ArticleInfo::get_filename() const {
    return filename;
}
