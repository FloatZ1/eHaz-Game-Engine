#include "Core/EventQueue.hpp"
#include <memory>

namespace eHaz {

std::unique_ptr<EventQueue> EventQueue::s_Instance = nullptr;

}
