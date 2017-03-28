#include <vector>
#include <sstream>

#include "BiblioServer.h"
#include "Database.h"
#include "BiblioManager.h"

void BiblioServer::ev_handler(mg_connection *conn, int event, void *data) {
    if (event == MG_EV_HTTP_REQUEST) {

        std::string directoryPath = Config::get_instance().lookup("articles.path");
        std::vector<std::string> filenames = read_pdf_files_recursive(directoryPath);

        Database *db = Database::connect_database();
        std::vector<ArticleInfo> data_from_db = {};

        for (const auto &filename : filenames) {
            ArticleInfo *result_ptr = nullptr;
            if (db != nullptr) {
                result_ptr = db->get_data(filename);
                if (result_ptr != nullptr) {
                    data_from_db.push_back(*result_ptr);
                    delete result_ptr;
                }
            }
        }

        if (db != nullptr) {
            delete db;
        }

        BiblioManager manager;
        std::stringstream out_html("biblio.html");
        BiblioManager::start_print_html(out_html);
        manager.print_html(out_html, data_from_db);
        BiblioManager::end_print_html(out_html);

        mg_send_head(conn, 200, -1, "Content-Type: text/html");
        mg_printf_http_chunk(conn, "%s", out_html.str().c_str());
        mg_send_http_chunk(conn, "", 0);
    }
}

void BiblioServer::startServer() {
    static const char *s_http_port = "8000";
    struct mg_mgr mgr;
    struct mg_connection *conn;

    mg_mgr_init(&mgr, NULL);
    conn = mg_bind(&mgr, s_http_port, ev_handler);
    mg_set_protocol_http_websocket(conn);

    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);

}