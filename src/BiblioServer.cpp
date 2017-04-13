#include <vector>
#include <queue>
#include <sstream>
#include <thread>
#include <ctime>

#include "BiblioServer.h"
#include "Database.h"
#include "BiblioManager.h"
#include "BiblioThreadContext.h"

void print_rescan_button_html(std::ostream &out) {
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
    tm *timeinfo = localtime(&seconds);
    char buf[10];
    sprintf(buf, "%02d:%02d:%02d: ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    std::cout << buf << message << std::endl;
}

BiblioServer::BiblioServer() {}

BiblioServer &BiblioServer::get_instance() {
    static BiblioServer server;
    return server;
}

std::string BiblioServer::get_content_copy() {
    return content;
}

std::string BiblioServer::rescan_and_get_content() {
    std::string directoryPath = Config::get_instance().lookup("articles.path");
    std::vector<std::string> filenames = read_pdf_files_recursive(directoryPath);

    Database *db = Database::connect_database();

    if (db != nullptr) {
        db->purge();
    }

    BiblioManager manager;
    std::vector<ArticleInfo> result = manager.get_info(filenames, db, false);

    if (db != nullptr) {
        delete db;
    }

    std::stringstream out_html;
    BiblioManager::start_print_html(out_html);
    print_rescan_button_html(out_html);
    manager.print_html(out_html, result);
    BiblioManager::end_print_html(out_html);

    return out_html.str();
}

void BiblioServer::update_db_thread_function() {
    BiblioServer &instance = BiblioServer::get_instance();
    int timeout = std::stoi(Config::get_instance().lookup("articles.timeout"));
    std::mutex m_tmp;
    while (true) {
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
    BiblioServer &instance = BiblioServer::get_instance();

    if (event == MG_EV_HTTP_REQUEST) {

        struct http_message *hm = (struct http_message *) data;
        std::string query(hm->query_string.p, hm->query_string.len);
        std::string uri(hm->uri.p, hm->uri.len);

        std::stringstream log_message;
        log_message << "got request to " << uri.c_str() << " with params " << query.c_str();
        log(log_message.str().c_str());

        if (uri == "/") {
            mg_send_head(conn, 200, -1, "");
            if (query == "rescan") {
                instance.rescan_cond_var.notify_one();
            } else {
                std::string out_html;
                {
                    std::unique_lock<std::mutex> lock(instance.m_content);
                    out_html = instance.get_content_copy();
                }
                mg_printf_http_chunk(conn, "%s", out_html.c_str());
            }
            mg_send_http_chunk(conn, "", 0);
        } else {
            mg_http_serve_file(conn, hm, uri.c_str(),mg_mk_str("application/pdf"), mg_mk_str(""));
        }

    }
}

void BiblioServer::start_server() {
    const char *http_port = Config::get_instance().lookup("articles.port");
    struct mg_mgr mgr;
    struct mg_connection *conn;

    log("scanning folder");
    BiblioServer::get_instance().content = rescan_and_get_content();

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
