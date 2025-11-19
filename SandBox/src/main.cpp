#include "Animation/AnimatedModelManager.hpp"
#include "BitFlags.hpp"
#include "Core/Event.hpp"
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

  void processInput(Window *c_window, bool &quit, Camera &camera) {

    SDL_Window *window = c_window->GetWindowPtr();
    // Delta time calculation using performance counters

    double deltaTime = eHaz_Core::Application::instance->GetDeltaTime();

    // Static mouse state
    static bool firstMouse = true;
    static float lastX = 0.0f;
    static float lastY = 0.0f;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        quit = true;
        break;

      case SDL_EVENT_KEY_DOWN:
        if (event.key.which == SDLK_ESCAPE)
          quit = true;
        break;

      case SDL_EVENT_KEY_UP:

        break;

      // --- Added mouse movement support for camera ---
      case SDL_EVENT_MOUSE_MOTION: {
        float xpos = static_cast<float>(event.motion.x);
        float ypos = static_cast<float>(event.motion.y);

        if (firstMouse) {
          lastX = xpos;
          lastY = ypos;
          firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset =
            lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
        break;
      }

      // --- Added mouse scroll support for zoom ---
      case SDL_EVENT_MOUSE_WHEEL: {
        float yoffset = static_cast<float>(event.wheel.y);
        camera.ProcessMouseScroll(yoffset);
        break;
      }

      // --- Added window resize support (framebuffer_size_callback equivalent)
      // ---
      case SDL_EVENT_WINDOW_RESIZED: {
        int width = event.window.data1;
        int height = event.window.data2;
        c_window->SetDimensions(width, height);
        glViewport(0, 0, width, height);
        break;
      }

      default:
        break;
      }
    }

    // Continuous key input using scancodes
    const auto *state = SDL_GetKeyboardState(nullptr);
    if (state[SDL_SCANCODE_W])
      camera.ProcessKeyboard(FORWARD, static_cast<float>(deltaTime));
    if (state[SDL_SCANCODE_S])
      camera.ProcessKeyboard(BACKWARD, static_cast<float>(deltaTime));
    if (state[SDL_SCANCODE_A])
      camera.ProcessKeyboard(LEFT, static_cast<float>(deltaTime));
    if (state[SDL_SCANCODE_D])
      camera.ProcessKeyboard(RIGHT, static_cast<float>(deltaTime));
    if (state[SDL_SCANCODE_SPACE])
      camera.ProcessKeyboard(UP, static_cast<float>(deltaTime));
    if (state[SDL_SCANCODE_LSHIFT])
      camera.ProcessKeyboard(DOWN, static_cast<float>(deltaTime));
    if (state[SDL_SCANCODE_R])
      c_window->ToggleMouseCursor();
  }
  bool firstMouse = true;
  float lastX = 0.0f;
  float lastY = 0.0f;
  SDL_Window *window;

  double deltaTime;
  void processCameraInput(Window *c_window, Camera &camera,
                          std::unique_ptr<eHaz::Event> &event) {
    window = c_window->GetWindowPtr();
    // Delta time calculation using performance counters

    // Static mouse state

    switch (event->GetEventType()) {

    case eHaz::EVENT_KEY_DOWN: {

      eHaz::KeyDownEvent *kd_event =
          static_cast<eHaz::KeyDownEvent *>(event.get());

      switch (kd_event->GetKey()) {

      case EHAZK_W: {

        camera.ProcessKeyboard(FORWARD, static_cast<float>(deltaTime));
        SDL_Log("W KEY PRESSED\n");

      } break;
      case EHAZK_S: {

        camera.ProcessKeyboard(BACKWARD, static_cast<float>(deltaTime));
        SDL_Log("S KEY PRESSED\n");

      } break;
      case EHAZK_A: {

        camera.ProcessKeyboard(LEFT, static_cast<float>(deltaTime));
        SDL_Log("A KEY PRESSED\n");

      } break;
      case EHAZK_D: {

        camera.ProcessKeyboard(RIGHT, static_cast<float>(deltaTime));
        SDL_Log("D KEY PRESSED\n");

      } break;
      case EHAZK_SPACE: {

        camera.ProcessKeyboard(UP, static_cast<float>(deltaTime));

      } break;
      case EHAZK_LSHIFT: {

        camera.ProcessKeyboard(DOWN, static_cast<float>(deltaTime));

      } break;
      }
      kd_event->handled = true;

    } break;

    case eHaz::EVENT_MOUSE_MOVE: {

      eHaz::MouseMoveEvent *mmv_event =
          static_cast<eHaz::MouseMoveEvent *>(event.get());

      float xpos = static_cast<float>(mmv_event->GetX());
      float ypos = static_cast<float>(mmv_event->GetY());

      if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
      }

      float xoffset = xpos - lastX;
      float yoffset =
          lastY - ypos; // reversed since y-coordinates go from bottom to top

      lastX = xpos;
      lastY = ypos;

      camera.ProcessMouseMovement(xoffset, yoffset);
      mmv_event->handled = true;
    } break;

    case eHaz::EVENT_APPLICATION_QUIT:
      Renderer::r_instance->shouldQuit = true;

      break;
    case eHaz::EVENT_WINDOW_RESIZE: {
      eHaz::WindowResizeEvent *wr_event =
          static_cast<eHaz::WindowResizeEvent *>(event.get());
      int width = wr_event->GetWidth();
      int height = wr_event->GetHeight();
      c_window->SetDimensions(width, height);
      glViewport(0, 0, width, height);
      wr_event->handled = true;

    } break;
    }
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

  void OnEvent(std::unique_ptr<eHaz::Event> &event) override {

    processCameraInput(Renderer::p_window.get(), camera, event);
  }

  void OnUpdate(float ts) override {

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
