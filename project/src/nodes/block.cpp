#include "block.h"

#include "world.h"
#include "chunk.h"

Block::TypeData Block::type_data[Block::TYPE_COUNT];
bool Block::type_data_loaded = false;

Block::Block(Chunk* _chunk, int _x, int _y, int _z) {
    chunk = _chunk;
    x = _x;
    y = _y;
    z = _z;

    ASSERT(x < CHUNK_SIZE);
    ASSERT(y < CHUNK_HEIGHT);
    ASSERT(z < CHUNK_SIZE);

    Vector3 chunk_position = chunk->getPosition();
    global_x = x + chunk_position.x;
    global_y = y + chunk_position.y;
    global_z = z + chunk_position.z;
}

Block* Block::getNeighbour(DIRECTION_3 dir) {
    switch (dir) {
        case DIRECTION_3::UP: return getUp();
        case DIRECTION_3::DOWN: return getDown();
        case DIRECTION_3::LEFT: return getLeft();
        case DIRECTION_3::RIGHT: return getRight();
        case DIRECTION_3::FRONT: return getFront();
        case DIRECTION_3::BACK: return getBack();
    }
    return NULL;
}

Vector3 Block::getCenter() const {
    return Vector3(global_x + 0.5f, global_y + 0.5f, global_z + 0.5f);
}

bool Block::isTransparent() const {
    return false;
}

bool Block::isTangible() const {
    return getTypeData().tangible;
}

void Block::setType(TYPE value) {
    if (value == type)
        return;
    
    shared_ptr<Shape3D> old_shape = getTypeCollisionShape();
    type = value;
    shared_ptr<Shape3D> new_shape = getTypeCollisionShape();

    if (old_shape != new_shape) {
        subchunk->collision_shape->removeShape(x, y % SUBCHUNK_HEIGHT, z);

        if (new_shape != NULL)
            subchunk->collision_shape->addShape(x, y & SUBCHUNK_HEIGHT, z, new_shape);
    }
}

Block::TYPE Block::getType() const {
    return type;
}

Block::TypeData& Block::getTypeData() const {
    return type_data[(int)getType()];
}

Block::TypeData& Block::getTypeData(TYPE type) {
    return type_data[(int)type];
}

int* Block::getFaceTexcoords(DIRECTION_3 face) {
    return getTypeData().texcoords[(int)face];
}

RayCollision Block::checkInteractionRay(Ray& ray, Vector3 chunk_position) {
    BoundingBox* type_box = getTypeData().int_box;
    if (type_box == NULL) {
        return RayCollision{false};
    }

    chunk_position.y = -1;
    chunk_position += Vector3(x, y, z);

    BoundingBox box = BoundingBox(*type_box);
    box.min = chunk_position + box.min;
    box.max = chunk_position + box.max;

    return GetRayCollisionBox(ray, box);
}

RayCollision Block::checkInteractionRay(Ray& ray) {
    return checkInteractionRay(ray, chunk->getGlobalPosition());
}

shared_ptr<Shape3D> Block::getTypeCollisionShape() {
    TypeData& type_data = getTypeData();
    if (type_data.col_box == NULL)
        return NULL;
    
    if (type_data.collision_shape.expired()) {
        shared_ptr<Shape3D> ret = BoxShape3D::getFromBoundingBox(*type_data.col_box);
        type_data.collision_shape = ret;
        return ret;
    }
    else {
        return type_data.collision_shape.lock();
    }
}

Block* Block::getLeft() {
    if (x > 0)
        return chunk->getBlock(x - 1, y, z);
    else if (chunk->getLeft())
        return chunk->getLeft()->getBlock(CHUNK_SIZE - 1, y, z);
    return NULL;
}
Block* Block::getRight() {
    if (x < CHUNK_SIZE - 1)
        return chunk->getBlock(x + 1, y, z);
    else if (chunk->getRight())
        return chunk->getRight()->getBlock(0, y, z);
    return NULL;
}

Block* Block::getDown() {
    if (y > 0)
        return chunk->getBlock(x, y - 1, z);
    return NULL;
}
Block* Block::getUp() {
    if (y < CHUNK_HEIGHT - 1)
        return chunk->getBlock(x, y + 1, z);
    return NULL;
}

Block* Block::getFront() {
    if (z > 0)
        return chunk->getBlock(x, y, z - 1);
    else if (chunk->getFront())
        return chunk->getFront()->getBlock(x, y, CHUNK_SIZE - 1);
    return NULL;
}
Block* Block::getBack() {
    if (z < CHUNK_SIZE - 1)
        return chunk->getBlock(x, y, z + 1);
    else if (chunk->getBack())
        return chunk->getBack()->getBlock(x, y, 0);
    return NULL;
}

string Block::getTypeName(TYPE type) {
    switch (type) {
        case TYPE::AIR: return "air";
        case TYPE::GRASS: return "grass";
        case TYPE::DIRT: return "dirt";
        case TYPE::STONE: return "stone";
        default: return "invalid type";
    }
}

Block::TYPE Block::getTypeFromString(string name) {
    name = lowerString(name);
    for (int type = 0; type < TYPE_COUNT; type++) {
        if (name == getTypeName((TYPE)type)) {
            return (TYPE)type;
        }
    }
    return TYPE::INVALID;
}

void generateTypeDataIcon(Block::TypeData& data) {

    ASSERT(data.tangible);

    Draw::beginDrawing();

    RayCam3D camera;
    camera.position = Vector3(1, 1, 1);
    camera.target = Vector3(0, 0, 0);
    camera.up = Vector3::UP();
    camera.fovy = 2.5f;
    camera.projection = CAMERA_ORTHOGRAPHIC;

    BeginMode3D(camera);

    Mesh mesh = { 0 };
    
    mesh.triangleCount = 12;
    mesh.vertexCount = 6 * DIRECTION_3_COUNT;

    mesh.vertices = (float*)malloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)malloc(mesh.vertexCount * 2 * sizeof(float));

    const int texcoords[6][2] = {
        {1, 0}, {0, 0}, {1, 1},
        {0, 0}, {0, 1}, {1, 1}
    };
    int** block_texcoords = data.texcoords;

    int v = 0;
    int t = 0;
    for (int face = 0; face < DIRECTION_3_COUNT; face++) {

        #define ADD_FACE(vertex_data) {\
        const int vertices[6][3] = vertex_data; \
        for (int vertex = 0; vertex < 6; vertex++) { \
            mesh.vertices[v++] = vertices[vertex][0] - 0.5f; \
            mesh.vertices[v++] = vertices[vertex][1] + 0.5f; \
            mesh.vertices[v++] = vertices[vertex][2] - 0.5f; \
            mesh.texcoords[t++] = (texcoords[vertex][0] + block_texcoords[face][0]) / TEXTURE_MAP_WIDTH; \
            mesh.texcoords[t++] = (texcoords[vertex][1] + block_texcoords[face][1]) / TEXTURE_MAP_HEIGHT; \
        } \
        break; }

        switch ((DIRECTION_3)face) {
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

    Material material = LoadMaterialDefault();
    material.shader = LoadShader(0, 0);
    Texture2D texture = OS::getRaylibTexture(OS::loadTexture("project/resources/terrain.png"));
    material.maps[MATERIAL_MAP_DIFFUSE].texture = texture;
    material.maps[MATERIAL_MAP_DIFFUSE].color = Colour::WHITE();

    UploadMesh(&mesh, false);
    DrawMesh(mesh, material, MatrixIdentity());
    UnloadMesh(mesh);

    EndMode3D();

    #define CAPTURE_SIZE 384
    Image capture = LoadImageFromScreen();
    ImageCrop(&capture, Rectangle{
        (float)GetScreenWidth() / 2 - CAPTURE_SIZE / 2,
        (float)GetScreenHeight() / 2 - CAPTURE_SIZE / 2,
        CAPTURE_SIZE, CAPTURE_SIZE
    });

    ImageColorReplace(&capture, GetImageColor(capture, 0, 0), Colour::TRANSPARENT());

    #define ICON_SIZE 64
    ImageResizeNN(&capture, ICON_SIZE, ICON_SIZE);

    data.icon = LoadTextureFromImage(capture);

    UnloadImage(capture);

    Draw::endDrawing();
}

void Block::loadTypeData() {

    if (type_data_loaded)
        return;

    type_data_loaded = true;

    json block_data = json::parse(OS::loadFileText(OS::getResPath("project/resources/block_data.json")), nullptr, false, true);
    ASSERT(block_data.is_object());

    if (block_data.contains("globals")) {
        json globals = block_data["globals"];
        block_data.erase("globals");

        // Insert globals into data
        for (auto& [name, data] : block_data.items()) {
            ASSERT(data.is_object());

            for (auto& [key, value] : data.items()) {
                if (value.is_string() && value.get<string>().front() == '$') {
                    ASSERT_MSG(globals.contains(key) && globals[key].is_object(), key);

                    string global_key = value.get<string>();
                    global_key.erase(0, 1);

                    ASSERT_MSG(globals[key].contains(global_key), global_key);

                    data[key] = globals[key][global_key];
                }
            }
        }
    }

    for (auto& [name, data] : block_data.items()) {
        ASSERT(data.is_object());
        
        // Get block type enum from string name
        Block::TYPE type = Block::getTypeFromString(name);
        ASSERT_MSG(type != TYPE::INVALID, "Invalid block name: " + name);

        // Get the data container struct for this type
        Block::TypeData& type_data = Block::type_data[(int)type];
        ASSERT(!type_data.populated);
        type_data.populated = true;

        // Get collision box and interaction box
        auto getBox = [data = data](string key) -> BoundingBox* {

            if (!data.contains(key) || data[key].is_null()) {
                return NULL;
            }

            json rect_data = data[key];

            ASSERT(rect_data.is_array())
            ASSERT(rect_data.size() == 3 || rect_data.size() == 6);

            ASSERT(rect_data[0].is_number());
            ASSERT(rect_data[1].is_number());
            ASSERT(rect_data[2].is_number());

            BoundingBox* ret = new BoundingBox;

            // The first three values are width, height, and depth
            Vector3 size = Vector3(
                rect_data[0].get<float>(),
                rect_data[1].get<float>(),
                rect_data[2].get<float>()
            );

            ASSERT(size.x > 0.0f && size.x <= 1.0f);
            ASSERT(size.y > 0.0f && size.y <= 1.0f);
            ASSERT(size.z > 0.0f && size.z <= 1.0f);

            Vector3 position;

            // If an additional three values are provided, use them as the position
            if (rect_data.size() == 6) {
                ASSERT(rect_data[3].is_number());
                ASSERT(rect_data[4].is_number());
                ASSERT(rect_data[5].is_number());

                position.set(
                    rect_data[3].get<float>(),
                    rect_data[4].get<float>(),
                    rect_data[5].get<float>()
                );
            }
            // Otherwise, center the rectangle on the ground
            else {
                position.x = 0.5f - size.x / 2.0f;
                position.y = -1.0f + size.y;
                position.z = 0.5f - size.z / 2.0f;
            }

            ASSERT(position.x >= 0.0f && position.x <= 1.0f);
            ASSERT(position.y <= 0.0f && position.y >= -1.0f);
            ASSERT(position.z >= 0.0f && position.z <= 1.0f);

            ret->min = position;
            ret->max = position + size;

            return ret;
        };
        type_data.col_box = getBox("col_box");
        type_data.int_box = getBox("int_box");

        // Get tangible value
        if (data.contains("tangible")) {
            ASSERT(data["tangible"].is_boolean());
            type_data.tangible = data["tangible"].get<bool>();
        }

        // An intangible block may not have textures
        ASSERT(!(!type_data.tangible && data.contains("textures")));

        if (type_data.tangible) {
            type_data.texcoords = new int*[DIRECTION_3_COUNT];
            for (int i = 0; i < DIRECTION_3_COUNT; i++) {
                type_data.texcoords[i] = new int[2];
                type_data.texcoords[i][0] = -1;
                type_data.texcoords[i][1] = -1;
            }
        }

        // Get texture coordinates
        if (data.contains("textures") && !data["textures"].empty()) {
            json texture_coords = data["textures"];
            ASSERT_MSG(texture_coords.is_object() || texture_coords.is_array(), texture_coords.type_name());
            
            #define CHECK_COORDS(coords) \
            ASSERT(coords.size() == 2); \
            ASSERT(coords[0].is_number_integer() && coords[0].get<int>() >= 0); \
            ASSERT(coords[1].is_number_integer() && coords[1].get<int>() >= 0);

            // Dictionary of faces
            if (texture_coords.is_object()) {
                json other = json::object();
                for (auto& [_face_name, coords] : texture_coords.items()) {
                    CHECK_COORDS(coords);
                    const string face_name = lowerString(_face_name);

                    // The 'else' coordinates are used for all unspecified faces
                    if (face_name == "e" || face_name == "else") {
                        ASSERT(other.empty());
                        other = coords;
                    }
                    else {
                        // Get face direction enum from string name
                        DIRECTION_3 face = stringToDirection3(face_name);
                        ASSERT(face != DIRECTION_3::NONE);
                        ASSERT(type_data.texcoords[(int)face][0] == -1)

                        type_data.texcoords[(int)face][0] = coords[0].get<int>();
                        type_data.texcoords[(int)face][1] = coords[1].get<int>();
                    }
                }

                if (!other.empty()) {
                    // Fill unspecified faces with the 'else' coordinates
                    bool other_used = false;
                    for (int face = 0; face < DIRECTION_3_COUNT; face++) {
                        if (type_data.texcoords[face][0] < 0) {
                            type_data.texcoords[face][0] = other[0].get<int>();
                            type_data.texcoords[face][1] = other[1].get<int>();
                            other_used = true;
                        }
                    }
                    WARN_IF(!other_used, "An 'else' texcoord was specified for block '" + name + "', but isn't needed");
                }
            }
            // Single coordinate set for all faces
            else {
                CHECK_COORDS(texture_coords);

                for (int face = 0; face < DIRECTION_3_COUNT; face++) {
                    type_data.texcoords[face][0] = texture_coords[0].get<int>();
                    type_data.texcoords[face][1] = texture_coords[1].get<int>();
                }
            }
        }
        // Use the void texture if no texcoords are specified for a tangible block
        else if (type_data.tangible) {
            for (int face = 0; face < DIRECTION_3_COUNT; face++) {
                type_data.texcoords[(int)face][0] = TEXTURE_MAP_WIDTH - 1;
                type_data.texcoords[(int)face][1] = TEXTURE_MAP_HEIGHT - 1;
            }
        }

        if (type_data.tangible)
            generateTypeDataIcon(type_data);
    }
}
