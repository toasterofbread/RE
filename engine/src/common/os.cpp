#include "core/os.h"

#include "node/node.h"

list<any> db_print_stack;
void OS::dbPrintAny(any message) {
    db_print_stack.push_back(message);
}
void OS::dbPrint(string message) {
    dbPrintAny(message);
}

list<any>* OS::getDbPrintStack() { 
    return &db_print_stack;
}

void OS::resetDbPrint() {
    db_print_stack.clear();
}

void OS::print(Vector2 msg, int type) {
    print(msg.toString(), type);
}
void OS::print(Vector3 msg, int type) {
    print(msg.toString(), type);
}
void OS::print(double msg, int type) {
    print(to_string(msg), type);
}
void OS::print(float msg, int type) {
    print(to_string(msg), type);
}
void OS::print(int msg, int type) {
    print(to_string(msg), type);
}
void OS::print(Node* msg, int type) {
    print(msg->operator string(), type);
}

Vector2 OS::getScreenSize() {
    return Vector2(OS::getScreenWidth(), OS::getScreenHeight());
}

float OS::getAspectRatio() {
    return (float)OS::getScreenWidth() / (float)OS::getScreenHeight();
}

double last_frame_step = 0.0;

void OS::frameStep(string name) {
    OS::dbPrint(name + " | " + to_string(OS::getTime() - last_frame_step));
    last_frame_step = OS::getTime();
}

void OS::resetFrameStep() {

}