//
// Created by Maxwell on 2021-01-27.
//

#ifndef VALKYRIE_GRAPHICS_ENGINE_TYPES_HPP
#define VALKYRIE_GRAPHICS_ENGINE_TYPES_HPP

#include <valkyrie/Core/Utility/String.hpp>
#include <valkyrie/Core/ADT/SmallArray.hpp>
#include <valkyrie/Core/ADT/FlatMap.hpp>

namespace valkyrie::Graphics::Engine{

  struct RegisteredType{

  };

  struct Module;


  struct TypeInterface{
    u32 id;

  };

  struct Type{
    u32                     id;
    bool                    isPrimitive;
    Core::String            name;
    u32                     size;
    u32                     alignment;
    Core::DynamicArray<u32> implementedInterfaces;
  };




  struct TypeMode{ // Akin to Rust traits kinda

    struct Requirement{
      u32  modeId;

    };


    u32  id;
    Core::String name;

  };

  struct TypeDescriptor{
    const Type* pUnderlyingType;
    u32 objectCount; // Any value other than 1 indicates an array, and provides its length. if objectCount is UINT32_MAX, the array is dynamically sized, and its length is stored within the array object itself.
  };

  struct ClassMember{
    TypeDescriptor type;
    Core::String   memberName;
    u32            objectOffset;
  };

  struct Class : Type{
    Core::SmallArray<ClassMember> memberVariables;
  };

  struct Module{
    u32                       id;
    Core::String              name;
    Core::String              path;
    Core::String              aliasName;
    u32                       parentId;
    Core::SmallArray<u32>     childrenIds;
    Core::FlatMap<u32, Class> types;
    byte*                     moduleData;
  };

  struct EngineInstance{
    u32 nextModuleId;
    u32 nextTypeId;
    Core::FlatMap<u32, Module> modules;
  };

  struct FunctionTableEntry{

  };

  struct DynamicFunctionTable{

  };

}

#endif //VALKYRIE_GRAPHICS_ENGINE_TYPES_HPP
