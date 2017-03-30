#include <vector>
#include <queue>
#include <sstream>

#include "BiblioServer.h"
#include "Database.h"
#include "BiblioManager.h"
#include "BiblioThreadContext.h"

BiblioServer::BiblioServer() {

    std::string directoryPath = Config::get_instance().lookup("articles.path");
    std::vector<std::string> filenames = read_pdf_files_recursive(directoryPath);

    Database *db = Database::connect_database();
    std::vector<ArticleInfo> data_from_db;
    if (db != nullptr) {
        data_from_db = db->get_data(filenames);
        delete db;
    }

    BiblioManager manager;
    std::stringstream out_html;
    BiblioManager::start_print_html(out_html);
    manager.print_html(out_html, data_from_db);
    BiblioManager::end_print_html(out_html);

    content = out_html.str();
}

BiblioServer& BiblioServer::get_instance() {
    static BiblioServer server;
    return server;
}

std::string& BiblioServer::get_content() {
    return content;
}

void BiblioServer::ev_handler(mg_connection *conn, int event, void *data) {
    if (event == MG_EV_HTTP_REQUEST) {
        const char *out_html = BiblioServer::get_instance().get_content().c_str();

        mg_send_head(conn, 200, -1, "");
        mg_printf_http_chunk(conn, "%s", out_html);
        mg_send_http_chunk(conn, "", 0);
    }

    if (event == MG_EV_TIMER) {
        std::string directoryPath = Config::get_instance().lookup("articles.path");
        std::vector<std::string> filenames = read_pdf_files_recursive(directoryPath);

        Database *db = Database::connect_database();
        std::vector<ArticleInfo> data_from_db;
        std::vector<std::string> filenames_to_search;
        if (db != nullptr) {
            data_from_db = db->get_data(filenames, &filenames_to_search);
        }

        BiblioManager manager;
        std::queue<std::string, std::deque<std::string>> in(std::deque<std::string>(filenames_to_search.begin(), filenames_to_search.end()));
        BiblioThreadContext::init(in);
        std::vector<ArticleInfo> result = manager.search_distance(levenshtein_distance, false);

        if (db != nullptr) {
            db->add_data(result);
            delete db;
        }
        data_from_db.insert(data_from_db.end(), result.begin(), result.end());

        std::stringstream out_html;
        BiblioManager::start_print_html(out_html);
        manager.print_html(out_html, data_from_db);
        BiblioManager::end_print_html(out_html);

        BiblioServer::get_instance().get_content() = out_html.str();

        double timeout = std::stod(Config::get_instance().lookup("articles.timeout"));
        mg_set_timer(conn, mg_time() + timeout);
    }
}

void BiblioServer::startServer() {
    const char *http_port = Config::get_instance().lookup("articles.port");
    double timeout = std::stod(Config::get_instance().lookup("articles.timeout"));
    struct mg_mgr mgr;
    struct mg_connection *conn;



    mg_mgr_init(&mgr, NULL);
    conn = mg_bind(&mgr, http_port, ev_handler);
    mg_set_protocol_http_websocket(conn);
    mg_set_timer(conn, mg_time() + timeout);

    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);

}