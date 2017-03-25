#include "mongoose.h"
#include <map>
#include <string>
#include <sstream>

static const char *s_http_port = "8000";

std::map<std::string, std::string> tokenize(const char* cStr) {
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

static void ev_handler(struct mg_connection *conn, int ev, void *p) {
  if (ev == MG_EV_HTTP_REQUEST) {
    struct http_message *hm = (struct http_message *) p;

    // We have received an HTTP request. Parsed request is contained in `hm`.
    // Send HTTP reply to the client which shows full original request.
		char msg[] = "<html><body>Hello</body></html>";
    mg_send_head(conn, 200, sizeof(msg), "Content-Type: text/plain");
    //mg_printf(conn, "%.*s", hm->message.len, hm->message.p);



    mg_printf(conn, "%s", msg);
    //mg_printf(conn, "%.*s", hm->query_string.len, hm->query_string.p);
    
    /*std::map<std::string, std::string> map = tokenize(hm->query_string.p);
    for (auto it = map.begin(); it != map.end(); ++it)
    {
      mg_printf(conn, "%.*s", it->first.size(), it->first);
      mg_printf(conn, "\t", 1);
      mg_printf(conn, "%.*s", it->second.size(), it->second);
      mg_printf(conn, "\n", 1);
    }*/
  }
}

int main(void) {
  struct mg_mgr mgr;
  struct mg_connection *conn;

  mg_mgr_init(&mgr, NULL);
  conn = mg_bind(&mgr, s_http_port, ev_handler);
  mg_set_protocol_http_websocket(conn);

  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

  return 0;
}
