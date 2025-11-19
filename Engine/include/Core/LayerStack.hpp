#ifndef EHAZ_CORE_LAYER_STACK_HPP
#define EHAZ_CORE_LAYER_STACK_HPP

#include "EventQueue.hpp"
#include "Layer.hpp"
#include <any>
#include <memory>
#include <type_traits>
#include <vector>
namespace eHaz {

class LayerStack {

public:
  template <typename TLayer>
    requires(std::is_base_of_v<Layer, TLayer>)
  void PushUILayer() {
    int wtf;
    m_LayerStack.insert(m_LayerStack.begin(), std::make_unique<TLayer>());
    m_LayerStack[0]->OnCreate();
  }
  template <typename TLayer>
    requires(std::is_base_of_v<Layer, TLayer>)
  void push_layer() {
    m_LayerStack.push_back(std::make_unique<TLayer>());
    m_LayerStack[m_LayerStack.size() - 1]->OnCreate();
  }

  void UpdateLayers(float ts) {

    for (auto &layer : m_LayerStack) {

      layer->OnUpdate(ts);
    }
  }
  void RenderUILayer() { m_LayerStack[0]->OnRender(); }
  void RenderLayers() {

    for (auto &layer : m_LayerStack) {
      layer->OnRender();
    }
  }

  void NotifyEvents(EventQueue &events) {

    for (auto &event : events) {

      for (auto &layer : m_LayerStack) {
        layer->OnEvent(event);
      }
    }
  }

private:
  std::vector<std::unique_ptr<Layer>> m_LayerStack;
};

} // namespace eHaz

#endif
