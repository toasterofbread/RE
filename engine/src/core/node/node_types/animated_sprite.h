#include "raylib-cpp.hpp"
#include <iostream>
#include "json.hpp"
using json = nlohmann::json;

#include "engine/src/core/node/node.h"

// Forward declarations
class AnimatedSprite;
class SpriteAnimation;
class SpriteAnimationSet;
class Engine;

class AnimatedSprite: public Node {
    private:
        SpriteAnimationSet* animation_set = NULL;
        string current_animation_key = "";

        int current_frame = 0;
        float frame_timer = 0.0f;

        bool playing = false;
        bool flip_x = false;
        bool flip_y = false;
        Vector2 rotation_origin = Vector2{0, 0};
        bool rotate_around_center = false;

        void process(float delta);
    public:
        AnimatedSprite(Engine* engine_singleton): Node(engine_singleton) { name = getTypeName(); }
        virtual string getTypeName() {return "AnimatedSprite";}
        
        template<typename NodeType>
        static ObjectConstructor<NodeType>* registerNodeProperties(string node_name, Engine* engine) {
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

        void play(string animation_key = "");
        bool hasAnimation(string animation_key);
        SpriteAnimation* getCurrentAnimation();

        void setAnimationSet(SpriteAnimationSet* value);
        SpriteAnimationSet* getAnimationSet() { return animation_set; }
        void setCurrentAnimationKey(string value);
        string getCurrentAnimationKey() { return current_animation_key; }
        void setCurrentFrame(int value) { current_frame = value; }
        int getCurrentFrame() { return current_frame; }
        void setPlaying(bool value) {playing = value;}
        bool getPlaying() {return playing;}
        void setFlipX(bool value) {flip_x = value;}
        bool getFlipX() {return flip_x;}
        void setFlipY(bool value) {flip_y = value;}
        bool getFlipY() {return flip_y;}
        void setRotationOrigin(Vector2 value) { rotation_origin = value; }
        Vector2 getRotationOrigin() { return rotation_origin; }
        void setRotateAroundCetner(bool value) { rotate_around_center = value; }
        bool getRotateAroundCenter() { return rotate_around_center; }
};