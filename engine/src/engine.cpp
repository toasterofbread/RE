#include "engine.h"

#include <raylib-cpp.hpp>
#include <memory>

#include "engine/src/utils.h"
#include "engine/src/core/resource/resource.h"
#include "engine/src/core/node/node.h"
#include "engine/src/core/object_constructor.h"
#include "engine/src/core/node/node_manager.h"
#include "engine/src/input/input_manager.h"
#include "engine/src/input/input_event.h"
#include "engine/src/core/node/node_types/animated_sprite.h"
#include "engine/src/engine_texture.h"
#include "engine/src/core/node/registration.cpp"

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
    return plusFile("/home/spectre7/Projects/raylib/RE/", absolute_path);
}

void Engine::ensureAsync() {
    if (this_thread::get_id() == main_thread_id) {
        warn("Thread is not asynchronous", true);
    }
}

void Engine::rebuildAndRun() {
    system("clear && cd /home/spectre7/Projects/raylib/RE && ./run.sh &");
    exit(0);
}

// - Texture management -

shared_ptr<EngineTexture> Engine::loadTexture(string file_path) {

    file_path = getResPath(file_path);
    shared_ptr<EngineTexture> ret;

    if (isTextureLoaded(file_path)) { // Texture is already loaded, just add the node to the userlist
        ret = (shared_ptr<EngineTexture>)loaded_textures[file_path];
    }
    else {
        ret = make_shared<EngineTexture>(LoadTexture(file_path.c_str()));
        loaded_textures[file_path] = ret;
    }

    return ret;
}

bool Engine::isTextureLoaded(string file_path) {
    return loaded_textures.count(getResPath(file_path));
}

// - Resource management -

void Engine::resourceCreated(Resource* resource) {
    all_resources.push_back(resource);
}

void Engine::resourceDeleted(Resource* resource) {
    vectorRemoveValue(&all_resources, resource);
}

// - InputEvent management -

void Engine::inputEventCreated(InputEvent* event) {
    all_inputevents.push_back(event);
}

void Engine::inputEvenDeleted(InputEvent* event) {
    vectorRemoveValue(&all_inputevents, event);
}