#ifndef INCLUDED_ENGINE_TEXTURE
#define INCLUDED_ENGINE_TEXTURE

#include <raylib-cpp.hpp>
#include <string>

#include "engine/src/core/signal.h"

// !maybe move into Engine class
// Stores a Texture2D. One unique instance for each loaded texture.
class EngineTexture {
    public:
        Texture2D getTexture() { return texture; }
        string getFilePath() { return file_path; }
        EngineTexture(Texture2D _texture, string _file_path) { texture = _texture; file_path = _file_path; }
        ~EngineTexture() {
            SIGNAL_DELETED->emit(file_path, texture);
            delete SIGNAL_DELETED;
        }
    
        int getWidth() { return texture.width; }
        int getHeight() { return texture.height; }
        Vector2 getSize() { return Vector2{(float)getWidth(), (float)getHeight()}; }
        Signal<void, string, Texture2D>* SIGNAL_DELETED = new Signal<void, string, Texture2D>();

    private:

        Texture2D texture;
        string file_path;
};

#endif