#include "projectinit.h"

#include <ponder/classbuilder.hpp>
#include <ponder/uses/runtime.hpp>

#include "project/src/nodes/samus/samus.h"
#include "engine/src/core/node/node_manager.h"
#include "engine/src/utils.h"
#include "engine/src/input/input.h"

#include "engine/src/input/macro.h"
#include "engine/src/input/input_event.h"

NodeManager* manager;
Samus* samus;

void printRootTree() {
    manager->getRoot()->printTree();
}

Macro* tree_macro = new Macro(vector<KEYBOARD_BUTTON>{TAB});
InputEvent* event;

PONDER_TYPE(Samus)

void samusReady(void* argument, Node* node) {
    print(node->getName());
    print(argument == manager);
}

void projectInit(NodeManager* node_manager) {
    Samus::declareSetters<Samus>("Samus");
    Samus::getNodeBuilder<Samus>("Samus").constructor<NodeManager*>();

    manager = node_manager;

    // addMacro(printRootTree, vector<KEYBOARD_BUTTON>{TAB}, "Print root node tree", false);

    samus = new Samus(manager);
    manager->addNode(samus);

    
    // !todo finish InputEvent
    // event = new InputEvent(samus, vector<Macro*>{new Macro(vector<KEYBOARD_BUTTON>{LCTRL, LSHIFT})});
}

void projectLoop(float delta) {
    if (tree_macro->isJustPressed()) {
        printRootTree();
    }
}

