#include "engine.h"

#include <raylib-cpp.hpp>
#include <ponder/classbuilder.hpp>
#include <ponder/uses/runtime.hpp>

#include "engine/src/utils.h"
#include "engine/src/core/node/node_manager.h"
#include "engine/src/input/input_manager.h"
#include "engine/src/input/input_event.h"
#include "engine/src/core/node/animated_sprite.h"
#include "engine/src/core/resource/sprite_animation.h"

#include "engine/src/core/node/constructor.h"

PONDER_TYPE(Vector2)
PONDER_TYPE(Engine)
PONDER_TYPE(AnimatedSprite)
PONDER_TYPE(SpriteAnimationSet)
Engine::Engine() {

    getNodeManager()->init();
    getInputManager()->init();

    ponder::Class::declare<Vector2>("Vector2");
    ponder::Class::declare<Engine>("Engine");
    Node::declareSetters<Node>("Node");
    Node::getNodeBuilder<Node>("Node").constructor<Engine*>();
    AnimatedSprite::declareSetters<AnimatedSprite>("AnimatedSprite");
    Node::getNodeBuilder<AnimatedSprite>("AnimatedSprite").constructor<Engine*>();


    // ObjectConstructor<Node>* constructor = registerObjectForConstruction<Node>("Node");

    // constructor->registerProperty<Vector2>("position", &Node::setPosition);

    // Node* new_node = constructor->createInstance(this);
    
    // print(new_node->getPosition());

    // constructor->setProperty(new_node, "position", Vector2{69, 420});

    // print(new_node->getPosition());

    // print(constructor->inheritsType<Node>());
    // print(constructor->inheritsType<Vector2>());
    

    

}

void Engine::process(float delta) {
    node_manager_singleton->process(delta);
    input_manager_singleton->process(delta);

    for (auto i = all_resources.begin(); i != all_resources.end(); ++i) {
        (*i)->process(delta);
    }
    for (auto i = all_inputevents.begin(); i != all_inputevents.end(); ++i) {
        (*i)->process(delta);
    }
}

void Engine::ensureAsync() {
    if (this_thread::get_id() == main_thread_id) {
        warn("Thread is not asynchronous", true);
    }
}

void Engine::rebuildAndRun() {
    system("clear && cd /home/spectre7/Projects/raylib/SSG && ./run.sh &");
    exit(0);
}

// - Resource loading / unloading -

Texture2D Engine::loadTexture(string file_path, NodeTexture* node_texture) {

    string path = getResPath(file_path);
    Texture2D texture;

    if (isTextureLoaded(file_path)) { // Texture is already loaded, just add the node to the userlist
        texture = loaded_textures[path];

        if (!vectorContainsValue(&texture_resources[texture.id], node_texture)) {
            texture_resources[texture.id].push_back(node_texture);
        }
    }
    else {
        texture = LoadTexture(string2char(path));
        loaded_textures[path] = texture;;
        texture_resources[texture.id] = vector<NodeTexture*>{node_texture};
    }

    return texture;
}

void Engine::unloadTexture(Texture2D texture, NodeTexture* node_texture) {
}

bool Engine::isTextureLoaded(string file_path) {
    return loaded_textures.count(getResPath(file_path)) > 0;
}

void Engine::resourceCreated(Resource* resource) {
    all_resources.push_back(resource);
}

void Engine::resourceDeleted(Resource* resource) {
    vectorRemoveValue(&all_resources, resource);
}

void Engine::inputEventCreated(InputEvent* event) {
    all_inputevents.push_back(event);
}

void Engine::inputEvenDeleted(InputEvent* event) {
    vectorRemoveValue(&all_inputevents, event);
}