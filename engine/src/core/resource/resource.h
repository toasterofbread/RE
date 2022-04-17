#ifndef RESOURCE_INCLUDED
#define RESOURCE_INCLUDED

#include <vector>
#include <memory>
using namespace std;

#include "engine/src/core/signal.h"

// Forward declarations
class Node;
class Engine;

#define REGISTER_RESOURCE(name, pool) \
private: \
bool registered() { return getName() == name::getNameStatic(); } \
string getName() { return name::getNameStatic(); } \
static string getNameStatic() { return #name; } \
public: \
static pool* getPool() { \
    if (resource_pool == NULL) { \
        resource_pool = new pool; \
    } \
    return (pool*)resource_pool; \
}

class Resource {
    public:
        ~Resource();

        virtual void process(float delta) {};
        Signal<> SIGNAL_DELETED;

        struct ResourcePool {
            template<typename MapType, typename KeyType>
            void eraseMapKey(MapType* map, KeyType key) {
                map->erase(key);
            }
        };

    protected:
        static ResourcePool* resource_pool;
        Resource();
    
    private:

        virtual string getName() {
            return Resource::getNameStatic();
        }

        static string getNameStatic() {
            return "Resource";
        }

        virtual bool registered() {
            return getName() == Resource::getNameStatic();
        }
};

#endif