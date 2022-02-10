#include "node.h"
#include "utils.h"
#include "vector"
#include "icecream.hpp"

// - Core -
void Node::init(Node* root_node, Node* parent_node) {
    root = root_node;
    parent = parent_node;

    if (root_node == this) {
        is_root = true;
        print("Root node initialised");
    }
    else {
        print("Node initialised");
    }
}
void Node::process(float delta) {
    print(delta);
}

// - Children -
void Node::addChild(Node* child) {
    child->init(getRoot(), this);
    children.push_back(child);
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

    return addVector2(getParent()->getGlobalPosition(), getPosition());
}
void Node::setGlobalPosition(Vector2 value) {
    if (!isInsideTree()) {
        warn("Node is not inside tree, cannot set global position");
    }

    setPosition(subVector2(value, getGlobalPosition(true)));
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

    return addVector2(getParent()->getGlobalScale(), getScale());
}
void Node::setGlobalScale(Vector2 value) {
    if (!isInsideTree()) {
        warn("Node is not inside tree, cannot set global scale");
    }

    setScale(subVector2(value, getGlobalScale(true)));
}