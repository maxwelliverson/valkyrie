//
// Created by Maxwell on 2020-11-16.
//

#ifndef VALKYRIE_SPIRV_HPP
#define VALKYRIE_SPIRV_HPP

#include <spirv/enums.hpp>
#include <valkyrie/ADT/arrays.hpp>
#include <valkyrie/utility/enums.hpp>
#include <valkyrie/adt/string_view.hpp>
#include <valkyrie/primitives.hpp>

#define VK_spv_version 0x10500
#define VK_spv_revision 3



namespace valkyrie::Spv {

  using SpvId = u32;

  inline constexpr static u32 MagicNumber = 0x07230203;
  inline constexpr static u32 Version = VK_spv_version;
  inline constexpr static u32 Revision = VK_spv_revision;
  inline constexpr static u32 OpCodeMask = 0xffff;
  inline constexpr static u32 WordCountShift = 16;




  // Reflect
  
  namespace Reflect {
    
    inline constexpr static u32 maxArrayDims = 32;
    inline constexpr static u32 maxDescriptorSets = 64;
    inline constexpr static auto bindingNumberDontChange = ~0;
    inline constexpr static auto bindingSetDontChange = ~0;


    struct NumericTraits {
      struct Scalar {
        u32                        width;
        u32                        signedness;
      } scalar;

      struct Vector {
        u32                        component_count;
      } vector;

      struct Matrix {
        u32                        column_count;
        u32                        row_count;
        u32                        stride; // Measured in bytes
      } matrix;
    };
    struct ImageTraits {
      Dim    dim;
      u32    depth;
      u32    arrayed;
      u32    ms; // 0: single-sampled; 1: multisampled
      u32    sampled;
      ImageFormat image_format;
    };
    struct ArrayTraits {
      u32                          dimsCount;
      u32                          dims[maxArrayDims];
      u32                          stride; // Measured in bytes
    };
    struct BindingArrayTraits {
      u32                          dimsCount;
      u32                          dims[maxArrayDims];
    };
    struct TypeDescription {
      u32                id;
      Op                 op;
      const utf8*        typeName;
      const utf8*        structMemberName;
      StorageClass       storageClass;
      Type               typeFlags;
      Decoration         decorationFlags;

      struct Traits {
        NumericTraits    numeric;
        ImageTraits      image;
        ArrayTraits      array;
      } traits;

      u32                member_count;
      struct TypeDescription* members;
    };
    struct InterfaceVariable {
      u32                  spirv_id;
      const utf8*          name;
      u32                  location;
      StorageClass         storage_class;
      const utf8*          semantic;
      Decoration           decoration_flags;
      BuiltIn              built_in;
      NumericTraits        numeric;
      ArrayTraits          array;

      u32                  member_count;
      struct InterfaceVariable* members;

      Format               format;

      // NOTE: SPIR-V shares type references for variables
      //       that have the same underlying type. This means
      //       that the same type name will appear for multiple
      //       variables.
      TypeDescription*     type_description;

      struct {
        u32                location;
      } word_offset;
    };
    struct BlockVariable {
      u32                     spirv_id;
      const utf8*             name;
      u32                     offset;           // Measured in bytes
      u32                     absolute_offset;  // Measured in bytes
      u32                     size;             // Measured in bytes
      u32                     padded_size;      // Measured in bytes
      Decoration              decoration_flags;
      NumericTraits           numeric;
      ArrayTraits             array;
      Variable                flags;

      u32                          member_count;
      struct BlockVariable*   members;

      TypeDescription*        type_description;
    };
    struct DescriptorBinding {
      u32                       spirv_id;
      const utf8*               name;
      u32                       binding;
      u32                       input_attachment_index;
      u32                       set;
      DescriptorType            descriptor_type;
      ResourceType              resource_type;
      ImageTraits               image;
      BlockVariable             block;
      BindingArrayTraits        array;
      u32                       count;
      u32                       accessed;
      u32                       uav_counter_id;
      struct DescriptorBinding* uav_counter_binding;

      TypeDescription*          type_description;

      struct {
        u32                          binding;
        u32                          set;
      } word_offset;
    };
    struct DescriptorSet {
      u32                set;
      u32                binding_count;
      DescriptorBinding**     bindings;
    };
    struct EntryPoint {
      const utf8*        name;
      u32                id;

      ExecutionModel     spirv_execution_model;
      Bits::ReflectShaderStage shader_stage;

      u32                input_variable_count;
      InterfaceVariable* input_variables;
      u32                output_variable_count;
      InterfaceVariable* output_variables;

      u32                descriptor_set_count;
      DescriptorSet*     descriptor_sets;

      u32                used_uniform_count;
      u32*               used_uniforms;
      u32                used_push_constant_count;
      u32*               used_push_constants;

      struct LocalSize {
        u32              x;
        u32              y;
        u32              z;
      } local_size;
    };
    struct ShaderModuleStruct {
      Generator          generator;
      const utf8*        entry_point_name;
      u32                entry_point_id;
      u32                entry_point_count;
      EntryPoint*        entry_points;
      SourceLanguage     source_language;
      u32                source_language_version;
      const utf8*        source_file;
      const utf8*        source_source;
      ExecutionModel     spirv_execution_model;
      ShaderStage        shader_stage;
      u32                descriptor_binding_count;
      DescriptorBinding* descriptor_bindings;
      u32                descriptor_set_count;
      DescriptorSet      descriptor_sets[maxDescriptorSets];
      u32                input_variable_count;
      InterfaceVariable* input_variables;
      u32                output_variable_count;
      InterfaceVariable* output_variables;
      u32                push_constant_block_count;
      BlockVariable*     push_constant_blocks;

      struct Internal {
        size_t           spirv_size;
        u32*             spirv_code;
        u32              spirv_word_count;

        size_t           type_description_count;
        TypeDescription* type_descriptions;
      } * _internal;

    };

    template <typename T>
    class ResultSpan{
      Result result_ = Result::Unknown;
      u32 Size = 0;
      T* pData = nullptr;
    public:

      using value_type = T;
      using pointer = T*;
      using reference = T&;
      using iterator = pointer;
      using size_type = u32;

      ResultSpan() = default;
      ResultSpan(Result result_) noexcept : result_(result_){}
      ResultSpan(pointer pData, size_type Size) noexcept : pData(pData), Size(Size){}
      ResultSpan(const ResultSpan&) noexcept = default;
      ResultSpan(ResultSpan&&) noexcept = default;

      pointer data() const noexcept { return pData; }
      reference operator[](size_t N) const noexcept { return pData[N]; }

      size_type size() const noexcept { return Size; }
      bool empty() const noexcept { return !Size; }

      iterator begin() const noexcept { return data(); }
      iterator end() const noexcept { return data() + size(); }


      reference front() const noexcept { return pData[0]; }
      reference back() const noexcept { return pData[Size - 1]; }

      Result result() const noexcept { return result_; }

      explicit operator bool() const noexcept {
        return result_ == Result::Success;
      }
    };



    template <typename T>
    class Expected;
    template <typename T> requires(sizeof(T) <= sizeof(void*) && is_register_copyable<T>)
    class Expected<T>{
      Result result_ = Result::Unknown;
      T value_;
    public:
      Expected() = default;
      Expected(Result result) noexcept : result_(result){}
      Expected(T value) noexcept : value_(value){}
      Expected(const Expected&) = default;
      Expected(Expected&&) noexcept = default;


      T& operator*() & noexcept {
        VK_assert(result_ == Result::Success);
        return value_;
      }
      const T& operator*() const & noexcept {
        VK_assert(result_ == Result::Success);
        return value_;
      }
      T&& operator*() && noexcept {
        VK_assert(result_ == Result::Success);
        return std::move(value_);
      }
      const T&& operator*() const && noexcept {
        VK_assert(result_ == Result::Success);
        return std::move(value_);
      }

      explicit operator T() const noexcept { return value_; }

      Result result() const noexcept { return result_; }

      explicit operator bool() const noexcept { return result_ == Result::Success; }
    };

    class ShaderModule;
    class Compiler;
    class ShaderObject;
    class ShaderLibrary;

    class CompilerOptions{
    public:
      void addMacroDefinition(std::string_view Macro) noexcept;
      void addMacroDefinition(std::string_view Macro, std::string_view Value) noexcept;
      void addIncludePath(std::string_view Path) noexcept;
      void addIncludeFile(std::string_view Filename, std::string_view fileContents) noexcept;
    };
    class Compiler{
      class Impl;
      Impl* pImpl;
    public:
      
    };
    class Assembler{};
    class Disassembler{};
    class Linker{};
    class Optimizer{};
    class Validator{};
    class Fuzzer{};
    class Reducer{};

    class ShaderModule {
    public:

      class Internal;

      ShaderModule() = default;
      ShaderModule(size_t size, const void* p_code) noexcept;
      ShaderModule(Span<u8> code) noexcept : ShaderModule(code.size(), code.data()){}
      ShaderModule(Span<u32> code) noexcept : ShaderModule(code.size() * sizeof(u32), code.data()){}
      ~ShaderModule();

      Result getStatus() const noexcept;

      Span<const u32> getCode() const noexcept;

      /*u32        getCodeSize() const;
      const u32* getCode() const;*/

      string_view getEntryPointName() const noexcept;

      string_view getSourceFile() const noexcept;

      u32         getEntryPointCount() const noexcept;
      string_view getEntryPointName(u32 index) const;

      ShaderStage getShaderStage() const noexcept;

      ResultSpan<const DescriptorBinding> enumerateDescriptorBindings() const;
      ResultSpan<const DescriptorBinding> enumerateEntryPointDescriptorBindings(string_view entry_point) const;
      ResultSpan<const DescriptorSet>     enumerateDescriptorSets() const ;
      ResultSpan<const DescriptorSet>     enumerateEntryPointDescriptorSets(string_view entry_point) const ;
      ResultSpan<const InterfaceVariable> enumerateInputVariables() const;
      ResultSpan<const InterfaceVariable> enumerateEntryPointInputVariables(string_view entry_point) const;
      ResultSpan<const InterfaceVariable> enumerateOutputVariables() const;
      ResultSpan<const InterfaceVariable> enumerateEntryPointOutputVariables(string_view entry_point) const;
      ResultSpan<const BlockVariable>     enumeratePushConstantBlocks() const;
      ResultSpan<const BlockVariable>     enumerateEntryPointPushConstantBlocks(string_view entry_point) const;

      Expected<const DescriptorBinding*>  getDescriptorBinding(u32 binding_number, u32 set_number) const;
      Expected<const DescriptorBinding*>  getEntryPointDescriptorBinding(string_view entry_point, u32 binding_number, u32 set_number) const;
      Expected<const DescriptorSet*>      getDescriptorSet(u32 set_number) const;
      Expected<const DescriptorSet*>      getEntryPointDescriptorSet(string_view entry_point, u32 set_number) const;
      Expected<const InterfaceVariable*>  getInputVariableByLocation(u32 location) const;

      Expected<const InterfaceVariable*>  getEntryPointInputVariableByLocation(string_view entry_point, u32 location) const;
      Expected<const InterfaceVariable*>  getInputVariableBySemantic(string_view semantic) const;
      Expected<const InterfaceVariable*>  getEntryPointInputVariableBySemantic(string_view entry_point, string_view semantic) const;
      Expected<const InterfaceVariable*>  getOutputVariableByLocation(u32 location) const;

      Expected<const InterfaceVariable*>  getEntryPointOutputVariableByLocation(string_view entry_point, u32 location) const;
      Expected<const InterfaceVariable*>  getOutputVariableBySemantic(string_view semantic) const;
      Expected<const InterfaceVariable*>  getEntryPointOutputVariableBySemantic(string_view entry_point, string_view semantic) const;
      Expected<const BlockVariable*>      getPushConstantBlock(u32 index) const;
      Expected<const BlockVariable*>      getEntryPointPushConstantBlock(string_view entry_point) const;

      Result ChangeDescriptorBindingNumbers(const DescriptorBinding* p_binding,
                                            u32 new_binding_number = bindingNumberDontChange,
                                            u32 optional_new_set_number = bindingSetDontChange);

      Result ChangeDescriptorSetNumber(const DescriptorSet* p_set, u32 new_set_number = bindingSetDontChange);
      Result ChangeInputVariableLocation(const InterfaceVariable* p_input_variable, u32 new_location);
      Result ChangeOutputVariableLocation(const InterfaceVariable* p_output_variable, u32 new_location);

    private:
      mutable Result m_result = Result::NotReady;
      ShaderModuleStruct m_module = {};
    };
  }

  

  

  

  
  
  

}

#undef VK_inline_bitflag

#endif//VALKYRIE_SPIRV_HPP
