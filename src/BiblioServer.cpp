#include <vector>
#include <queue>
#include <sstream>
#include <thread>
#include <ctime>
#include <map>
#include <curl/curl.h>

#include "BiblioServer.h"
#include "Database.h"
#include "BiblioManager.h"
#include "BiblioThreadContext.h"

std::string get_current_time() {
    time_t seconds = time(NULL);
    tm *timeinfo = localtime(&seconds);
    char buf[10];
    sprintf(buf, "%02d:%02d:%02d ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    return std::string(buf);
}

void log(const char *message) {
    std::cout << get_current_time() << message << std::endl;
}

void start_print_search_html(std::ostream &out, std::string &query) {
    out << "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "<head>\n"
            "    <meta charset=\"UTF-8\">\n"
            "    <title>Biblio search results</title>\n"
            "    <style>\n"
            "        button {\n"
            "            font: 14pt serif;\n"
            "            margin-left: 10px;\n"
            "        }\n"
            "        input.field {\n"
            "            width: 25%;\n"
            "            height: 16pt;\n"
            "        }\n"
            "        input.button {\n"
            "            font: 14pt serif;\n"
            "            margin: 10px;\n"
            "        }"
            "        image {\n"
            "            opacity: 0.5;\n"
            "            width: 40px;\n"
            "            height: 40px;\n"
            "            border: 0px;\n"
            "        }"
            "        table.results {\n"
            "            width: 100%;\n"
            "            table-layout: fixed;\n"
            "            border: 2pt grey groove;\n"
            "            padding: 5pt;\n"
            "            border-collapse: separate;\n"
            "            border-radius: 6px;\n"
            "            background-color: rgba(245,245,255,0.75);\n"
            "            font: normal 14pt serif;\n"
            "        }\n"
            "        table.results caption{\n"
            "            text-align: left;\n"
            "            font: 14pt serif;\n"
            "        }\n"
            "        table.results thead{\n"
            "            background-color: lightgrey;\n"
            "        }\n"
            "        tr.even{\n"
            "            background-color: white;\n"
            "        }\n"
            "        tr.even td{\n"
            "            border-top: 1pt lightgrey solid;\n"
            "            border-bottom: 1pt lightgrey solid;\n"
            "        }\n"
            "        td.view {\n"
            "            width: 60px;\n"
            "            text-align: center;\n"
            "        }\n"
            "        td.author{\n"
            "            width: 20%;\n"
            "            text-align: center;\n"
            "        }\n"
            "        td.title{\n"
            "            width: 40%;\n"
            "            text-align: center;\n"
            "        }\n"
            "    </style>\n"
            "</head>\n"
            "<body>\n"
            "<p><a href=\"/\">Back to catalogue</a></p>\n"
            "<form align=\"center\"action=\"search\"><input class=\"field\" name=\"any\" type=\"text\"><input class=\"button\" type=\"submit\" value=\"Search\"></form>\n"
            "<table class=\"results\">\n"
            "    <caption>Search results for \"" << query <<"\"</caption>\n"
            "    <thead>\n"
            "        <tr>\n"
            "            <td class=\"view\">Read</td>\n"
            "            <td class=\"author\">Authors</td>\n"
            "            <td class=\"title\">Title</td>\n"
            "            <td>Article info</td>\n"
            "        </tr>\n"
            "    </thead>";
}
void start_print_html(std::ostream &out) {
    std::string directoryPath = Config::get_instance().lookup("directory.path");
    out << "<!DOCTYPE html>\n"
            "<html lang=\"en\">\n"
            "<head>\n"
            "    <meta charset=\"UTF-8\">\n"
            "    <title>Biblio catalogue</title>\n"
            "    <style>\n"
            "        button {\n"
            "            font: 14pt serif;\n"
            "            margin-left: 10px;\n"
            "        }\n"
            "        table.service {\n"
            "            width: 100%;\n"
            "        }\n"
            "        input.field {\n"
            "            width: 50%;\n"
            "            height: 16pt;\n"
            "        }\n"
            "        input.button {\n"
            "            font: 14pt serif;\n"
            "            margin: 10px;\n"
            "        }"
            "        span.rescantime{\n"
            "            color: grey;\n"
            "            font: italic 12pt serif;\n"
            "        }\n"
            "        img {\n"
            "            opacity: 0.5;\n"
            "            width: 40px;\n"
            "            height: 40px;\n"
            "            border: 0px;\n"
            "        }"
            "        table.results {\n"
            "            width: 100%;\n"
            "            table-layout: fixed;\n"
            "            border: 2pt grey groove;\n"
            "            padding: 5pt;\n"
            "            border-collapse: separate;\n"
            "            border-radius: 6px;\n"
            "            background-color: rgba(245,245,255,0.75);\n"
            "            font: normal 14pt serif;\n"
            "        }\n"
            "        table.results caption{\n"
            "            text-align: left;\n"
            "            font: 14pt serif;\n"
            "        }\n"
            "        table.results thead{\n"
            "            background-color: lightgrey;\n"
            "        }\n"
            "        tr.even{\n"
            "            background-color: white;\n"
            "        }\n"
            "        tr.even td{\n"
            "            border-top: 1pt lightgrey solid;\n"
            "            border-bottom: 1pt lightgrey solid;\n"
            "        }\n"
            "        td.view {\n"
            "            width: 60px;\n"
            "            text-align: center;\n"
            "        }\n"
            "        td.author{\n"
            "            width: 20%;\n"
            "            text-align: center;\n"
            "        }\n"
            "        td.title{\n"
            "            width: 40%;\n"
            "            text-align: center;\n"
            "        }\n"
            "    </style>\n"
            "</head>\n"
            "<body>\n"
            "<script type=\"text/javascript\">\n"
            "    function rescan_click(){\n"
            "        var request = new XMLHttpRequest();\n"
            "        request.open(\"GET\", \"/?rescan\", true);\n"
            "        request.send();\n"
            "    }\n"
            "</script>\n"
            "<table class=\"service\">\n"
            "    <tr>\n"
            "        <td><form action=\"search\"><input class=\"field\" name=\"any\" type=\"text\"><input class=\"button\" type=\"submit\" value=\"Search\"></form></td>\n"
            "        <td align=\"right\"><button type=\"button\" onclick=\"rescan_click()\">Rescan directory</button></td>\n"
            "        <td align=\"right\"><button type=\"button\" onclick=\"document.location.href='/settings'\">Settings</button></td>\n"
            "    </tr>\n"
            "    <tr><td></td><td align=\"right\"><span class=\"rescantime\"> last rescan: " << get_current_time() << "</span></td><td></td></tr>\n"
            "</table>\n"
            "<table class=\"results\">\n"
            "    <caption>" << directoryPath << "</caption>\n"
            "    <thead>\n"
            "    <tr>\n"
            "        <td class=\"view\">Read</td>\n"
            "        <td class=\"author\">Authors</td>\n"
            "        <td class=\"title\">Title</td>\n"
            "        <td>Article info</td>\n"
            "    </tr>\n"
            "    </thead>";
}

void print_res_table_html(std::ostream &out, const std::vector<ArticleInfo> &result) {
    for (size_t i = 0; i < result.size(); i++) {
        std::string filename = result[i].get_filename();
        std::string name = filename.substr(filename.find_last_of('/') + 1);
        if (i % 2) {
            out << "    <tr class=\"odd\">";
        } else {
            out << "    <tr class=\"even\">";
        }
        out << "        <td class=\"view\">\n"
                "            <a href=\"read?" << filename << "\"><img src=\"viewIcon.png\"></a>\n";
        out << "        </td>\n"
                "        <td class=\"author\">";
        std::vector<std::string> authors = result[i].get_authors();
        size_t t = authors.size();
        if (t > 0) {
            for (size_t j = 0; j < t - 1; ++j) {
                out << authors[j] << ", ";
            }
            out << authors[t - 1] << "</td>\n";
        }
        out <<  "        <td class=\"title\">" << result[i].get_title() << "</td>\n"
                "        <td>";
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
            out << " [<a href=\"" << result[i].get_url() << "\">more</a>]\n";
        }
        out << "        </td>\n"
                "    </tr>";
    }
}

void end_print_html(std::ostream &out) {
    out << "</table>\n"
            "</body>\n"
            "</html>";
}

std::map<std::string, std::string> tokenize(const char *cStr) {
    std::map<std::string, std::string> result;

    std::istringstream stream(cStr);
    std::string parameter;
    while (getline(stream, parameter, '&')) {
        std::istringstream paramStream(parameter);
        std::string key;
        std::string value;
        getline(paramStream, key, '=');
        getline(paramStream, value);
        result.insert(std::make_pair(key, value));
    }

    return result;
}

std::map<std::string, std::string> get_decoded_parameters(const std::string &query) {
    CURL * curl = curl_easy_init();
    char * output = curl_easy_unescape(curl, query.c_str(), query.length(), nullptr);
    std::map<std::string, std::string> values = tokenize(output);
    curl_free(output);
    curl_easy_cleanup(curl);
    return values;
}

bool has_element(const std::map<std::string, std::string> & values, const std::string & element) {
    return values.find(element) != values.cend();
}

void set_params(const Config &cfg, const std::map<std::string, std::string> &params) {
    cfg.lookup("server.timeout") = params.at("server.timeout");
    cfg.lookup("server.port") = params.at("server.port");
    cfg.lookup("directory.path") = params.at("directory.path");
    cfg.lookup("database.filename") = params.at("database.filename");
    cfg.lookup("springer.apikey") = params.at("springer.apikey");
    cfg.lookup("scopus.apikey") = params.at("scopus.apikey");
    cfg.lookup("sciencedirect.apikey") = params.at("sciencedirect.apikey");
    cfg.lookup("dblp.enabled") = has_element(params, "dblp.enabled");
    cfg.lookup("arxiv.enabled") = has_element(params, "arxiv.enabled");
    cfg.lookup("nature.enabled") = has_element(params, "nature.enabled");
    cfg.lookup("springer.enabled") = has_element(params, "springer.enabled");
    cfg.lookup("scopus.enabled") = has_element(params, "scopus.enabled");
    cfg.lookup("sciencedirect.enabled") = has_element(params, "sciencedirect.enabled");
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
    std::string directoryPath = Config::get_instance().lookup("directory.path");
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
    start_print_html(out_html);
    print_res_table_html(out_html, result);
    end_print_html(out_html);

    return out_html.str();
}

void BiblioServer::update_db_thread_function() {
    BiblioServer &instance = BiblioServer::get_instance();
    int timeout = std::stoi(Config::get_instance().lookup("server.timeout"));
    while (true) {
        std::unique_lock<std::mutex> lock_to_wait(instance.is_rescan);
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

        Config &cfg = Config::get_instance();

        if (uri == "/") {
            std::string out_html;
            mg_send_head(conn, 200, -1, "");

            if (query == "rescan") {
                instance.rescan_cond_var.notify_one();
            } else {
                std::unique_lock<std::mutex> lock(instance.m_content);
                out_html = instance.get_content_copy();
                mg_printf_http_chunk(conn, "%s", out_html.c_str());
            }
            mg_send_http_chunk(conn, "", 0);
        } else if (uri == "/settings") {
            if (query != "") {
                std::map<std::string, std::string> values = get_decoded_parameters(query);
                if (!file_exists(values["directory.path"])) {
                    values["directory.path"] = cfg.lookup("directory.path").c_str();
                }
                set_params(cfg, values);
                cfg.save_to_file();
            }
            char buf[4096];
            sprintf(buf, instance.settings.c_str(),
                    cfg.lookup("server.timeout").c_str(), cfg.lookup("server.port").c_str(),
                    cfg.lookup("directory.path").c_str(), cfg.lookup("database.filename").c_str(),
                    cfg.lookup("dblp.enabled") ? "checked" : "",
                    cfg.lookup("arxiv.enabled") ? "checked" : "",
                    cfg.lookup("nature.enabled") ? "checked" : "",
                    cfg.lookup("springer.enabled") ? "checked" : "", cfg.lookup("springer.apikey").c_str(),
                    cfg.lookup("sciencedirect.enabled") ? "checked" : "", cfg.lookup("sciencedirect.apikey").c_str(),
                    cfg.lookup("scopus.enabled") ? "checked" : "", cfg.lookup("scopus.apikey").c_str());

            mg_send_head(conn, 200, -1, "");
            mg_printf_http_chunk(conn, "%s", buf);
            mg_send_http_chunk(conn, "", 0);

        } else if(uri == "/search") {
            std::vector<ArticleInfo> search_result;
            std::stringstream out_html;
            start_print_search_html(out_html, query);
            print_res_table_html(out_html,search_result);
            end_print_html(out_html);

            mg_send_head(conn, 200, -1, "");
            mg_printf_http_chunk(conn, "%s", out_html.str().c_str());
            mg_send_http_chunk(conn, "", 0);

        }else if (uri == "/read") {
            int query_len = query.length();
            char filename[query_len + 1];
            mg_url_decode(query.c_str(), query_len, filename, query_len + 1, 0);
            mg_http_serve_file(conn, hm, filename, mg_mk_str("application/pdf"), mg_mk_str(""));
        } else {
            mg_http_serve_file(conn, hm, uri.c_str()+1, mg_mk_str("image/png"), mg_mk_str(""));
        }

    }
}

void BiblioServer::start_server() {
    const char *http_port = Config::get_instance().lookup("server.port");
    struct mg_mgr mgr;
    struct mg_connection *conn;

    const char *path = Config::get_instance().lookup("directory.path");
    std::stringstream log_message;
    log_message << "scanning folder " << path;
    log(log_message.str().c_str());

    BiblioServer::get_instance().content = rescan_and_get_content();
    std::ifstream ifs("settings.html");
    BiblioServer::get_instance().settings = std::string((std::istreambuf_iterator<char>(ifs)),
                                                        (std::istreambuf_iterator<char>()));

    mg_mgr_init(&mgr, NULL);
    conn = mg_bind(&mgr, http_port, ev_handler);
    mg_set_protocol_http_websocket(conn);

    log("server started");

    std::thread thread_updating_db(update_db_thread_function);

    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);

}
