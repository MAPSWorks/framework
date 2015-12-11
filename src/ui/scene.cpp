#include "scene.h"
#include "nice_grid.h"
#include "shader.h"
#include "renderable_object.h"
#include "camera_manager.h"
#include "resource_manager.h"

Scene::Scene(unique_ptr<CameraManager>& camManager)
    : m_cameraManager(camManager),
      m_activeIdx(-1) {
    init();
}

Scene::~Scene() {}

void Scene::initShaders() {
    ResourceManager::inst().loadShader("../shader/Default.vert.glsl",
                                       "../shader/Default.frag.glsl",
                                       "default");
    ResourceManager::inst().loadShader("../shader/DefaultDepth.vert.glsl",
                                       "../shader/DefaultDepth.frag.glsl",
                                       "defaultDepth");
    ResourceManager::inst().loadShader("../shader/Model.vert.glsl",
                                       "../shader/Model.geom.glsl",
                                       "../shader/Model.frag.glsl", "model");

    // Default texture
    ResourceManager::inst().loadTexture("../data/Texture/floor_grey.png",
                                        GL_REPEAT, "floor_grey");
    ResourceManager::inst().loadTexture("../data/Texture/floor_blue.png",
                                        GL_REPEAT, "floor_blue");
    ResourceManager::inst().loadTexture("../data/Texture/floor.png", GL_REPEAT,
                                        "floor");
    ResourceManager::inst().loadTexture("../data/Texture/default.png",
                                        GL_REPEAT, "default");
}

void Scene::init() {
    initShaders();

    float scale = params::inst().scale;
    m_light.reset(
        new Light(this, glm::vec3(-0.4f * scale, scale, 0.2f * scale)));
    m_niceGrid.reset(new NiceGrid(params::inst().scale, 40.0f));

    m_model.reset(new Model("../data/Models/nanosuit/nanosuit.obj"));

    m_trajectories.reset(new Trajectories());
    m_osmMap.reset(new OpenStreetMap());

    // ResourceManager::inst().printStats();
}

void Scene::renderWorld(const Transform& trans) {
    unique_ptr<Shader>& default_shader =
        ResourceManager::inst().getShader("default");
    default_shader->bind();
    default_shader->selectSubroutine("renderPlain", GL_FRAGMENT_SHADER);
    default_shader->setMatrix("matModel", glm::mat4(1.0f));
    default_shader->setMatrix("matViewProjection", trans.matViewProjection);
    default_shader->setf("gamma", params::inst().gamma);
    m_light->render(default_shader);
    m_osmMap->render(default_shader);
    // m_cameraManager->renderCameras(default_shader);
    default_shader->release();
}

void Scene::renderObjects(const Transform& trans) {
    if (params::inst().boundBox.updated) {
        m_trajectories->prepareForRendering();
        m_osmMap->prepareForRendering();
        params::inst().boundBox.updated = false;
    }

    glm::mat3 matNormal;
    glm::mat4 matShadow(1.0f);
    glm::mat4 matViewport(1.0f);
    matViewport[0][0] = params::inst().windowSize.x / 2.0f;
    matViewport[1][1] = params::inst().windowSize.y / 2.0f;
    matViewport[3][0] = params::inst().windowSize.x / 2.0f;
    matViewport[3][1] = params::inst().windowSize.y / 2.0f;

    unique_ptr<Shader>& model_shader =
        ResourceManager::inst().getShader("model");
    model_shader->bind();
    // Common uniforms
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, params::inst().shadowInfo.shadowMapID);
    model_shader->seti("ShadowMap", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, params::inst().shadowInfo.offsetTexID);
    model_shader->seti("OffsetTex", 1);

    model_shader->setMatrix("matModel", glm::mat4(1.0));
    matNormal = glm::mat3(1.0f);
    model_shader->setMatrix("matNormal", matNormal);
    model_shader->setMatrix("matViewProjection", trans.matViewProjection);
    model_shader->setMatrix("matLightSpace", trans.matLightSpace);

    model_shader->setMatrix("matViewport", matViewport);
    model_shader->setf("gamma", params::inst().gamma);

    model_shader->set3f("camPos", m_cameraManager->currentCamPos());
    model_shader->set3f("OffsetTexSize",
                        params::inst().shadowInfo.offsetTexSize());

    model_shader->set3f("Light.Position", m_light->position());
    model_shader->set3f("Light.Intensity", glm::vec3(1.0f));

    if (params::inst().shadowInfo.applyShadow) {
        // Render with shadow
        if (params::inst().polygonMode == 0) {
            // Render normally
            model_shader->selectSubroutine("shadeWithShadow",
                                           GL_FRAGMENT_SHADER);
        } else {
            // Render with wire frame
            model_shader->selectSubroutine("shadeWithShadowAndWireframe",
                                           GL_FRAGMENT_SHADER);
        }
    } else {
        // Render without shadow
        if (params::inst().polygonMode == 0) {
            // Render normally
            model_shader->selectSubroutine("shadeNoShadow", GL_FRAGMENT_SHADER);
        } else {
            // Render with wire frame
            model_shader->selectSubroutine("shadeNoShadowWireframe",
                                           GL_FRAGMENT_SHADER);
        }
    }

    // Object specific uniforms
    glActiveTexture(GL_TEXTURE2);
    switch (params::inst().gridRenderMode) {
        case 0:
            glBindTexture(GL_TEXTURE_2D,
                          ResourceManager::inst().getTexture("floor")->id());
            break;
        case 1:
            glBindTexture(
                GL_TEXTURE_2D,
                ResourceManager::inst().getTexture("floor_grey")->id());
            break;
        case 2:
            glBindTexture(
                GL_TEXTURE_2D,
                ResourceManager::inst().getTexture("floor_blue")->id());
            break;
        case 3:
            glBindTexture(GL_TEXTURE_2D,
                          ResourceManager::inst().getTexture("default")->id());
            break;
        default:
            glBindTexture(GL_TEXTURE_2D,
                          ResourceManager::inst().getTexture("default")->id());
            break;
    }
    model_shader->seti("DiffTex1", 2);

    glm::vec3 nice_grid_color(0.5f);
    model_shader->set3f("Material.Kd", nice_grid_color);
    model_shader->set3f("Material.Ka", nice_grid_color);
    model_shader->set3f("Material.Ks", glm::vec3(0.1f));
    model_shader->setf("Material.Shininess", 64.0f);

    // Render nice grid
    m_niceGrid->render(model_shader);

    // Model
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,
                  ResourceManager::inst().getTexture("default")->id());
    model_shader->seti("DiffTex1", 2);

    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::scale(model, glm::vec3(10.0, 10.0, 10.0));
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
    model_shader->setMatrix("matModel", model);
    matNormal = glm::mat3(glm::inverseTranspose(model));
    model_shader->setMatrix("matNormal", matNormal);

    model_shader->set3f("Material.Kd", glm::vec3(0.9f));
    model_shader->set3f("Material.Ka", glm::vec3(0.2f));
    model_shader->set3f("Material.Ks", glm::vec3(0.7f));
    model_shader->setf("Material.Shininess", 64.0f);

    glDisable(GL_BLEND);
    m_model->render(model_shader);
    glEnable(GL_BLEND);

    model_shader->release();
}

void Scene::renderObjectsDepth(const Transform& trans) {
    unique_ptr<Shader>& model_shader =
        ResourceManager::inst().getShader("defaultDepth");
    model_shader->bind();

    glCullFace(GL_FRONT);

    // Render nice grid
    model_shader->setMatrix("matViewProjection", trans.matViewProjection);
    model_shader->setMatrix("matModel", glm::mat4(1.0f));
    m_niceGrid->render(model_shader);

    // Model
    glm::mat4 model = glm::mat4(1.0f);
    // model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
    model_shader->setMatrix("matModel", model);
    model_shader->setMatrix("matViewProjection", trans.matViewProjection);
    m_model->render(model_shader);

    glCullFace(GL_BACK);
    model_shader->release();
}

void Scene::update(float delta) {
    // m_light->update(delta);
    m_trajectories->update(delta);
}

void Scene::updateDrawingData() {
    m_trajectories->prepareForRendering();
    m_osmMap->prepareForRendering();
}

void Scene::select(const Transform& trans, int sw, int sh, int mx, int my) {
    // float minDist = math_maxfloat;
    // int idx = -1;

    // Picking pick;

    // for(int i=0; i<m_objects.size(); ++i)
    //{
    //    Object *o = m_objects[i];

    //    glm::vec3 mi = o->m_min;
    //    glm::vec3 ma = o->m_max;

    //    glm::mat4 model(1.0);
    //    model = glm::scale(model, o->m_scale);
    //    model = glm::rotate(model, glm::radians(o->m_rotation.y), glm::vec3(0,
    //    1, 0));
    //    model = glm::translate(model, o->m_position);

    //    float t = pick.select(trans, model, mi, ma, sw, sh, mx, my);

    //    if( t > 0.0f && t<minDist)
    //    {
    //        minDist = t;
    //        idx = i;
    //    }
    //}

    // if(idx >= 0)
    //{
    //    m_activeIdx = idx;
    //    m_objects[m_activeIdx]->m_isSelected = true;
    //}
}

void Scene::move(const Transform& trans, int x, int y) {
    glm::vec3 dir;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 pos;

    getCameraFrame(trans, dir, up, right, pos);

    glm::vec3 movZ;
    glm::vec3 movX;
    glm::vec3 movY(0, 0, 0);

    glm::vec3 onPlane = glm::cross(right, glm::vec3(0, 1, 0));

    if (m_activeIdx >= 0) {
        // glm::vec3 p = m_objects[m_activeIdx]->m_position;

        // movY = glm::vec3(0, p.y, 0);
        // float v = 0.1f * x;
        // glm::vec3 dx = v * right;
        // movX = glm::vec3(p.x, 0, 0) + dx;
        // glm::vec3 dy = v * onPlane;
        // movZ = glm::vec3(0, 0, p.z) + dy;

        // glm::vec3 result = movX + movY + movZ ;

        // m_objects[m_activeIdx]->m_position = result;
    }
}

void Scene::resetSelection() {
    // for(int i=0; i<m_objects.size(); ++i)
    //{
    //    m_objects[i]->m_isSelected = false;
    //}

    m_activeIdx = -1;
}
