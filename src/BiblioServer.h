#include "../lib/mongoose/mongoose.h"

class BiblioServer {
public:
	void startServer();
	static void ev_handler(mg_connection *conn, int event, void * data);
};