#include "Renderer.hpp"
#include <SDL3/SDL.h>
#include <functional>
#include <string>
#include <vector>

namespace eHaz {

// Define a type for our callback function: takes the full path as a string
using FileDialogCallback = std::function<void(const std::string &)>;

inline void OpenNativeFileDialog(bool isSave,
                                 const FileDialogCallback &onFileSelected) {
  // 1. Define the file filters (Optional: restrict to .ehaz or .json)
  const SDL_DialogFileFilter filters[] = {{"Scene Files", "scene"},
                                          {"All Files", "*"}};

  // 2. The Internal SDL Callback
  auto sdl_callback = [](void *userdata, const char *const *filelist,
                         int filter) {
    // Cast the userdata back to our std::function
    auto *callbackPtr = static_cast<FileDialogCallback *>(userdata);

    if (filelist && *filelist) {
      // Success: Invoke the function with the full path (first item in list)
      (*callbackPtr)(std::string(*filelist));
    }

    // Clean up the heap-allocated function pointer
    delete callbackPtr;
  };

  // 3. Move the callback to the heap so it survives until the user clicks
  // "Open"
  auto *persistentCallback = new FileDialogCallback(onFileSelected);

  if (isSave) {

    SDL_ShowSaveFileDialog(sdl_callback, persistentCallback,
                           eHazGraphics::Renderer::p_window->GetWindowPtr(),
                           filters, 2, eRESOURCES_PATH);

  } else {
    SDL_ShowOpenFileDialog(sdl_callback, persistentCallback,
                           eHazGraphics::Renderer::p_window->GetWindowPtr(),
                           filters, 2, eRESOURCES_PATH, false);
  }
}

} // namespace eHaz
