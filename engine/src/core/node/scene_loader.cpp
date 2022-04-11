#include "scene_loader.h"

#include "engine/src/engine.h"
#include "common/utils.h"
#include "engine/src/core/node/node.h"
#include "engine/src/core/object_constructor.h"
#include "engine/src/core/node/node_types/animated_sprite.h"

Node* SceneLoader::loadSceneFromFile(string file_path, Node* root_node_override, YAML::Node* config_container) {

    if (!FileExists(Engine::getResPath(file_path).c_str())) {
        warn("No file exists at path '" + Engine::getResPath(file_path) + "'", true);
        return NULL;
    }

    YAML::Node scene = YAML::LoadFile(Engine::getResPath(file_path));

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


void SceneLoader::processSceneConfig(YAML::Node& config, Node* root_node) {
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

Node* SceneLoader::synthesiseNode(string node_type, YAML::Node& node_data, Node* root_node_override) {
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

    auto add_node_children = [](Node* node, vector<pair<string, YAML::Node>>* children) {
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

Node* SceneLoader::instanceNodeFromType(string node_type, YAML::Node& node_data, bool suppress_warning, Node* root_node_override) {

    // Ensure node_type is a registered object and inherits Node
    if (!Engine::getSingleton()->isObjectTypeRegistered(node_type)) {
        warn("Node type '" + node_type + "' is not registered. Skipping...");
        return NULL;
    }
    else if (Engine::getSingleton()->doesRegisteredObjectInheritType<Node>(node_type)) {
        warn("Node type '" + node_type + "' is registered, but doesn't inherit Node. Skipping...");
        return NULL;
    }

    // Get the constructor for node_type
    ObjectConstructor<Node>* constructor = Engine::getSingleton()->getObjectConstructor<Node>(node_type);

    Node* ret;

    // If root_node_override was provided, use it as the root node
    if (root_node_override != NULL) {
        ret = root_node_override;
    }
    // Otherwise, instantiate a new node using the constructor
    else {
        ret = constructor->createInstance();
    }

    // Set node properties
    for (auto i = node_data.begin(); i != node_data.end(); ++i) {
        string property_name = i->first.as<string>();
        if (property_name == "animation_set" || true) {
            AnimatedSprite* bruh = (AnimatedSprite*)ret;
        }

        // Children are handled externally
        if (property_name == "children") {
            continue;
        }

        // Check if the property is registered with a setter
        if (!constructor->isPropertyRegistered(property_name)) {
            warn(node_type + " has no registered property named '" + property_name + "', skipping");
            continue;
        }

        constructor->setProperty(ret, property_name, i->second);
    }

    return ret;
}
