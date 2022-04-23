#include "node/types/node_3d.h"

class CameraBase {

    public:

        virtual bool isEnabled() = 0;
        virtual void enable() = 0;
        virtual void disable() = 0;

        virtual Vector3 getCenter() = 0;

        virtual void setZoom(float value) = 0;
        virtual float getZoom() = 0;

        void setEnableOnReady(bool value);
        bool getEnableOnReady();

    protected:
        
        template<typename NodeType>
        static void registerCameraBase(ObjectConstructor<NodeType>* c) {
            c->template registerProperty<bool>("enable_on_ready", &NodeType::setEnableOnReady);
        }

    private:

        bool enable_on_ready = false;

};