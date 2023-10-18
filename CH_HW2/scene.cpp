#include "scene.h"
#include "binary/animation.h"
#include "binary/skeleton.h"
#include "binary/player.h"

Shader* Scene::vertexShader = nullptr;
Shader* Scene::fragmentShader = nullptr;
Program* Scene::program = nullptr;
Camera* Scene::camera = nullptr;
Object* Scene::player = nullptr;
Texture* Scene::diffuse = nullptr;
Material* Scene::material = nullptr;
Object* Scene::lineDraw = nullptr;
Texture* Scene::lineColor = nullptr;
Material* Scene::lineMaterial = nullptr;
int Scene::width=0;
int Scene::height=0;

mat4 toparent[28], Md[28], Ma[28], M[28];
vec3 position[2812];


void Scene::setup(AAssetManager* aAssetManager) {
    Asset::setManager(aAssetManager);

    Scene::vertexShader = new Shader(GL_VERTEX_SHADER, "vertex.glsl");
    Scene::fragmentShader = new Shader(GL_FRAGMENT_SHADER, "fragment.glsl");

    Scene::program = new Program(Scene::vertexShader, Scene::fragmentShader);

    Scene::camera = new Camera(Scene::program);
    Scene::camera->eye = vec3(0.0f, 0.0f, 80.0f);

    Scene::diffuse = new Texture(Scene::program, 0, "textureDiff", playerTexels, playerSize);
    Scene::material = new Material(Scene::program, diffuse);
    Scene::player = new Object(program, material, playerVertices, playerIndices);
    player->worldMat = scale(vec3(1.0f / 3.0f));

    Scene::lineColor = new Texture(Scene::program, 0, "textureDiff", {{0xFF, 0x00, 0x00}}, 1);
    Scene::lineMaterial = new Material(Scene::program, lineColor);
    Scene::lineDraw = new Object(program, lineMaterial, {{}}, {{}}, GL_LINES);

    Md[0] = glm::mat4(1.0f);
    Ma[0] = glm::mat4(1.0f);
    M[0] = glm::mat4(1.0f);

    for (int i = 0; i < 28; i++)
        toparent[i] = glm::translate(glm::mat4(1.0f), vec3(jOffsets[i].x,jOffsets[i].y,jOffsets[i].z));

    for (int i = 1; i < 28; i++)
        Md[i] = Md[jParents[i]] * toparent[i];

    for (int i = 0; i < 28; i++) {
        quat quat_x = glm::quat_cast(glm::rotate(glm::mat4(1.0f), radians(motions[0][i * 3 + 3]), vec3(1.0f, 0.0f, 0.0f)));
        quat quat_y = glm::quat_cast(glm::rotate(glm::mat4(1.0f), radians(motions[0][i * 3 + 4]), vec3(0.0f, 1.0f, 0.0f)));
        quat quat_z = glm::quat_cast(glm::rotate(glm::mat4(1.0f), radians(motions[0][i * 3 + 5]), vec3(0.0f, 0.0f, 1.0f)));

        quat Quat = quat_z * quat_x * quat_y;
        mat4 rotation = glm::mat4_cast(Quat);

        if(i == 0);
        else
            Ma[i] = Ma[jParents[i]] * toparent[i] * rotation;

        M[i] = Ma[i] * inverse(Md[i]);

    }

    for (int i = 0; i < 2812; i++)
        position[i] = playerVertices[i].pos;

    for (int i = 0; i < 2812; i++) {
        vec3 skin[4];

        if(playerVertices[i].bone.x == -1)
            skin[0] = vec3(0.0f, 0.0f, 0.0f);
        else
            skin[0] = playerVertices[i].weight.x * vec3(M[playerVertices[i].bone.x] * vec4(position[i], 1.0f));

        if(playerVertices[i].bone.y == -1)
            skin[1] = vec3(0.0f, 0.0f, 0.0f);
        else
            skin[1] = playerVertices[i].weight.y * vec3(M[playerVertices[i].bone.y] * vec4(position[i], 1.0f));

        if(playerVertices[i].bone.z == -1)
            skin[2] = vec3(0.0f, 0.0f, 0.0f);
        else
            skin[2] = playerVertices[i].weight.z * vec3(M[playerVertices[i].bone.z] * vec4(position[i], 1.0f));

        if(playerVertices[i].bone.w == -1)
            skin[3] = vec3(0.0f, 0.0f, 0.0f);
        else
            skin[3] = playerVertices[i].weight.w * vec3(M[playerVertices[i].bone.w] * vec4(position[i], 1.0f));

        playerVertices[i].pos = skin[0] + skin[1] + skin[2] + skin[3];
    }

}

void Scene::screen(int width, int height) {
    Scene::camera->aspect = (float) width/height;
    Scene::width = width;
    Scene::height = height;
}

void Scene::update(float deltaTime) {
    static float Time = 0.0f;
    Time += deltaTime;

    if(Time >= 4.0f)
        Time -= 4.0f;

    int intTime = (int)Time;
    float remainTime = Time - float(intTime);

    for (int i = 0; i < 28; i++) {
        quat quat_x_1 = glm::quat_cast(glm::rotate(glm::mat4(1.0f), radians(motions[intTime % 4][i * 3 + 3]), vec3(1.0f, 0.0f, 0.0f)));
        quat quat_y_1 = glm::quat_cast(glm::rotate(glm::mat4(1.0f), radians(motions[intTime % 4][i * 3 + 4]), vec3(0.0f, 1.0f, 0.0f)));
        quat quat_z_1 = glm::quat_cast(glm::rotate(glm::mat4(1.0f), radians(motions[intTime % 4][i * 3 + 5]), vec3(0.0f, 0.0f, 1.0f)));
        quat quat_1 = quat_z_1 * quat_x_1 * quat_y_1;

        quat quat_x_2 = glm::quat_cast(glm::rotate(glm::mat4(1.0f), radians(motions[(intTime + 1) % 4][i * 3 + 3]), vec3(1.0f, 0.0f, 0.0f)));
        quat quat_y_2 = glm::quat_cast(glm::rotate(glm::mat4(1.0f), radians(motions[(intTime + 1) % 4][i * 3 + 4]), vec3(0.0f, 1.0f, 0.0f)));
        quat quat_z_2 = glm::quat_cast(glm::rotate(glm::mat4(1.0f), radians(motions[(intTime + 1) % 4][i * 3 + 5]), vec3(0.0f, 0.0f, 1.0f)));
        quat quat_2 = quat_z_2 * quat_x_2 * quat_y_2;

        quat Quat = glm::slerp(quat_1, quat_2, remainTime);

        mat4 rotation = glm::mat4_cast(Quat);

        if(i == 0);
        else
            Ma[i] = Ma[jParents[i]] * toparent[i] * rotation;

        M[i] = Ma[i] * inverse(Md[i]);

    }

    for (int i = 0; i < 2812; i++) {
        vec3 skin[4];

        if(playerVertices[i].bone.x == -1)
            skin[0] = vec3(0.0f, 0.0f, 0.0f);
        else
            skin[0] = playerVertices[i].weight.x * vec3(M[playerVertices[i].bone.x] * vec4(position[i], 1.0f));

        if(playerVertices[i].bone.y == -1)
            skin[1] = vec3(0.0f, 0.0f, 0.0f);
        else
            skin[1] = playerVertices[i].weight.y * vec3(M[playerVertices[i].bone.y] * vec4(position[i], 1.0f));

        if(playerVertices[i].bone.z == -1)
            skin[2] = vec3(0.0f, 0.0f, 0.0f);
        else
            skin[2] = playerVertices[i].weight.z * vec3(M[playerVertices[i].bone.z] * vec4(position[i], 1.0f));

        if(playerVertices[i].bone.w == -1)
            skin[3] = vec3(0.0f, 0.0f, 0.0f);
        else
            skin[3] = playerVertices[i].weight.w * vec3(M[playerVertices[i].bone.w] * vec4(position[i], 1.0f));

        playerVertices[i].pos = skin[0] + skin[1] + skin[2] + skin[3];
    }

    Scene::program->use();

    Scene::camera->update();

    //////////////////////////////
    /* TODO */

    // Line Drawer & Debugger
    glLineWidth(20);
    Scene::lineDraw->load({{vec3(-20.0f, 0.0f, 0.0f)}, {vec3(20.0f, 0.0f, 0.0f)}}, {0, 1});
    Scene::lineDraw->draw();

    // LOG_PRINT_DEBUG("You can also debug variables with this function: %f", M_PI);

    //////////////////////////////

    Scene::player->load(playerVertices, playerIndices);
    Scene::player->draw();

}

float vx1, vy1, vz1, vx2, vy2, vz2;
vec3 vector1, vector2;

void Scene::mouseDownEvents(float x, float y) {
    //////////////////////////////
    /* TODO: Optional problem
     * object rotating
     * Automatically called when mouse down
     */

    vx1 = 2 * x / width - 1;
    vy1 = -1 * (2 * y / height - 1);
    if(vx1 * vx1 + vy1 * vy1 > 1) {
        vz1 = 0;
        vx1 = normalize(vx1);
        vy1 = normalize(vy1);
    }
    else
        vz1 = sqrt(1 - vx1 * vx1 - vy1 * vy1);

    vector1 = vec3(vx1, vy1, vz1);

    //////////////////////////////
}

void Scene::mouseMoveEvents(float x, float y) {
    //////////////////////////////
    /* TODO: Optional problem
     * object rotating
     * Automatically called when mouse move
     */

    vx2 = 2 * x / width - 1;
    vy2 = -1 * (2 * y / height - 1);
    if(vx2 * vx2 + vy2 * vy2 > 1) {
        vz2 = 0;
        vx2 = normalize(vx2);
        vy2 = normalize(vy2);
    }
    else
        vz2 = sqrt(1 - vx2 * vx2 - vy2 * vy2);

    vector2 = vec3(vx2, vy2, vz2);

    vec3 Axis = normalize(cross(vector1, vector2));

    float angle = acos(dot(vector1, vector2));

    mat4 drag = glm::rotate(glm::mat4(1.0f), radians(angle), Axis);

    player->worldMat = drag * (player->worldMat);

    //////////////////////////////
}

