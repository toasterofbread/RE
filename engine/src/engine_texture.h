#ifndef INCLUDED_ENGINE_TEXTURE
#define INCLUDED_ENGINE_TEXTURE

#include <string>

#include "engine/src/core/signal.h"
#include "common/draw.h"
#include "common/vector2.h"

class Engine;

// !maybe move into Engine class
// Stores a Texture2D. One unique instance for each loaded texture.
class EngineTexture {
    public:
        Signal<string, TEXTURE_TYPE> SIGNAL_DELETED;

        TEXTURE_TYPE getTexture() { return texture; }
        string getFilePath() { ASSERT(!generated); return file_path; }
        bool isGenerated() { return generated; }

        ~EngineTexture() {
            SIGNAL_DELETED.emit(file_path, texture);
        }
    
        int getWidth() { return OS::getTextureWidth(texture); }
        int getHeight() { return OS::getTextureHeight(texture); }
        Vector2 getSize() { return Vector2{(float)getWidth(), (float)getHeight()}; }

        static shared_ptr<EngineTexture> create(TEXTURE_TYPE _texture) {
            return shared_ptr<EngineTexture>(new EngineTexture(_texture));
        }

        static shared_ptr<EngineTexture> create(TEXTURE_TYPE _texture, string _file_path) {
            return shared_ptr<EngineTexture>(new EngineTexture(_texture, _file_path));
        }

        static shared_ptr<EngineTexture> generate(int width = 1, int height = 1) {
            return shared_ptr<EngineTexture>(new EngineTexture(OS::generateTexture(width, height)));
        }

    private:
        TEXTURE_TYPE texture;
        string file_path = "";
        bool generated = false;

        EngineTexture(TEXTURE_TYPE _texture, string _file_path) { texture = _texture; file_path = _file_path; }
        EngineTexture(TEXTURE_TYPE _texture) { texture = _texture; generated = true; }
};

#endif