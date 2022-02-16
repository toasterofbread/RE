using namespace std;

#include <string>

class SamusState {
    public:
        virtual string getName() { return "Unnamed state"; }
        virtual void process(float delta);
};

class SamusStateStanding: public SamusState {
    public:
        string getName() { return "standing"; }
};