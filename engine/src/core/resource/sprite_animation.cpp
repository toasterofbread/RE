#include "sprite_animation.h"

#include <unordered_map>
#include <json.hpp>
using json = nlohmann::json;
using namespace std;

#include "engine/src/node/node.h"
#include "common/utils.h"
#include "engine/src/engine.h"
#include "engine/src/engine_texture.h"
#include "engine/src/node/scene_tree.h"
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

    ASSERT_MSG(animation_data.count("s") || animation_data.count("speed"), "SpriteAnimation with name '" + animation_name + "' has no speed/framerate value");
    if (animation_data.count("s")) {
        framerate = animation_data["s"];
    }
    else {
        framerate = animation_data["speed"];
    }

    frames.clear();

    json frames_data;
    
    ASSERT_MSG(animation_data.count("f") || animation_data.count("frames"), "SpriteAnimation with name '" + animation_name + "' has no frames value");
    if (animation_data.count("f")) {
        frames_data = animation_data["f"];
    }
    else {
        frames_data = animation_data["frames"];
    }

    for (auto item : frames_data ) {
        if (item.is_null()) { 
            break; 
        }
        frames.push_back(Engine::getSingleton()->loadTexture(plusFile(load_directory, file_data[to_string(item)].get<string>())));
    }
}

shared_ptr<EngineTexture> SpriteAnimation::getFrame(int frame_idx) {
    ASSERT_MSG(hasFrame(frame_idx), "Frame index " + to_string(frame_idx) + " is out of bounds");
    return frames[frame_idx];
}

int SpriteAnimation::getFrameCount() {
    return frames.size();
}

bool SpriteAnimation::hasFrame(int frame_idx) {
    return frame_idx >= 0 && frame_idx < getFrameCount();
}

int SpriteAnimation::getFrameWidth(int frame_idx) { 
    ASSERT_MSG(hasFrame(frame_idx), "Frame index " + to_string(frame_idx) + " is out of bounds");
    return frames[frame_idx]->getWidth(); 
}

int SpriteAnimation::getFrameHeight(int frame_idx) { 
    ASSERT_MSG(hasFrame(frame_idx), "Frame index " + to_string(frame_idx) + " is out of bounds");
    return frames[frame_idx]->getHeight(); 
}

Vector2 SpriteAnimation::getFrameSize(int frame_idx) { 
    ASSERT_MSG(hasFrame(frame_idx), "Frame index " + to_string(frame_idx) + " is out of bounds");
    return frames[frame_idx]->getSize(); 
}

// - SpriteAnimationSet -

SpriteAnimationSet::SpriteAnimationSet(string _file_path, string _base_directory_override): Resource() {
    loadFile(_file_path, _base_directory_override);
}

void SpriteAnimationSet::loadFile(string _file_path, string base_directory_override) {
    
    ASSERT_MSG(isFileValid(_file_path), "SpriteAnimationSet file is not valid");

    file_path = _file_path;
    base_directory_override = base_directory_override;
    
    data = OS::loadJsonFile(file_path);

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

    json animations = data["animations"];
    json files = data["files"];

    animation_container.reset();
    animation_container = make_shared<AnimationContainer>(tree_depth, animations, files, base_directory);

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
        return false;
    }
    return animation_container->hasKeys(animation_keys);
}

shared_ptr<SpriteAnimation> SpriteAnimationSet::getAnimation(string animation_key) {
    ASSERT_MSG(hasAnimation(animation_key), "SpriteAnimationSet does not contain the animation key '" + animation_key + "'");
    return animation_container->getSubAnimation(animation_key);
}

shared_ptr<SpriteAnimation> SpriteAnimationSet::getAnimation(vector<string> animation_keys) {
    ASSERT_MSG(hasAnimation(animation_keys), "SpriteAnimationSet does not contain the animation key set [" + strVector2str(animation_keys) + "]");
    return animation_container->getKeys(animation_keys);
}

shared_ptr<EngineTexture> SpriteAnimationSet::getFrame(string animation_key, int frame_idx) {
    ASSERT_MSG(hasAnimation(animation_key), "SpriteAnimationSet does not contain the animation key '" + animation_key + "'");
    return getAnimation(animation_key)->getFrame(frame_idx);
}

shared_ptr<EngineTexture> SpriteAnimationSet::getFrame(vector<string> animation_keys, int frame_idx) {
    ASSERT_MSG(hasAnimation(animation_keys), "SpriteAnimationSet does not contain the animation key set [" + strVector2str(animation_keys) + "]");
    return getAnimation(animation_keys)->getFrame(frame_idx);
}

bool SpriteAnimationSet::isFileValid(string file_path, string base_directory_override, string* error_container) {
    
    if (!OS::fileExists(file_path)) {
        if (error_container != NULL) *error_container = "No file exists at path '" + file_path + "'";
        return false;
    }

    json file_data = OS::loadJsonFile(file_path);

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
        if (!OS::fileExists(plusFile(base_directory, *i))) {
            if (error_container != NULL) *error_container = "Texture at path '" + plusFile(base_directory, *i) + "' does not exist";
            return false;
        }
    }

    return true;
};

shared_ptr<SpriteAnimationSet> SpriteAnimationSet::LocalResourcePool::getResource(string file_path, string base_directory_override) {
    
    if (pool.count(file_path) && pool[file_path].count(base_directory_override)) {
        return (shared_ptr<SpriteAnimationSet>)pool[file_path][base_directory_override];
    }

    // Can't use make_shared as SpriteAnimationSet constructor is private
    shared_ptr<SpriteAnimationSet> ret = shared_ptr<SpriteAnimationSet>(new SpriteAnimationSet(file_path, base_directory_override));

    ret->SIGNAL_DELETED.connect(&ResourcePool::eraseMapKey, this, false, &pool, file_path);

    if (pool.count(file_path)) {
        pool[file_path][base_directory_override] = (weak_ptr<SpriteAnimationSet>)ret;
    }
    else {
        pool[file_path] = unordered_map<string, weak_ptr<SpriteAnimationSet>>{{base_directory_override, (weak_ptr<SpriteAnimationSet>)ret}};
    }

    return ret;
}