#include "core/os.h"

#include "node/node.h"

void OS::print(Vector2 msg, int type) {
    print(msg.toString(), type);
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