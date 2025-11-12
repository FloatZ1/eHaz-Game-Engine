#ifndef EHAZ_CORE_LAYER_STACK_HPP
#define EHAZ_CORE_LAYER_STACK_HPP

#include "EventQueue.hpp"
#include "Layer.hpp"
#include <memory>
#include <type_traits>
#include <vector>
namespace eHaz {

class LayerStack {

public:
  template <typename TLayer>
    requires(std::is_base_of_v<Layer, TLayer>)
  void PushUILayer() {

    m_LayerStack.emplace(0, std::make_unique<TLayer>());
  }
  template <typename TLayer>
    requires(std::is_base_of_v<Layer, TLayer>)
  void push_layer() {
    m_LayerStack.push_back(std::make_unique<TLayer>());
  }

  void UpdateLayers(float ts) {

    for (auto &layer : m_LayerStack) {

      layer->OnUpdate(ts);
    }
  }

  void RenderLayers() {

    for (auto &layer : m_LayerStack) {
      layer->OnRender();
    }
  }

  void NotifyEvents(EventQueue &events) {

    for (auto &event : events) {

      for (auto &layer : m_LayerStack) {
        layer->OnEvent(*event.get());
      }
    }
  }

private:
  std::vector<std::unique_ptr<Layer>> m_LayerStack;
};

} // namespace eHaz

#endif
