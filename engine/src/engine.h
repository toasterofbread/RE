#ifndef INCLUDED_ENGINE
#define INCLUDED_ENGINE

#include <thread>
#include <unordered_map>
using namespace std;

#include "engine/src/core/signal.h"
#include "engine/src/core/node/node_manager.h"
#include "engine/src/input/input_manager.h"
#include "engine/src/core/object_constructor.h"
#include "engine/src/core/yaml_data_converter.h"

// Forward declarations
class InputEvent;
class Resource;
class NodeTexture;

class Engine {
    public:
        Engine();
        void process(float delta);

        Signal<void, string, bool>* SIGNAL_OBJECT_TYPE_REGISTERED = new Signal<void, string, bool>();

        NodeManager* getNodeManager() { return node_manager_singleton; }
        InputManager* getInputManager() { return input_manager_singleton; }
        YAMLDataConverter* getYAMLDataConverter() { return yaml_data_converter_singleton; }

        Texture2D loadTexture(string file_path, NodeTexture* node_texture);
        void unloadTexture(Texture2D texture, NodeTexture* node_texture);
        bool isTextureLoaded(string file_path);
        // bool isNodeUsingTexture() // !TODO

        void resourceCreated(Resource* resource);
        void resourceDeleted(Resource* resource);

        void inputEventCreated(InputEvent* event);
        void inputEvenDeleted(InputEvent* event);

        template<typename T>
        void requestDeletion(T* what) { delete what; }

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

            return constructor;
        }

        bool isObjectTypeRegistered(string object_type_name) {
            return registered_object_constructors.count(object_type_name);
        }

        template<typename InheritedType>
        bool doesObjectInheritType(string object_type_name) {
            if (!isObjectTypeRegistered(object_type_name)) {
                warn("Object '" + object_type_name + "' is not registered", true);
                return NULL;
            }
            return (registered_object_constructors[object_type_name])->inheritsType<InheritedType>();
        }


    private:
        NodeManager* node_manager_singleton = new NodeManager(this);
        InputManager* input_manager_singleton = new InputManager(this);
        YAMLDataConverter* yaml_data_converter_singleton = new YAMLDataConverter();

        unordered_map<string, Texture2D> loaded_textures;
        unordered_map<unsigned int, vector<NodeTexture*>> texture_resources;

        vector<Resource*> all_resources;
        vector<InputEvent*> all_inputevents;
        thread::id main_thread_id = this_thread::get_id();

        unordered_map<string, ObjectConstructorBase*> registered_object_constructors;
};

#endif