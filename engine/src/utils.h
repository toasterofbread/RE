#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <thread>
#include <functional>
#include <raylib-cpp.hpp>
using namespace std;

#include "include/prettyprint.hpp"

// Forward declaration
class Node;

void markPosition(Vector2 position, std::string text, Color colour = RED, float radius = 10.0f, float width = 1.0f);

template <typename Any>
float sign(Any value) {
    return (value > 0.0f) - (value < 0.0f);
}
Vector2 sign(Vector2 value);

vector<string> splitString(string str, char splitter);
const char* plusFile(const char* path, const char* file_to_add);
std::string plusFile(std::string path, std::string file_to_add);
std::string getResPath(std::string absolute_path);

constexpr unsigned int str2int(const char* str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}
int getStringWidth(string str);
const vector<string> long_characters = {
    "─", "━", "│", "┃",
    "┌", "┐", "┏", "┓",
    "└", "┘", "┗", "┛",
    "├", "┤", "┣", "┫"
};
int getStringActualLength(string str);
string equaliseStringLineWidths(string str);
bool stringBeginsWith(string str, string begins);
string vector2str(Vector2 value);
const char* int2char(int value);
const char* string2char(std::string value);
string encaseStringInBox(string str, bool thick = false, int margin = 2);
string repeatString(string str, int amount);

template <typename Any>
int vectorFindValue(vector<Any>* vector, Any value) {
    int ret = -1;
    for (auto i = vector->begin(); i != vector->end(); ++i) {
        ret++;
        if (*i == value) {
            break;
        }
    }
    return ret;
}
template <typename Any>
bool vectorContainsValue(std::vector<Any>* vector, Any value) {
    return std::find(vector->begin(), vector->end(), value) != vector->end();
}
template <typename Any>
void vectorRemoveValue(std::vector<Any>* vector, Any value) {
    vector->erase(std::remove(vector->begin(), vector->end(), value), vector->end());
}

// Add vector2 ( + )
Vector2 aV(Vector2 a, Vector2 b);
// Subtract vector2 ( - )
Vector2 sV(Vector2 a, Vector2 b);
// Multiply vector2 ( * )
Vector2 mV(Vector2 a, Vector2 b);
Vector2 mV(Vector2 a, float b);
Vector2 mV(Vector2 a, int b);
// Divide vector2 ( / )
Vector2 dV(Vector2 a, Vector2 b);
Vector2 dV(Vector2 a, float b);
Vector2 dV(Vector2 a, int b);
// Compare vector2 ( == )
bool cV(Vector2 a, Vector2 b);

void printNode(Node* value);
template <typename Any>
void print(Any msg) {
    cout << msg << endl; 
}
void print(Vector2 value);
void print(bool value);

void stacker(int additional_skip = 0);

void warn(string message, bool throw_error = false);



//   template<typename _Func, typename... _BoundArgs>
//     inline _GLIBCXX20_CONSTEXPR typename
//     _Bind_helper<__is_socketlike<_Func>::value, _Func, _BoundArgs...>::type
//     bind(_Func&& __f, _BoundArgs&&... __args)

template<typename F, typename O, typename... A>
void makeThread(F function, O* object, A... args) {
    thread(bind(function, object, args...), args...).detach();
}

template<class ObjectType, class ReturnType, class... ArgumentTypes>
struct Functor {

    using TMemberFunction = ReturnType (ObjectType::*)(ArgumentTypes...);
    TMemberFunction ptr;
    ObjectType     *object;


    ReturnType operator()(ArgumentTypes... params) {
        return (object->*ptr)(params...);
    }
};