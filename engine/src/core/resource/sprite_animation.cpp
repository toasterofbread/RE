#include "sprite_animation.h"

#include <json.hpp>
using json = nlohmann::json;

#include "engine/src/core/node/node.h"
#include "common/utils.h"
#include "engine/src/engine.h"
#include "engine/src/engine_texture.h"
#include "engine/src/core/node/scene_tree.h"
#include "engine/src/core/signal.h"

SpriteAnimation::SpriteAnimation(string animation_name, json animation_data, json file_data, string load_directory): Resource() {
    name = animation_name;

    if (animation_data.count("loop"))  {
        loop = animation_data["loop"];
    }
    else if (animation_data.count("l")) {
        loop = animation_data["l"];
    }
    else {
        loop = false;
    }

    if (animation_data.count("speed")) {
        framerate = animation_data["speed"];
    }
    else if (animation_data.count("s")) {
        framerate = animation_data["s"];
    }
    else {
        warn("SpriteAnimation with name '" + animation_name + "' has no speed/framerate value", true);
    }

    frames.clear();

    json frames_data;
    if (animation_data.count("frames")) {
        frames_data = animation_data["frames"];
    }
    else if (animation_data.count("f")) {
        frames_data = animation_data["f"];
    }
    else {
        warn("SpriteAnimation with name '" + animation_name + "' has no frames value", true);
    }

    for (auto item : frames_data ) {
        if (item.is_null()) { break; }
        frames.push_back(Engine::getSingleton()->loadTexture(plusFile(load_directory, file_data[int2char(item)])));
    }
}

shared_ptr<EngineTexture> SpriteAnimation::getFrame(int frame_idx) {
    if (!hasFrame(frame_idx)) {
        warn("Frame index out of bounds", true);
    }
    return frames[frame_idx];
}

int SpriteAnimation::getFrameCount() {
    return frames.size();
}

bool SpriteAnimation::hasFrame(int frame_idx) {
    return frame_idx >= 0 && frame_idx < getFrameCount();
}

int SpriteAnimation::getFrameWidth(int frame_idx) { 
    if (!hasFrame(frame_idx)) {
        warn("Frame index out of bounds", true);
    }
    return frames[frame_idx]->getWidth(); 
}

int SpriteAnimation::getFrameHeight(int frame_idx) { 
    if (!hasFrame(frame_idx)) {
        warn("Frame index out of bounds", true);
    }
    return frames[frame_idx]->getHeight(); 
}

Vector2 SpriteAnimation::getFrameSize(int frame_idx) { 
    if (!hasFrame(frame_idx)) {
        warn("Frame index out of bounds", true);
    }
    return frames[frame_idx]->getSize(); 
}

// - SpriteAnimationSet -

SpriteAnimationSet::SpriteAnimationSet(string _file_path, string _base_directory_override): Resource() {
    loadFile(_file_path, _base_directory_override);
}

void SpriteAnimationSet::loadFile(string _file_path, string base_directory_override) {
    
    if (!isFileValid(_file_path)) {
        warn("SpriteAnimationSet file is not valid");
        return;
    }

    file_path = _file_path;
    base_directory_override = base_directory_override;
    data = json::parse(LoadFileText(Engine::getResPath(file_path).c_str()));

    string base_directory;
    if (base_directory_override != "//") {
        base_directory = base_directory_override;
    }
    else if (data.count("base_directory")) {
        base_directory = data["base_directory"];
    }

    if (data.count("animation_tree_depth")) {
        tree_depth = data["animation_tree_depth"];
    }    
    else {
        tree_depth = 1;
    }

    animation_container.reset();
    animation_container = make_shared<AnimationContainer>(tree_depth, data["animations"], data["files"], base_directory);

    // for (auto& i : data["animations"].items()) {
    //     string animation_key = i.key();

    //     shared_ptr<SpriteAnimation> animation = make_shared<SpriteAnimation>(engine, animation_key, data["animations"][animation_key], data["files"], base_directory);
    //     animations[animation_key] = animation;
    //     // animations.insert(make_pair(animation_key, animation));
    // }
}

bool SpriteAnimationSet::hasAnimation(string animation_key) {
    if (animation_container->isFinalLayer()) {
        return animation_container->hasKey(animation_key);
    }
    return false;
}
bool SpriteAnimationSet::hasAnimation(vector<string> animation_keys) {
    if (getTreeDepth() != animation_keys.size()) {
        // warn("animation_keys size (" + int2str(animation_keys.size()) + ") doesn't match the animation tree depth (" + int2str(getTreeDepth()) + ")", true);
        return false;
    }
    return animation_container->hasKeys(animation_keys);
}

shared_ptr<SpriteAnimation> SpriteAnimationSet::getAnimation(string animation_key) {
    if (!hasAnimation(animation_key)) {
        warn("SpriteAnimationSet does not contain the animation key '" + animation_key + "'");
        return NULL;
    }
    return animation_container->getSubAnimation(animation_key);
}

shared_ptr<SpriteAnimation> SpriteAnimationSet::getAnimation(vector<string> animation_keys) {
    if (!hasAnimation(animation_keys)) {
        warn("SpriteAnimationSet does not contain the animation key set [" + strVector2str(animation_keys) + "]");
        return NULL;
    }
    return animation_container->getKeys(animation_keys);
}

shared_ptr<EngineTexture> SpriteAnimationSet::getFrame(string animation_key, int frame_idx) {
    if (!hasAnimation(animation_key)) {
        warn("SpriteAnimationSet does not contain the animation key: " + animation_key, true);
    }
    return getAnimation(animation_key)->getFrame(frame_idx);
}

shared_ptr<EngineTexture> SpriteAnimationSet::getFrame(vector<string> animation_keys, int frame_idx) {
    if (!hasAnimation(animation_keys)) {
        warn("SpriteAnimationSet does not contain the animation key set [" + strVector2str(animation_keys) + "]");
    }
    return getAnimation(animation_keys)->getFrame(frame_idx);
}

bool SpriteAnimationSet::isFileValid(string file_path, string base_directory_override, string* error_container) {
    file_path = Engine::getResPath(file_path);
    if (!FileExists(file_path.c_str())) {
        if (error_container != NULL) *error_container = "No file exists at path '" + file_path + "'";
        return false;
    }

    json file_data = json::parse(LoadFileText(file_path.c_str()));

    if (!file_data.count("files")) {
        if (error_container != NULL) *error_container = "Must contain 'files' (dictionary)";
        return false;
    }
    if (!file_data["files"].is_object()) {
        if (error_container != NULL) *error_container = "'files' must be a dictionary";
        return false;
    }

    if (!file_data.count("animations")) {
        if (error_container != NULL) *error_container = "Must contain 'animations' (dictionary)";
        return false;
    }
    if (!file_data["animations"].is_object()) {
        if (error_container != NULL) *error_container = "'animations' must be a dictionary";
        return false;
    }

    if (file_data.count("base_directory") && !file_data["base_directory"].is_string()) {
        if (error_container != NULL) *error_container = "'base_directory' must be a string";
        return false;
    }

    if (file_data.count("animation_tree_depth") && (!file_data["animation_tree_depth"].is_number_integer() || file_data["animation_tree_depth"] <= 0)) {
        if (error_container != NULL) *error_container = "'animation_tree_depth' must be a positive integer";
        return false;
    }

    string base_directory;
    if (base_directory_override != "//") {
        base_directory = base_directory_override;
    }
    else if (file_data.count("base_directory")) {
        base_directory = file_data["base_directory"];
    }

    for (auto i = file_data["files"].begin(); i != file_data["files"].end(); ++i) {
        if (!FileExists(Engine::getResPath(plusFile(base_directory, *i)).c_str())) {
            if (error_container != NULL) *error_container = "Texture at path '" + Engine::getResPath(plusFile(base_directory, *i)) + "' does not exist";
            return false;
        }
    }

    return true;
};