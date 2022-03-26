#ifndef INCLUDED_INPUT_EVENT
#define INCLUDED_INPUT_EVENT

#include <vector>
using namespace std;

// Forward declarations
class Macro;
template<typename A, typename... B>
class Signal;

class InputEvent {
    public:
        InputEvent() { init(); }
        InputEvent(vector<Macro*> input_macros) { macros = input_macros; init(); }

        bool isTriggered();
        bool isJustTriggered();
        bool isJustUntriggered();

        virtual void process(float delta);
        vector<Macro*> getMacros() { return macros; }

        Signal<void>* SIGNAL_TRIGGERED;
        Signal<void>* SIGNAL_UNTRIGGERED;
    private:
        void init();
        vector<Macro*> macros;
};

#endif
