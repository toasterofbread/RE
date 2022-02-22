#include "engine/src/core/node/node_types/animated_sprite.h"
#include "engine/src/core/node/node.h"

#include "engine/src/core/object_constructor.h"

template<typename NodeType>
ObjectConstructor<NodeType>* Node::registerNodeProperties(string node_name, Engine* engine) {
    return getNodeConstructor<NodeType>(node_name, engine)
        ->template registerProperty<bool>("show_gizmos", &NodeType::setShowGizmos)
        ->template registerProperty<string>("name", &NodeType::setName)
        ->template registerProperty<Vector2>("position", &NodeType::setPosition)
        ->template registerProperty<Vector2>("scale", &NodeType::setScale)
        ->template registerProperty<float>("rotation", &NodeType::setRotation)
        ->template registerProperty<bool>("visible", &NodeType::setVisible)
        ;
}

template<typename NodeType>
ObjectConstructor<NodeType>* AnimatedSprite::registerNodeProperties(string node_name, Engine* engine) {
    return Node::registerNodeProperties<NodeType>(node_name, engine)
        ->template registerProperty<SpriteAnimationSet*>("animation_set", &NodeType::setAnimationSet)
        ->template registerProperty<string>("current_animation_key", &NodeType::setCurrentAnimationKey)
        ->template registerProperty<int>("current_frame", &NodeType::setCurrentFrame)
        ->template registerProperty<bool>("playing", &NodeType::setPlaying)
        ->template registerProperty<bool>("flip_x", &NodeType::setFlipX)
        ->template registerProperty<bool>("flip_y", &NodeType::setFlipY)
        ->template registerProperty<Vector2>("rotation_origin", &NodeType::setRotationOrigin)
        ->template registerProperty<bool>("rotate_around_center", &NodeType::setRotateAroundCetner)
        ;
}