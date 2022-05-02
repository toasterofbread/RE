#include "world.h"

#include "common/draw.h"
#include "node/types/camera_3d.h"

#include <ode/ode.h>

void SubChunk::addFace(DIRECTION_3 face, Block* block, int face_i) {

    const int texcoords[6][2] = {
        {1, 0}, {0, 0}, {1, 1},
        {0, 0}, {0, 1}, {1, 1}
    };

    #define ADD_FACE(vertex_data) {\
    const int vertices[6][3] = vertex_data; \
    const int* block_texcoords = block->getFaceTexcoords(face); \
    int v = 0; \
    for (int vertex = 0; vertex < 6; vertex++) { \
        int v_i = (face_i * 6 + vertex) * 3; \
        if (v_i + 2 >= allocated_vertices * 3) { \
            break; \
        } \
        mesh.vertices[v_i] = vertices[vertex][0] + block->x; \
        mesh.vertices[v_i + 1] = vertices[vertex][1] + block->y; \
        mesh.vertices[v_i + 2] = vertices[vertex][2] + block->z; \
        mesh.texcoords[(face_i * 6 + vertex) * 2] = (texcoords[vertex][0] + block_texcoords[0]) / TEXTURE_MAP_WIDTH; \
        mesh.texcoords[(face_i * 6 + vertex) * 2 + 1] = (texcoords[vertex][1] + block_texcoords[1]) / TEXTURE_MAP_HEIGHT; \
    } \
    break; }

    switch (face) {

        case DIRECTION_3::UP:
            #define VERTICES { \
                {1, 0, 0}, {0, 0, 0}, {1, 0, 1}, \
                {0, 0, 0}, {0, 0, 1}, {1, 0, 1}, \
            }
            ADD_FACE(VERTICES);
        case DIRECTION_3::DOWN:
            #undef VERTICES
            #define VERTICES { \
                {0, -1, 0}, {1, -1, 0}, {1, -1, 1}, \
                {0, -1, 1}, {0, -1, 0}, {1, -1, 1}, \
            }
            ADD_FACE(VERTICES);
        case DIRECTION_3::LEFT:
            #undef VERTICES
            #define VERTICES { \
                {0, 0, 1}, {0, 0, 0}, {0, -1, 1}, \
                {0, 0, 0}, {0, -1, 0}, {0, -1, 1}, \
            }
            ADD_FACE(VERTICES);
        case DIRECTION_3::RIGHT:
            #undef VERTICES
            #define VERTICES { \
                {1, 0, 0}, {1, 0, 1}, {1, -1, 0}, \
                {1, 0, 1}, {1, -1, 1}, {1, -1, 0}, \
            }
            ADD_FACE(VERTICES);
        case DIRECTION_3::FRONT:
            #undef VERTICES
            #define VERTICES { \
                {0, 0, 0}, {1, 0, 0}, {0, -1, 0}, \
                {1, 0, 0}, {1, -1, 0}, {0, -1, 0} \
            }
            ADD_FACE(VERTICES);
        case DIRECTION_3::BACK:
            #undef VERTICES
            #define VERTICES { \
                {1, 0, 1}, {0, 0, 1}, {1, -1, 1}, \
                {0, 0, 1}, {0, -1, 1}, {1, -1, 1} \
            }
            ADD_FACE(VERTICES);
        
        default:
            #undef VERTICES
            #define VERTICES { \
                {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, \
                {0, 0, 0}, {0, 0, 0}, {0, 0, 0} \
            }
            ADD_FACE(VERTICES);
    }
}

void SubChunk::init(Chunk* _chunk, int _index) {
    chunk = _chunk;
    index = _index;
    mesh = { 0 };
    mesh.vertices = NULL;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = SUBCHUNK_HEIGHT * index; y < SUBCHUNK_HEIGHT * (index + 1); y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                chunk->getBlock(x, y, z)->subchunk = this;
            }
        }
    }

    generateBoundingBox();
    generateMesh();
}

Vector3 SubChunk::getCenter() {
    return Vector3((chunk->getGridPos().x + 0.5f) * CHUNK_SIZE, (index + 0.5f) * SUBCHUNK_HEIGHT, (chunk->getGridPos().y + 0.5f) * CHUNK_SIZE);
}

void Chunk::setup(Vector2 grid_pos, Chunk* chunks[CHUNK_AMOUNT][CHUNK_AMOUNT]) {
    grid_position = grid_pos;
    setPosition(Vector3((grid_pos * CHUNK_SIZE).x, 0, (grid_pos * CHUNK_SIZE).y));

    front = grid_position.y < CHUNK_AMOUNT - 1 ? chunks[(int)grid_position.x][(int)grid_position.y + 1] : NULL;
    back = grid_position.y > 0 ? chunks[(int)grid_position.x][(int)grid_position.y - 1] : NULL;
    left = grid_position.x > 0 ? chunks[(int)grid_position.x - 1][(int)grid_position.y] : NULL;
    right = grid_position.x < CHUNK_AMOUNT - 1 ? chunks[(int)grid_position.x + 1][(int)grid_position.y] : NULL;

    bounding_box.min = Vector3(grid_position.x * CHUNK_SIZE, 0.0f, grid_position.y * CHUNK_SIZE);
    bounding_box.max = Vector3((grid_position.x + 1) * CHUNK_SIZE, CHUNK_HEIGHT, (grid_position.y + 1) * CHUNK_SIZE);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks[x][y][z] = new Block(this, x, y, z);
            }
        }
    }

    Vector3 position = getPosition();
    for (int i = 0; i < SUBCHUNK_COUNT; i++) {
        SubChunk* sub = new SubChunk;
        sub->init(this, i);

        // !todo y pos
        sub->setPosition(Vector3(position.x, position.y + i * SUBCHUNK_HEIGHT, position.z));

        world->addChild(sub);
        sub_chunks[i] = sub;
    }
}

Block* Chunk::getBlock(int x, int y, int z, bool allow_nonexistent) {
    Block* ret = blocks[x][y][z];
    if (allow_nonexistent || ret->isBlock()) {
        return ret;
    }
    return NULL;
}

void SubChunk::generateMesh() {

    double start_time = OS::getTime();

    #define ITERATE_BLOCKS(code) \
    for (int x = 0; x < CHUNK_SIZE; x++) { \
        for (int y = SUBCHUNK_HEIGHT * index; y < SUBCHUNK_HEIGHT * (index + 1); y++) { \
            for (int z = 0; z < CHUNK_SIZE; z++) { \
                code; \
            } \
        } \
    }


    mesh.triangleCount = 0;
    ITERATE_BLOCKS({
        Block* block = chunk->getBlock(x, y, z);
        if (block == NULL) {
            continue;
        }

        for (int dir = 0; dir < DIRECTION_3_COUNT; dir++) {
            if (block->get((DIRECTION_3)dir) == NULL) {
                mesh.triangleCount += 2;
            }

            // if (block->get((DIRECTION_3)dir) == NULL) {
            //     for (int offset = 0; offset < DIRECTION_COUNT; offset++) {
            //         Block* neighbour = block->get(relativeDirection((DIRECTION_3)dir, (DIRECTION_3)offset));
            //         if (neighbour == NULL || neighbour->get((DIRECTION_3)dir) != NULL) {
            //             mesh.triangleCount += 2;
            //             break;
            //         }
            //     }
            // }
        }
    });
    
    mesh.vertexCount = mesh.triangleCount * 3;

    if (mesh.vertexCount > allocated_vertices && mesh_loaded) {
        allocated_vertices = mesh.vertexCount;

        UnloadMesh(mesh);
        mesh = { 0 };
        mesh.vertexCount = allocated_vertices;
        mesh.triangleCount = mesh.vertexCount / 3;
        mesh.vertices = (float*)malloc(allocated_vertices * 3 * sizeof(float));
        mesh.texcoords = (float*)malloc(allocated_vertices * 2 * sizeof(float));
        UploadMesh(&mesh, false);

        // OS::print("----------------RESIZE (" + to_string(mesh.vertexCount) + ")----------------");
    }

    if (mesh.vertices == NULL) {
        mesh.vertexCount = max(10002, mesh.vertexCount);
        allocated_vertices = mesh.vertexCount;
        mesh.vertices = (float*)malloc(allocated_vertices * 3 * sizeof(float));
        mesh.texcoords = (float*)malloc(allocated_vertices * 2 * sizeof(float));
        // mesh.normals = (float *)malloc(mesh.vertexCount*3*sizeof(float)); 
    }
    
    int face = 0;
    int v = 0;
    ITERATE_BLOCKS({
        Block* block = chunk->getBlock(x, y, z);
        if (block == NULL) {
            continue;
        }
        
        for (int dir = 0; dir < DIRECTION_3_COUNT; dir++) {
            if (block->get((DIRECTION_3)dir) == NULL) {
                addFace((DIRECTION_3)dir, block, face++);
                v += 3;
            }

            // if (block->get((DIRECTION_3)dir) == NULL) {
            //     for (int offset = 0; offset < DIRECTION_COUNT; offset++) {
            //         Block* neighbour = block->get(relativeDirection((DIRECTION_3)dir, (DIRECTION_3)offset));
            //         if (neighbour == NULL || neighbour->get((DIRECTION_3)dir) != NULL) {
            //             addFace(mesh, (DIRECTION_3)dir, x, y, z, face++);
            //             break;
            //         }
            //     }
            // }
        }
    });

    for (int i = face * 6 * 3; i < allocated_vertices * 3; i++) {
        mesh.vertices[i] = 0;
    }
    for (int i = face * 6 * 2; i < allocated_vertices * 2; i++) {
        mesh.texcoords[i] = 0;
    }

    if (!mesh_loaded) {
        UploadMesh(&mesh, false);
        mesh_loaded = true;
    }
    else {
        UpdateMeshBuffer(mesh, 0, mesh.vertices, allocated_vertices * 3 * sizeof(float), 0);
        UpdateMeshBuffer(mesh, 1, mesh.texcoords, allocated_vertices * 2 * sizeof(float), 0);
    }

    int *indices = (int* )malloc(mesh.vertexCount * sizeof(int));
    for (int i = 0; i < mesh.vertexCount; i++ ) {
        indices[i] = i;
    }

    setMeshShape(mesh);
    // dGeomSetCollideBits(getShape(), 0x0001);
    // dGeomSetCategoryBits(getShape(), 0x0002);

    // dTriMeshDataID mesh_data = dGeomTriMeshDataCreate();
    // dGeomTriMeshDataBuildSingle(mesh_data, mesh.vertices, 3 * sizeof(float), mesh.vertexCount, indices, mesh.vertexCount, 3 * sizeof(int));
    // dCreateTriMesh(chunk->world->space, mesh_data, NULL, NULL, NULL);

    // OS::print("Chunkmesh generation took " + to_string(OS::getTime() - start_time) + " seconds");
}

void SubChunk::generateBoundingBox() {
    bounding_box.min = Vector3(chunk->getGridPos().x * CHUNK_SIZE, index * SUBCHUNK_HEIGHT, chunk->getGridPos().y * CHUNK_SIZE);
    bounding_box.max = Vector3((chunk->getGridPos().x + 1) * CHUNK_SIZE, (index + 1) * SUBCHUNK_HEIGHT, (chunk->getGridPos().y + 1) * CHUNK_SIZE);
}

void SubChunk::draw() {

    if (drawn) {
        return;
    }
    drawn = true;

    Vector3 position = chunk->getGlobalPosition();
    position.y = index * SUBCHUNK_HEIGHT;

    // rlDisableBackfaceCulling();

    const float* pos = dGeomGetPosition(getShape());
    Draw::drawCube(Vector3(pos[0], pos[1], pos[2]), Vector3::ONE(), Colour::GREEN());

    if (!isVisible(chunk->getTree()->getEnabledCamera3D(), position)) {
        return;
    }

    if (chunk->world->starting_chunk == this) {
        Draw::drawCube(getCenter() + Vector3(0.0f, CHUNK_SIZE * 0.5f, 0.0f), Vector3::ONE(), Colour::RED());
    }

    Draw::drawMesh(mesh, chunk->world->material, MatrixTranslate(position.x, position.y - (SUBCHUNK_HEIGHT * (index)), position.z));
}

bool SubChunk::isVisible(Camera3D* camera, Vector3 chunk_position) {

    Camera3D::Frustum& frustum = camera->getFrustum();

    for (int face = 0; face < Camera3D::Frustum::FACE_COUNT - 1; face++) {

        const float r = CHUNK_SIZE * 0.5 * abs(frustum.faces[face].normal.x) +
                SUBCHUNK_HEIGHT * 0.5 * abs(frustum.faces[face].normal.y) + CHUNK_SIZE * 0.5 * abs(frustum.faces[face].normal.z);

        if (frustum.faces[face].getDistanceTo(Vector3Transform(chunk_position + Vector3(CHUNK_SIZE, SUBCHUNK_HEIGHT, CHUNK_SIZE) * 0.5, GetCameraMatrix(*camera->getCamera()))) < -r) {
            return false;
        }
    }

    return true;
}

bool SubChunk::isAxisOpaque(AXIS axis) {

    switch (axis) {
        case AXIS::X: {
            for (int y = SUBCHUNK_HEIGHT * index; y < SUBCHUNK_HEIGHT * (index + 1); y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    bool opaque = false;
                    for (int x = 0; x < CHUNK_SIZE; x++) {
                        Block* block = chunk->getBlock(x, y, z);
                        if (block != NULL || !block->isTransparent()) {
                            opaque = true;
                            break;
                        }
                    }

                    if (!opaque) {
                        return false;
                    }
                }
            }
            return true;;
        }
        case AXIS::Y: {
            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    bool opaque = false;
                    for (int y = SUBCHUNK_HEIGHT * index; x < SUBCHUNK_HEIGHT * (index + 1); x++) {
                        Block* block = chunk->getBlock(x, y, z);
                        if (block != NULL || !block->isTransparent()) {
                            opaque = true;
                            break;
                        }
                    }

                    if (!opaque) {
                        return false;
                    }
                }
            }
            return true;
        }
        default: { // Z
            for (int x = 0; x < CHUNK_SIZE; x++) {
                for (int y = SUBCHUNK_HEIGHT * index; y < SUBCHUNK_HEIGHT * (index + 1); y++) {
                    bool opaque = false;
                    for (int z = 0; z < CHUNK_SIZE; z++) {
                        Block* block = chunk->getBlock(x, y, z);
                        if (block != NULL || !block->isTransparent()) {
                            opaque = true;
                            break;
                        }
                    }

                    if (!opaque) {
                        return false;
                    }
                }
            }
            return true;
        }
    }
}
