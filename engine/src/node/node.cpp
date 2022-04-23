#include "node.h"
#include <vector>
// #include <yaml-cpp/yaml.h>

#include "common/utils.h"
#include "engine/src/engine.h"
#include "engine/src/core/signal.h"
#include "engine/src/node/types/node_2d.h"
#include "engine/src/node/types/node_3d.h"

const int INDICATOR_RADIUS = 10;

// - Core -
Node::Node() {
    ASSERT(registered());

    name = getValidName();
    id = Engine::getSingleton()->getNewNodeId();

    Engine::getSingleton()->onNodeCreated(this);
}

void Node::enteredTree() {
    if (!isReady()) {
        ready();
    }
}

void Node::addedToNode() {
    ASSERT(tree == NULL);
    ASSERT(parent != NULL);
    
    name = getValidName();
    tree = getParent()->getTree();
    
    if (tree) {
        tree->onNodeAddedToTree(this);
        enteredTree();

        for (auto child : getChildrenRecursive()) {
            child.first->tree = tree;
            tree->onNodeAddedToTree(child.first);
            child.first->enteredTree();
        }
    }
}

void Node::removedFromNode(Node* former_parent_node) {
    parent = NULL;

    if (tree) {
        tree->onNodeRemovedFromTree(this);
        
        for (auto child : getChildrenRecursive()) {
            tree->onNodeRemovedFromTree(child.first);
            child.first->tree = NULL;
        }
        
        tree = NULL;
    }
}

void Node::process(float delta) {
    vector<Node*>* nodes = getChildren();
    for (auto i = nodes->cbegin(); i != nodes->cend(); ++i) {
        Node& node = **i;
        node.process(delta);
    }
}

void Node::physicsProcess(float delta) {
    vector<Node*>* nodes = getChildren();
    for (auto i = nodes->cbegin(); i != nodes->cend(); ++i) {
        Node& node = **i;
        node.physicsProcess(delta);
    }
}

void Node::ready() {
    ASSERT(!ready_called);
    SIGNAL_READY.emit();
    ready_called = true;
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
            ret.append(to_string(append));
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

void Node::makeRoot(SceneTree* of_tree) {
    ASSERT(!is_root);
    ASSERT(tree == NULL);
    ASSERT(of_tree->getRoot() == this);

    is_root = true;
    tree = of_tree;
}

// - Children -
void Node::addChild(Node* child) {
    ASSERT(!child->isInsideTree());
    children.push_back(child);
    child->parent = this;
    child->addedToNode();
}

void Node::removeChild(int child_idx) {
    ASSERT(hasChild(child_idx));
    auto child = children.begin() + child_idx;
    getChildren()->erase(child);
    (*child)->removedFromNode(this);
}

void Node::removeChild(string child_name) {
    for (auto i = getChildren()->begin(); i != getChildren()->end(); ++i) {
        if ((*i)->getName() == child_name) {
            getChildren()->erase(i);
            (*i)->removedFromNode(this);
            return;
        }
    }
    throw runtime_error("No child with name " + child_name);
}

void Node::removeChild(Node* child) {
    for (auto i = getChildren()->begin(); i != getChildren()->end(); ++i) {
        if (*i == child) {
            getChildren()->erase(i);
            child->removedFromNode(this);
            return;
        }
    }
    throw runtime_error("Child is not a child of this node");
}

Node* Node::getChild(int child_idx) {
    ASSERT(hasChild(child_idx));
    return children[child_idx];
}

Node* Node::getChild(string child_name) {
    for (auto i = getChildren()->begin(); i != getChildren()->end(); ++i) {
        if ((*i)->getName() == child_name) {
            return *i;
        }
    }
    throw runtime_error("No child with name " + child_name);
}

Node* Node::getChildAtPath(string child_path) {
    vector<string> path = splitString(child_path, '/');
    Node* current_node = this;

    for (auto i = path.begin(); i != path.end(); ++i) {
        ASSERT(current_node->hasChild(*i));
        current_node = current_node->getChild(*i);
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
    ASSERT_MSG(false, "Child does not belong to node");
}

// - Hierarchy -
bool Node::isInsideTree() {
    return tree != NULL;
}

SceneTree* Node::getTree() {
    return tree;
}

bool Node::isIndirectParentOf(Node* node) {
    if (node == this) {
        return true;
    }
    if (node->isRoot()) {
        return false;
    }

    Node* parent = node->getParent();
    while (!parent->isRoot()) {
        if (parent == this) {
            return true;
        }
    }

    return false;
}

Node* Node::getParent() {
    return parent;
}

bool Node::isRoot() {
    return is_root;
}

void Node::printTree(int max_depth) {
    const string branch_string = "├─ ";
    const string branch_string_blank = "   ";
    const string indent_string = "  ";

    auto getInfoString = [](Node* node) {
        string ret = "   │ Name: " + node->getName() + ", ID: " + to_string(node->getId()) + ", Children: " + to_string(node->getChildCount());
        
        if (Node2D* node_2d = dynamic_cast<Node2D*>(node)) {
            ret += ", Position: " + node_2d->getPosition().toString();
            ret += ", Global position: " + node_2d->getGlobalPosition().toString();
        }
        else if (Node3D* node_2d = dynamic_cast<Node3D*>(node)) {
            ret += ", Position: " + node_2d->getPosition().toString();
            ret += ", Global position: " + node_2d->getGlobalPosition().toString();
        }
        
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

    OS::print(out + "└──────────────────────────────────────────────────────────────────────────────────────┘");
}

int Node::getIndex() {
    return getParent()->getChildIndex(this);
}

int Node::getId() {
    return id;
}

void Node::kill() {

    ASSERT_MSG(!isInsideTree() || tree->getCurrentState() == SceneTree::STATE::KILL_NODES, "Cannot kill node, SceneTree is in process state");
    ASSERT_MSG(!isRoot(),"The root node cannot be killed (its power level is over 9000)");

    SIGNAL_KILLED.emit();
    
    Engine::getSingleton()->onNodeKilled(this);
    getParent()->removeChild(this);

    if (tree) {
        tree->onNodeRemovedFromTree(this);
    }
    
    while (getChildCount() > 0) {
        Node* child = getChild(0);
        child->kill();
        delete child;
    }
}

void Node::queueKill() {
    ASSERT(tree);
    tree->queueNodeKill(this);
}

// --- Values ---

void Node::setName(string value) {

    ASSERT_MSG(value.find("/") == string::npos, "Node name '" + value + "' may not contain '/'");
    name = getValidName(value); 
}

