#ifndef INCLUDED_UTILS
#define INCLUDED_UTILS

#include <box2d/box2d.h>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <thread>
#include <functional>
using namespace std;

#include "include/prettyprint.hpp"
#include "common/colour.h"
#include "common/vector2.h"
#include "core/os.h"

// !todo Clean this file up

// Forward declaration
class Node;

#define TYPE_COUNT(types) sizeof...(types)

#ifdef DEG2RAD
#undef DEG2RAD
#endif
#define DEG2RAD(x) ((PI/180.0f) * (x))

#ifdef RAD2DEG
#undef RAD2DEG
#endif
#define RAD2DEG(x) ((180.0f/PI) * (x))

void markPosition(Vector2 position, string text, Colour colour = Colour::RED(), float radius = 10.0f, float width = 1.0f);
void markPosition(int layer, Vector2 position, string text, Colour colour = Colour::RED(), float radius = 10.0f, float width = 1.0f);

template <typename Any>
int sign(Any value) {
    if (value == 0.0f) {
        return 0;
    }
    else if (value > 0.0f) {
        return 1;
    }
    else {
        return -1;
    }
}
Vector2 sign(Vector2 value);

bool stringBeginsWith(string str, string match);

vector<string> splitString(string str, char splitter);
const char* plusFile(const char* path, const char* file_to_add);
string plusFile(string path, string file_to_add);

string strVector2str(vector<string> vector, string splitter = ", ");
string stringPadDecimals(string str, int max_decimals);

template <typename Any>
bool vectorContainsValue(vector<Any>& vector, Any value) {
    for (Any& item : vector) {
        if (item == value) {
            return true;
        }
    }
    return false;
}
template <typename Any>
void vectorRemoveValue(vector<Any>* vector, Any value) {
    vector->erase(remove(vector->begin(), vector->end(), value), vector->end());
}

void print_stacktrace(int additional_skip = 0);

#undef assert

#if DEBUG_ENABLED
#define ASSERT_MSG(expr, msg)						                   \
if (!static_cast<bool>(expr)) {                                        \
    OS::print("Assertion failed at " + string(__FILE__) + ":" + to_string(__LINE__) + " with expression \"" + string(#expr) + "\" | " + string(msg), 3);             \
    print_stacktrace();                                                \
    throw exception(); \
}
#define ASSERT(expr) ASSERT_MSG(expr, "(no message provided)")
#define WARN_IF(cond, msg) \
if (static_cast<bool>(cond)) { \
    OS::print("Warning at " + string(__FILE__) + ":" + to_string(__LINE__) + " with condition  \"" + string(#cond) + "\" | " + string(msg), 2); \
}

#else
#define ASSERT_MSG(expr, msg)
#define ASSERT(expr)
#define WARN_IP(cond, msg)
#endif

string formatPath(string& path);

template<typename Type = void, typename... Tail>
void _addTypes(vector<string>& list) {
    list.push_back(typeid(Type).name());
    if (TYPE_COUNT(Tail) > 0) {
        _addTypes<Tail...>(list);
    }
}

template<typename Type, typename... Tail>
vector<string> getTypeList() {
    vector<string> ret = {typeid(Type).name()};
    if (TYPE_COUNT(Tail) > 0) {
        _addTypes<Tail...>(ret);
    }
    return ret;
}

#endif