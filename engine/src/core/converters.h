#include <unordered_map>
#include <functional>
#include <string>
#include <yaml-cpp/yaml.h>
#include <ponder/classbuilder.hpp>
#include <ponder/uses/runtime.hpp>

using namespace std;

// Forward declarations
class NodeManager;

string callNodeSetter(string type_name, YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager);

static string setString(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager);
static string setBool(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager);
static string setInt(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager);
static string setFloat(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager);
static string setVector2(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager);
static string setSpriteAnimationSet(YAML::Node data, ponder::UserObject node, const ponder::Function* setter, NodeManager* manager);
