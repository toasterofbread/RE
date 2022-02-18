#include "node.h"
#include <vector>
#include <yaml-cpp/yaml.h>

#include "engine/src/utils.h"
#include "engine/src/engine.h"
#include "engine/src/core/signal.h"

#include <icecream.hpp> // Debug

const int INDICATOR_RADIUS = 10;

// - Core -
Node::Node(Engine* engine_singleton) {

    SIGNAL_READY = new Signal<void, Node*>();
    SIGNAL_KILLED = new Signal<void, Node*>();
    
    if (engine_singleton != NULL) init(engine_singleton);
}

void Node::init(Engine* engine_singleton) {
    name = getValidName();
    engine = engine_singleton;
    manager = engine->getNodeManager();
    id = manager->getNewNodeId();
    is_root = id == 1;
}

void Node::addedToNode(Node* parent_node) {
    parent = parent_node;
    name = getValidName();
}
void Node::removedFromNode(Node* former_parent_node) {
    parent = NULL;
}
void Node::process(float delta) {

    if (show_gizmos) {
        string text = getTypeName();
        if (getName() != text) {
            text += " | " + getName();
        }
        if (!getVisible()) {
            text += " (invisible)";
        }
        // text += " | " + (string)int2char(getId());
        markPosition(getGlobalPosition(true), text);
    }

    vector<Node*>* nodes = getChildren();
    for (auto i = nodes->cbegin(); i != nodes->cend(); ++i) {
        Node& node = **i;
        node.process(delta);
    }

}
void Node::ready() {
    SIGNAL_READY->emit(this);
    // print(getTypeName(), " is ready");
}

string Node::getValidName(string base_name) {
    string ret;
    
    if (!base_name.empty()) {
        ret = base_name;
    }
    else if (!getName().empty()) {
        ret = getName();
    }
    else {
        ret = getTypeName();
    }

    if (getParent() == NULL) {
        return ret;
    }

    auto getAppendedName = [](string name, int append) {
        if (append > 1) {
            string ret = name;
            ret.append("_");
            ret.append(int2char(append));
            return ret;
        }
        else {
            return name;
        }
    };

    int append = 1;
    vector<Node*>* parent_children = getParent()->getChildren();
    for (auto i = parent_children->begin(); i != parent_children->end(); ++i) {
        Node* node = *i;
        if (node == this) {
            continue;
        }

        if (append == 1 && node->getName() == getName()) {
            append++;
        }
        else if (append > 1 && node->getName() == getAppendedName(getName(), append)) {
            append++;
        }
    }
    
    return getAppendedName(ret, append);
}

// - Children -
void Node::addChild(Node* child) {
    
    if (child->isInsideTree()) {
        warn("Node is already inside tree", true);
        return;
    }
    
    children.push_back(child);
    child->addedToNode(this);

    child->ready();
}

void Node::removeChild(int child_idx) {

    if (!hasChild(child_idx)) {
        warn("Child index '" + (string)int2char(child_idx) + "' is out of bounds", true);
        return;
    }

    int index = 0;
    for (auto i = getChildren()->begin(); i != getChildren()->end(); ++i) {
        index++;
        if (index == child_idx) {
            getChildren()->erase(i);
            (*i)->removedFromNode(this);
            return;
        }
    }
}

void Node::removeChild(string child_name) {
    Node* child = NULL;
    for (auto i = getChildren()->begin(); i != getChildren()->end(); ++i) {
        if ((*i)->getName() == child_name) {
            getChildren()->erase(i);
            child = *i;
            break;
        }
    }

    if (child == NULL) {
        warn("Parent has no child with name '" + child_name + "'", true);
        return;
    }

    child->removedFromNode(this);
}

void Node::removeChild(Node* child) {
    bool found_child = false;
    for (auto i = getChildren()->begin(); i != getChildren()->end(); ++i) {
        if (*i == child) {
            getChildren()->erase(i);
            found_child = true;
            break;
        }
    }

    if (!found_child) {
        warn("Passed child does not belong to node", true);
        return;
    }

    child->removedFromNode(this);
}

Node* Node::getChild(int child_idx) {
    if (!hasChild(child_idx)) {
        return NULL;
    }
    return children[child_idx];
}

Node* Node::getChild(string child_name) {
    for (auto i = getChildren()->begin(); i != getChildren()->end(); ++i) {
        if ((*i)->getName() == child_name) {
            return *i;
        }
    }
    return NULL;
}

Node* Node::getChildAtPath(string child_path) {
    vector<string> path = splitString(child_path, '/');
    Node* current_node = this;

    for (auto i = path.begin(); i != path.end(); ++i) {
        if (current_node->hasChild(*i)) {
            current_node = current_node->getChild(*i);
        }
        else {
            warn("Node has no child at path '" + child_path + "'", true);
            return NULL;
        }
    }

    return current_node;
}

bool Node::hasChild(int child_idx) {
    return child_idx < getChildCount();
}

bool Node::hasChild(string child_name) {
    for (auto i = getChildren()->begin(); i != getChildren()->end(); ++i) {
        if ((*i)->getName() == child_name) {
            return true;
        }
    }
    return false;
}

bool Node::hasChildAtPath(string child_path) {
    vector<string> path = splitString(child_path, '/');
    Node* current_node = this;

    for (auto i = path.begin(); i != path.end(); ++i) {
        if (current_node->hasChild(*i)) {
            current_node = current_node->getChild(*i);
        }
        else {
            return false;
        }
    }

    return true;
}

vector<Node*>* Node::getChildren() {
    return &children;
}

vector<pair<Node*, int>> Node::getChildrenRecursive(int max_depth, int depth_offset) {
    vector<pair<Node*, int>> ret;

    for (auto i = getChildren()->begin(); i != getChildren()->end(); ++i) {
        ret.push_back(make_pair(*i, depth_offset));

        if (max_depth < 0 || depth_offset <= max_depth) {
            vector<pair<Node*, int>> grandchildren = (*i)->getChildrenRecursive(max_depth - 1, depth_offset + 1);
            for (auto i = grandchildren.begin(); i != grandchildren.end(); ++i) {
                ret.push_back(*i);
            }
        }
    }

    return ret;
}

int Node::getChildCount() {
    return children.size();
}

int Node::getChildIndex(Node* child) {
    int index = 0;
    for (auto i = getChildren()->begin(); i != getChildren()->end(); ++i) {
        index++;
        if (*i == child) {
            return index;
        }
    }
    warn("Child does not belong to node", true);
    return -1;
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
    return getManager()->getRoot();
}

bool Node::isRoot() {
    return is_root;
}

NodeManager* Node::getManager() {
    return manager;
}

Engine* Node::getEngine() {
    return engine;
}

void Node::printTree(int max_depth) {
    const string branch_string = "├─ ";
    const string branch_string_blank = "   ";
    const string indent_string = "  ";

    auto getInfoString = [](Node* node) {
        string ret = "   │ Name: " + node->getName() + ", ID: " + int2char(node->getId()) + ", Children: " + int2char(node->getChildCount());
        return ret;
    };

    vector<pair<Node*, string>> messages = {
        {this, isRoot() ? getTypeName() + " (Root)" : getTypeName()}
    };

    vector<pair<Node*, int>> all_children = getChildrenRecursive(max_depth);

    int max_length = getTypeName().length();
    for (auto i = all_children.begin(); i != all_children.end(); ++i) {
        Node* node = i->first;
        int depth = i->second;

        string msg = branch_string + node->getTypeName();
        for (int i = 0; i < depth; i++) {
            msg.insert(0, indent_string);
        }
        
        messages.push_back(make_pair(node, msg));
        
        if (msg.length() > max_length) {
            max_length = msg.length();
        }
    }
    
    string out = "┌──────────────────────────────────────────────────────────────────────────────────────┐\n";
    // string out = "";

    for (auto i = messages.begin(); i != messages.end(); ++i) {
        Node* node = i->first;
        string message = i->second;

        while (message.length() < max_length - (node == this ? branch_string.length() - branch_string_blank.length() : 0)) {
            message += " ";
        }

        message += getInfoString(node);
        out += "  " + message + "\n";
    }

    // print(encaseStringInBox(out));
    print(out + "└──────────────────────────────────────────────────────────────────────────────────────┘");
}

int Node::getIndex() {
    return getParent()->getChildIndex(this);
}

int Node::getId() {
    return id;
}

void Node::kill() {

    if (isRoot()) {
        warn("The root node cannot be killed (it's power level is over 9000)");
        return;
    }

    SIGNAL_KILLED->emit(this);
    getParent()->removeChild(this);
    while (getChildCount() > 0) {
        getChild(0)->kill();
    }
    getEngine()->requestDeletion(this);
}

// --- Values ---

void Node::setName(string value) {

    if (value.find("/") != string::npos) {
        warn("Node name '" + value + "' may not contain '/'", true);
        return;
    }

    name = getValidName(value); 
}

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
        return getPosition();
    }

    return aV(getParent()->getGlobalPosition(suppress_warning), getPosition());
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
        return getScale();
    }

    return mV(getParent()->getGlobalScale(suppress_warning), getScale());
}
void Node::setGlobalScale(Vector2 value) {
    if (!isInsideTree()) {
        warn("Node is not inside tree, cannot set global scale");
    }

    setScale(sV(value, getGlobalScale(true)));
}

float Node::getRotation() {
    return rotation;
}
void Node::setRotation(float value) {
    rotation = value;
}

float Node::getGlobalRotation(bool suppress_warning) {
    if (!isInsideTree() && !suppress_warning) {
        warn("Node is not inside tree, cannot provide global rotation");
    }
    
    if (getParent() == NULL) {
        return getRotation();
    }

    return getParent()->getGlobalRotation(suppress_warning) + getRotation();
}
void Node::setGlobalRotation(float value) {
    if (!isInsideTree()) {
        warn("Node is not inside tree, cannot set global scale");
    }

    setRotation(value - getGlobalRotation(true));
}