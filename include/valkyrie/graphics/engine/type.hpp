//
// Created by Maxwell on 2021-01-27.
//

#ifndef VALKYRIE_GRAPHICS_ENGINE_TYPES_HPP
#define VALKYRIE_GRAPHICS_ENGINE_TYPES_HPP

#include <valkyrie/adt/flat_map.hpp>
#include <valkyrie/adt/small_array.hpp>
#include <valkyrie/utility/string.hpp>

namespace valkyrie::graphics::engine{

  struct RegisteredType{

  };

  struct Module;


  struct TypeInterface{
    u32 id;

  };

  struct Type{
    u32                     id;
    bool                    isPrimitive;
    String            name;
    u32                     size;
    u32                     alignment;
    DynamicArray<u32> implementedInterfaces;
  };




  struct TypeMode{ // Akin to Rust traits kinda

    struct Requirement{
      u32  modeId;

    };


    u32  id;
    String name;

  };

  struct TypeDescriptor{
    const Type* pUnderlyingType;
    u32 objectCount; // Any value other than 1 indicates an array, and provides its length. if objectCount is UINT32_MAX, the array is dynamically sized, and its length is stored within the array object itself.
  };

  struct ClassMember{
    TypeDescriptor type;
    String   memberName;
    u32            objectOffset;
  };

  struct Class : Type{
    small_array<ClassMember> memberVariables;
  };

  struct Module{
    u32                       id;
    String              name;
    String              path;
    String              aliasName;
    u32                       parentId;
    small_array<u32>     childrenIds;
    flat_map<u32, Class> types;
    byte*                     moduleData;
  };

  struct EngineInstance{
    u32 nextModuleId;
    u32 nextTypeId;
    flat_map<u32, Module> modules;
  };

  struct FunctionTableEntry{

  };

  struct DynamicFunctionTable{

  };

}

#endif //VALKYRIE_GRAPHICS_ENGINE_TYPES_HPP
