#pragma once

#include <functional>
#include <mutex>
#include <queue>

#include "BiblioException.h"
#include "Requesters/Requester.h"
#include "PictureParser.h"
#include "tools.h"
#include "RequesterManager.h"

class BiblioManager {

private:
    int threads_num;

private:
    static bool greater(const ArticleInfo &info_1, const ArticleInfo &info_2);
    static void thread_function(std::function<size_t(const std::string &, const std::string &)> dist, bool is_offline);
    static void process_pdf(const std::string &filename, bool is_offline, const RequesterManager &req_manager,
                            std::function<size_t(const std::string &, const std::string &)> dist);
    static void process_fb2(const std::string &filename);
    static std::vector<ArticleInfo> search_requester(Requester &requester, std::string query);
public:
    BiblioManager();
    BiblioManager(int threads);
    std::vector<ArticleInfo> search_distance(std::function<size_t(const std::string &,
                                              const std::string &)> dist, bool is_offline);

    std::vector<ArticleInfo> get_info(const std::vector<std::string> &filenames,
                                      Database * db, bool is_offline);
    static void start_print_html(std::ostream &out);
    static void end_print_html(std::ostream &out);
    static void print_html(std::ostream &out, const std::vector<ArticleInfo> &result);
    static void cout_not_found_articles(const std::vector<ArticleInfo> &result);
    void print_bib(std::ostream &out, const std::vector<ArticleInfo> &result) const;
};
