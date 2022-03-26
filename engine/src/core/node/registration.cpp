#include "engine/src/core/node/node.h"
#include "engine/src/core/node/node_types/node_2d.h"
#include "engine/src/core/node/node_types/animated_sprite.h"
#include "engine/src/core/node/node_types/sprite.h"

#include "engine/src/core/object_constructor.h"

template<typename NodeType>
ObjectConstructor<NodeType>* Node::registerNodeProperties(string node_type_name) {

    if (!Engine::getSingleton()->isObjectTypeRegistered(node_type_name)) {
        return Engine::getSingleton()->registerObjectType<NodeType>(node_type_name);
    }

    ObjectConstructor<NodeType>* constructor = Engine::getSingleton()->getObjectConstructor<NodeType>(node_type_name);
    assert(constructor->template inheritsType<Node>());

    return constructor
        ->template registerProperty<string>("name", &NodeType::setName)
        ;
}

template<typename NodeType>
ObjectConstructor<NodeType>* Node2D::registerNodeProperties(string node_type_name) {
    return Node::registerNodeProperties<NodeType>(node_type_name)
        ->template registerProperty<bool>("show_gizmos", &NodeType::setShowGizmos)
        ->template registerProperty<Vector2>("position", &NodeType::setPosition)
        ->template registerProperty<Vector2>("scale", &NodeType::setScale)
        ->template registerProperty<float>("rotation", &NodeType::setRotation)
        ->template registerProperty<bool>("visible", &NodeType::setVisible)
        ;
}

template<typename NodeType>
ObjectConstructor<NodeType>* Sprite::registerNodeProperties(string node_type_name) {
    return Node2D::registerNodeProperties<NodeType>(node_type_name)
        ->template registerProperty<bool>("flip_x", &NodeType::setFlipX)
        ->template registerProperty<bool>("flip_y", &NodeType::setFlipY)
        ->template registerProperty<Vector2>("rotation_origin", &NodeType::setRotationOrigin)
        ->template registerProperty<bool>("rotate_around_center", &NodeType::setRotateAroundCetner)
        ;
}

template<typename NodeType>
ObjectConstructor<NodeType>* AnimatedSprite::registerNodeProperties(string node_type_name) {
    return Sprite::registerNodeProperties<NodeType>(node_type_name)
        ->template registerProperty<shared_ptr<SpriteAnimationSet>>("animation_set", &NodeType::setAnimationSet)
        ->template registerProperty<string>("current_animation_key", &NodeType::setCurrentAnimationKey)
        ->template registerProperty<int>("current_frame", &NodeType::setCurrentFrame)
        ->template registerProperty<bool>("playing", &NodeType::setPlaying)
        ;
}