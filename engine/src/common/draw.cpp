#include "draw.h"

#include "node/types/camera_3d.h"

#define DRAW_2D(code, screen_position) \
if (!screen_position && current_draw_mode != DRAW_MODE::TWO) { \
    if (current_draw_mode == DRAW_MODE::THREE) { \
        EndMode3D(); \
        current_draw_mode = DRAW_MODE::NONE; \
    } \
    if (Camera2D* camera = Engine::getSingleton()->getTree()->getEnabledCamera2D()) { \
        BeginMode2D(*camera->getCamera()); \
        current_draw_mode = DRAW_MODE::TWO; \
    } \
} \
else if (screen_position && current_draw_mode != DRAW_MODE::NONE) { \
    current_draw_mode == DRAW_MODE::TWO ? EndMode2D() : EndMode3D(); \
    current_draw_mode = DRAW_MODE::NONE; \
} \
code; \
draw_calls++

#define DRAW_3D(code) \
if (current_draw_mode != DRAW_MODE::THREE) { \
    if (Camera3D* camera = Engine::getSingleton()->getTree()->getEnabledCamera3D()) { \
        if (current_draw_mode == DRAW_MODE::TWO) { \
            EndMode2D(); \
        } \
        BeginMode3D(*camera->getCamera()); \
        current_draw_mode = DRAW_MODE::THREE; \
    } \
} \
code; \
draw_calls++

enum class DRAW_MODE {
    TWO, THREE, NONE
};
DRAW_MODE current_draw_mode = DRAW_MODE::NONE;

string load_message = "";
int load_count = 0;
const float LOAD_MARGIN = 0.1f;
const Colour LOAD_COLOUR = Colour::GREEN();
int draw_calls = 0;

int Draw::getDrawCallCount() {
    return draw_calls;
}

void Draw::beginDrawing() {
    BeginDrawing();
    ClearBackground(Colour{100, 100, 100, 255});
    draw_calls = 0;
}

void Draw::endDrawing() {
    if (current_draw_mode == DRAW_MODE::TWO) {
        EndMode2D();
    }
    else if (current_draw_mode == DRAW_MODE::THREE) {
        EndMode3D();
    }
    current_draw_mode = DRAW_MODE::NONE;

    EndDrawing();
}

#include "project/src/nodes/world.h"
#define LOAD_TOTAL (CHUNK_AMOUNT * CHUNK_AMOUNT * SUBCHUNK_COUNT)
void Draw::loadStep(string message, bool reverse) {

    Draw::beginDrawing();

    if (load_message != message) {
        load_message = message;
        load_count = reverse ? LOAD_TOTAL : 0;
    }

    load_count += reverse ? -1 : 1;

    float w = OS::getScreenWidth();
    float h = OS::getScreenHeight();
    float progress = (float)load_count / (float)LOAD_TOTAL;

    drawLine(Vector2(w * LOAD_MARGIN, h / 2.0f), Vector2((w - (w * LOAD_MARGIN * 2.0f)) * progress + (w * LOAD_MARGIN), h / 2.0f), LOAD_COLOUR, true);
    drawText(message + " " + to_string(int(progress * 100.0f)) + "%", Vector2(w * LOAD_MARGIN, h / 2.0f - 20.0f), LOAD_COLOUR, 1.0f, true);

    Draw::endDrawing();
}

void Draw::drawLine(float start_x, float start_y, float end_x, float end_y, Colour colour, bool screen_position) {
    drawLine(Vector2(start_x, start_y), Vector2(end_x, end_y), colour, screen_position);
}

void Draw::drawLine(Vector2 start, Vector2 end, Colour colour, bool screen_position) {
    DRAW_2D(DrawLine(start.x, start.y, end.x, end.y, colour), screen_position);
}

void Draw::drawTexture(Texture2D texture, Vector2 position, Colour tint, bool screen_position) {
    DRAW_2D(DrawTexture(texture, position.x, position.y, tint), screen_position);
}

void Draw::drawTextureRST(Texture2D texture, Vector2 position, float rotation, Vector2 scale, Colour tint, bool screen_position) {
    DRAW_2D(DrawTexturePro(
        texture,
        Rectangle{
            0,
            0,
            (float)texture.width,// * (flip_x ? -1.0f : 1.0f),
            (float)texture.height// * (flip_y ? -1.0f : 1.0f)
        },
        Rectangle{
            position.x,
            position.y,
            (float)texture.width * scale.x,
            (float)texture.height * scale.y
        },
        Vector2::ZERO(),
        RAD2DEG(rotation),
        tint
    ), screen_position);
}

void Draw::drawText(string text, float pos_x, float pos_y, Colour colour, float size, bool screen_position) {
    drawText(text, Vector2(pos_x, pos_y), colour, size, screen_position);
}
void Draw::drawText(string text, Vector2 position, Colour colour, float size, bool screen_position) {
    DRAW_2D(DrawTextPro(GetFontDefault(), text.c_str(), position, Vector2::ZERO(), 0.0f, size * 10.0f, 1.0f, colour), screen_position);
}

void Draw::markPoint(Vector3 point, Colour colour, float size) {
    size /= 2.0f;
    DRAW_3D({
        drawLine(point - Vector3(size, 0, 0), point + Vector3(size, 0, 0), colour);
        drawLine(point - Vector3(0, size, 0), point + Vector3(0, size, 0), colour);
        drawLine(point - Vector3(0, 0, size), point + Vector3(0, 0, size), colour);
    });
}

void Draw::drawLine(Vector3 start, Vector3 end, Colour colour) {
    DRAW_3D(DrawLine3D(start, end, colour));
}

void Draw::drawRay(Ray ray, Colour colour) {
    DRAW_3D(DrawRay(ray, colour));
}

void Draw::drawTriangle(Vector3 p1, Vector3 p2, Vector3 p3, Colour colour) {
    DRAW_3D(DrawTriangle3D(p1, p2, p3, colour));
}

void Draw::drawCube(Vector3 position, Vector3 size, Colour colour) {
    DRAW_3D(DrawCubeV(position, size, colour));
    DRAW_3D(DrawCubeWiresV(position, size, Colour::WHITE()));
}

void Draw::drawCube(Vector3 position, Vector3 size, Texture2D texture, Colour colour) {
    DRAW_3D(DrawCubeTexture(texture, position, size.x, size.y, size.z, colour));
}

void Draw::drawBoundingBox(BoundingBox box, Colour colour, Vector3 offset, Vector3 scale) {
    drawCube((Vector3)box.min + offset + ((Vector3)box.max - box.min) * 0.5, ((Vector3)box.max - box.min) * scale, colour);
}

void Draw::drawMesh(Mesh mesh, Material material, Matrix transform) {
    DRAW_3D(DrawMesh(mesh, material, transform));
}

void drawMeshInstanced(Mesh mesh, Material material, Matrix *transforms, int instances) {
    DRAW_3D(DrawMeshInstanced(mesh, material, transforms, instances));
}

void Draw::drawModel(Model model, Vector3 position, float scale, Color tint, bool wires) {
    DRAW_3D(
        if (wires) {
            DrawModelWires(model, position, scale, tint);
        }
        else {
            DrawModel(model, position, scale, tint);
        }
    );
}

void Draw::drawGrid(int slices, float spacing) {
    DRAW_3D(DrawGrid(slices, spacing));
}