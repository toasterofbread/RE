#ifndef SAMUS_STATE_INCLUDED
#define SAMUS_STATE_INCLUDED

#include <string>
using namespace std;

// Forward declarations
class Samus;
class NodeManager;

class SamusState {   
    public:
        SamusState(Samus* samus_node);

        virtual string getName() { return "Unnamed state"; }
        virtual void process(float delta);
    
        Samus* getSamus() { return samus; }
        NodeManager* getManager() { return manager; }

        virtual void stateEnabled(SamusState* previous_state) {};
        virtual void stateDisabled(SamusState* next_state) {};
        
    private:
        Samus* samus;
        NodeManager* manager;
};

class SamusStateStand: public SamusState {
    public:
        SamusStateStand(Samus* samus_node): SamusState(samus_node) {}
        string getName() { return "stand"; }
        void process(float delta);
};

class SamusStateRun: public SamusState {
    public:
        SamusStateRun(Samus* samus_node): SamusState(samus_node) {}
        string getName() { return "run"; }
        void process(float delta);
};

#endif