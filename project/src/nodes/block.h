#ifndef INCLUDED_BLOCK
#define INCLUDED_BLOCK

#include "core/resource/shape/shape.h"
#include "common/enums.h"

// Forward declarations
class Chunk;
class SubChunk;
class Player;

struct Block {

    // Chunk-relative coordinates
    int x; int y; int z;
    
    // Global coordinates
    int global_x; int global_y; int global_z;

    enum class TYPE {
        AIR, GRASS, DIRT, STONE, INVALID
    };
    static const int TYPE_COUNT = (int)TYPE::INVALID;

    static int getTypeCount() {
        return (int)TYPE::INVALID;
    }

    struct TypeData {
        bool populated = false;

        bool tangible = true;

        int** texcoords = NULL;
        BoundingBox* col_box = NULL;
        BoundingBox* int_box = NULL;

        weak_ptr<Shape3D> collision_shape;

        Texture2D icon;

        ~TypeData() {
            if (col_box)
                delete col_box;
            if (int_box)
                delete int_box;
            
            if (texcoords) {
                for (int i = 0; i < DIRECTION_3_COUNT; i++) {
                    delete texcoords[i];
                }
                delete texcoords;
            }

            UnloadTexture(icon);
        }
    };
    static TypeData type_data[TYPE_COUNT];
    static void loadTypeData();

    static string getTypeName(TYPE type);
    static TYPE getTypeFromString(string name);

    Chunk* chunk;
    SubChunk* subchunk;

    Block(Chunk* _chunk, int _x, int _y, int _z);

    Block* getNeighbour(DIRECTION_3 dir);

    Vector3 getCenter() const;
    bool isTransparent() const;
    bool isTangible() const;

    virtual bool isInteractable() const { return false; };
    virtual void interact(Player* player) {}

    void setType(TYPE value);
    TYPE getType() const;

    TypeData& getTypeData() const;
    static TypeData& getTypeData(TYPE type);

    DIRECTION_3 getNearestFace(Vector3 point);

    int* getFaceTexcoords(DIRECTION_3 face);

    RayCollision checkInteractionRay(Ray& ray, Vector3 chunk_position);

    RayCollision checkInteractionRay(Ray& ray);

    shared_ptr<Shape3D> getTypeCollisionShape();

    Block* getLeft();
    Block* getRight();

    Block* getDown();
    Block* getUp();

    Block* getFront();
    Block* getBack();

    private:
        TYPE type = TYPE::GRASS;
        static bool type_data_loaded;
};

#endif