#include <string.h>
#include "cstring"
#include <stdio.h>
#include <box2d/box2d.h>

#include "common/utils.h"
#include "common/draw.h"
#include "engine/src/core/node/node.h"

#if DEBUG_ENABLED
#include <execinfo.h>
#include "include/backward.hpp"
#endif

#if PLATFORM == PLATFORM_RAYLIB
#include "raylib_include.h"
#endif

using namespace std;

void markPosition(Vector2 position, string text, Colour colour, float radius, float width) {
    Draw::drawLine(position - Vector2{radius, 0}, position + Vector2{radius, 0}, colour, Draw::DRAW_MODE::WORLD);
    Draw::drawLine(position - Vector2{0, radius}, position + Vector2{0, radius}, colour, Draw::DRAW_MODE::WORLD);
    Draw::drawText(text.c_str(), position.x + 5, position.y - 15, colour, 1.0f, Draw::DRAW_MODE::WORLD);
}

Vector2 sign(Vector2 value) {
    return Vector2{sign(value.x), sign(value.y)};
}

string wrapString(string str, int line_chars) {

    string ret = "";

    int current_line = 0;
    for (int i = 0; i < str.length(); i++) {
        ret += str[i];
        current_line++;
        if (current_line == line_chars) {
            ret += "\n";
            current_line = 0;
        }
    }

    return ret;

}

vector<string> splitString(string str, char splitter) {
    vector<string> result;
    string current = ""; 
    for(int i = 0; i < str.size(); i++){
        if(str[i] == splitter){
            if(current != ""){
                result.push_back(current);
                current = "";
            } 
            continue;
        }
        current += str[i];
    }
    if(current.size() != 0)
        result.push_back(current);
    return result;
}


const char* plusFile(const char* path, const char* file_to_add) {
    string pathstr(path);
    if (pathstr.back() != '/') {
        pathstr.append("/");
    }
    string addstr(file_to_add);
    if (addstr.front() == '/') {
        pathstr.erase(0);
    }
    pathstr.append(addstr);

    char* ret = (char*)calloc(pathstr.length() + 1, sizeof(char));
    strcpy(ret, pathstr.c_str());

    return ret;
}
string plusFile(string path, string file_to_add) {
    if (path.back() == '/') {
        if (file_to_add.front() == '/') {
            path.pop_back();
        }
    }
    else if (file_to_add.front() != '/') {
        path.append("/");
    }
    path.append(file_to_add);
    return path;
}

string encaseStringInBox(string str, bool thick, int margin) {
    if (margin < 0) {
        margin = 0;
    }

    const string vertical = thick ? "┃" : "│";
    const string horizontal = thick ? "━" : "─";
    
    if (str[-1] != '\n') {
        str += "\n";
    }

    str.insert(0, vertical + repeatString(" ", margin));
    for (int i = 0; i+1 < str.length(); i++) {
        if (str[i] == '\n') {
            str.insert(i + 1, vertical + repeatString(" ", margin));
        }
    }

    str = equaliseStringLineWidths(str);

    str.insert(str.length(), repeatString(" ", margin));
    int last = -1;
    for (int i = 0; i+1 < str.length(); i++) {
        if (str[i] == '\n') {
            str.insert(i, repeatString(" ", margin) + vertical);
            i += margin + vertical.length();
            last = i;
        }
    }

    const string line = repeatString(horizontal, getStringWidth(str) - ((string)"┏┓").length());


    if (last >= 0) {
        str.insert(last + 1, thick ? ("┗" + line + "┛") : ("└" + line + "┘"));
    }
    else {
        str.insert(str.length(), (thick ? ("┗" + line + "┛") : ("└" + line + "┘")));
    }

    if (thick) {
        str = "┏" + line + "┓\n" + str;// + "┗" + line + "┛";
    }
    else {
        str = "┌" + line + "┐\n" + str;// + "└" + line + "┘";
    }

    return str;
}
string repeatString(string str, int amount) {
    if (amount < 0) {
        return "";
    }
    string original = str;
    while (amount > 1) {
        amount--;
        str += original;
    }
    return str;
}
string strVector2str(vector<string> vector, string splitter) {
    string ret;

    for (int i = 0; i < vector.size(); i++) {
        ret += vector[i];
        if (i + 1 < vector.size()) {
            ret += splitter;
        }
    }

    return ret;
}
string concatChars(const char* A, const char* B) {
    return (string(A) + string(B));
}

string stringPadDecimals(string str, int max_decimals) {
    int decimal_point = str.find(".");
    if (decimal_point >= 0) {
        str.erase(decimal_point + max_decimals + 1, str.size() - decimal_point - max_decimals);
    }
    return str;
}

int getStringWidth(string str) {
    int max_length = 0;
    int current_length = 0;
    // string current_sequence;

    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '\n') {
            if (current_length > max_length) max_length = current_length;
            current_length = 0;
            // current_sequence.clear();
        }
        else {
            current_length++;
            // current_sequence += str[i];

            // for (auto i = long_characters.begin(); i != long_characters.end(); ++i) {
            //     if (*i == current_sequence) {
            //         current_length -= i->length() - 1;
            //         current_sequence.clear();
            //         print("CLIP");
            //         break;
            //     }
            // }
        }
    }

    return max_length;
}

string equaliseStringLineWidths(string str) {

    int max_width = getStringWidth(str);

    int current_width = 0;
    for (int i = 0; i < str.length(); i++) {
        if (str[i] == '\n' || i + 1 == str.length()) {

            if (i + 1 == str.length()) {
                i++;
                current_width++;
            }
            
            while (current_width < max_width) {
                str.insert(i, " ");
                current_width++;
                i++;
            }
            current_width = 0;
        }
        else {
            current_width++;
        }
    }

    return str;
}

bool stringBeginsWith(string str, string begins) {
    if (begins.empty() || str.empty() || str.size() < begins.size()) {
        return false;
    }
    return str.compare(begins) == (str.size() - begins.size());
}

string vector2str(Vector2 value, int max_decimals, bool decorate) {

    assert(max_decimals >= 0);
    if (max_decimals == 0) {
        max_decimals = -1;
    }

    string s0{decorate ? "Vector2{ " : "{ "}, s1{", "}, s2{" }"};
    string sink;

    char bufferx[64];
    char buffery[64];
    sprintf(bufferx, "%f", value.x);
    sprintf(buffery, "%f", value.y);

    string bx{bufferx};
    string by{buffery};

    int decimal_point = bx.find(".");
    if (decimal_point >= 0) {
        bx.erase(decimal_point + max_decimals + 1, bx.size() - decimal_point - max_decimals);
    }

    decimal_point = by.find(".");
    if (decimal_point >= 0) {
        by.erase(decimal_point + max_decimals + 1, by.size() - decimal_point - max_decimals);
    }

    sink.reserve(s0.size() + s1.size() + s2.size() + 1 + bx.size() + by.size());
    sink += s0;
    sink += bx;
    sink += s1;
    sink += by;
    sink += s2;

    return sink;
}

string vector2str(b2Vec2 value, int max_decimals, bool decorate) {
    return vector2str(convertVector2(value), max_decimals, decorate);
}

const char* int2char(int value) {
    char* ret = (char*)calloc(100, sizeof(char));
    sprintf(ret, "%d", value);
    return ret;
}

// void print(Node* value) {
//     print(value->operator string());
// }
// void print(Vector2 value) {
//     cout << vector2str(value) << endl;
// }
// void print(b2Vec2 value) {
//     cout << vector2str(value) << endl;
// }
// void print(bool value) {
//     cout << (value ? "true" : "false") << endl;
// }
// void print(Colour value) {
//     cout << "(" << to_string(value.r) << ", " << to_string(value.g) << ", " << to_string(value.b) << ", " << to_string(value.a) << ")" << endl;
// }

void print_trace(void) {
    #if DEBUG_ENABLED
    char **strings;
    size_t i, size;
    enum Constexpr { MAX_SIZE = 1024 };
    void *array[MAX_SIZE];
    size = backtrace(array, MAX_SIZE);
    strings = backtrace_symbols(array, size);
    for (i = 0; i < size; i++)
        printf("%s\n", strings[i]);
    puts("");
    free(strings);
    #endif
}

void print_stacktrace(int additional_skip) {
    #if DEBUG_ENABLED
    using namespace backward;
    StackTrace st;

    st.load_here(9); //Limit the number of trace depth to 99
    st.skip_n_firsts(3 + additional_skip);//This will skip some backward internal function from the trace

    Printer p;
    p.snippet = true;
    p.object = true;
    p.address = true;
    p.print(st, stderr);
    #endif
}

void warn(string message, bool throw_error) {

    auto format_message = [](string msg, string prefix) {

        const string splitter = " | ";

        string blank_prefix;
        for (int i = 0; i < prefix.length(); i++) {
            blank_prefix += " ";
        }
        blank_prefix += splitter;

        msg.insert(0, prefix + splitter);

        vector<int> newline_positions;
        int max_length = 0;
        int current_length = 0;
        for (int i = 0; i < msg.length(); i++) {
            current_length++;
            if (msg[i] == '\n') {
                newline_positions.push_back(i);

                if (current_length > max_length) {
                    max_length = current_length;
                }
                current_length = 0;
            }
        }

        int offset = 0;
        for (auto i = newline_positions.begin(); i != newline_positions.end(); ++i) {
            msg.insert((*i) + offset + 1, blank_prefix);
            offset += blank_prefix.length();
        }

        // return encaseStringInBox(msg, false);
        return msg;
    };

    if (throw_error) {
        if (PLATFORM == PLATFORM_VITA) {
            Engine::fatal_error_occurred = true;
            Engine::fatal_error = wrapString(message, 75);
        }
        else {
            print_stacktrace(1);
            throw runtime_error(format_message(message, "ERROR"));
        }
    }
    else {
        OS::print(format_message(message, "WARNING"));
    }
}

Vector2 convertVector2(b2Vec2 value) {
    return Vector2{value.x, value.y};
}

b2Vec2 convertVector2(Vector2 value) {
    return b2Vec2(value.x, value.y);
}

string formatPath(string& path) {
    if (path.back() == '/') {
        path.erase(path.back());
    }
    if (path.front() != '/') {
        path = "/" + path;
    }
    return path;
}
