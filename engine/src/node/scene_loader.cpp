#include "scene_loader.h"

#include "engine/src/engine.h"
#include "common/utils.h"
#include "engine/src/node/node.h"
#include "engine/src/core/object_constructor.h"
#include "engine/src/node/types/animated_sprite.h"

Node* SceneLoader::loadSceneFromFile(string file_path, Node* root_node_override, YAML::Node* config_container) {

    ASSERT_MSG(OS::fileExists(file_path), "No file exists at path '" + OS::getResPath(file_path) + "'");

    YAML::Node scene = YAML::LoadFile(OS::getResPath(file_path));

    ASSERT_MSG(scene.size() > 0, "Scene must not be empty");

    YAML::Node config = scene["config"];
    if (config_container != NULL) {
        *config_container = config;
    }

    WARN_IF(scene.size() > (config.IsDefined() ? 2 : 1), "Scene at path " + file_path + " contains more than 1 root node. Using only the first one.");

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
        WARN_IF(true, "config is not a dictionary, ignoring it");
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
                WARN_IF(true, "Scene has no child at '" + i->first + "'");
                continue;
            }
            if (root_node->hasRegisteredSceneNode(i->second)) {
                WARN_IF(true, "Scene already has a node registered at '" + i->second + "'");
                continue;
            }

            root_node->registerSceneNode(i->second, root_node->getChildAtPath(i->first));
        }
    }

}

Node* SceneLoader::synthesiseNode(string node_type, YAML::Node& node_data, Node* root_node_override) {
    unordered_map<Node*, vector<pair<string, YAML::Node>>> to_add;

    Node* root = instanceNodeFromType(node_type, node_data, true, root_node_override);
    ASSERT_MSG(root != NULL, "Root node type is unregistered: " + node_type);

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

    ASSERT_MSG(Engine::getSingleton()->isObjectTypeRegistered(node_type), "Node type '" + node_type + "' must be registered");
    ASSERT_MSG(Engine::getSingleton()->doesRegisteredObjectInheritType<Node>(node_type), "Object type '" + node_type + "' must inherit Node");

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
        string name = i->first.as<string>();

        // Children are handled externally
        if (name == "children") {
            continue;
        }

        // Check if name is a property
        if (constructor->isPropertyRegistered(name)) {
            constructor->setProperty(ret, name, i->second);
            continue;
        }
        // Check if name is a method
        else if (constructor->isMethodRegistered(name)) {
            constructor->callMethod(ret, name, i->second);
            continue;
        }

        WARN_IF(true, node_type + " has no registered property or method named '" + name + "', skipping");
    }

    return ret;
}
