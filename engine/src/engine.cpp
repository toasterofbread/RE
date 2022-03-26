#include "engine.h"

#include <raylib-cpp.hpp>
#include <memory>

#include "engine/src/utils.h"
#include "engine/src/core/resource/resource.h"
#include "engine/src/core/node/node.h"
#include "engine/src/core/object_constructor.h"
#include "engine/src/core/node/scene_tree.h"
#include "engine/src/input/input_manager.h"
#include "engine/src/input/input_event.h"
#include "engine/src/core/node/node_types/animated_sprite.h"
#include "engine/src/engine_texture.h"
#include "engine/src/core/node/registration.cpp"
#include "engine/src/core/resource/sprite_animation.h"
#include "engine/src/physics/physics_server.h"

Engine* Engine::singleton = NULL;

Engine::Engine() {
    assert(singleton == NULL);
    singleton = this;
    
    system("clear");

    getTree()->init();
    new InputManager();
    new PhysicsServer();

    Node::registerNodeProperties<Node>(Node::getTypeName());
    Node2D::registerNodeProperties<Node2D>(Node2D::getTypeName());
    Sprite::registerNodeProperties<Sprite>(Sprite::getTypeName());
    AnimatedSprite::registerNodeProperties<AnimatedSprite>(AnimatedSprite::getTypeName());

}

Engine* Engine::getSingleton() {
    return singleton;
}

void Engine::process(float delta) {
    
    PhysicsServer::getSingleton()->physicsProcess(delta); // !Temp 
    InputManager::getSingleton()->process(delta);

    scene_tree_singleton->process(delta);

    for (auto i = all_resources.begin(); i != all_resources.end(); ++i) {
        (*i)->process(delta);
    }
    for (auto i = all_inputevents.begin(); i != all_inputevents.end(); ++i) {
        (*i)->process(delta);
    }

    DrawFPS(GetScreenWidth() - 85, 10);
    DrawText(("Resource count: " + (string)int2char(all_resources.size())).c_str(), GetScreenWidth() - 125, 35, 12, BLACK);
    DrawText(("Node count: " + (string)int2char(getGlobalNodeCount())).c_str(), GetScreenWidth() - 125, 50, 12, BLACK);
    DrawText(("Texture count: " + (string)int2char(loaded_textures.size())).c_str(), GetScreenWidth() - 125, 65, 12, BLACK);
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
    system("clear && cd /home/spectre7/Projects/raylib/RE && build build_and_run &");
    exit(0);
}

// - Texture management -

shared_ptr<EngineTexture> Engine::loadTexture(string file_path) {

    file_path = getResPath(file_path);
    shared_ptr<EngineTexture> ret;

    if (isTextureLoaded(file_path)) {
        ret = (shared_ptr<EngineTexture>)loaded_textures[file_path];
    }
    else {
        ret = make_shared<EngineTexture>(LoadTexture(file_path.c_str()), file_path);
        ret->SIGNAL_DELETED->connect(&Engine::onTextureContainerDeleted, this);
        loaded_textures[file_path] = ret;
    }

    return ret;
}

bool Engine::isTextureLoaded(string file_path) {
    return loaded_textures.count(getResPath(file_path));
}

void Engine::onTextureContainerDeleted(string file_path, Texture2D texture) {
    loaded_textures.erase(file_path);
    UnloadTexture(texture);
}

// - Resource management -
void Engine::resourceCreated(Resource* resource) {
    all_resources.push_back(resource);
    resource->SIGNAL_DELETED->connect(&Engine::resourceDeleted, this);
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

int Engine::getNewNodeId() {
    current_node_id_max++;
    return current_node_id_max;
}

void Engine::onNodeCreated(Node* node) {
    global_node_count++;
}

void Engine::onNodeKilled(Node* node) {
    global_node_count--;
}

int Engine::getGlobalNodeCount() {
    return global_node_count;
}