#include <raylib-cpp.hpp>
#include <string>

#include "engine/src/core/signal.h"

// !maybe move into Engine class
// Stores a Texture2D. One unique instance for each loaded texture.
class EngineTexture {
    public:
        Texture2D getTexture() { return texture; }
        string getFilePath() { return file_path; }
        EngineTexture(Texture2D _texture) { texture = _texture; }
        ~EngineTexture() {
            SIGNAL_DELETED->emit(file_path, texture);
            delete SIGNAL_DELETED;
        }
    
        int getWidth() { return texture.width; }
        int getHeight() { return texture.height; }
        Vector2 getSize() { return Vector2{(float)getWidth(), (float)getHeight()}; }

    private:

        Texture2D texture;
        string file_path;
        Signal<void, string, Texture2D>* SIGNAL_DELETED = new Signal<void, string, Texture2D>();
};