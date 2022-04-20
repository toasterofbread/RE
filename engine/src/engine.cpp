#include "engine.h"

#include "engine/compiler_settings.h"
#include "defs.h"

#include <memory>

#include "common/utils.h"
#include "common/draw.h"
#include "engine/src/core/resource/resource.h"
#include "engine/src/node/node.h"
#include "engine/src/core/object_constructor.h"
#include "engine/src/node/scene_tree.h"
#include "engine/src/input/input_manager.h"
#include "engine/src/input/input_event.h"
#include "engine/src/node/types/animated_sprite.h"
#include "engine/src/engine_texture.h"
#include "engine/src/core/resource/sprite_animation.h"
#include "engine/src/physics/physics_server.h"
#include "engine/src/physics/node/physics_body.h"
#include "engine/src/physics/node/collision_shape.h"

Engine* Engine::singleton = NULL;
string Engine::fatal_error = "No message provided";
bool Engine::fatal_error_occurred = false;
bool Engine::print_disabled = false;

Engine::Engine() {
    ASSERT(singleton == NULL);
    singleton = this;
    
    // system("clear");

    getTree()->init();
    new InputManager();
    new PhysicsServer();

    Node::registerNodeProperties();
    Node2D::registerNodeProperties();
    Sprite::registerNodeProperties();
    AnimatedSprite::registerNodeProperties();
    PhysicsBody::registerNodeProperties();
    CollisionShape::registerNodeProperties();
}

Engine* Engine::getSingleton() {
    return singleton;
}

void Engine::process(float delta) {
    
    PhysicsServer::getSingleton()->physicsProcess(delta); // !Temp 

    #if INPUT_HAS_PROCESS
    InputManager::getSingleton()->process(delta);
    #endif

    SIGNAL_PROCESS.emit(delta);

    scene_tree_singleton->process(delta);

    for (auto i = all_resources.begin(); i != all_resources.end(); ++i) {
        (*i)->process(delta);
    }
    for (auto i = all_inputevents.begin(); i != all_inputevents.end(); ++i) {
        (*i)->process(delta);
    }

    Draw::drawText("FPS", Vector2(OS::getScreenWidth() - 85, 10), Colour::GREEN());
    Draw::drawText(("Resource count: " + (string)to_string(all_resources.size())).c_str(), Vector2(OS::getScreenWidth() - 125, 35), Colour::BLACK());
    Draw::drawText(("Node count: " + (string)to_string(getGlobalNodeCount())).c_str(), Vector2(OS::getScreenWidth() - 125, 50), Colour::BLACK());
    Draw::drawText(("Texture count: " + (string)to_string(loaded_textures.size())).c_str(), Vector2(OS::getScreenWidth() - 125, 65), Colour::BLACK());
}

void Engine::rebuildAndRun() {
    system("cd /home/spectre7/Projects/raylib/RE ; clear ; scons -j8 --run --clear-console --platform=raylib --target=release &");
    exit(0);
}

// - Texture management -

shared_ptr<EngineTexture> Engine::loadTexture(string file_path) {
    file_path = formatPath(file_path);
    shared_ptr<EngineTexture> ret;

    if (isTextureLoaded(file_path)) {
        ret = loaded_textures[file_path].lock();
    }
    else {
        ret = EngineTexture::create(OS::loadTexture(file_path), file_path);
        ret->SIGNAL_DELETED.connect(&Engine::onTextureContainerDeleted, this);
        loaded_textures.insert(make_pair<string, weak_ptr<EngineTexture>>(string(file_path), ret));
    }
    return ret;
}

bool Engine::isTextureLoaded(string file_path) {
    return loaded_textures.count(formatPath(file_path));
}

void Engine::onTextureContainerDeleted(string file_path, TEXTURE_TYPE texture) {
    loaded_textures.erase(file_path);
    OS::unloadTexture(texture);
}

// - Resource management -
void Engine::resourceCreated(Resource* resource) {
    all_resources.push_back(resource);
    resource->SIGNAL_DELETED.connect(&Engine::resourceDeleted, this, false, resource);
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