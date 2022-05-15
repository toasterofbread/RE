#ifndef INCLUDED_ENGINE
#define INCLUDED_ENGINE

#include <thread>
#include <unordered_map>
#include <memory>
using namespace std;

#include "engine/src/core/signal.h"
#include "engine/src/core/object_constructor.h"
#include "engine/src/node/scene_tree.h"
#include "engine/src/core/yaml_data_converter.h"
#include "engine/src/core/resource/resource.h"

// Forward declarations
class Resource;
class EngineTexture;

class Engine {
    public:
        Signal<float> SIGNAL_PROCESS;
        Signal<> SIGNAL_SCREEN_SIZE_CHANGED;

        Engine();
        void process(float delta);

        static Engine* getSingleton();

        SceneTree* getTree() { return scene_tree_singleton; }
        YAMLDataConverter* getYAMLDataConverter() { return yaml_data_converter_singleton; }

        template<typename ObjectType, typename... Arguments, typename MethodReturnType = void>
        static void callDeferred(MethodReturnType (ObjectType::*method)(Arguments...), ObjectType* object, Arguments... args) {
            Engine::getSingleton()->SIGNAL_PROCESS.connectWithoutArgs(method, object, true, args...);
        }

        // - Texture management -
        shared_ptr<EngineTexture> loadTexture(string file_path);
        bool isTextureLoaded(string file_path);

        // - Resource management -
        void resourceCreated(Resource* resource);
        void resourceDeleted(Resource* resource);

        // Debug
        void rebuildAndRun();

        // - ObjectConstructor -
        template<typename ObjectType>
        ObjectConstructor<ObjectType>* getObjectConstructor(string object_type_name) {
            ASSERT(isObjectTypeRegistered(object_type_name));
            return (ObjectConstructor<ObjectType>*)registered_object_constructors[object_type_name];
        }

        template<typename ObjectType, typename... ConstructorArgs>
        ObjectConstructor<ObjectType>* registerObjectType(string object_type_name) {
            ASSERT(!isObjectTypeRegistered(object_type_name));

            ObjectConstructor<ObjectType>* constructor = new ObjectConstructor<ObjectType>(getYAMLDataConverter());
            constructor->template init<ConstructorArgs...>();
            registered_object_constructors[object_type_name] = constructor;
            // onObjectRegisteredAsConstructor();

            return constructor;
        }

        bool isObjectTypeRegistered(string object_type_name) {
            return registered_object_constructors.count(object_type_name);
        }

        template<typename InheritedType>
        bool doesRegisteredObjectInheritType(string object_type_name) {
            ASSERT(isObjectTypeRegistered(object_type_name));
            return (registered_object_constructors[object_type_name])->inheritsNode();
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

        static string fatal_error;
        static bool fatal_error_occurred;
        static bool print_disabled;

        vector<Resource*> all_resources;

    private:
        static Engine* singleton;

        int current_node_id_max = 0;
        int global_node_count = 0;

        SceneTree* scene_tree_singleton = new SceneTree();
        YAMLDataConverter* yaml_data_converter_singleton = new YAMLDataConverter();

        thread::id main_thread_id = this_thread::get_id();
        unordered_map<string, ObjectConstructorBase*> registered_object_constructors;

        Vector2 previous_screen_size = Vector2::ZERO();

    public:
        // - Texture management -
        unordered_map<string, weak_ptr<EngineTexture>> loaded_textures;
        void onTextureContainerDeleted(string file_path, TEXTURE_TYPE texture);
        // void spriteAnimationSetDeleted(SpriteAnimationSet resource);

};

#endif