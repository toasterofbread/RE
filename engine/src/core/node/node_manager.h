#ifndef INCLUDED_NODE_MANAGER
#define INCLUDED_NODE_MANAGER

#include <yaml-cpp/yaml.h>
#include <string>
using namespace std;

#include "engine/src/core/node/constructor.h"
#include "engine/src/utils.h"

// Forward declaration
class Engine;
class Node;

class NodeManager {
    public:
        NodeManager(Engine* engine_singleton);
        void init();

        void process(float delta);
        void addNode(Node* node);
        Node* loadScene(string file_path, Node* root_node_override = NULL, YAML::Node* config_container = NULL);
        static void processSceneConfig(YAML::Node& config, Node* root_node);

        Node* getRoot() { return root_node; }

        int getNewNodeId();

        Node* synthesiseNode(string node_type, YAML::Node& node_data, Node* root_node_override);

        
        template<typename PropertyType>
        void registerNodePropertyType(string type_name, function<PropertyType(YAML::Node)> converter_method) {
            if (isNodePropertyTypeRegistered(type_name)) {
                warn("Node '" + type_name + "' is already registered", true);
                return;
            }
            NodePropertyTypeConverter<PropertyType> converter = new NodePropertyTypeConverter<PropertyType>(converter_method);
            registered_property_type_converters[type_name] = converter;
        }

        template<typename PropertyType>
        PropertyType convertYAMLData(string type_name, YAML::Node data) {
            if (!isNodePropertyTypeRegistered(type_name)) {
                warn("Property type '" + type_name + "' is not registered", true);
            }
            return registered_property_type_converters[type_name]->convertData<PropertyType>(data);
        }

        bool isNodePropertyTypeRegistered(string type_name) {
            return registered_property_type_converters.count(type_name);
        }        

        // template<typename ObjectType>
        // void connect(CallbackReturnType (ObjectType::*callback)(CallbackArgs...), ObjectType* object) {
        //     member_callbacks.push_back(new MemberCallback<ObjectType>(callback, object));
        // }

    private:
        Engine* engine;
        Node* root_node;

        Node* instanceNodeFromType(string& node_type, YAML::Node& node_data, bool suppress_warning = false, Node* root_node_override = NULL);

        int current_id_max = 0;

        
        unordered_map<string, NodePropertyTypeConveterBase*> registered_property_type_converters;
};

#endif