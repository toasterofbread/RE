using namespace std;

#include <vector>

#include "engine/src/core/resource/resource.h"

// Forward declarations
class Macro;

class InputEvent: public Resource {
    public:
        InputEvent(Node* initial_linked_node): Resource(initial_linked_node) {};
        InputEvent(Node* initial_linked_node, vector<Macro*> input_macros): Resource(initial_linked_node) { macros = input_macros; }

        bool isTriggered();
        bool isJustTriggered();
        bool isJustUntriggered();
    private:
        vector<Macro*> macros;
};