#include "sprite_animation.h"
#include "utils.h"
#include "json.hpp"
using json = nlohmann::json;
#include "icecream.hpp"

void SpriteAnimation::init(const char* animation_name, json* animation_data, json* file_data, Node* host, const char* load_directory) {
    name = animation_name;
    loop = (*animation_data)["loop"];
    framerate = (*animation_data)["speed"];

    for (auto it = frames.begin(); it != frames.end(); ++it) {
        host->unloadTexture(*it);
    }
    frames.clear();

    for (auto item : (*animation_data)["frames"] ) {
        if (item.is_null()) { break; }
        Texture2D* texture = host->loadTexture(plusFile(load_directory, (*file_data)[int2char(item)]));
        frames.push_back(texture);
    }
    
}

Texture2D* SpriteAnimation::getFrame(int frame_idx) {
    if (frame_idx >= getFrameCount()) {
        warn("Frame index out of bounds", true);
        return NULL;
    }
    return frames[frame_idx];
}

int SpriteAnimation::getFrameCount() {
    return frames.size();
}

// - SpriteAnimationSet -
void SpriteAnimationSet::loadFile(const char* path, const char* load_directory) {
    file_path = path;
    data = json::parse(LoadFileText(getResPath(file_path)));

    // Delete existing animations and clear the map
    for (std::unordered_map<const char*, SpriteAnimation*>::iterator it=animations.begin(); it!=animations.end(); ++it) {
        delete it->second;
    }
    animations.clear();

    for (auto& i : data["animations"].items()) {
        std::string animation_key = i.key();
        const char* animation_key_char = string2char(animation_key);
        json animation_data = data["animations"][animation_key];

        SpriteAnimation* animation = new SpriteAnimation;
        animation->init(animation_key_char, &animation_data, &data["files"], host, load_directory);
        animations.insert(std::make_pair(animation_key_char, animation));
    }
}

bool SpriteAnimationSet::hasAnimation(const char* animation_key) {
    int count = animations.count(animation_key);
    // print("COUNT: ", int2char(count));
    // return animations.count(animation_key) > 0;
    return true; // TEMP
}
SpriteAnimation* SpriteAnimationSet::getAnimation(const char* animation_key) {
    if (!hasAnimation(animation_key)) {
        warn("SpriteAnimationSet does not contain the animation key: ", animation_key);
        return NULL;
    }
    return animations[animation_key];
}

Texture2D* SpriteAnimationSet::getFrame(const char* animation_key, int frame_idx) {
    if (!hasAnimation(animation_key)) {
        warn("SpriteAnimationSet does not contain the animation key: ", animation_key);
        return NULL;
    }
    return getAnimation(animation_key)->getFrame(frame_idx);
}