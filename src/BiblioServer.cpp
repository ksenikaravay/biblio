#include <vector>
#include <queue>
#include <sstream>
#include <thread>
#include <ctime>

#include "BiblioServer.h"
#include "Database.h"
#include "BiblioManager.h"
#include "BiblioThreadContext.h"
#include <iomanip>
#include <algorithm>

void print_rescan_button_html(std::ostream &out){
    out << "<script type=\"text/javascript\">\n"
            "    function on_button_click(){\n"
            "        var request = new XMLHttpRequest();\n"
            "        request.open(\"GET\", \"/?rescan\", true);\n"
            "        request.send();\n"
            "    }\n"
            "</script>\n"
            "<p align=\"center\"><button type=\"button\" onclick=\"on_button_click()\">Rescan directory</button></p>";
}

void log(const char *message) {
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    char buf[10];
    sprintf(buf, "%02d:%02d:%02d: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    std::cout << buf << message << std::endl;
}

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
    print_rescan_button_html(out_html);
    manager.print_html(out_html, data_from_db);
    BiblioManager::end_print_html(out_html);

    content = out_html.str();
}

BiblioServer& BiblioServer::get_instance() {
    static BiblioServer server;
    return server;
}

std::string BiblioServer::get_content_copy() {
    return content;
}

std::string BiblioServer::rescan_and_get_content(){
    std::string directoryPath = Config::get_instance().lookup("articles.path");
    std::vector<std::string> filenames = read_pdf_files_recursive(directoryPath);

    Database *db = Database::connect_database();

    if (db != nullptr){
        db->purge();
    }

    std::vector<ArticleInfo> data_from_db;
    std::vector<std::string> filenames_to_search;
    if (db != nullptr) {
        data_from_db = db->get_data(filenames, &filenames_to_search);
    }

    BiblioManager manager;
    if (!filenames_to_search.empty()) {

        std::queue<std::string, std::deque<std::string>> in(
                std::deque<std::string>(filenames_to_search.begin(), filenames_to_search.end()));
        BiblioThreadContext::init(in);
        std::vector<ArticleInfo> result = manager.search_distance(levenshtein_distance, false);

        if (db != nullptr) {
            db->add_data(result);
            delete db;
        }
        data_from_db.insert(data_from_db.end(), result.begin(), result.end());
    }

    std::stringstream out_html;
    BiblioManager::start_print_html(out_html);
    print_rescan_button_html(out_html);
    manager.print_html(out_html, data_from_db);
    BiblioManager::end_print_html(out_html);

    return out_html.str();
}

void BiblioServer::update_db_thread_function(){
    BiblioServer& instance = BiblioServer::get_instance();
    int timeout = std::stoi(Config::get_instance().lookup("articles.timeout"));
    std::mutex m_tmp;
    while (true){
        std::unique_lock<std::mutex> lock_to_wait(m_tmp);
        instance.rescan_cond_var.wait_for(lock_to_wait, std::chrono::seconds(timeout));

        std::string out_html = rescan_and_get_content();
        if (!out_html.empty()) {
            std::unique_lock<std::mutex> lock_content(instance.m_content);
            instance.content = out_html;

            log("out_html updated");
        }
    }
}

void BiblioServer::ev_handler(mg_connection *conn, int event, void *data) {
    BiblioServer& instance = BiblioServer::get_instance();

    if (event == MG_EV_HTTP_REQUEST) {

        struct http_message *hm = (struct http_message *) data;
        std::string query(hm->query_string.p, hm->query_string.len);
        std::string uri(hm->uri.p, hm->uri.len);
        mg_send_head(conn, 200, -1, "");
        if (uri == "/") {
            if (query == "rescan") {
                instance.rescan_cond_var.notify_one();

                log("force rescan started");
            } else {
                std::string out_html;
                {
                    std::unique_lock<std::mutex> lock(instance.m_content);
                    out_html = instance.get_content_copy();
                }
                mg_printf_http_chunk(conn, "%s", out_html.c_str());

                log("got new request to /");
            }
        }
        mg_send_http_chunk(conn, "", 0);
    }
}

void BiblioServer::startServer() {
    const char *http_port = Config::get_instance().lookup("articles.port");
    struct mg_mgr mgr;
    struct mg_connection *conn;

    mg_mgr_init(&mgr, NULL);
    conn = mg_bind(&mgr, http_port, ev_handler);
    mg_set_protocol_http_websocket(conn);

    log("starting server");

    std::thread thread_updating_db(update_db_thread_function);

    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);

}
