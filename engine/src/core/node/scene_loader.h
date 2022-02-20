#include <yaml-cpp/yaml.h>
#include <string>
using namespace std;

// Forward declarations
class Node;
class Engine;

class SceneLoader {

    public:
        static Node* loadSceneFromFile(string file_path, Engine* engine, Node* root_node_override = NULL, YAML::Node* config_container = NULL);
        static void processSceneConfig(YAML::Node& config, Node* root_node);

    private:
        static Node* synthesiseNode(string node_type, YAML::Node& node_data, Engine* engine, Node* root_node_override = NULL);
        static Node* instanceNodeFromType(string& node_type, YAML::Node& node_data, Engine* engine, bool suppress_warning = false, Node* root_node = NULL);

};
