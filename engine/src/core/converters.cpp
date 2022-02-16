#include "converters.h"

#include <raylib-cpp.hpp>

#include "engine/src/utils.h"
#include "engine/src/core/resource/sprite_animation.h"

unordered_map<string, vector<string>> conversion_data_types = {
    {"string", {"single value"}},
    {"bool", {"single value"}},
    {"int", {"single value"}},
    {"float", {"single value"}},
    {"Vector2", {"array"}},
    {"SpriteAnimationSet", {"single value", "array"}}
};

string callNodeSetter(string type_name, YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager) {

    if (!conversion_data_types.count(type_name)) {
        warn("Type " + type_name + " isn't registered", true);
        return "";
    }

    if (conversion_data_types[type_name].size() > 0) {
        bool valid_type = false;
        string error_message = "Data type must be one of the following:";
        
        for (auto i = conversion_data_types[type_name].begin(); i != conversion_data_types[type_name].end(); ++i) {
            switch (str2int(i->c_str())) {
                case str2int("single value"): {if (data.IsScalar()) {valid_type = true;} break;}
                case str2int("array"): {if (data.IsSequence()) {valid_type = true;} break;}
                case str2int("map"): {if (data.IsMap()) {valid_type = true;} break;}
            }
            if (valid_type) {
                break;
            }

            if (i != conversion_data_types[type_name].begin()) {
                error_message += ",";
            }
            error_message += " " + *i;
        }

        if (!valid_type) {
            return error_message;
        }
    }

    switch (str2int(type_name.c_str())) {
        case str2int("string"): return setString(data, node, setter, manager);
        case str2int("bool"): return setBool(data, node, setter, manager);
        case str2int("int"): return setInt(data, node, setter, manager);
        case str2int("float"): return setFloat(data, node, setter, manager);
        case str2int("Vector2"): return setVector2(data, node, setter, manager);
        case str2int("SpriteAnimationSet"): return setSpriteAnimationSet(data, node, setter, manager);
    };
    return "";
}

static string setString(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager) {
    try {
        ponder::runtime::call(*setter, node, data.as<string>());
    }
    catch (YAML::TypedBadConversion<string>) {
        return "Could not convert value to string";
    }
    return "";
}
static string setBool(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager) {
    try {
        ponder::runtime::call(*setter, node, data.as<bool>());
    }
    catch (YAML::TypedBadConversion<bool>) {
        return "Could not convert value to bool";
    }
    return "";
}
static string setInt(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager) {
    try {
        ponder::runtime::call(*setter, node, data.as<int>());
    }
    catch (YAML::TypedBadConversion<int>) {
        return "Could not convert value to int";
    }
    return "";
}
static string setFloat(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager) {
    try {
        ponder::runtime::call(*setter, node, data.as<float>());
    }
    catch (YAML::TypedBadConversion<float>) {
        return "Could not convert value to float";
    }
    return "";
}

PONDER_TYPE(Vector2)
static string setVector2(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager) {
    if (data.size() != 2) {
        return "Must contain two numbers (X, Y)";
    }

    Vector2 value;

    // Convert and set X value
    try { value.x = data[0].as<float>(); }
    catch (YAML::TypedBadConversion<float>) { return "Could not convert the first (X) value to float"; }

    // Convert and set Y value
    try { value.y = data[1].as<float>(); }
    catch (YAML::TypedBadConversion<float>) { return "Could not convert the second (Y) value to float"; }

    ponder::runtime::call(*setter, node, value);

    return "";
}

PONDER_TYPE(SpriteAnimationSet)
static string setSpriteAnimationSet(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager) {
    
    string file_path;
    string base_directory_override = "//";

    if (data.IsScalar()) {
        file_path = getResPath(data.as<string>());
    }
    else { // Array
        if (data.size() > 2 || data.size() == 0) {
            return "SpriteAnimationSet as an array must be in the format [file_path, base_directory_override (optional)]";
        }

        if (!data[0].IsScalar()) {
            return "file_path (item 0) must be a single value";
        }
        if (data.size() == 2 && !data[1].IsScalar()) {
            return "base_directory_override (item 1) must be a single value";
        }

        file_path = data[0].as<string>();
        if (data.size() == 2) {
            base_directory_override = data[1].as<string>();
        }
    }

    string error_container;
    if (!SpriteAnimationSet::isFileValid(file_path, base_directory_override, &error_container)) {
        return error_container;
    }

    SpriteAnimationSet* set = new SpriteAnimationSet(&node.ref<Node>(), file_path, base_directory_override);
    ponder::runtime::call(*setter, node, set);

    return "";
}