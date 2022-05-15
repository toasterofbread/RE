#include "engine.h"

#include "engine/compiler_settings.h"
#include "defs.h"

#include <memory>

#include "common/utils.h"
#include "common/draw.h"
#include "engine/src/core/resource/resource.h"
#include "engine/src/node/node.h"
#include "engine/src/node/types/node_3d.h"
#include "engine/src/core/object_constructor.h"
#include "engine/src/node/scene_tree.h"
#include "engine/src/input/input_event.h"
#include "engine/src/node/types/animated_sprite.h"
#include "engine/src/engine_texture.h"
#include "engine/src/core/resource/sprite_animation.h"
#include "engine/src/physics/physics_server.h"
#include "engine/src/physics/node/physics_body_2d.h"
#include "engine/src/physics/node/collision_object_2d.h"
#include "node/types/camera_2d.h"
#include "node/types/camera_3d.h"

Engine* Engine::singleton = NULL;
string Engine::fatal_error = "No message provided";
bool Engine::fatal_error_occurred = false;
bool Engine::print_disabled = false;

Engine::Engine() {
    ASSERT(singleton == NULL);
    singleton = this;
    
    getTree()->init();

    // Create singletons
    new PhysicsServer();

    // Register nodes
    Node::registerNode();
    Node2D::registerNode();
    Node3D::registerNode();
    Sprite::registerNode();
    AnimatedSprite::registerNode();
    Camera2D::registerNode();
    Camera3D::registerNode();

    #if PHYSICS_2D_ENABLED
    PhysicsBody2D::registerNode();
    CollisionObject2D::registerNode();
    #endif
}

Engine* Engine::getSingleton() {
    if (singleton == NULL) {
        new Engine; // Engine constructor assigns itself to singleton
    }
    return singleton;
}

void Engine::process(float delta) {
    
    SIGNAL_PROCESS.emit(delta);

    PhysicsServer::getSingleton()->physicsProcess(delta); // !Temp 

    scene_tree_singleton->process(delta);

    for (auto i = all_resources.begin(); i != all_resources.end(); ++i) {
        (*i)->process(delta);
    }

    #if PLATFORM != PLATFORM_VITA
    if (previous_screen_size.isZero()) {
        previous_screen_size = OS::getScreenSize();
    }
    else if (previous_screen_size != OS::getScreenSize()) {
        SIGNAL_SCREEN_SIZE_CHANGED.emit();
    }
    #endif

    const int x_basis = OS::getScreenWidth();
    Draw::drawText("FPS " + stringPadDecimals(to_string(1.0f / GetFrameTime()), 1), Vector2(x_basis - 55, 10), Colour::GREEN());
    Draw::drawText("TIME " + stringPadDecimals(to_string(OS::getTime()), 1), Vector2(x_basis - 60, 25), Colour::GREEN());
    Draw::drawText("Resource count: " + to_string(all_resources.size()), Vector2(x_basis - 110, 45), Colour::BLACK());
    Draw::drawText("Node count: " + to_string(getGlobalNodeCount()), Vector2(x_basis - 110, 60), Colour::BLACK());
    Draw::drawText("Texture count: " + to_string(loaded_textures.size()), Vector2(x_basis - 110, 75), Colour::BLACK());
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
        ret->SIGNAL_DELETED.connect(this, &Engine::onTextureContainerDeleted);
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
    resource->SIGNAL_DELETED.connect(this, &Engine::resourceDeleted, false, resource);
}
void Engine::resourceDeleted(Resource* resource) {
    vectorRemoveValue(&all_resources, resource);
}

// - InputEvent management -

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