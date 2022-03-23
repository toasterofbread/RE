#ifndef INCLUDED_SPRITE
#define INCLUDED_SPRITE

#include "engine/src/core/node/node_types/node_2d.h"
#include "engine/src/engine_texture.h"

#include "raylib-cpp.hpp"
#include <memory>
using namespace std;

class Sprite: public Node2D {

    public:
        Sprite(Engine* engine_singleton): Node2D(engine_singleton) { name = getTypeName(); }
        virtual string getTypeName() {return "Sprite";}

        void process(float delta);

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

        void setModulate(Color value) { modulate = value; }
        Color getModulate() { return modulate; }

    private:
        shared_ptr<EngineTexture> texture = NULL;

        bool flip_x = false;
        bool flip_y = false;
        Vector2 rotation_origin = Vector2{0, 0};
        bool rotate_around_center = false;
        Color modulate = RAYWHITE;
};

#endif