#include <algorithm>
#include <ctime>
#include <iomanip>
#include <thread>
#include <queue>

#include "BiblioManager.h"
#include "BiblioThreadContext.h"
#include "tools.h"
#include "FB2Parser.h"


using namespace std;

std::vector<ArticleInfo> BiblioManager::search_requester(Requester &requester, std::string query) {
    transform(query.begin(), query.end(), query.begin(), ::tolower);
    vector<ArticleInfo> result = {};
    vector<ArticleInfo> additional_result = {};
    result = requester.publication_request(query);
    vector<string> words = split(query, ' ');

    // discard first word
    string new_query = "";
    for (unsigned int i = 1; i < words.size()-1; i++) {
        new_query += words[i] + " ";
    }
    new_query += words[words.size()-1];
    additional_result = requester.publication_request(new_query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());

    // discard last word
    new_query = "";
    for (unsigned int i = 0; i < words.size()-2; i++) {
        new_query += words[i] + " ";
    }
    new_query += words[words.size()-2];
    additional_result = requester.publication_request(new_query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());

    // discard first two words
    new_query = "";
    for (unsigned int i = 2; i < words.size()-1; i++) {
        new_query += words[i] + " ";
    }
    new_query += words[words.size()-1];
    additional_result = requester.publication_request(new_query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());

    // discard last two words
    new_query = "";
    for (unsigned int i = 0; i < words.size()-3; i++) {
        new_query += words[i] + " ";
    }
    new_query += words[words.size()-3];
    additional_result = requester.publication_request(new_query);
    result.insert(result.end(), additional_result.begin(), additional_result.end());

    if (words.size() > 10) {
        // discard first five words
        new_query = "";
        for (unsigned int i = 5; i < words.size()-1; i++) {
         new_query += words[i] + " ";
        }
        new_query += words[words.size()-1];
        additional_result = requester.publication_request(new_query);
        result.insert(result.end(), additional_result.begin(), additional_result.end());

        // discard last five words
        new_query = "";
        for (unsigned int i = 0; i < words.size()-6; i++) {
         new_query += words[i] + " ";
        }
        new_query += words[words.size()-6];
        additional_result = requester.publication_request(new_query);
        result.insert(result.end(), additional_result.begin(), additional_result.end());
     }
     return result;
 }


bool BiblioManager::greater(const ArticleInfo &info_1, const ArticleInfo &info_2) {
    return (info_1.get_precision() > info_2.get_precision());
}

void BiblioManager::print_bib(ostream &out, const vector<ArticleInfo> &result) const {
    for (size_t k = 0; k < result.size(); k++) {
        if (result[k].get_authors().size() > 0) {
            vector<string> authors = result[k].get_authors();
            size_t t = authors.size();
            string label = "";

            if (t > 1) {
                label = short_name(authors[0]) + short_name(authors[1]) + result[k].get_year();
            } else {
                label = short_name(authors[0]) + result[k].get_year();
            }

            out << "@ARTICLE{" << label << ",\n";
            out << "author = {";
            for (size_t i = 0; i < t - 1; ++i) {
                out << authors[i] << " and\n";
            }
            out << authors[t - 1];
            out << "},\n";
            out << "title = {" << result[k].get_title() << "}";

            if (result[k].get_pages().size() > 0) {
                out << ",\n" << "pages = {" << result[k].get_pages() << "}";
            }
            if (result[k].get_number().size() > 0) {
                out << ",\n" << "number = {" << result[k].get_number() << "}";
            }
            if (result[k].get_venue().size() > 0) {
                out << ",\n" << "venue = {" << result[k].get_venue() << "}";
            }
            if (result[k].get_type().size() > 0) {
                out << ",\n" << "type = {" << result[k].get_type() << "}";
            }
            if (result[k].get_url().size() > 0) {
                out << ",\n" << "url = {" << result[k].get_url() << "}";
            }
            if (result[k].get_volume().size() > 0) {
                out << ",\n" << "volume = {" << result[k].get_volume() << "}";
            }
            if (result[k].get_year().size() > 0) {
                out << ",\n" << "year = {" << result[k].get_year() << "}";
            }
            out << "\n}\n\n";
        } else {
            out << "@ARTICLE{" << result[k].get_filename() << ",\n";
            out << "title = {" << result[k].get_title() << "}";
            out << "\n}\n\n";
        }
    }
}

void BiblioManager::start_print_html(ostream &out) {
    out << "<!DOCTYPE html>\n";
    out << "<html>\n";
    out << "\t<head>\n";
    out << "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n";
    out << "\t\t<title>Biblio results</title>\n";
    out << "\t</head>\n";
    out << "\t<body>\n";
}

void BiblioManager::end_print_html(ostream &out) {
    out << "\t</body>\n";
    out << "</html>\n";
}

void BiblioManager::print_html(ostream &out, const vector<ArticleInfo> &result) {
    out << "\t\t<table border=\"1\" width=\"100%\" cellpadding=\"5\" bgcolor=\"#E6E6FA\">\n";
    for (size_t i = 0; i < result.size(); i++) {
        out << "\t\t\t<tr>\n";
        string filename = result[i].get_filename();
        string name = filename.substr(filename.find_last_of('/') + 1);
        out << "\t\t\t\t<td align=\"center\"><a href=\"" << filename << "\">" << name << "</a></td>\n";
        if (i % 2 == 0) {
            out << "\t\t\t\t<td bgcolor=\"#F5DEB3\">";
        } else {
            out << "\t\t\t\t<td bgcolor=\"#FFF8DC\">";
        }
        vector<string> authors = result[i].get_authors();
        size_t t = authors.size();
        if (t > 0) {
            for (size_t j = 0; j < t - 1; ++j) {
                out << authors[j] << ", ";
            }
            out << authors[t - 1] << ":<br>\n  ";
        }
        out << "\t\t\t\t\t" << result[i].get_title();
        if (result[i].get_type() != "") {
            out << " " << result[i].get_type() << ".";
        }
        if (result[i].get_venue() != "") {
            out << " " << result[i].get_venue() << ".";
        }
        if (result[i].get_volume() != "") {
            out << " " << result[i].get_volume() << ".";
        }
        if (result[i].get_year() != "") {
            out << " " << result[i].get_year() << ".";
        }
        if (result[i].get_pages() != "") {
            out << " " << result[i].get_pages() << ".";
        }
        if (result[i].get_number() != "") {
            out << " " << result[i].get_number() << ".";
        }
        if (result[i].get_url() != "") {
            out << "<br>\n  \t\t\t\t\t" << "<a href=\"" << result[i].get_url() << "\">" << result[i].get_url() << "</a>\n";
        }
        out << "\t\t\t\t</td>\n";
        out << "\t\t\t</tr>\n";
    }

    out << "\t\t</table>" << endl;
}

BiblioManager::BiblioManager(int threads) {
    this->threads_num = threads;
}

std::vector<ArticleInfo> BiblioManager::search_distance(std::function<size_t(const std::string &,
                                                    const std::string &)> dist, bool is_offline) {
    vector<thread> threads;
    for(int i = 0; i < threads_num; ++i){
        threads.push_back(std::thread(thread_function, dist, is_offline));
    }

    for(auto& thread : threads){
        thread.join();
    }
    return BiblioThreadContext::instance().get_output();
}

void BiblioManager::process_pdf(const string &filename, bool is_offline, const RequesterManager &req_manager,
                                std::function<size_t(const std::string &, const std::string &)> dist) {
    const vector<Requester *> &requesters = req_manager.get_all_requesters();

    PictureParser picture_parser = PictureParser(filename, 300, 300, get_random_filename() + ".png", "png", 700);
    string saved_title = raw_to_formatted(picture_parser.find_title());
    string title = low_letters_only(saved_title);
    if (is_offline || title.size() == 0) {
        BiblioThreadContext::instance().my_push(ArticleInfo(saved_title, filename));
        return;
    }

    for (size_t k = 0; k < requesters.size(); k++) {
        try {
            vector<ArticleInfo> result = search_requester(*requesters[k], saved_title);
            if (result.size() > 0) {
                for (size_t i = 0; i < result.size(); i++) {
                    string cur_title = raw_to_formatted(result[i].get_title());
                    cur_title = low_letters_only(cur_title);
                    size_t distance = dist(cur_title, title);
                    int precision = 100 - (int) (100 * distance / max(title.size(), cur_title.size()));
                    result[i].set_precision(precision);
                }
                stable_sort(result.begin(), result.end(), greater);
                if (result[0].get_precision() > 90) {
                    result[0].set_filename(filename);
                    BiblioThreadContext::instance().my_push(result[0]);

                    return;
                }
            }
        } catch (BiblioException e) {
            continue;
        }
    }

    BiblioThreadContext::instance().my_push(ArticleInfo(saved_title, filename));
    return;
}
void BiblioManager::process_fb2(const string &filename) {
    try {
        FB2Parser fb2_parser;
        const ArticleInfo result = fb2_parser.parse(filename);
        BiblioThreadContext::instance().my_push(result);
    } catch (BiblioException e) {
        std::cout << e.what() << std::endl;
    }

}

void BiblioManager::thread_function(std::function<size_t(const std::string &, const std::string &)> dist,
                                    bool is_offline) {
    std::string filename;
    RequesterManager req_manager = RequesterManager();

    while(!BiblioThreadContext::instance().my_empty()) {
        filename = BiblioThreadContext::instance().my_pop();
        if (get_file_extention(filename) == "pdf") {
            process_pdf(filename, is_offline, req_manager, dist);
        } else if (get_file_extention(filename) == "fb2") {
            process_fb2(filename);
        }
    }
}

BiblioManager::BiblioManager() {
    threads_num = 1;
}

void BiblioManager::cout_not_found_articles(const vector<ArticleInfo> &result) {
    cout << "=========================================================================" << endl;
    cout << "                       Start not found articles                          " << endl;
    cout << "=========================================================================" << endl << endl;
    size_t result_size = result.size(), found = result_size;
    for (size_t k = 0; k < result_size; k++) {
        if (get_file_extention(result[k].get_filename()) == "pdf" && result[k].get_year() == "") {
            found--;
            cout << "filename: " << result[k].get_filename() << endl;
            cout << "title: " << result[k].get_title() << endl << endl;
        }
    }
    cout << "Found information about " << found << " articles from " << result_size << endl;
    cout << "=========================================================================" << endl;
    cout << "                         End not found articles                          " << endl;
    cout << "=========================================================================" << endl;
}

std::vector<ArticleInfo> BiblioManager::get_info(const std::vector<std::string> &filenames,
                                  Database * db, bool is_offline) {
    vector<string> filenames_to_search;
    vector<ArticleInfo> data_from_db;
    if (db != nullptr) {
        data_from_db = db->get_data(filenames, &filenames_to_search);
    } else {
        filenames_to_search = filenames;
    }

    if (!filenames_to_search.empty()) {
        queue<string, deque<string>> in(deque<string>(filenames_to_search.begin(), filenames_to_search.end()));
        BiblioThreadContext::init(in);
        vector<ArticleInfo> result = search_distance(levenshtein_distance, is_offline);
        BiblioManager::cout_not_found_articles(result);

        if (db != nullptr) {
            db->add_data(result);
        }
        data_from_db.insert(data_from_db.end(), result.begin(), result.end());
    }
    return data_from_db;
}

