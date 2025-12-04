#ifndef EHAZ_COMPONENT_MACRO_HPP
#define EHAZ_COMPONENT_MACRO_HPP

#include <boost/preprocessor.hpp>
#include <entt/entt.hpp>

using namespace entt::literals;

// ------------------------------------------------------------
// Generate one .data<&Type::Field>("Field"_hs)
// ------------------------------------------------------------
#define EHAZ_REGISTER_FIELD(r, Type, Field)                                    \
  type.data<&Type::Field>(#Field##_hs);

// ------------------------------------------------------------
// Iterate over all fields in BOOST_PP sequence
// ------------------------------------------------------------
#define EHAZ_REGISTER_FIELDS(Type, FIELDS)                                     \
  BOOST_PP_SEQ_FOR_EACH(EHAZ_REGISTER_FIELD, Type, FIELDS)

// ------------------------------------------------------------
// MASTER MACRO
// ------------------------------------------------------------
#define REGISTER_COMPONENT(Type, FIELDS, ...)                                  \
  struct Type {                                                                \
    __VA_ARGS__                                                                \
    static void RegisterComponent();                                           \
  };                                                                           \
                                                                               \
  inline void Type::RegisterComponent() {                                      \
    auto type = entt::meta<Type>().type(#Type##_hs);                           \
    EHAZ_REGISTER_FIELDS(Type, FIELDS);                                        \
  }

#endif
