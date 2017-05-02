#include "../lib/mongoose/mongoose.h"
#include "Config.h"
#include <string>
#include <mutex>
#include <condition_variable>

class BiblioServer { //singleton
private:
    std::string content;
    std::string settings;
    std::mutex m_content;
    std::mutex is_rescan;
    std::condition_variable rescan_cond_var;

    BiblioServer();
    BiblioServer(BiblioServer const &);
    BiblioServer& operator= (BiblioServer const &);

    static void ev_handler(mg_connection *conn, int event, void * data);
    static void update_db_thread_function();
    static std::string rescan_and_get_content();

public:

    static BiblioServer & get_instance();
    std::string get_content_copy();

    void start_server();
};