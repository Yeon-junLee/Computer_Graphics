#include "scene.h"

#include "obj_teapot.h"
#include "tex_flower.h"

Shader* Scene::vertexShader = nullptr;
Shader* Scene::fragmentShader = nullptr;
Program* Scene::program = nullptr;
Camera* Scene::camera = nullptr;
Light* Scene::light = nullptr;
Object* Scene::teapot = nullptr;
Material* Scene::flower = nullptr;

void Scene::setup(AAssetManager* aAssetManager) {

    // set asset manager
    Asset::setManager(aAssetManager);

    // create shaders
    vertexShader = new Shader(GL_VERTEX_SHADER, "vertex.glsl");
    fragmentShader = new Shader(GL_FRAGMENT_SHADER, "fragment.glsl");

    // create program
    program = new Program(vertexShader, fragmentShader);

    // create camera
    camera = new Camera(program);
    camera->eye = vec3(60.0f, 00.0f, 0.0f);
    camera->updateCameraUVN();

    // create light
    light = new Light(program);
    light->position = vec3(100.0f, 0.0f, 0.0f);

    // create floral texture
    flower = new Material(program, texFlowerData, texFlowerSize);

    // create teapot object
    teapot = new Object(program, flower, objTeapotVertices, objTeapotIndices,
                        objTeapotVerticesSize, objTeapotIndicesSize);

    //////////////////////////////
    /* TODO: Problem 2.
     *  Scale the teapot by 2.0 along the y-axis.
     *  Rotate the teapot by 90° CW about the rotation axis defined by two points
     *  (0, 0, 10) → (10, 0, 20).
     */

    mat4 scaleM, rotMat;
    scaleM = transpose(mat4(1.0f, 0.0f, 0.0f, 0.0f,  // In OpenGL, the matrix must be transposed
                            0.0f, 2.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f));

    rotMat =transpose(mat4(0.5f, 1/sqrt(2), 0.5f, -5.0f,
                           -1/sqrt(2), 0.0f, 1/sqrt(2), -5.0f * sqrt(2),
                           0.5f, -1/sqrt(2), 0.5f, 5.0f,
                           0.0f, 0.0f, 0.0f, 1.0f));
    teapot->worldMatrix =rotMat * scaleM;
    //////////////////////////////
}

void Scene::screen(int width, int height) {

    // set camera aspect ratio
    camera->aspect = (float) width / height;
}

void Scene::update(float deltaTime) {
    // use program
    program->use();

    //////////////////////////////
    /* TODO: Problem 3.
     *  Rotate the teapot about the z-axis.
     */

    teapot->worldMatrix = transpose(mat4(cos(deltaTime), -sin(deltaTime), 0.0f, 0.0f,
                                         sin(deltaTime), cos(deltaTime), 0.0f, 0.0f,
                                         0.0f, 0.0f, 1.0f, 0.0f,
                                         0.0f, 0.0f, 0.0f, 1.0f)) * teapot->worldMatrix;
    //////////////////////////////


    camera->updateViewMatrix();
    camera->updateProjectionMatrix();
    light->setup();


    // draw teapot
    teapot->draw();
}

void Scene::rotateCamera(float dx,float dy) {
    float rotationSensitivity = 0.03;

    float thetaYaw=glm::radians(rotationSensitivity*dx);
    float thetaPinch=glm::radians(rotationSensitivity*dy);

    rotateCameraYaw(thetaYaw);
    rotateCameraPitch(thetaPinch);
}

void Scene::rotateCameraYaw(float theta) {

    //////////////////////////////
    /* TODO: Problem 4.
     *  calculate the rotated u,n vector about v axis.
     *  Argument theta is amount of rotation in radians. theta is positive when CCW.
     *  Note that u,v,n should always be orthonormal.
     *  The u vector can be accessed via camera->cameraU.
     *  The v vector can be accessed via camera->cameraV.
     *  The n vector can be accessed via camera->cameraN.
     */

    glm::mat4 rotateY;
    rotateY = glm::rotate(rotateY, theta, camera->cameraV);

    camera->cameraU = rotateY * vec4(camera->cameraU,1.0f);
    camera->cameraN = rotateY * vec4(camera->cameraN,1.0f);

    camera->updateViewMatrix();
    //////////////////////////////
}

void Scene::rotateCameraPitch(float theta) {

    //////////////////////////////
    /* TODO: Problem 4.
     *  calculate the rotated v,n vector about u axis.
     *  Argument theta is amount of rotation in radians. Theta is positive when CCW.
     *  Note that u,v,n should always be orthonormal.
     *  The u vector can be accessed via camera->cameraU.
     *  The v vector can be accessed via camera->cameraV.
     *  The n vector can be accessed via camera->cameraN.
     */

    glm::mat4 rotateP;
    rotateP = glm::rotate(rotateP, theta, camera->cameraU);

    camera->cameraV = rotateP * vec4(camera->cameraV,1.0f);
    camera->cameraN = rotateP * vec4(camera->cameraN,1.0f);

    camera->updateViewMatrix();
    //////////////////////////////
}

void Scene::translateLeft(float amount) {

    //////////////////////////////
    /* TODO: Problem 4.
     *  Calculate the camera position(eye) when translated left.
     */

    glm::mat4 transl = glm::mat4(1.0f);
    transl = glm::translate(transl, -1 * amount *(camera->cameraU));
    camera->eye = transl * vec4(camera->eye, 1.0f);

    camera->updateViewMatrix();
    //////////////////////////////
}

void Scene::translateFront(float amount) {

    //////////////////////////////
    /* TODO: Problem 4.
     *  Calculate the camera position(eye) when translated front.
     */

    glm::mat4 transf = glm::mat4(1.0f);
    transf = glm::translate(transf, -1 * amount *(camera->cameraN));
    camera->eye = transf * vec4(camera->eye, 1.0f);

    camera->updateViewMatrix();
    //////////////////////////////
}

void Scene::translateRight(float amount) {

    //////////////////////////////
    /* TODO: Problem 4.
     *  Calculate the camera position(eye) when translated right.
     */

    glm::mat4 transr = glm::mat4(1.0f);
    transr = glm::translate(transr, amount *(camera->cameraU));
    camera->eye = transr * vec4(camera->eye, 1.0f);

    camera->updateViewMatrix();
    //////////////////////////////
}

void Scene::translateBack(float amount) {

    //////////////////////////////
    /* TODO: Problem 4.
     *  Calculate the camera position(eye) when translated back.
     */

    glm::mat4 transb = glm::mat4(1.0f);
    transb = glm::translate(transb, amount *(camera->cameraN));
    camera->eye = transb * vec4(camera->eye, 1.0f);

    camera->updateViewMatrix();
    //////////////////////////////
}
