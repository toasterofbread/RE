#include "node.h"
#include "utils.h"
#include "vector"
#include "icecream.hpp"

static int INDICATOR_RADIUS = 10;

// - Core -
void Node::init(Node* root_node, Node* parent_node, NodeManagerParent* node_manager) {
    root = root_node;
    is_root = root_node == this;
    parent = parent_node;
    manager = node_manager;
    id = manager->getNewId();

    initialised = true;
}
void Node::process(float delta) {
    if (show_gizmos) {
        Vector2 global_position = getGlobalPosition(true);
        DrawLine(global_position.x - INDICATOR_RADIUS, global_position.y, global_position.x + INDICATOR_RADIUS, global_position.y, RED);
        DrawLine(global_position.x, global_position.y - INDICATOR_RADIUS, global_position.x, global_position.y + INDICATOR_RADIUS, RED);

        DrawText(getTypeName(), global_position.x + 5, global_position.y - 15, 10, RED);
    }
}
void Node::ready() {
    print(getTypeName(), " is ready");
}

// - Resources -
Texture2D* Node::loadTexture(const char* file_path) {

    if (!initialised) {
        warn("Node is not initialised, cannot provide Texture", true);
        return NULL;
    }

    return manager->loadTexture(file_path, getId());
}

// - Children -
void Node::addChild(Node* child) {
    child->init(getRoot(), this, manager);
    children.push_back(child);
    child->ready();
}
Node* Node::getChild(int child_idx) {
    if (child_idx <= getChildCount()) {
        print("In function 'getChild', child_idx: ");
        print(child_idx);
        print("Is larger than or equal to getChildCount(): ");
        print(getChildCount());
        throw;
    }
    else {
        return children[child_idx];
    }
}
std::vector<Node*> Node::getChildren() {
    return children;
}
int Node::getChildCount() {
    return children.size();
}

// - Hierarchy -
bool Node::isInsideTree() {

    if (getParent() == NULL) {
        return is_root;
    }
    else {
        return getParent()->isInsideTree();
    }
}
Node* Node::getParent() {
    return parent;
}
Node* Node::getRoot() {
    return root;
}
NodeManagerParent* Node::getManager() {
    return manager;
}
int Node::getIndex() {
    return 0; // TODO
}
int Node::getId() {
    return 0; // TODO
}
void Node::kill() {
    delete this; // TODO
}

// --- Values ---
Vector2 Node::getPosition() {
    return position;
}
void Node::setPosition(Vector2 value) {
    position = value;
}
Vector2 Node::getGlobalPosition(bool suppress_warning) {
    if (!isInsideTree() && !suppress_warning) {
        warn("Node is not inside tree, cannot provide global position");
    }
    
    if (getParent() == NULL) {
        return Vector2{0, 0};
    }

    return aV(getParent()->getGlobalPosition(), getPosition());
}
void Node::setGlobalPosition(Vector2 value) {
    if (!isInsideTree()) {
        warn("Node is not inside tree, cannot set global position");
    }

    setPosition(sV(value, getGlobalPosition(true)));
}

Vector2 Node::getScale() {
    return scale;
}
void Node::setScale(Vector2 value) {
    scale = value;
}
Vector2 Node::getGlobalScale(bool suppress_warning) {
    if (!isInsideTree() && !suppress_warning) {
        warn("Node is not inside tree, cannot provide global scale");
    }
    
    if (getParent() == NULL) {
        return Vector2{1, 1};
    }

    return aV(getParent()->getGlobalScale(), getScale());
}
void Node::setGlobalScale(Vector2 value) {
    if (!isInsideTree()) {
        warn("Node is not inside tree, cannot set global scale");
    }

    setScale(sV(value, getGlobalScale(true)));
}