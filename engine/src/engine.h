#ifndef INCLUDED_ENGINE
#define INCLUDED_ENGINE

#include <raylib-cpp.hpp>

#include "engine/src/debug_options.h"
#include "project/src/debug_options.h"

#include <thread>
#include <unordered_map>
#include <memory>
using namespace std;

#include "engine/src/core/signal.h"
#include "engine/src/core/object_constructor.h"
#include "engine/src/core/node/scene_tree.h"
#include "engine/src/input/input_manager.h"
#include "engine/src/core/yaml_data_converter.h"
#include "engine/src/core/resource/resource.h"

// Forward declarations
class InputEvent;
class Resource;
class EngineTexture;

class Engine {
    public:
        Engine();
        void process(float delta);

        static Engine* getSingleton();

        Signal<void, string, bool>* SIGNAL_OBJECT_TYPE_REGISTERED = new Signal<void, string, bool>();

        SceneTree* getTree() { return scene_tree_singleton; }
        YAMLDataConverter* getYAMLDataConverter() { return yaml_data_converter_singleton; }

        // - Texture management -
        shared_ptr<EngineTexture> loadTexture(string file_path);
        bool isTextureLoaded(string file_path);

        // - Resource management -
        void resourceCreated(Resource* resource);
        void resourceDeleted(Resource* resource);

        template<typename ResourcePoolType>
        Resource::ResourcePool* getResourcePool() {
            if (resource_pools.count(ResourcePoolType::getName())) {
                return resource_pools[ResourcePoolType::getName()];
            }
            Resource::ResourcePool* ret = new ResourcePoolType();
            resource_pools[ResourcePoolType::getName()] = ret;
            return ret;
        }

        // - InputEvent management -
        void inputEventCreated(InputEvent* event);
        void inputEvenDeleted(InputEvent* event);

        static string getResPath(string absolute_path);

        // Debug
        void ensureAsync();
        void rebuildAndRun();

        // - ObjectConstructor -
        template<typename ObjectType>
        ObjectConstructor<ObjectType>* getObjectConstructor(string object_type_name) {
            if (!isObjectTypeRegistered(object_type_name)) {
                warn("Object '" + object_type_name + "' must be registeded in order to create a constructor for it", true);
                return NULL;
            }
            return (ObjectConstructor<ObjectType>*)registered_object_constructors[object_type_name];
        }

        template<typename ObjectType, typename... ConstructorArgs>
        ObjectConstructor<ObjectType>* registerObjectType(string object_type_name) {
            if (isObjectTypeRegistered(object_type_name)) {
                warn("Object '" + object_type_name + "' is already registered", true);
                return NULL;
            }

            ObjectConstructor<ObjectType>* constructor = new ObjectConstructor<ObjectType>(getYAMLDataConverter());
            constructor->template init<ConstructorArgs...>();
            registered_object_constructors[object_type_name] = constructor;
            
            SIGNAL_OBJECT_TYPE_REGISTERED->emit(object_type_name, constructor->template inheritsType<Node>());
            // onObjectRegisteredAsConstructor();

            return constructor;
        }

        bool isObjectTypeRegistered(string object_type_name) {
            return registered_object_constructors.count(object_type_name);
        }

        template<typename InheritedType>
        bool doesRegisteredObjectInheritType(string object_type_name) {
            if (!isObjectTypeRegistered(object_type_name)) {
                warn("Object '" + object_type_name + "' is not registered", true);
                return false;
            }
            return (registered_object_constructors[object_type_name])->inheritsType<InheritedType>();
        }

        // void onObjectRegisteredAsConstructor(string object_type_name, bool is_node) {
        //     if (is_node) {
        //         registered_nodes.push_back(object_type_name);
        //     }
        // }

        // Nodes
        int getNewNodeId();

        void onNodeCreated(Node* node);
        void onNodeKilled(Node* node);
        int getGlobalNodeCount();

    private:
        static Engine* singleton;

        int current_node_id_max = 0;
        int global_node_count = 0;

        SceneTree* scene_tree_singleton = new SceneTree();
        YAMLDataConverter* yaml_data_converter_singleton = new YAMLDataConverter();

        vector<InputEvent*> all_inputevents;
        thread::id main_thread_id = this_thread::get_id();
        unordered_map<string, ObjectConstructorBase*> registered_object_constructors;

        // - Texture management -
        unordered_map<string, weak_ptr<EngineTexture>> loaded_textures;
        void onTextureContainerDeleted(string file_path, Texture2D texture);
        // void spriteAnimationSetDeleted(SpriteAnimationSet resource);

        // - Resource pool -
        vector<Resource*> all_resources;
        unordered_map<string, Resource::ResourcePool*> resource_pools;
};

#endif