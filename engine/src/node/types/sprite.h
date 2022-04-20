#ifndef INCLUDED_SPRITE
#define INCLUDED_SPRITE

#include "engine/src/node/types/node_2d.h"
#include "engine/src/engine_texture.h"

#include <memory>
using namespace std;

class Sprite: public Node2D {
    
    public:

        REGISTER_NODE(Sprite, Node2D, {
            c->template registerProperty<bool>("flip_x", &NodeType::setFlipX)
            ->template registerProperty<bool>("flip_y", &NodeType::setFlipY)
            ->template registerProperty<Vector2>("rotation_origin", &NodeType::setRotationOrigin)
            ->template registerProperty<bool>("rotate_around_center", &NodeType::setRotateAroundCetner);
        });

        // void process(float delta);
        void draw();

        bool hasTexture() { return texture != NULL; }
        void setTexture(shared_ptr<EngineTexture> value) { texture = value; }
        shared_ptr<EngineTexture> getTexture() { return texture; }

        void setFlipX(bool value) { flip_x = value; }
        bool getFlipX() { return flip_x; }
        
        void setFlipY(bool value) {flip_y = value; }
        bool getFlipY() { return flip_y; }
        
        void setRotationOrigin(Vector2 value) { rotation_origin = value; }
        Vector2 getRotationOrigin() { return rotation_origin; }
        
        void setRotateAroundCetner(bool value) { rotate_around_center = value; }
        bool getRotateAroundCenter() { return rotate_around_center; }

    private:
        shared_ptr<EngineTexture> texture = NULL;

        bool flip_x = false;
        bool flip_y = false;
        Vector2 rotation_origin = Vector2{0, 0};
        bool rotate_around_center = false;
};

#endif