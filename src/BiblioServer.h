#include "../lib/mongoose/mongoose.h"
#include <string>

class BiblioServer { //singleton
private:
    std::string content;

    BiblioServer();
    BiblioServer(BiblioServer const &);
    BiblioServer& operator= (BiblioServer const &);

    static void ev_handler(mg_connection *conn, int event, void * data);
public:
    static BiblioServer & get_instance();

    std::string& get_content();
    void startServer();

};