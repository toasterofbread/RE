#include "raylib-cpp.hpp"
#include "core/node.h"

class Resource {
    protected:
        Node* host;
    public:
        Resource(Node* host_node);
};