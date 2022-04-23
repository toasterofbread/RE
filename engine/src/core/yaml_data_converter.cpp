#include "yaml_data_converter.h"

#include <yaml-cpp/yaml.h>

#include "common/utils.h"
#include "engine/src/core/resource/sprite_animation.h"
#include "engine/src/engine.h"

Vector2 YAMLDataConverter::convertToVector2(YAML::Node data, string type_name, string error_prefix) {
    Vector2 ret = Vector2::ZERO();

    // Convert and set X value
    try { ret.x = data[0].as<float>(); }
    catch (YAML::TypedBadConversion<float>) { 
        ASSERT_MSG(false, "Could not convert the first (X) value to float"); 
    }

    // Convert and set Y value
    try { ret.y = data[1].as<float>(); }
    catch (YAML::TypedBadConversion<float>) { 
        ASSERT_MSG(false, "Could not convert the second (Y) value to float"); 
    }

    return ret;
}

Vector3 YAMLDataConverter::convertToVector3(YAML::Node data, string type_name, string error_prefix) {
    Vector3 ret = Vector3::ZERO();

    // Convert and set X value
    try { ret.x = data[0].as<float>(); }
    catch (YAML::TypedBadConversion<float>) { 
        ASSERT_MSG(false, "Could not convert the first (X) value to float"); 
    }

    // Convert and set Y value
    try { ret.y = data[1].as<float>(); }
    catch (YAML::TypedBadConversion<float>) { 
        ASSERT_MSG(false, "Could not convert the second (Y) value to float"); 
    }

    // Convert and set Z value
    try { ret.z = data[2].as<float>(); }
    catch (YAML::TypedBadConversion<float>) { 
        ASSERT_MSG(false, "Could not convert the third (Z) value to float"); 
    }

    return ret;
}

shared_ptr<SpriteAnimationSet> YAMLDataConverter::convertToSpriteAnimationSet(YAML::Node data, string type_name, string error_prefix) {
    string file_path;
    string base_directory_override = "//";

    if (data.IsScalar()) {
        file_path = OS::getResPath(data.as<string>());
    }
    else { // Array

        ASSERT_MSG(!(data.size() > 2 || data.size() == 0), error_prefix + "SpriteAnimationSet as an array must be in the format [file_path, base_directory_override (optional)]");

        ASSERT_MSG(data[0].IsScalar(), error_prefix + "file_path (item 0) must be a single value")

        ASSERT_MSG(!(data.size() == 2 && !data[1].IsScalar()), error_prefix + "base_directory_override (item 1) must be a single value");

        file_path = data[0].as<string>();
        if (data.size() == 2) {
            base_directory_override = data[1].as<string>();
        }
    }

    string error_container;
    if (!SpriteAnimationSet::isFileValid(file_path, base_directory_override, &error_container)) {
        ASSERT_MSG(false, error_prefix + error_container);
        return NULL;
    }

    shared_ptr<SpriteAnimationSet> ret = SpriteAnimationSet::getInstance(file_path, base_directory_override);
    return ret;
}