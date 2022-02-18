#include <string.h>
#include "cstring"
#include <stdio.h>
#include <execinfo.h>

#include "engine/src/utils.h"
#include "engine/src/core/node/node.h"
#include "include/backward.hpp"

using namespace std;

void markPosition(Vector2 position, string text, Color colour, float radius, float width) {
    DrawLineEx(sV(position, Vector2{radius, 0}), aV(position, Vector2{radius, 0}), 1.0f, colour);
    DrawLineEx(sV(position, Vector2{0, radius}), aV(position, Vector2{0, radius}), 1.0f, colour);
    DrawText(text.c_str(), position.x + 5, position.y - 15, 10, colour);
}

Vector2 sign(Vector2 value) {
    return Vector2{sign(value.x), sign(value.y)};
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
    if (path.back() != '/') {
        path.append("/");
    }
    if (file_to_add.front() == '/') {
        path.erase(0);
    }
    path.append(file_to_add);

    // char* ret = (char*)calloc(path.length() + 1, sizeof(char));
    // strcpy(ret, path.c_str());

    return path;
}
string getResPath(string absolute_path) {
    return plusFile("/home/spectre7/Projects/raylib/SSG/", absolute_path);
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

int getStringActualLength(string str) {
    for (int i = 0; i < long_characters.size(); i++) {
        print(long_characters[i].size());
    }
    return str.length();
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

string vector2str(Vector2 value) {

    string s0{"Vector2{ "}, s1{", "}, s2{" }"};
    string sink;

    char bufferx[64];
    char buffery[64];
    sprintf(bufferx, "%f", value.x);
    sprintf(buffery, "%f", value.y);

    string bx{bufferx};
    string by{buffery};

    sink.reserve(s0.size() + s1.size() + s2.size() + 1 + bx.size() + by.size());
    sink += s0;
    sink += bx;
    sink += s1;
    sink += by;
    sink += s2;

    return sink;
}

const char* int2char(int value) {
    char* ret = (char*)calloc(100, sizeof(char));
    sprintf(ret, "%d", value);
    return ret;
}
const char* string2char(string value) {
    char* ret = (char*)calloc(value.length() + 1, sizeof(char));
    strcpy(ret, value.c_str());
    return ret;
}

Vector2 aV(Vector2 a, Vector2 b) {
    return Vector2{a.x+b.x, a.y+b.y};
}
Vector2 sV(Vector2 a, Vector2 b) {
    return Vector2{a.x-b.x, a.y-b.y};
}
Vector2 mV(Vector2 a, Vector2 b) {
    return Vector2{a.x*b.x, a.y*b.y};
}
Vector2 mV(Vector2 a, float b) {
    return Vector2{a.x*b, a.y*b};
}
Vector2 mV(Vector2 a, int b) {
    return Vector2{a.x*b, a.y*b};
}
Vector2 dV(Vector2 a, Vector2 b) {
    return Vector2{a.x/b.x, a.y/b.y};
}
Vector2 dV(Vector2 a, float b) {
    return Vector2{a.x/b, a.y/b};
}
Vector2 dV(Vector2 a, int b) {
    return Vector2{a.x/b, a.y/b};
}
bool cV(Vector2 a, Vector2 b) {
    return (a.x == b.x && a.y == b.y);
}

void printNode(Node* value) {
    print(value->getTypeName() + " | ID: " + int2char(value->getId()));
}
void print(Vector2 value) {
    cout << vector2str(value) << endl;
}
void print(bool value) {
    cout << (value ? "true" : "false") << endl;
}

void print_trace(void) {
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
}

void stacker(int additional_skip) {
    using namespace backward;
    StackTrace st;

    st.load_here(9); //Limit the number of trace depth to 99
    st.skip_n_firsts(3 + additional_skip);//This will skip some backward internal function from the trace

    Printer p;
    p.snippet = true;
    p.object = true;
    p.address = true;
    p.print(st, stderr);
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
        stacker(1);
        throw runtime_error(format_message(message, "ERROR"));
    }
    else {
        print(format_message(message, "WARNING"));
    }
}
