#include "engine.h"

#include <raylib-cpp.hpp>
#include <ponder/classbuilder.hpp>
#include <ponder/uses/runtime.hpp>

#include "engine/src/core/node/node_manager.h"
#include "engine/src/core/node/animated_sprite.h"
#include "engine/src/core/resource/sprite_animation.h"

PONDER_TYPE(Vector2)
PONDER_TYPE(NodeManager)
PONDER_TYPE(AnimatedSprite)
PONDER_TYPE(SpriteAnimationSet)
Engine::Engine(NodeManager* node_manager) {
    ponder::Class::declare<Vector2>("Vector2");
    ponder::Class::declare<NodeManager>("NodeManager");
    Node::declareSetters<Node>("Node");
    Node::getNodeBuilder<Node>("Node").constructor<NodeManager*>();
    AnimatedSprite::declareSetters<AnimatedSprite>("AnimatedSprite");
    Node::getNodeBuilder<AnimatedSprite>("AnimatedSprite").constructor<NodeManager*>();

    manager = node_manager;
}

void Engine::process(float delta) {
    manager->process(delta);
}