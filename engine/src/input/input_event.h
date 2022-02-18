#ifndef INCLUDED_INPUT_EVENT
#define INCLUDED_INPUT_EVENT

#include <vector>
using namespace std;

// Forward declarations
class Macro;
template<typename A, typename... B>
class Signal;
class Engine;

class InputEvent {
    public:
        InputEvent(Engine* engine_singleton) { engine = engine_singleton; init(); }
        InputEvent(Engine* engine_singleton, vector<Macro*> input_macros) { engine = engine_singleton; macros = input_macros; init(); }

        bool isTriggered();
        bool isJustTriggered();
        bool isJustUntriggered();

        virtual void process(float delta);
    
        Signal<void>* SIGNAL_TRIGGERED;
        Signal<void>* SIGNAL_UNTRIGGERED;
    private:
        void init();
        vector<Macro*> macros;


        Engine* engine;
};

#endif
