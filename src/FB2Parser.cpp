#include "FB2Parser.h"
#include "BiblioException.h"
#include "tools.h"

struct Author {
    std::string first_name;
    std::string middle_name;
    std::string last_name;
};

struct Info {
    std::vector<Author> authors;
    std::string title;
};

struct State {
    bool inside_title_info = false;
    bool inside_author = false;
    bool inside_title = false;
    bool inside_first_name = false;
    bool inside_middle_name = false;
    bool inside_last_name = false;
    bool finished_description = false;
};

struct ParsingState {
    Info info;
    State state;
};

void FB2Parser::start(void *data, const char *el, const char **attr) {

    ParsingState *parsing_state_ptr = static_cast<ParsingState*>(data);

    if (!strcmp(el, "title-info")) {
        parsing_state_ptr->state.inside_title_info = true;
    }
    if (!strcmp(el, "author") && parsing_state_ptr->state.inside_title_info) {
        parsing_state_ptr->state.inside_author = true;
        parsing_state_ptr->info.authors.push_back(Author());
    }
    if (!strcmp(el, "first-name")) {
        parsing_state_ptr->state.inside_first_name = true;
    }
    if (!strcmp(el, "middle-name")) {
        parsing_state_ptr->state.inside_middle_name = true;
    }
    if (!strcmp(el, "last-name")) {
        parsing_state_ptr->state.inside_last_name = true;
    }
    if (!strcmp(el, "book-title")) {
        parsing_state_ptr->state.inside_title = true;
    }
}

void FB2Parser::end(void *data, const char *el) {
    ParsingState *parsing_state_ptr = static_cast<ParsingState*>(data);
    if (!strcmp(el, "title-info")) {
        parsing_state_ptr->state.inside_title_info = false;
    }
    if (!strcmp(el, "author") && parsing_state_ptr->state.inside_title_info) {
        parsing_state_ptr->state.inside_author = false;
    }
    if (!strcmp(el, "book-title")) {
        parsing_state_ptr->state.inside_title = false;
    }

    if (!strcmp(el, "first-name")) {
        parsing_state_ptr->state.inside_first_name = false;
    }
    if (!strcmp(el, "middle-name")) {
        parsing_state_ptr->state.inside_middle_name = false;
    }
    if (!strcmp(el, "last-name")) {
        parsing_state_ptr->state.inside_last_name = false;
    }
    if (!strcmp(el, "description")) {
        parsing_state_ptr->state.finished_description = true;
    }
}

void FB2Parser::text_saver (void * userData, const XML_Char * s, int length) {
    ParsingState *parsing_state_ptr = static_cast<ParsingState*>(userData);
    if (parsing_state_ptr->state.inside_title_info) {
        if ( parsing_state_ptr->state.inside_author) {
            if (parsing_state_ptr->state.inside_first_name) {
                parsing_state_ptr->info.authors.back().first_name = std::string(s, length);
            }
            if (parsing_state_ptr->state.inside_middle_name) {
                parsing_state_ptr->info.authors.back().middle_name = std::string(s, length);
            }
            if (parsing_state_ptr->state.inside_last_name) {
                parsing_state_ptr->info.authors.back().last_name = std::string(s, length);
            }
        }

        if (parsing_state_ptr->state.inside_title_info && parsing_state_ptr->state.inside_title) {
            parsing_state_ptr->info.title = std::string(s, length);
        }
    }
}

ArticleInfo FB2Parser::parse(const std::string &filename) {

    XML_Parser p = XML_ParserCreate(NULL);
    if (!p) {
        throw BiblioException("Couldn't allocate memory for FB2Parser");
    }

    XML_SetElementHandler(p, start, end);
    ParsingState parsing_state;
    XML_SetUserData(p, &parsing_state);
    XML_SetCharacterDataHandler(p, text_saver);


    FILE * file = fopen(filename.c_str(), "r");
    if (!file) {
        throw BiblioException(std::string("Failed to open file ") += filename);
    }

    int done;
    int len;
    char buff[BUFFSIZE];

    do {
        len = fread(buff, 1, BUFFSIZE, file);
        if (ferror(file)) {
            throw BiblioException(std::string("Failed reading from file ") += filename);
        }
        done = feof(file);

        if (! XML_Parse(p, buff, len, done)) {
            throw BiblioException(std::string("Failed parsing file ") += filename);
        }
    } while (!done || !parsing_state.state.finished_description);

    fclose(file);
    XML_ParserFree(p);

    std::vector<std::string> full_name_authors;
    for (Author author : parsing_state.info.authors) {
        full_name_authors.push_back(join_without_empty({author.first_name,
                                                        author.middle_name, author.last_name}, " "));
    }

    return ArticleInfo(parsing_state.info.title, full_name_authors, filename);

}