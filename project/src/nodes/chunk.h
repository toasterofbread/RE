#ifndef INCLUDED_CHUNK
#define INCLUDED_CHUNK

#include "block.h"
#include "world.h"
#include "entity.h"

#include "physics/node/physics_body_3d.h"

struct Chunk: public PhysicsBody3D {
    REGISTER_NODE(Chunk, PhysicsBody3D, {});

    // Emitted when an entity enters while none are contained, or when the last contained entity exits
    Signal<> SIGNAL_ENTITY_CONTAINED_CHANGED;

    Chunk* next = NULL;
    Chunk* previous = NULL;

    SubChunk* sub_chunks[SUBCHUNK_COUNT];
    
    Block* blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
    Block* getBlock(int x, int y, int z);
    Block* getBlock(Vector3 position);
    
    Block::TYPE getBlockType(int x, int y, int z);
    Block::TYPE getBlockType(Vector3 position);
    
    void setup(Vector2 grid_pos, Chunk* chunks[CHUNK_AMOUNT][CHUNK_AMOUNT]);
    void createBlocks();

    Vector2 getGridPos() { return grid_position; }

    BoundingBox bounding_box;

    Chunk* getNeighbour(DIRECTION direction);
    void setNeighbour(DIRECTION direction, Chunk* chunk);

    void onNeighbourEntityContainedChanged(Chunk* neighbour);
    void onEntityEntered(Entity* entity);
    void onEntityExited(Entity* entity);

    Chunk* getFront();
    Chunk* getBack();
    Chunk* getLeft();
    Chunk* getRight();

    int getContainedEntityCount();
    list<Entity*> contained_entities;

    private:
        Vector2 grid_position;
        Matrix transform;
        bool mesh_loaded = false;

        Chunk* front = NULL;
        Chunk* back = NULL;
        Chunk* left = NULL;
        Chunk* right = NULL;
};

struct SubChunk: public CollisionObject3D  {

    REGISTER_NODE(SubChunk, CollisionObject3D, {});
    
    struct ChunkMesh {
        void addVertex(const Vector3& vertex, const int width, const int height, const Vector3& normal) {
            // Texture width, height, and rotation are passed using the built-in normals array
            mesh.normals[current_vertex] = width;
            mesh.normals[current_vertex + 1] = height;
            mesh.normals[current_vertex + 2] = DEG2RAD(normal.z != 0 ? 270 : 180);

            mesh.vertices[current_vertex++] = vertex.x;
            mesh.vertices[current_vertex++] = vertex.y;
            mesh.vertices[current_vertex++] = vertex.z;
        }
        
        void addIndex(const int index) {
            mesh.indices[current_index++] = index;
        }

        void addFace(Vector3 v1, Vector3 v2, Vector3 v3, Vector3 v4, Vector3 normal_mask, int normal, Block::TYPE block_type, int width, int height) {
            const int vertex_start = current_vertex / 3;

            addVertex(v1, width, height, normal_mask);
            addVertex(v2, width, height, normal_mask);
            addVertex(v3, width, height, normal_mask);
            addVertex(v4, width, height, normal_mask);

            const int texcoords[4][2] = {
                {1, 0}, {1, 1}, {0, 0}, {0, 1}
            };

            const int* block_texcoords = Block::type_data[(int)block_type].texcoords[(int)convertDirection(normal_mask)];

            for (int i = 0; i < 4; i++) {
                mesh.texcoords[current_texcoord] = texcoords[i][0];
                mesh.texcoords[current_texcoord + 1] = texcoords[i][1];

                mesh.texcoords2[current_texcoord++] = block_texcoords[0];
                mesh.texcoords2[current_texcoord++] = block_texcoords[1];
            }

            if (normal == -1) {
                addIndex(vertex_start + 2);
                addIndex(vertex_start + 1);
                addIndex(vertex_start);
                
                addIndex(vertex_start + 3);
                addIndex(vertex_start + 1);
                addIndex(vertex_start + 2);
            }
            else {
                addIndex(vertex_start);
                addIndex(vertex_start + 1);
                addIndex(vertex_start + 2);
                
                addIndex(vertex_start + 2);
                addIndex(vertex_start + 1);
                addIndex(vertex_start + 3);
            }

            mesh.vertexCount += 4;
            mesh.triangleCount += 2;
        }
        
        void reset() {
            mesh.triangleCount = 0;
            mesh.vertexCount = 0;
            current_vertex = 0;
            current_index = 0;
            current_texcoord = 0;
        }
        
        void resize(unsigned int _max_faces) {
            if (_max_faces == max_faces)
                return;
            max_faces = _max_faces;
            
            if (!allocated) {
                mesh.vertices = (float*)malloc(max_faces * 4 * 3 * sizeof(float));
                mesh.normals = (float*)malloc(max_faces * 4 * 3 * sizeof(float));
                mesh.texcoords = (float*)malloc(max_faces * 4 * 2 * sizeof(float));
                mesh.texcoords2 = (float*)malloc(max_faces * 4 * 2 * sizeof(float));
                mesh.indices = (unsigned short*)malloc(max_faces * 6 * sizeof(unsigned short));
                allocated = true;
            }
            else {
                mesh.vertices = (float*)realloc(mesh.vertices, max_faces * 4 * 3 * sizeof(float));
                mesh.normals = (float*)realloc(mesh.normals, max_faces * 4 * 3 * sizeof(float));
                mesh.texcoords = (float*)realloc(mesh.vertices, max_faces * 4 * 2 * sizeof(float));
                mesh.texcoords2 = (float*)realloc(mesh.vertices, max_faces * 4 * 2 * sizeof(float));
                mesh.indices = (unsigned short*)realloc(mesh.vertices, max_faces * 6 * sizeof(unsigned short));
            }
        }
        
        void updateBuffer() {
            auto vertices = mesh.vertices;
            mesh.vertices = 0;
            auto normals = mesh.normals;
            mesh.normals = 0;
            auto texcoords = mesh.texcoords;
            mesh.texcoords = 0;
            auto texcoords2 = mesh.texcoords2;
            mesh.texcoords2 = 0;
            auto indices = mesh.indices;
            mesh.indices = 0;
            
            UnloadMesh(mesh);
            
            mesh.vaoId = 0;
            
            mesh.vertices = vertices;
            mesh.normals = normals;
            mesh.texcoords = texcoords;
            mesh.texcoords2 = texcoords2;
            mesh.indices = indices;
            
            upload();

            // UpdateMeshBuffer(mesh, 0, mesh.vertices, SIZE(mesh.vertices), 0);
            // UpdateMeshBuffer(mesh, 1, mesh.texcoords, SIZE(mesh.texcoords), 0);
            // UpdateMeshBuffer(mesh, 6, mesh.indices, SIZE(mesh.indices), 0);
        }
        
        void upload() {
            UploadMesh(&mesh, false);
        }

        operator Mesh() {
            return mesh;
        }
        
        ChunkMesh() {
            mesh.vertices = NULL;
        }
        
        int max_faces = 0;
        
        private:
            bool allocated = false;
            Mesh mesh = { 0 };
            int current_vertex = 0;
            int current_index = 0;
            int current_texcoord = 0;
        
    };
    ChunkMesh mesh;
    
    Chunk* chunk;
    int index;

    bool drawn = false;

    void setup(Chunk* _chunk, int _index);
    bool isVisible(Camera3D* camera, Vector3 chunk_position);

    bool isAxisOpaque(AXIS axis);
    void drawChunk();

    BoundingBox bounding_box;

    Vector3 getCenter();
    void requestMeshGeneration();
    
    static int getAxisLength(int axis);

    private:
        friend class Block;

        typedef GridShape3D<CHUNK_SIZE, SUBCHUNK_HEIGHT, CHUNK_SIZE> GridType;
        shared_ptr<GridType> collision_shape = Shape::create<GridType>();

        void generateBoundingBox();
        bool mesh_loaded = false;
        bool pending_generation = false;

        friend class World;
        void generateMesh();
        void onMeshGenerationFinished();
};

#endif