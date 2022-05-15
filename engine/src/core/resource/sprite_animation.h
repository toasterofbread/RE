#include <memory>
#include <json.hpp>
using json = nlohmann::json;

#include "engine/src/engine.h"
#include "engine/src/core/resource/resource.h"

class SpriteAnimation: public Resource {
    private:
        string name = "Unnamed SpriteAnimation";
        bool loop = false;
        int framerate = 30;
        vector<shared_ptr<EngineTexture>> frames;
    public:
        SpriteAnimation(string animation_name, json animation_data, json file_data, string load_directory);
        ~SpriteAnimation() {
            frames.clear();
        }

        string getName() { return name; }
        bool getLoop() { return loop; }
        int getFramerate() { return framerate; }
        vector<shared_ptr<EngineTexture>> getFrames() { return frames; }
        shared_ptr<EngineTexture> getFrame(int frame_idx);
        
        int getFrameCount();
        bool hasFrame(int frame_idx);

        int getFrameWidth(int frame_idx = 0);
        int getFrameHeight(int frame_idx = 0);
        Vector2 getFrameSize(int frame_idx = 0);
};

class SpriteAnimationSet: public Resource {
    private:
        friend struct LocalResourcePool;
        struct LocalResourcePool {
            template<typename MapType, typename KeyType>
            void eraseMapKey(MapType* map, KeyType key) {
                map->erase(key);
            }
            shared_ptr<SpriteAnimationSet> getResource(string file_path, string base_directory_override = "//");
            private:
                unordered_map<string, unordered_map<string, weak_ptr<SpriteAnimationSet>>> pool;
        };
        
        static LocalResourcePool resource_pool;

    public:

        static shared_ptr<SpriteAnimationSet> getInstance(string _file_path, string _base_directory_override = "//") {
            return resource_pool.getResource(_file_path, _base_directory_override);
        }

        bool hasAnimation(string animation_key);
        bool hasAnimation(vector<string> animation_keys);

        shared_ptr<SpriteAnimation> getAnimation(string animation_key);
        shared_ptr<SpriteAnimation> getAnimation(vector<string> animation_keys);

        shared_ptr<EngineTexture> getFrame(string animation_key, int frame_idx);
        shared_ptr<EngineTexture> getFrame(vector<string> animation_keys, int frame_idx);

        int getTreeDepth() { return tree_depth; }

        static bool isFileValid(string file_path, string base_directory_override = "//", string* error_container = NULL);
        string file_path;
        string base_directory_override;

    private:

        json data;
        class AnimationContainer;
        shared_ptr<AnimationContainer> animation_container;

        int tree_depth = 1;

        SpriteAnimationSet(string _file_path, string _base_directory_override = "//");
        void loadFile(string _file_path, string _base_directory_override = "//");

        class AnimationContainer {
            
            public:
                bool isFinalLayer() { return is_final_layer; }

                AnimationContainer(int tree_depth, json data, json file_data, string base_directory) {
                    is_final_layer = tree_depth == 1;
                    
                    if (is_final_layer) {
                        for (auto i : data.items()) {
                            string animation_key = i.key();
                            json animation_data = data[animation_key];
                            shared_ptr<SpriteAnimation> animation = make_shared<SpriteAnimation>(animation_key, animation_data, file_data, base_directory);
                            auto pair = make_pair(animation_key, animation);
                            animations.insert(pair);
                        }
                    }
                    else {
                        for (auto i : data.items()) {
                            string animation_key = i.key();
                            containers.insert(make_pair(animation_key, make_shared<AnimationContainer>(tree_depth - 1, data[animation_key], file_data, base_directory)));
                        }
                    }
                }

                shared_ptr<AnimationContainer> getSubContainer(string key) {
                    ASSERT(!is_final_layer);
                    return containers[key];
                }

                shared_ptr<SpriteAnimation> getSubAnimation(string key) {
                    ASSERT(is_final_layer);
                    return animations[key];
                }

                shared_ptr<SpriteAnimation> getKeys(vector<string> keys) {
                    ASSERT(hasKeys(keys));
                    if (is_final_layer) {
                        return getSubAnimation(keys[0]);
                    }
                    else {
                        shared_ptr<AnimationContainer> sub_container = getSubContainer(keys[0]);
                        keys.erase(keys.begin());
                        return sub_container->getKeys(keys);
                    }
                }

                bool hasKey(string key) {
                    if (is_final_layer) { 
                        return animations.count(key);
                    }
                    else {
                        return containers.count(key);
                    }
                }

                bool hasKeys(string key) {
                    return is_final_layer && animations.count(key);
                }

                bool hasKeys(vector<string> keys) {
                    
                    if (keys.size() <= 0) {
                        return false;
                    }

                    if (is_final_layer) {
                        if (keys.size() != 1) {
                            return false;
                        }
                        return animations.count(keys[0]);
                    }
                    else {
                        if (containers.count(keys[0])) {
                            shared_ptr<AnimationContainer> sub_container = containers[keys[0]];
                            keys.erase(keys.begin());
                            return sub_container->hasKeys(keys);
                        }
                        return false;
                    }
                }
            
            private:
                bool is_final_layer = false;

                unordered_map<string, shared_ptr<AnimationContainer>> containers;
                unordered_map<string, shared_ptr<SpriteAnimation>> animations;
        };
};