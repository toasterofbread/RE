#include "engine/src/core/node/node.h"
#include "engine.h"

#include <raylib-cpp.hpp>

#include "engine/src/utils.h"
#include "engine/src/core/node/node_manager.h"
#include "engine/src/input/input_manager.h"
#include "engine/src/input/input_event.h"
#include "engine/src/core/node/node_types/animated_sprite.h"
#include "engine/src/core/resource/sprite_animation.h"

#include "engine/src/core/object_constructor.h"

Engine::Engine() {
    getNodeManager()->init();
    getInputManager()->init();
    Node::registerNodeProperties<Node>("Node", this);
    AnimatedSprite::registerNodeProperties<AnimatedSprite>("AnimatedSprite", this);
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

string Engine::getResPath(string absolute_path) {
    return plusFile("/home/spectre7/Projects/raylib/SSG/", absolute_path);
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