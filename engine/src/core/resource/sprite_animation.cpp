#include "sprite_animation.h"

#include <json.hpp>
using json = nlohmann::json;

#include "engine/src/utils.h"
#include "engine/src/core/node/node.h"
#include "engine/src/core/resource/node_texture.h"
#include "engine/src/core/node/node_manager.h"

// Debug
#include "icecream.hpp"

void SpriteAnimation::init(std::string animation_name, json* animation_data, json* file_data, Node* initial_linked_node, std::string load_directory) {
    name = animation_name;
    loop = (*animation_data)["loop"];
    framerate = (*animation_data)["speed"];

    for (auto it = frames.begin(); it != frames.end(); ++it) {
        initial_linked_node->getManager()->unloadTexture((*it)->getTexture(), *it);
    }
    frames.clear();

    for (auto item : (*animation_data)["frames"] ) {
        if (item.is_null()) { break; }
        // Texture2D texture = manager->loadTexture(plusFile(load_directory, (*file_data)[int2char(item)]));
        NodeTexture* texture = new NodeTexture(initial_linked_node, plusFile(load_directory, (*file_data)[int2char(item)]));
        frames.push_back(texture);
    }
    
}

NodeTexture* SpriteAnimation::getFrame(int frame_idx) {
    if (frame_idx >= getFrameCount()) {
        warn("Frame index out of bounds", true);
    }
    return frames[frame_idx];
}

int SpriteAnimation::getFrameCount() {
    return frames.size();
}

int SpriteAnimation::getFrameWidth() { 
    if (frames.empty()) {
        return 0;
    }
    return frames[0]->getWidth(); 
}

// - SpriteAnimationSet -

SpriteAnimationSet::SpriteAnimationSet(Node* initial_linked_node, string file_path, string base_directory_override): Resource(initial_linked_node) {
    SIGNAL_NODE_LINKED->connect(onNodeLinked, this);
    SIGNAL_NODE_UNLINKED->connect(onNodeUnlinked, this);

    loadFile(initial_linked_node, file_path, base_directory_override);
}

void SpriteAnimationSet::onNodeLinked(void* _self, Node* node) {
    SpriteAnimationSet* self = (SpriteAnimationSet*)_self;

    for( const std::pair<std::string, SpriteAnimation*>& n : self->getAnimations() ) {
        std::vector<NodeTexture*> animation_frames = n.second->getFrames();

        for (auto i = animation_frames.begin(); i != animation_frames.end(); ++i)
            (*i)->linkNode(node);
    }
}

void SpriteAnimationSet::onNodeUnlinked(void* _self, Node* node) {
    SpriteAnimationSet* self = (SpriteAnimationSet*)_self;
    
    for( const std::pair<std::string, SpriteAnimation*>& n : self->getAnimations() ) {
        std::vector<NodeTexture*> animation_frames = n.second->getFrames();

        for (auto i = animation_frames.begin(); i != animation_frames.end(); ++i)
            (*i)->unlinkNode(node);
    }
}

void SpriteAnimationSet::loadFile(Node* initial_linked_node, string path, string base_directory_override) {
    
    if (!isFileValid(path)) {
        warn("SpriteAnimationSet file is not valid");
        return;
    }
    
    file_path = path;
    data = json::parse(LoadFileText(getResPath(file_path).c_str()));

    string base_directory;
    if (base_directory_override != "//") {
        base_directory = base_directory_override;
    }
    else if (data.count("base_directory")) {
        base_directory = data["base_directory"];
    }

    // Delete existing animations and clear the map
    for (std::unordered_map<std::string, SpriteAnimation*>::iterator it=animations.begin(); it!=animations.end(); ++it) {
        delete it->second;
    }
    animations.clear();

    for (auto& i : data["animations"].items()) {
        std::string animation_key = i.key();
        json animation_data = data["animations"][animation_key];

        SpriteAnimation* animation = new SpriteAnimation;
        animation->init(animation_key, &animation_data, &data["files"], initial_linked_node, base_directory);
        animations[animation_key] = animation;
        // animations.insert(std::make_pair(animation_key, animation));
    }
}

bool SpriteAnimationSet::hasAnimation(string animation_key) {
    return animations.count(animation_key);
}
SpriteAnimation* SpriteAnimationSet::getAnimation(string animation_key) { // This function returns null. Input is definitely a valid string.
    if (!hasAnimation(animation_key)) {
        warn("SpriteAnimationSet does not contain the animation key:");
        print(animation_key);
        return NULL;
    }
    return animations[animation_key];
}

NodeTexture* SpriteAnimationSet::getFrame(string animation_key, int frame_idx) {
    if (!hasAnimation(animation_key)) {
        warn("SpriteAnimationSet does not contain the animation key: " + animation_key, true);
    }
    return getAnimation(animation_key)->getFrame(frame_idx);
}

bool SpriteAnimationSet::isFileValid(string file_path, string base_directory_override, string* error_container) {
    file_path = getResPath(file_path);
    if (!FileExists(file_path.c_str())) {
        if (error_container != NULL) *error_container = "No file exists at path '" + file_path + "'";
        return false;
    }

    json file_data = json::parse(LoadFileText(file_path.c_str()));

    if (file_data.count("base_directory") && !file_data["base_directory"].is_string()) {
        if (error_container != NULL) *error_container = "Base directory must be a string";
        return false;
    }

    if (!file_data.count("files")) {
        if (error_container != NULL) *error_container = "Must contain 'files' item (dictionary)";
        return false;
    }
    if (!file_data["files"].is_object()) {
        if (error_container != NULL) *error_container = "'files' item must be a dictionary";
        return false;
    }

    if (!file_data.count("animations")) {
        if (error_container != NULL) *error_container = "Must contain 'animations' item (dictionary)";
        return false;
    }
    if (!file_data["animations"].is_object()) {
        if (error_container != NULL) *error_container = "'animations' item must be a dictionary";
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
        if (!FileExists(getResPath(plusFile(base_directory, *i)).c_str())) {
            if (error_container != NULL) *error_container = "Texture at path '" + getResPath(plusFile(base_directory, *i)) + "' does not exist";
            return false;
        }
    }

    return true;
};