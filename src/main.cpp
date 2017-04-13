#include <iostream>
#include <algorithm>

#include <tclap/CmdLine.h>
#include <unistd.h>

#include "Requesters/Requester.h"
#include "BiblioManager.h"
#include "BiblioServer.h"
#include "BiblioThreadContext.h"

using namespace std;

vector<string> get_filenames(TCLAP::UnlabeledMultiArg<string> &files, TCLAP::MultiArg<string> &directories);
void print_bib_html(const BiblioManager &manager, const vector<ArticleInfo> &result);

int main(int argc, char **argv) {
    try {
        TCLAP::CmdLine cmd("This util will generate .bib files for your articles in PDF format.", ' ', "0.1");
        TCLAP::SwitchArg is_offline_switch("f", "offline", "Does only offline part.", cmd, false);
        TCLAP::SwitchArg purge_switch("p", "purge", "Purges database from non-existent files.", false);
        TCLAP::SwitchArg using_db_switch("b", "database", "Disable database", cmd, true);
        TCLAP::SwitchArg start_server_switch("s", "start_server", "Starts embedded web-server to use biblio in the browser", false);
        TCLAP::UnlabeledMultiArg<string> files("files", "file names", true, "files");
        TCLAP::MultiArg<string> directories("d", "directory", "directories for recursive search of PDF documents", true, "path");
        TCLAP::ValueArg<string> config_file("c", "config", "name of the config file", false, "biblio.cfg", "filename");
        cmd.add(&config_file);
        vector<TCLAP::Arg *> xorlist;
        xorlist.push_back(&purge_switch);
        xorlist.push_back(&files);
        xorlist.push_back(&directories);
        xorlist.push_back(&start_server_switch);
        cmd.xorAdd(xorlist);
        cmd.parse(argc, (const char *const *) argv);


        if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
            throw BiblioException("Curl global init failed.\n");
        }

        Config::init(config_file.getValue());

        Database *db = nullptr;
        if (using_db_switch.getValue()) {
            db = Database::connect_database();
        }


        if (purge_switch.isSet()) {
            if (db != nullptr) {
                db->purge();
            }
        } else if (start_server_switch.isSet()) {
            BiblioServer::get_instance().start_server();
        } else {
            int threads = sysconf(_SC_NPROCESSORS_ONLN);
            BiblioManager manager(threads);

            vector<string> filenames = get_filenames(files, directories);
            try {
                vector<ArticleInfo> result = manager.get_info(filenames,db, is_offline_switch.getValue());

                print_bib_html(manager, result);

            } catch (const BiblioException &e) {
                cerr << e.what() << '\n';
            }
        }

        if (db != nullptr) {
            delete db;
        }
    }
    catch (TCLAP::ArgException &e) {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }

    return 0;
}

vector<string> get_filenames(TCLAP::UnlabeledMultiArg<string> &files, TCLAP::MultiArg<string> &directories) {
    vector<string> filenames;

    if (files.isSet()) {
        filenames = files.getValue();
    }

    if (directories.isSet()) {
        vector<string> dirs = directories.getValue();
        vector<string> files_in_dir = {};
        for (string dir : dirs) {
            files_in_dir = read_pdf_files_recursive(dir);
            filenames.insert(filenames.end(), files_in_dir.begin(), files_in_dir.end());
        }
    }

    size_t filenames_size = filenames.size();
    for (size_t i = 0; i < filenames_size; i++) {
        filenames[i] = get_absolute_path(filenames[i]);
    }
    return filenames;
}

void print_bib_html(const BiblioManager &manager, const vector<ArticleInfo> &result) {
    ofstream out_bib("biblio.bib");
    manager.print_bib(out_bib, result);
    out_bib.close();

    ofstream out_html("biblio.html");
    BiblioManager::start_print_html(out_html);
    manager.print_html(out_html, result);
    BiblioManager::end_print_html(out_html);
    out_html.close();

}