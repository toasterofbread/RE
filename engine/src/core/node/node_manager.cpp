#include "node_manager.h"

#include <vector>

#include "engine/src/utils.h"
#include "engine/src/engine.h"
#include "engine/src/core/node/node.h"
#include "engine/src/core/node/animated_sprite.h"
#include "engine/src/core/converters.h"
#include "engine/src/core/resource/resource.h"
#include "engine/src/input/input_event.h"

// Debug
#include "icecream.hpp"

NodeManager::NodeManager(Engine* engine_singleton) {
    engine = engine_singleton;
}

void NodeManager::init() {
    root_node = new Node(engine);
    root_node->setName("Root");
}

void NodeManager::process(float delta) {

}

void NodeManager::addNode(Node* node) {
    root_node->addChild(node);
}

int NodeManager::getNewNodeId() {
    current_id_max++;
    return current_id_max;
}

Node* NodeManager::loadScene(string file_path, Node* root_node_override, YAML::Node* config_container) {

    if (!FileExists(getResPath(file_path).c_str())) {
        warn("No file exists at path '" + file_path + "'", true);
        return NULL;
    }

    YAML::Node scene = YAML::LoadFile(getResPath(file_path));

    if (scene.size() == 0) {
        warn("Scene is empty", true);
        return NULL;
    }

    YAML::Node config = scene["config"];
    if (config_container != NULL) {
        *config_container = config;
    }
    if (scene.size() > (config.IsDefined() ? 2 : 1)) {
        warn("Scene contains more than 1 root node. Using only the first one.");
    }
    
    Node* ret = NULL;
    for (auto i = scene.begin(); i != scene.end(); ++i) {
        if (i->first.as<string>() != "config") {
            ret = synthesiseNode(i->first.as<string>(), i->second, root_node_override);
            break;
        }
    }

    processSceneConfig(config, ret);

    return ret;
}

void NodeManager::processSceneConfig(YAML::Node& config, Node* root_node) {
    if (!config.IsDefined()) {
        return;
    }
    if (!config.IsMap()) {
        warn("config is not a dictionary, ignoring it");
        return;
    }

    if (config["print_tree"].IsDefined() && config["print_tree"].as<bool>()) {
        root_node->printTree();
    }

    if (config["register_nodes"].IsDefined()) {
        map<string, string> nodes_to_register;

        if (config["register_nodes"].IsSequence()) {
            vector<string> nodes = config["register_nodes"].as<vector<string>>();
            for (auto i = nodes.begin(); i != nodes.end(); ++i) {
                nodes_to_register[*i] = *i;
            }
        }
        else if (config["register_nodes"].IsMap()) {
            nodes_to_register = config["register_nodes"].as<map<string, string>>();
        }
        else {
            nodes_to_register[config["register_nodes"].as<string>()] = config["register_nodes"].as<string>();
        }

        for (auto i = nodes_to_register.begin(); i != nodes_to_register.end(); ++i) {
            if (!root_node->hasChildAtPath(i->first)) {
                warn("Scene has no child at '" + i->first + "'");
                continue;
            }
            if (root_node->hasRegisteredSceneNode(i->second)) {
                warn("Scene already has a node registered at '" + i->second + "'");
                continue;
            }

            root_node->registerSceneNode(i->second, root_node->getChildAtPath(i->first));
        }
    }

}

// Helper function for NodeManager::synthesiseNode()
PONDER_TYPE(Engine)
Node* NodeManager::instanceNodeFromType(string& node_type, YAML::Node& node_data, bool suppress_warning, Node* root_node_override) {

    // Ensure a node with name node_type exists
    if (!Node::isPonderClassDeclared(node_type)) {
        warn("Node type '" + node_type + "' does not exist, skipping");
        return NULL;
    }

    auto cont = [node_type, node_data, this](const ponder::Class& node_class, ponder::UserObject& node) {
        // Set node properties
        for (auto i = node_data.begin(); i != node_data.end(); ++i) {
            string property_name = i->first.as<string>();

            // Children are handled externally
            if (property_name == "children") {
                continue;
            }

            // Iterate through each function to find the setter matching property_name
            string setter_name = Node::getSetterName(property_name);
            const ponder::Function* setter = NULL;
            for (int i = 0; i < node_class.functionCount(); i++) {
                const ponder::Function& function = node_class.function(i);
                if (stringBeginsWith(function.name(), setter_name)) {
                    setter = &function;
                    break;
                }
            }

            if (setter != NULL) {
                string property_type = setter->name();
                property_type.erase(0, setter_name.size());

                YAML::Node data = i->second;
                string result = callNodeSetter(property_type, data, node, setter, this);
                if (!result.empty()) {
                    warn("An error occurred while setting a node property:\n" + result + "\n\nNode type: " + node_type + "\nProperty name: " + property_name + "\nProperty type: " + property_type);
                    // cout << "Data: ";
                    // cout << data << endl;
                    // todo
                }
            }
            else {
                warn(node_type + " has no registered property named '" + property_name + "', skipping");
            }
        }
    };

    // ponder::Class* node_class;
    // ponder::UserObject* node;
    Node* ret;

    if (root_node_override == NULL) {
        const ponder::Class& node_class = ponder::classByName(node_type); // Get class object matching node_type
        const ponder::Constructor* constructor = NULL;

        // Find a valid class constructor
        for (int i = 0; i < node_class.constructorCount(); i++) {
            constructor = node_class.constructor(i);
            if (constructor->matches(engine)) {
                break;
            }
        }

        // No valid constructor was found
        if (constructor == NULL) {
            warn("Node type has no valid registered constructor: " + node_type, true);
            return NULL;
        }

        // Construct node from constructor
        ponder::UserObject node = constructor->create(NULL, engine);
        ret = &node.ref<Node>();

        cont(node_class, node);
    }
    else {
        const ponder::Class& node_class = ponder::classByType<Node>(); //  !hack
        ponder::UserObject node = node_class.getUserObjectFromPointer(root_node_override);
        // ponder::UserObject n = ponder::UserObject::makeRef((Node*)root_node_override);
        ret = root_node_override;

        cont(node_class, node);
    }

    return ret;
}

Node* NodeManager::synthesiseNode(string node_type, YAML::Node& node_data, Node* root_node_override) {

    unordered_map<Node*, vector<pair<string, YAML::Node>>> to_add;
    Node* root = instanceNodeFromType(node_type, node_data, true, root_node_override);
    if (root == NULL) {
        warn("Root node type is unregistered: " + node_type, true);
        return NULL;
    }

    if (node_data["children"].IsDefined()) {
        vector<pair<string, YAML::Node>> children;
        for (auto i = node_data["children"].begin(); i != node_data["children"].end(); ++i) {
            children.push_back(make_pair(i->first.as<string>(), i->second));
        }
        to_add[root] = children;
    }

    auto add_node_children = [this](Node* node, vector<pair<string, YAML::Node>>* children) {
        unordered_map<Node*, vector<pair<string, YAML::Node>>> ret;

        for (auto i = children->begin(); i != children->end(); ++i) {
            string child_type = i->first;
            YAML::Node child_data = i->second;

            vector<pair<string, YAML::Node>> grandchildren;
            Node* child = instanceNodeFromType(child_type, child_data);
            if (child == NULL) {
                continue;
            }
            node->addChild(child);

            if (child_data["children"].IsDefined()) {
                for (auto i = child_data["children"].begin(); i != child_data["children"].end(); ++i) {
                    grandchildren.push_back(make_pair(i->first.as<string>(), i->second));
                }
            }

            ret[child] = grandchildren;
        }
        return ret;
    };

    while (!to_add.empty()) {
        unordered_map<Node*, vector<pair<string, YAML::Node>>>::iterator first_item = to_add.begin();
        unordered_map<Node*, vector<pair<string, YAML::Node>>> children = add_node_children(first_item->first, &(first_item->second));
        to_add.erase(first_item);
        for( const pair<Node*, vector<pair<string, YAML::Node>>>& i : children ) {
            to_add[i.first] = i.second;
        }
    }

    return root;
}

