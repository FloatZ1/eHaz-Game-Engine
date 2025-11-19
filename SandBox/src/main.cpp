#include "Animation/AnimatedModelManager.hpp"
#include "BitFlags.hpp"
#include "Core/Event.hpp"
#include "Core/Input/InputSystem.hpp"
#include "Core/Input/KeyCodes.hpp"
#include "Core/Layer.hpp"
#include "DataStructs.hpp"
#include "Engine/include/Core/Application.hpp"
#include "Engine/include/UI/EditorLayer.hpp"
#include "Renderer.hpp"
#include "camera.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_oldnames.h>
#include <iterator>
#include <memory>
#include <utility>
using namespace eHazGraphics;
class AppLayer : public eHaz::Layer {

  Camera camera;

  bool firstMouse = true;
  float lastX = 0.0f;
  float lastY = 0.0f;
  SDL_Window *window;

  double deltaTime;
  void processCameraInput(Window *c_window, Camera &camera) {
    window = c_window->GetWindowPtr();
    // Delta time calculation using performance counters

    // Static mouse state

    const eHaz::InputSystem &input_system =
        eHaz_Core::Application::instance->GetInputSystem();

    if (input_system.GetKeyDown(EHAZK_W)) {
      camera.ProcessKeyboard(FORWARD, static_cast<float>(deltaTime));
      SDL_Log("W KEY PRESSED\n");
    }
    if (input_system.GetKeyDown(EHAZK_A)) {
      camera.ProcessKeyboard(LEFT, static_cast<float>(deltaTime));
      SDL_Log("A KEY PRESSED\n");
    }
    if (input_system.GetKeyDown(EHAZK_S)) {
      camera.ProcessKeyboard(BACKWARD, static_cast<float>(deltaTime));
      SDL_Log("S KEY PRESSED\n");
    }
    if (input_system.GetKeyDown(EHAZK_D)) {
      camera.ProcessKeyboard(RIGHT, static_cast<float>(deltaTime));
      SDL_Log("D KEY PRESSED\n");
    }
    if (input_system.GetKeyDown(EHAZK_SPACE)) {
      camera.ProcessKeyboard(UP, static_cast<float>(deltaTime));
      SDL_Log("SPACE KEY PRESSED\n");
    }
    if (input_system.GetKeyDown(EHAZK_LCTRL)) {
      camera.ProcessKeyboard(DOWN, static_cast<float>(deltaTime));
      SDL_Log("CTRL KEY PRESSED\n");
    }

    if (input_system.GetKeyPressed(EHAZK_R)) {
      c_window->ToggleMouseCursor();

      SDL_Log("FOCUS CURSOR PRESSED (R)");
    }

    float xoffset = static_cast<float>(input_system.GetMouseRelativeX());
    float yoffset = static_cast<float>(input_system.GetMouseRelativeY());

    /*float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos; */
    yoffset = -yoffset;

    // Apply sensitivity

    xoffset *= 10;

    yoffset *= 10;

    if (xoffset != lastX && yoffset != lastY)
      camera.ProcessMouseMovement(xoffset, yoffset, false);
    lastX = xoffset;
    lastY = yoffset;
  }

  struct camData {
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
  };

  BufferRange camDt;
  BufferRange materials;
  std::pair<const std::vector<PBRMaterial> &, TypeFlags> *mat;
  // boiler plate above for testing
  void OnCreate() override {

    camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

    Renderer::r_instance->p_bufferManager->BeginWritting();

    glm::mat4 projection1 =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)Renderer::p_window->GetWidth() /
                             (float)Renderer::p_window->GetHeight(),
                         0.1f, 100.0f);

    camData deta{camera.GetViewMatrix(), projection1};

    camDt = Renderer::r_instance->SubmitDynamicData(
        &deta, sizeof(deta), TypeFlags::BUFFER_CAMERA_DATA);

    unsigned int AlbedoTexture =
        Renderer::p_materialManager->LoadTexture(eRESOURCES_PATH "rizz.png");

    unsigned int materialID = Renderer::p_materialManager->CreatePBRMaterial(
        AlbedoTexture, AlbedoTexture, AlbedoTexture, AlbedoTexture);

    auto mat = Renderer::r_instance->p_materialManager->SubmitMaterials();
    // bullshit hack

    this->mat = new std::pair<const std::vector<PBRMaterial> &, TypeFlags>(mat);
    materials = Renderer::p_bufferManager->InsertNewDynamicData(
        mat.first.data(), mat.first.size() * sizeof(PBRMaterial),
        TypeFlags::BUFFER_TEXTURE_DATA);

    ShaderComboID shader = Renderer::p_shaderManager->CreateShaderProgramme(
        eRESOURCES_PATH "shader.vert", eRESOURCES_PATH "shader.frag");

    SDL_Log("\n\n\n" eRESOURCES_PATH "\n\n\n");
    std::string path = eRESOURCES_PATH "boombox.glb";
    auto model = eHazGraphics::Renderer::p_meshManager->LoadModel(path);

    model->SetPositionMat4(
        glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

    Renderer::p_meshManager->SetModelShader(model, shader);

    Renderer::r_instance->SubmitStaticModel(model,
                                            TypeFlags::BUFFER_STATIC_MESH_DATA);
  }

  void OnEvent(eHaz::Event &event) override {

    if (event.GetEventType() == eHaz::EVENT_APPLICATION_QUIT)
      Renderer::r_instance->shouldQuit = true;
  }

  void OnUpdate(float ts) override {
    processCameraInput(Renderer::p_window.get(), camera);
    deltaTime = eHaz_Core::Application::instance->GetDeltaTime();

    glm::mat4 projection =
        glm::perspective(glm::radians(camera.Zoom),
                         (float)Renderer::r_instance->p_window->GetWidth() /
                             (float)Renderer::r_instance->p_window->GetHeight(),
                         0.1f, 100.0f);

    camData camcamdata = {camera.GetViewMatrix(), projection};

    Renderer::r_instance->UpdateDynamicData(camDt, &camcamdata,
                                            sizeof(camcamdata));

    Renderer::r_instance->UpdateDynamicData(
        materials, mat->first.data(), mat->first.size() * sizeof(PBRMaterial));
  }
  void OnRender() override {

    // Renderer::r_instance->RenderFrame(std::vector<DrawRange> DrawOrder)
    auto ranges = Renderer::p_renderQueue->SubmitRenderCommands();

    Renderer::r_instance->RenderFrame(ranges);
  }
};

int main() {

  std::cout << eRESOURCES_PATH "\n";

  eHaz_Core::AppSpec spec;
  spec.fullscreen = false;
  spec.title = "fuck";
  spec.w_width = 1280;
  spec.w_height = 720;
  eHaz_Core::Application app(spec);
  app.push_layer<eHaz::EditorUILayer>(true);
  app.push_layer<AppLayer>();

  app.Run();

  std::cout << "testy tetst\n";
}
