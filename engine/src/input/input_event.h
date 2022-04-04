#ifndef INCLUDED_INPUT_EVENT
#define INCLUDED_INPUT_EVENT

#include <vector>
using namespace std;

#include "engine/src/core/signal.h"

// Forward declarations
class Macro;

class InputEvent {
    public:
        Signal<> SIGNAL_TRIGGERED;
        Signal<> SIGNAL_UNTRIGGERED;

        InputEvent() { init(); }
        InputEvent(vector<Macro*> input_macros) { macros = input_macros; init(); }

        bool isTriggered();
        bool isJustTriggered();
        bool isJustUntriggered();

        virtual void process(float delta);
        vector<Macro*> getMacros() { return macros; }
    private:
        void init();
        vector<Macro*> macros;
};

#endif
