#ifndef INCLUDED_ENGINE_TEXTURE
#define INCLUDED_ENGINE_TEXTURE

#include "engine/src/raylib_include.h"
#include <string>

#include "engine/src/core/signal.h"

class Engine;

// !maybe move into Engine class
// Stores a Texture2D. One unique instance for each loaded texture.
class EngineTexture {
    public:
        Signal<string, Texture2D> SIGNAL_DELETED;

        Texture2D getTexture() { return texture; }
        string getFilePath() { assert(!generated); return file_path; }
        bool isGenerated() { return generated; }

        EngineTexture(Texture2D _texture, string _file_path) { texture = _texture; file_path = _file_path; }
        EngineTexture(Texture2D _texture) { texture = _texture; generated = true; }
        ~EngineTexture() {
            SIGNAL_DELETED.emit(file_path, texture);
        }
    
        int getWidth() { return texture.width; }
        int getHeight() { return texture.height; }
        Vector2 getSize() { return Vector2{(float)getWidth(), (float)getHeight()}; }

        // Generator methods
        static shared_ptr<EngineTexture> generateColour(Colour colour, int width = 1, int height = 1) {
            return make_shared<EngineTexture>(LoadTextureFromImage(GenImageColor(width, height, colour)));
        }

    private:
        Texture2D texture;
        string file_path = "";
        bool generated = false;

};

#endif