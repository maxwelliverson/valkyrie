//
// Created by Maxwell on 2020-11-16.
//

#include <spirv/SpirV.hpp>
//#include <spirv/Compiler.hpp>
#include <shaderc/shaderc.hpp>

using namespace valkyrie::Spv;
using valkyrie::utf8;
using valkyrie::u32;

#define IsNull(ptr) \
  (ptr == NULL)

#define IsNotNull(ptr) \
  (ptr != NULL)

#define SafeFree(ptr)    \
  {                      \
     if (ptr != NULL) {  \
       free((void*)ptr); \
       ptr = NULL;       \
     }                   \
  }

#define CHECKED_READU32(parser, word_offset, value)                                      \
  {                                                                                      \
    Reflect::Result checked_readu32_result = ReadU32(parser,                            \
                                                      word_offset, (u32*)&(value)); \
    if (checked_readu32_result != Reflect::Result::Success) {                          \
      return checked_readu32_result;                                                     \
    }                                                                                    \
  }

#define CHECKED_READU32_CAST(parser, word_offset, cast_to_type, value)         \
  {                                                                            \
    u32 checked_readu32_cast_u32 = UINT32_MAX;                            \
    Reflect::Result checked_readu32_cast_result = ReadU32(parser,             \
                                      word_offset,                             \
                                      (u32*)&(checked_readu32_cast_u32)); \
    if (checked_readu32_cast_result != Reflect::Result::Success) {           \
      return checked_readu32_cast_result;                                      \
    }                                                                          \
    value = (cast_to_type)checked_readu32_cast_u32;                            \
  }

#define IF_READU32(result, parser, word_offset, value)          \
  if ((result) == Reflect::Result::Success) {                 \
    result = ReadU32(parser, word_offset, (u32*)&(value)); \
  }

#define IF_READU32_CAST(result, parser, word_offset, cast_to_type, value) \
  if ((result) == Reflect::Result::Success) {                           \
    u32 if_readu32_cast_u32 = UINT32_MAX;                            \
    result = ReadU32(parser, word_offset, &if_readu32_cast_u32);          \
    if ((result) == Reflect::Result::Success) {                         \
      value = (cast_to_type)if_readu32_cast_u32;                          \
    }                                                                     \
  }

class Reflect::ShaderModule::Internal{
public:
  mutable Result          result = Result::NotReady;
  Generator               generator;
  const utf8*             entry_point_name;
  u32                entry_point_id;
  u32                entry_point_count;
  EntryPoint*             entry_points;
  SourceLanguage          source_language;
  u32                source_language_version;
  const utf8*             source_file;
  const utf8*             source_source;
  ExecutionModel          spirv_execution_model;
  ShaderStage             shader_stage;
  u32                descriptor_binding_count;
  DescriptorBinding*      descriptor_bindings;
  u32                descriptor_set_count;
  DescriptorSet           descriptor_sets[maxDescriptorSets];
  u32                input_variable_count;
  InterfaceVariable*      input_variables;
  u32                output_variable_count;
  InterfaceVariable*      output_variables;
  u32                push_constant_block_count;
  BlockVariable*          push_constant_blocks;

  struct Storage {
    size_t spirv_size;
    u32*   spirv_code;
    u32    spirv_word_count;

    size_t           type_description_count;
    TypeDescription* ype_descriptions;
  } * _internal;
};

namespace {
  inline constexpr u32 startingWordIndex = 5;
  inline constexpr u32 wordSize = sizeof(u32);
  inline constexpr u32 byteWidth = CHAR_BIT;
  inline constexpr u32 minimumFileSize = startingWordIndex * wordSize;
  inline constexpr u32 dataAlignment = 4 * wordSize;
  inline constexpr u32 accessChainIndexOffset = 4;
  inline constexpr u32 invalidValue = 0xFFFF'FFFFu;
  inline constexpr u32 nodeNameLength = 1024;
  inline constexpr u32 imageSampled = 1;
  inline constexpr u32 imageStorage = 2;

  struct ArrayTraits {
    u32 element_type_id;
    u32 length_id;
  };
  struct ImageTraits {
    u32              sampled_type_id;
    Dim              dim;
    u32              depth;
    u32              arrayed;
    u32              ms;
    u32              sampled;
    ImageFormat      image_format;
  };
  struct NumberDecoration {
    u32              word_offset;
    u32              value;
  };
  struct StringDecoration {
    u32                   word_offset;
    const utf8* value;
  };
  struct Decorations {
    bool                  is_block;
    bool                  is_buffer_block;
    bool                  is_row_major;
    bool                  is_column_major;
    bool                  is_built_in;
    bool                  is_noperspective;
    bool                  is_flat;
    bool                  is_non_writable;
    NumberDecoration      set;
    NumberDecoration      binding;
    NumberDecoration      input_attachment_index;
    NumberDecoration      location;
    NumberDecoration      offset;
    NumberDecoration      uav_counter_buffer;
    StringDecoration      semantic;
    u32                   array_stride;
    u32                   matrix_stride;
    BuiltIn               built_in;
  };
  struct Node {
    u32              result_id;
    Op               op;
    u32              result_type_id;
    u32              type_id;
    StorageClass     storage_class;
    u32              word_offset;
    u32              word_count;
    bool             is_type;

    ArrayTraits      array_traits;
    ImageTraits      image_traits;
    u32              image_type_id;

    const utf8*      name;
    Decorations      decorations;
    u32              member_count;
    const utf8**     member_names;
    Decorations*     member_decorations;
  };
  struct String {
    u32              result_id;
    const utf8*      string;
  };
  struct Function {
    u32               id;
    u32               callee_count;
    u32*              callees;
    struct Function** callee_ptrs;
    u32               accessed_ptr_count;
    u32*              accessed_ptrs;
  };
  struct AccessChain {
    u32              result_id;
    u32              result_type_id;
    //
    // Pointing to the base of a composite object.
    // Generally the id of descriptor block variable
    u32              base_id;
    //
    // From spec:
    //   The first index in Indexes will select the
    //   top-level member/element/component/element
    //   of the base composite
    u32              index_count;
    u32*             indexes;
  };
  struct Parser {
    size_t            spirv_word_count;
    u32*              spirv_code;
    u32               string_count;
    String*           strings;
    SourceLanguage    source_language;
    u32               source_language_version;
    u32               source_file_id;
    String            source_embedded;
    size_t            node_count;
    Node*             nodes;
    u32               entry_point_count;
    u32               function_count;
    Function*         functions;
    u32               access_chain_count;
    AccessChain*      access_chains;

    u32               type_count;
    u32               descriptor_count;
    u32               push_constant_count;
  };
  
  
  
  inline u32 Max(u32 a, u32 b) {
    return a > b ? a : b;
  }
  inline u32 RoundUp(u32 value, u32 multiple) {
    assert(multiple && ((multiple & (multiple - 1)) == 0));
    return (value + multiple - 1) & ~(multiple - 1);
  }
  inline int SortCompareUint32(const void* a, const void* b) {
    const u32* p_a = (const u32*)a;
    const u32* p_b = (const u32*)b;

    return (int)*p_a - (int)*p_b;
  }
  inline size_t DedupSortedUint32(u32* arr, size_t size) {
    if (size == 0) {
      return 0;
    }
    size_t dedup_idx = 0;
    for (size_t i = 0; i < size; ++i) {
      if (arr[dedup_idx] != arr[i]) {
        ++dedup_idx;
        arr[dedup_idx] = arr[i];
      }
    }
    return dedup_idx+1;
  }
  inline bool SearchSortedUint32(const u32* arr, size_t size, u32 target) {
    size_t lo = 0;
    size_t hi = size;
    while (lo < hi) {
      size_t mid = (hi - lo) / 2 + lo;
      if (arr[mid] == target) {
        return true;
      } else if (arr[mid] < target) {
        lo = mid+1;
      } else {
        hi = mid;
      }
    }
    return false;
  }
  inline Reflect::Result IntersectSortedUint32(
      const u32* p_arr0,
      size_t          arr0_size,
      const u32* p_arr1,
      size_t          arr1_size,
      u32**      pp_res,
      size_t*         res_size
  )
  {
    *res_size = 0;
    const u32* arr0_end = p_arr0 + arr0_size;
    const u32* arr1_end = p_arr1 + arr1_size;

    const u32* idx0 = p_arr0;
    const u32* idx1 = p_arr1;
    while (idx0 != arr0_end && idx1 != arr1_end) {
      if (*idx0 < *idx1) {
        ++idx0;
      } else if (*idx0 > *idx1) {
        ++idx1;
      } else {
        ++*res_size;
        ++idx0;
        ++idx1;
      }
    }

    *pp_res = NULL;
    if (*res_size > 0) {
      *pp_res = (u32*)calloc(*res_size, sizeof(**pp_res));
      if (IsNull(*pp_res)) {
        return Reflect::Result::ErrorAllocFailed;
      }
      u32* idxr = *pp_res;
      idx0 = p_arr0;
      idx1 = p_arr1;
      while (idx0 != arr0_end && idx1 != arr1_end) {
        if (*idx0 < *idx1) {
          ++idx0;
        } else if (*idx0 > *idx1) {
          ++idx1;
        } else {
          *(idxr++) = *idx0;
          ++idx0;
          ++idx1;
        }
      }
    }
    return Reflect::Result::Success;
  }
  inline bool InRange(const Parser* p_parser, u32 index)
  {
    bool in_range = false;
    if (IsNotNull(p_parser)) {
      in_range = (index < p_parser->spirv_word_count);
    }
    return in_range;
  }
  inline Reflect::Result ReadU32(Parser* p_parser, u32 word_offset, u32* p_value) {
    assert(IsNotNull(p_parser));
    assert(IsNotNull(p_parser->spirv_code));
    assert(InRange(p_parser, word_offset));
    Reflect::Result result = Reflect::Result::ErrorSpirvUnexpectedEof;
    if (IsNotNull(p_parser) && IsNotNull(p_parser->spirv_code) && InRange(p_parser, word_offset)) {
      *p_value = *(p_parser->spirv_code + word_offset);
      result = Reflect::Result::Success;
    }
    return result;
  }
  inline Reflect::Result ReadStr(
      Parser*   p_parser,
      u32  word_offset,
      u32  word_index,
      u32  word_count,
      u32* p_buf_size,
      char*     p_buf
  )
  {
    u32 limit = (word_offset + word_count);
    assert(IsNotNull(p_parser));
    assert(IsNotNull(p_parser->spirv_code));
    assert(InRange(p_parser, limit));
    Reflect::Result result = Reflect::Result::ErrorSpirvUnexpectedEof;
    if (IsNotNull(p_parser) && IsNotNull(p_parser->spirv_code) && InRange(p_parser, limit)) {
      const utf8* c_str = (const utf8*)(p_parser->spirv_code + word_offset + word_index);
      u32 n = word_count * wordSize;
      u32 length_with_terminator = 0;
      for (u32 i = 0; i < n; ++i) {
        char c = *(c_str + i);
        if (c == 0) {
          length_with_terminator = i + 1;
          break;
        }
      }

      if (length_with_terminator > 0) {
        result = Reflect::Result::ErrorNullPointer;
        if (IsNotNull(p_buf_size) && IsNotNull(p_buf)) {
          result = Reflect::Result::ErrorRangeExceeded;
          if (length_with_terminator <= *p_buf_size) {
            memset(p_buf, 0, *p_buf_size);
            memcpy(p_buf, c_str, length_with_terminator);
            result = Reflect::Result::Success;
          }
        }
        else {
          if (IsNotNull(p_buf_size)) {
            *p_buf_size = length_with_terminator;
            result = Reflect::Result::Success;
          }
        }
      }
    }
    return result;
  }
  inline Reflect::Decoration ApplyDecorations(const Decorations* p_decoration_fields)
  {
    Reflect::Decoration decorations;
    if (p_decoration_fields->is_block) {
      decorations |= Reflect::Decoration::Block;
    }
    if (p_decoration_fields->is_buffer_block) {
      decorations |= Reflect::Decoration::BufferBlock;
    }
    if (p_decoration_fields->is_row_major) {
      decorations |= Reflect::Decoration::RowMajor;
    }
    if (p_decoration_fields->is_column_major) {
      decorations |= Reflect::Decoration::ColumnMajor;
    }
    if (p_decoration_fields->is_built_in) {
      decorations |= Reflect::Decoration::BuiltIn;
    }
    if (p_decoration_fields->is_noperspective) {
      decorations |= Reflect::Decoration::Noperspective;
    }
    if (p_decoration_fields->is_flat) {
      decorations |= Reflect::Decoration::Flat;
    }
    if (p_decoration_fields->is_non_writable) {
      decorations |= Reflect::Decoration::NonWritable;
    }
    return decorations;
  }
  inline void ApplyNumericTraits(const Reflect::TypeDescription* p_type, Reflect::NumericTraits* p_numeric_traits) {
    memcpy(p_numeric_traits, &p_type->traits.numeric, sizeof(p_type->traits.numeric));
  }
  inline void ApplyArrayTraits(const Reflect::TypeDescription* p_type, Reflect::ArrayTraits* p_array_traits) {
    memcpy(p_array_traits, &p_type->traits.array, sizeof(p_type->traits.array));
  }
  inline Node* FindNode(Parser* p_parser, u32 result_id) {
    Node* p_node = NULL;
    for (size_t i = 0; i < p_parser->node_count; ++i) {
      Node* p_elem = &(p_parser->nodes[i]);
      if (p_elem->result_id == result_id) {
        p_node = p_elem;
        break;
      }
    }
    return p_node;
  }
  inline Reflect::TypeDescription* FindType(Reflect::ShaderModuleStruct* p_module, u32 type_id)
  {
    Reflect::TypeDescription* p_type = NULL;
    for (size_t i = 0; i < p_module->_internal->type_description_count; ++i) {
      Reflect::TypeDescription* p_elem = &(p_module->_internal->type_descriptions[i]);
      if (p_elem->id == type_id) {
        p_type = p_elem;
        break;
      }
    }
    return p_type;
  }
  inline Reflect::Result CreateParser(size_t size, void* p_code, Parser* p_parser)
  {
    if (p_code == NULL) {
      return Reflect::Result::ErrorNullPointer;
    }

    if (size < minimumFileSize) {
      return Reflect::Result::ErrorSpirvInvalidCodeSize;
    }
    if ((size % 4) != 0) {
      return Reflect::Result::ErrorSpirvInvalidCodeSize;
    }

    p_parser->spirv_word_count = size / wordSize;
    p_parser->spirv_code = (u32*)p_code;

    if (p_parser->spirv_code[0] != MagicNumber) {
      return Reflect::Result::ErrorSpirvInvalidMagicNumber;
    }

    return Reflect::Result::Success;
  }
  inline void DestroyParser(Parser* p_parser)
  {
    if (!IsNull(p_parser->nodes)) {
      // Free nodes
      for (size_t i = 0; i < p_parser->node_count; ++i) {
        Node* p_node = &(p_parser->nodes[i]);
        if (IsNotNull(p_node->member_names)) {
          SafeFree(p_node->member_names);
        }
        if (IsNotNull(p_node->member_decorations)) {
          SafeFree(p_node->member_decorations);
        }
      }

      // Free functions
      for (size_t i = 0; i < p_parser->function_count; ++i) {
        SafeFree(p_parser->functions[i].callees);
        SafeFree(p_parser->functions[i].callee_ptrs);
        SafeFree(p_parser->functions[i].accessed_ptrs);
      }

      // Free access chains
      for (u32 i = 0; i < p_parser->access_chain_count; ++i) {
        SafeFree(p_parser->access_chains[i].indexes);
      }

      SafeFree(p_parser->nodes);
      SafeFree(p_parser->strings);
      SafeFree(p_parser->functions);
      SafeFree(p_parser->access_chains);
      p_parser->node_count = 0;
    }
  }
  inline Reflect::Result ParseNodes(Parser* p_parser)
  {
    assert(IsNotNull(p_parser));
    assert(IsNotNull(p_parser->spirv_code));

    u32* p_spirv = p_parser->spirv_code;
    u32 spirv_word_index = startingWordIndex;

    // Count nodes
    u32 node_count = 0;
    while (spirv_word_index < p_parser->spirv_word_count) {
      u32 word = p_spirv[spirv_word_index];
      Op op = (Op)(word & 0xFFFF);
      u32 node_word_count = (word >> 16) & 0xFFFF;
      if (node_word_count == 0) {
        return Reflect::Result::ErrorSpirvInvalidInstruction;
      }
      if (op == Op::OpAccessChain) {
        ++(p_parser->access_chain_count);
      }
      spirv_word_index += node_word_count;
      ++node_count;
    }

    if (node_count == 0) {
      return Reflect::Result::ErrorSpirvUnexpectedEof;
    }

    // Allocate nodes
    p_parser->node_count = node_count;
    p_parser->nodes = (Node*)calloc(p_parser->node_count, sizeof(*(p_parser->nodes)));
    if (IsNull(p_parser->nodes)) {
      return Reflect::Result::ErrorAllocFailed;
    }
    // Mark all nodes with an invalid state
    for (u32 i = 0; i < node_count; ++i) {
      p_parser->nodes[i].op = (Op)invalidValue;
      p_parser->nodes[i].storage_class = (StorageClass)invalidValue;
      p_parser->nodes[i].decorations.set.value = (u32)invalidValue;
      p_parser->nodes[i].decorations.binding.value = (u32)invalidValue;
      p_parser->nodes[i].decorations.location.value = (u32)invalidValue;
      p_parser->nodes[i].decorations.offset.value = (u32)invalidValue;
      p_parser->nodes[i].decorations.uav_counter_buffer.value = (u32)invalidValue;
      p_parser->nodes[i].decorations.built_in = (BuiltIn)invalidValue;
    }
    // Mark source file id node
    p_parser->source_file_id = (u32)invalidValue;

    // Function node
    u32 function_node = (u32)invalidValue;

    // Allocate access chain
    if (p_parser->access_chain_count > 0) {
      p_parser->access_chains = (AccessChain*)calloc(p_parser->access_chain_count, sizeof(*(p_parser->access_chains)));
      if (IsNull(p_parser->access_chains)) {
        return Reflect::Result::ErrorAllocFailed;
      }
    }

    // Parse nodes
    u32 node_index = 0;
    u32 access_chain_index = 0;
    spirv_word_index = startingWordIndex;
    while (spirv_word_index < p_parser->spirv_word_count) {
      u32 word = p_spirv[spirv_word_index];
      Op op = (Op)(word & 0xFFFF);
      u32 node_word_count = (word >> 16) & 0xFFFF;

      Node* p_node = &(p_parser->nodes[node_index]);
      p_node->op = op;
      p_node->word_offset = spirv_word_index;
      p_node->word_count = node_word_count;

      switch (p_node->op) {
        default: break;

        case Op::OpString: {
          ++(p_parser->string_count);
        }
          break;

        case Op::OpSource: {
          CHECKED_READU32_CAST(p_parser, p_node->word_offset + 1, SourceLanguage, p_parser->source_language);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_parser->source_language_version);
          if (p_node->word_count >= 4) {
            CHECKED_READU32(p_parser, p_node->word_offset + 3, p_parser->source_file_id);
          }
        }
          break;

        case Op::OpEntryPoint: {
          ++(p_parser->entry_point_count);
        }
          break;

        case Op::OpName:
        case Op::OpMemberName:
        {
          u32 member_offset = (p_node->op == Op::OpMemberName) ? 1 : 0;
          u32 name_start = p_node->word_offset + member_offset + 2;
          p_node->name = (const utf8*)(p_parser->spirv_code + name_start);
        }
          break;

        case Op::OpTypeStruct:
        {
          p_node->member_count = p_node->word_count - 2;
        } // Fall through
        case Op::OpTypeVoid:
        case Op::OpTypeBool:
        case Op::OpTypeInt:
        case Op::OpTypeFloat:
        case Op::OpTypeVector:
        case Op::OpTypeMatrix:
        case Op::OpTypeSampler:
        case Op::OpTypeOpaque:
        case Op::OpTypeFunction:
        case Op::OpTypeEvent:
        case Op::OpTypeDeviceEvent:
        case Op::OpTypeReserveId:
        case Op::OpTypeQueue:
        case Op::OpTypePipe:
        {
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_node->result_id);
          p_node->is_type = true;
        }
          break;

        case Op::OpTypeImage: {
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_node->result_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_node->image_traits.sampled_type_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 3, p_node->image_traits.dim);
          CHECKED_READU32(p_parser, p_node->word_offset + 4, p_node->image_traits.depth);
          CHECKED_READU32(p_parser, p_node->word_offset + 5, p_node->image_traits.arrayed);
          CHECKED_READU32(p_parser, p_node->word_offset + 6, p_node->image_traits.ms);
          CHECKED_READU32(p_parser, p_node->word_offset + 7, p_node->image_traits.sampled);
          CHECKED_READU32(p_parser, p_node->word_offset + 8, p_node->image_traits.image_format);
          p_node->is_type = true;
        }
          break;

        case Op::OpTypeSampledImage: {
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_node->result_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_node->image_type_id);
          p_node->is_type = true;
        }
          break;

        case Op::OpTypeArray:  {
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_node->result_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_node->array_traits.element_type_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 3, p_node->array_traits.length_id);
          p_node->is_type = true;
        }
          break;

        case Op::OpTypeRuntimeArray:  {
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_node->result_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_node->array_traits.element_type_id);
          p_node->is_type = true;
        }
          break;

        case Op::OpTypePointer: {
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_node->result_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_node->storage_class);
          CHECKED_READU32(p_parser, p_node->word_offset + 3, p_node->type_id);
          p_node->is_type = true;
        }
          break;

        case Op::OpTypeForwardPointer:
        {
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_node->result_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_node->storage_class);
          p_node->is_type = true;
        }
          break;

        case Op::OpConstantTrue:
        case Op::OpConstantFalse:
        case Op::OpConstant:
        case Op::OpConstantComposite:
        case Op::OpConstantSampler:
        case Op::OpConstantNull: {
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_node->result_type_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_node->result_id);
        }
          break;

        case Op::OpSpecConstantTrue:
        case Op::OpSpecConstantFalse:
        case Op::OpSpecConstant:
        case Op::OpSpecConstantComposite:
        case Op::OpSpecConstantOp: {
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_node->result_type_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_node->result_id);
        }
          break;

        case Op::OpVariable:
        {
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_node->type_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_node->result_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 3, p_node->storage_class);
        }
          break;

        case Op::OpLoad:
        {
          // Only load enough so OpDecorate can reference the node, skip the remaining operands.
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_node->result_type_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_node->result_id);
        }
          break;

        case Op::OpAccessChain:
        {
          AccessChain* p_access_chain = &(p_parser->access_chains[access_chain_index]);
          CHECKED_READU32(p_parser, p_node->word_offset + 1, p_access_chain->result_type_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_access_chain->result_id);
          CHECKED_READU32(p_parser, p_node->word_offset + 3, p_access_chain->base_id);
          //
          // accessChainIndexOffset (4) is the number of words up until the first index:
          //   [Node, Result Type Id, Result Id, Base Id, <Indexes>]
          //
          p_access_chain->index_count = (node_word_count - accessChainIndexOffset);
          if (p_access_chain->index_count > 0) {
            p_access_chain->indexes = (u32*)calloc(p_access_chain->index_count, sizeof(*(p_access_chain->indexes)));
            if (IsNull( p_access_chain->indexes)) {
              return Reflect::Result::ErrorAllocFailed;
            }
            // Parse any index values for access chain
            for (u32 index_index = 0; index_index < p_access_chain->index_count; ++index_index) {
              // Read index id
              u32 index_id = 0;
              CHECKED_READU32(p_parser, p_node->word_offset + accessChainIndexOffset + index_index, index_id);
              // Find OpConstant node that contains index value
              Node* p_index_value_node = FindNode(p_parser, index_id);
              if ((p_index_value_node != NULL) && (p_index_value_node->op == Op::OpConstant)) {
                // Read index value
                u32 index_value = UINT32_MAX;
                CHECKED_READU32(p_parser, p_index_value_node->word_offset + 3, index_value);
                assert(index_value != UINT32_MAX);
                // Write index value to array
                p_access_chain->indexes[index_index] = index_value;
              }
            }
          }
          ++access_chain_index;
        }
          break;

        case Op::OpFunction:
        {
          CHECKED_READU32(p_parser, p_node->word_offset + 2, p_node->result_id);
          // Count function definitions, not function declarations.  To determine
          // the difference, set an in-function variable, and then if an OpLabel
          // is reached before the end of the function increment the function
          // count.
          function_node = node_index;
        }
          break;

        case Op::OpLabel:
        {
          if (function_node != (u32)invalidValue) {
            Node* p_func_node = &(p_parser->nodes[function_node]);
            CHECKED_READU32(p_parser, p_func_node->word_offset + 2, p_func_node->result_id);
            ++(p_parser->function_count);
          }
        } // Fall through

        case Op::OpFunctionEnd:
        {
          function_node = (u32)invalidValue;
        }
          break;
      }

      if (p_node->is_type) {
        ++(p_parser->type_count);
      }

      spirv_word_index += node_word_count;
      ++node_index;
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseStrings(Parser* p_parser)
  {
    assert(IsNotNull(p_parser));
    assert(IsNotNull(p_parser->spirv_code));
    assert(IsNotNull(p_parser->nodes));

    // Early out
    if (p_parser->string_count == 0) {
      return Reflect::Result::Success;
    }

    if (IsNotNull(p_parser) && IsNotNull(p_parser->spirv_code) && IsNotNull(p_parser->nodes)) {
      // Allocate string storage
      p_parser->strings = (String*)calloc(p_parser->string_count, sizeof(*(p_parser->strings)));

      u32 string_index = 0;
      for (size_t i = 0; i < p_parser->node_count; ++i) {
        Node* p_node = &(p_parser->nodes[i]);
        if (p_node->op != Op::OpString) {
          continue;
        }

        // Paranoid check against string count
        assert(string_index < p_parser->string_count);
        if (string_index >= p_parser->string_count) {
          return Reflect::Result::ErrorCountMismatch;
        }

        // Result id
        String* p_string = &(p_parser->strings[string_index]);
        CHECKED_READU32(p_parser, p_node->word_offset + 1, p_string->result_id);

        // String
        u32 string_start = p_node->word_offset + 2;
        p_string->string = (const utf8*)(p_parser->spirv_code + string_start);

        // Increment string index
        ++string_index;
      }
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseSource(Parser* p_parser, Reflect::ShaderModuleStruct* p_module)
  {
    assert(IsNotNull(p_parser));
    assert(IsNotNull(p_parser->spirv_code));

    if (IsNotNull(p_parser) && IsNotNull(p_parser->spirv_code)) {
      // Source file
      if (IsNotNull(p_parser->strings)) {
        for (u32 i = 0; i < p_parser->string_count; ++i) {
          String* p_string = &(p_parser->strings[i]);
          if (p_string->result_id == p_parser->source_file_id) {
            p_module->source_file = p_string->string;
            break;
          }
        }
      }
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseFunction(Parser* p_parser, Node* p_func_node, Function* p_func, size_t first_label_index)
  {
    p_func->id = p_func_node->result_id;

    p_func->callee_count = 0;
    p_func->accessed_ptr_count = 0;

    for (size_t i = first_label_index; i < p_parser->node_count; ++i) {
      Node* p_node = &(p_parser->nodes[i]);
      if (p_node->op == Op::OpFunctionEnd) {
        break;
      }
      switch (p_node->op) {
        case Op::OpFunctionCall: {
          ++(p_func->callee_count);
        }
          break;
        case Op::OpLoad:
        case Op::OpAccessChain:
        case Op::OpInBoundsAccessChain:
        case Op::OpPtrAccessChain:
        case Op::OpArrayLength:
        case Op::OpGenericPtrMemSemantics:
        case Op::OpInBoundsPtrAccessChain:
        case Op::OpStore:
        {
          ++(p_func->accessed_ptr_count);
        }
          break;
        case Op::OpCopyMemory:
        case Op::OpCopyMemorySized:
        {
          p_func->accessed_ptr_count += 2;
        }
          break;
        default: break;
      }
    }

    if (p_func->callee_count > 0) {
      p_func->callees = (u32*)calloc(p_func->callee_count,
                                     sizeof(*(p_func->callees)));
      if (IsNull(p_func->callees)) {
        return Reflect::Result::ErrorAllocFailed;
      }
    }

    if (p_func->accessed_ptr_count > 0) {
      p_func->accessed_ptrs = (u32*)calloc(p_func->accessed_ptr_count,
                                           sizeof(*(p_func->accessed_ptrs)));
      if (IsNull(p_func->accessed_ptrs)) {
        return Reflect::Result::ErrorAllocFailed;
      }
    }

    p_func->callee_count = 0;
    p_func->accessed_ptr_count = 0;
    for (size_t i = first_label_index; i < p_parser->node_count; ++i) {
      Node* p_node = &(p_parser->nodes[i]);
      if (p_node->op == Op::OpFunctionEnd) {
        break;
      }
      switch (p_node->op) {
        case Op::OpFunctionCall: {
          CHECKED_READU32(p_parser, p_node->word_offset + 3,
                          p_func->callees[p_func->callee_count]);
          (++p_func->callee_count);
        }
          break;
        case Op::OpLoad:
        case Op::OpAccessChain:
        case Op::OpInBoundsAccessChain:
        case Op::OpPtrAccessChain:
        case Op::OpArrayLength:
        case Op::OpGenericPtrMemSemantics:
        case Op::OpInBoundsPtrAccessChain:
        {
          CHECKED_READU32(p_parser, p_node->word_offset + 3,
                          p_func->accessed_ptrs[p_func->accessed_ptr_count]);
          (++p_func->accessed_ptr_count);
        }
          break;
        case Op::OpStore:
        {
          CHECKED_READU32(p_parser, p_node->word_offset + 2,
                          p_func->accessed_ptrs[p_func->accessed_ptr_count]);
          (++p_func->accessed_ptr_count);
        }
          break;
        case Op::OpCopyMemory:
        case Op::OpCopyMemorySized:
        {
          CHECKED_READU32(p_parser, p_node->word_offset + 2,
                          p_func->accessed_ptrs[p_func->accessed_ptr_count]);
          (++p_func->accessed_ptr_count);
          CHECKED_READU32(p_parser, p_node->word_offset + 3,
                          p_func->accessed_ptrs[p_func->accessed_ptr_count]);
          (++p_func->accessed_ptr_count);
        }
          break;
        default: break;
      }
    }

    if (p_func->callee_count > 0) {
      qsort(p_func->callees, p_func->callee_count,
            sizeof(*(p_func->callees)), SortCompareUint32);
    }
    p_func->callee_count = (u32)DedupSortedUint32(p_func->callees,
                                                  p_func->callee_count);

    if (p_func->accessed_ptr_count > 0) {
      qsort(p_func->accessed_ptrs, p_func->accessed_ptr_count,
            sizeof(*(p_func->accessed_ptrs)), SortCompareUint32);
    }
    p_func->accessed_ptr_count = (u32)DedupSortedUint32(p_func->accessed_ptrs,
                                                        p_func->accessed_ptr_count);

    return Reflect::Result::Success;
  }
  inline int SortCompareFunctions(const void* a, const void* b)
  {
    const Function* af = (const Function*)a;
    const Function* bf = (const Function*)b;
    return (int)af->id - (int)bf->id;
  }
  inline Reflect::Result ParseFunctions(Parser* p_parser)
  {
    assert(IsNotNull(p_parser));
    assert(IsNotNull(p_parser->spirv_code));
    assert(IsNotNull(p_parser->nodes));

    if (IsNotNull(p_parser) && IsNotNull(p_parser->spirv_code) && IsNotNull(p_parser->nodes)) {
      if (p_parser->function_count == 0) {
        return Reflect::Result::Success;
      }

      p_parser->functions = (Function*)calloc(p_parser->function_count,
                                              sizeof(*(p_parser->functions)));
      if (IsNull(p_parser->functions)) {
        return Reflect::Result::ErrorAllocFailed;
      }

      size_t function_index = 0;
      for (size_t i = 0; i < p_parser->node_count; ++i) {
        Node* p_node = &(p_parser->nodes[i]);
        if (p_node->op != Op::OpFunction) {
          continue;
        }

        // Skip over function declarations that aren't definitions
        bool func_definition = false;
        // Intentionally reuse i to avoid iterating over these nodes more than
        // once
        for (; i < p_parser->node_count; ++i) {
          if (p_parser->nodes[i].op == Op::OpLabel) {
            func_definition = true;
            break;
          }
          if (p_parser->nodes[i].op == Op::OpFunctionEnd) {
            break;
          }
        }
        if (!func_definition) {
          continue;
        }

        Function* p_function = &(p_parser->functions[function_index]);

        Reflect::Result result = ParseFunction(p_parser, p_node, p_function, i);
        if (result != Reflect::Result::Success) {
          return result;
        }

        ++function_index;
      }

      qsort(p_parser->functions, p_parser->function_count,
            sizeof(*(p_parser->functions)), SortCompareFunctions);

      // Once they're sorted, link the functions with pointers to improve graph
      // traversal efficiency
      for (size_t i = 0; i < p_parser->function_count; ++i) {
        Function* p_func = &(p_parser->functions[i]);
        if (p_func->callee_count == 0) {
          continue;
        }
        p_func->callee_ptrs = (Function**)calloc(p_func->callee_count,
                                                 sizeof(*(p_func->callee_ptrs)));
        for (size_t j = 0, k = 0; j < p_func->callee_count; ++j) {
          while (p_parser->functions[k].id != p_func->callees[j]) {
            ++k;
            if (k >= p_parser->function_count) {
              // Invalid called function ID somewhere
              return Reflect::Result::ErrorSpirvInvalidIdReference;
            }
          }
          p_func->callee_ptrs[j] = &(p_parser->functions[k]);
        }
      }
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseMemberCounts(Parser* p_parser)
  {
    assert(IsNotNull(p_parser));
    assert(IsNotNull(p_parser->spirv_code));
    assert(IsNotNull(p_parser->nodes));

    if (IsNotNull(p_parser) && IsNotNull(p_parser->spirv_code) && IsNotNull(p_parser->nodes)) {
      for (size_t i = 0; i < p_parser->node_count; ++i) {
        Node* p_node = &(p_parser->nodes[i]);
        if ((p_node->op != Op::OpMemberName) && (p_node->op != Op::OpMemberDecorate)) {
          continue;
        }

        u32 target_id = 0;
        u32 member_index = (u32)invalidValue;
        CHECKED_READU32(p_parser, p_node->word_offset + 1, target_id);
        CHECKED_READU32(p_parser, p_node->word_offset + 2, member_index);
        Node* p_target_node = FindNode(p_parser, target_id);
        // Not all nodes get parsed, so FindNode returning NULL is expected.
        if (IsNull(p_target_node)) {
          continue;
        }

        if (member_index == invalidValue) {
          return Reflect::Result::ErrorRangeExceeded;
        }

        p_target_node->member_count = Max(p_target_node->member_count, member_index + 1);
      }

      for (u32 i = 0; i < p_parser->node_count; ++i) {
        Node* p_node = &(p_parser->nodes[i]);
        if (p_node->member_count == 0) {
          continue;
        }

        p_node->member_names = (const utf8 **)calloc(p_node->member_count, sizeof(*(p_node->member_names)));
        if (IsNull(p_node->member_names)) {
          return Reflect::Result::ErrorAllocFailed;
        }

        p_node->member_decorations = (Decorations*)calloc(p_node->member_count, sizeof(*(p_node->member_decorations)));
        if (IsNull(p_node->member_decorations)) {
          return Reflect::Result::ErrorAllocFailed;
        }
      }
    }
    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseNames(Parser* p_parser)
  {
    assert(IsNotNull(p_parser));
    assert(IsNotNull(p_parser->spirv_code));
    assert(IsNotNull(p_parser->nodes));

    if (IsNotNull(p_parser) && IsNotNull(p_parser->spirv_code) && IsNotNull(p_parser->nodes)) {
      for (size_t i = 0; i < p_parser->node_count; ++i) {
        Node* p_node = &(p_parser->nodes[i]);
        if ((p_node->op != Op::OpName) && (p_node->op != Op::OpMemberName)) {
          continue;
        }

        u32 target_id = 0;
        CHECKED_READU32(p_parser, p_node->word_offset + 1, target_id);
        Node* p_target_node = FindNode(p_parser, target_id);
        // Not all nodes get parsed, so FindNode returning NULL is expected.
        if (IsNull(p_target_node)) {
          continue;
        }

        const utf8** pp_target_name = &(p_target_node->name);
        if (p_node->op == Op::OpMemberName) {
          u32 member_index = UINT32_MAX;
          CHECKED_READU32(p_parser, p_node->word_offset + 2, member_index);
          pp_target_name = &(p_target_node->member_names[member_index]);
        }

        *pp_target_name = p_node->name;
      }
    }
    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseDecorations(Parser* p_parser)
  {
    for (u32 i = 0; i < p_parser->node_count; ++i) {
      Node* p_node = &(p_parser->nodes[i]);

      if (((u32)p_node->op != (u32)Op::OpDecorate) &&
          ((u32)p_node->op != (u32)Op::OpMemberDecorate) &&
          ((u32)p_node->op != (u32)Op::OpDecorateId) &&
          ((u32)p_node->op != (u32)Op::OpDecorateStringGOOGLE) &&
          ((u32)p_node->op != (u32)Op::OpMemberDecorateStringGOOGLE))
      {
        continue;
      }

      // Need to adjust the read offset if this is a member decoration
      u32 member_offset = 0;
      if (p_node->op == Op::OpMemberDecorate) {
        member_offset = 1;
      }

      // Get decoration
      auto decoration = (Decoration)invalidValue;
      CHECKED_READU32(p_parser, p_node->word_offset + member_offset + 2, decoration);

      // Filter out the decoration that do not affect reflection, otherwise
      // there will be random crashes because the nodes aren't found.
      bool skip = false;
      switch (decoration) {
        default: {
          skip = true;
        }
          break;
        case Decoration::Block:
        case Decoration::BufferBlock:
        case Decoration::ColMajor:
        case Decoration::RowMajor:
        case Decoration::ArrayStride:
        case Decoration::MatrixStride:
        case Decoration::BuiltIn:
        case Decoration::NoPerspective:
        case Decoration::Flat:
        case Decoration::NonWritable:
        case Decoration::Location:
        case Decoration::Binding:
        case Decoration::DescriptorSet:
        case Decoration::Offset:
        case Decoration::InputAttachmentIndex:
        case Decoration::HlslCounterBufferGOOGLE:
        case Decoration::HlslSemanticGOOGLE: {
          skip = false;
        }
          break;
      }
      if (skip) {
        continue;
      }

      // Find target target node
      u32 target_id = 0;
      CHECKED_READU32(p_parser, p_node->word_offset + 1, target_id);
      Node* p_target_node = FindNode(p_parser, target_id);
      if (IsNull(p_target_node)) {
        return Reflect::Result::ErrorSpirvInvalidIdReference;
      }
      // Get decorations
      Decorations* p_target_decorations = &(p_target_node->decorations);
      // Update pointer if this is a member member decoration
      if (p_node->op == Op::OpMemberDecorate) {
        u32 member_index = (u32)invalidValue;
        CHECKED_READU32(p_parser, p_node->word_offset + 2, member_index);
        p_target_decorations = &(p_target_node->member_decorations[member_index]);
      }

      switch (decoration) {
        default: break;

        case Decoration::Block: {
          p_target_decorations->is_block = true;
        }
          break;

        case Decoration::BufferBlock: {
          p_target_decorations->is_buffer_block = true;
        }
          break;

        case Decoration::ColMajor: {
          p_target_decorations->is_column_major = true;
        }
          break;

        case Decoration::RowMajor: {
          p_target_decorations->is_row_major = true;
        }
          break;

        case Decoration::ArrayStride: {
          u32 word_offset = p_node->word_offset + member_offset + 3;
          CHECKED_READU32(p_parser, word_offset, p_target_decorations->array_stride);
        }
          break;

        case Decoration::MatrixStride: {
          u32 word_offset = p_node->word_offset + member_offset + 3;
          CHECKED_READU32(p_parser, word_offset, p_target_decorations->matrix_stride);
        }
          break;

        case Decoration::BuiltIn: {
          p_target_decorations->is_built_in = true;
          u32 word_offset = p_node->word_offset + member_offset + 3;
          CHECKED_READU32_CAST(p_parser, word_offset, BuiltIn, p_target_decorations->built_in);
        }
          break;

        case Decoration::NoPerspective: {
          p_target_decorations->is_noperspective = true;
        }
          break;

        case Decoration::Flat: {
          p_target_decorations->is_flat = true;
        }
          break;

        case Decoration::NonWritable: {
          p_target_decorations->is_non_writable = true;
        }
          break;

        case Decoration::Location: {
          u32 word_offset = p_node->word_offset + member_offset + 3;
          CHECKED_READU32(p_parser, word_offset, p_target_decorations->location.value);
          p_target_decorations->location.word_offset = word_offset;
        }
          break;

        case Decoration::Binding: {
          u32 word_offset = p_node->word_offset + member_offset+ 3;
          CHECKED_READU32(p_parser, word_offset, p_target_decorations->binding.value);
          p_target_decorations->binding.word_offset = word_offset;
        }
          break;

        case Decoration::DescriptorSet: {
          u32 word_offset = p_node->word_offset + member_offset+ 3;
          CHECKED_READU32(p_parser, word_offset, p_target_decorations->set.value);
          p_target_decorations->set.word_offset = word_offset;
        }
          break;

        case Decoration::Offset: {
          u32 word_offset = p_node->word_offset + member_offset+ 3;
          CHECKED_READU32(p_parser, word_offset, p_target_decorations->offset.value);
          p_target_decorations->offset.word_offset = word_offset;
        }
          break;

        case Decoration::InputAttachmentIndex: {
          u32 word_offset = p_node->word_offset + member_offset+ 3;
          CHECKED_READU32(p_parser, word_offset, p_target_decorations->input_attachment_index.value);
          p_target_decorations->input_attachment_index.word_offset = word_offset;
        }
          break;

        case Decoration::HlslCounterBufferGOOGLE: {
          u32 word_offset = p_node->word_offset + member_offset+ 3;
          CHECKED_READU32(p_parser, word_offset, p_target_decorations->uav_counter_buffer.value);
          p_target_decorations->uav_counter_buffer.word_offset = word_offset;
        }
          break;

        case Decoration::HlslSemanticGOOGLE: {
          u32 word_offset = p_node->word_offset + member_offset + 3;
          p_target_decorations->semantic.value = (const utf8*)(p_parser->spirv_code + word_offset);
          p_target_decorations->semantic.word_offset = word_offset;
        }
          break;
      }
    }
    return Reflect::Result::Success;
  }
  inline Reflect::Result EnumerateAllUniforms(
      Reflect::ShaderModuleStruct* p_module,
      size_t*                 p_uniform_count,
      u32**              pp_uniforms
  )
  {
    *p_uniform_count = p_module->descriptor_binding_count;
    if (*p_uniform_count == 0) {
      return Reflect::Result::Success;
    }
    *pp_uniforms = (u32*)calloc(*p_uniform_count, sizeof(**pp_uniforms));

    if (IsNull(*pp_uniforms)) {
      return Reflect::Result::ErrorAllocFailed;
    }

    for (size_t i = 0; i < *p_uniform_count; ++i) {
      (*pp_uniforms)[i] = p_module->descriptor_bindings[i].spirv_id;
    }
    qsort(*pp_uniforms, *p_uniform_count, sizeof(**pp_uniforms),
          SortCompareUint32);
    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseType(
      Parser*                     p_parser,
      Node*                       p_node,
      Decorations*                p_struct_member_decorations,
      Reflect::ShaderModuleStruct*     p_module,
      Reflect::TypeDescription*  p_type
  )
  {
    Reflect::Result result = Reflect::Result::Success;

    if (p_node->member_count > 0) {
      p_type->member_count = p_node->member_count;
      p_type->members = (Reflect::TypeDescription*)calloc(p_type->member_count, sizeof(*(p_type->members)));
      if (IsNotNull(p_type->members)) {
        // Mark all members types with an invalid state
        for (size_t i = 0; i < p_type->members->member_count; ++i) {
          Reflect::TypeDescription* p_member_type = &(p_type->members[i]);
          p_member_type->id = (u32)invalidValue;
          p_member_type->op = (Op)invalidValue;
          p_member_type->storageClass = (StorageClass)invalidValue;
        }
      }
      else {
        result = Reflect::Result::ErrorAllocFailed;
      }
    }

    if (result == Reflect::Result::Success) {
      // Since the parse descends on type information, these will get overwritten
      // if not guarded against assignment. Only assign if the id is invalid.
      if (p_type->id == invalidValue) {
        p_type->id = p_node->result_id;
        p_type->op = p_node->op;
        p_type->decorationFlags = {};
      }
      // Top level types need to pick up decorations from all types below it.
      // Issue and fix here: https://github.com/chaoticbob/SPIRV-Reflect/issues/64
      p_type->decorationFlags = ApplyDecorations(&p_node->decorations);

      switch (p_node->op) {
        default: break;
        case Op::OpTypeVoid:
          p_type->typeFlags |= Reflect::Type::Void;
          break;

        case Op::OpTypeBool:
          p_type->typeFlags |= Reflect::Type::Bool;
          break;

        case Op::OpTypeInt: {
          p_type->typeFlags |= Reflect::Type::Int;
          IF_READU32(result, p_parser, p_node->word_offset + 2, p_type->traits.numeric.scalar.width);
          IF_READU32(result, p_parser, p_node->word_offset + 3, p_type->traits.numeric.scalar.signedness);
        }
          break;

        case Op::OpTypeFloat: {
          p_type->typeFlags |= Reflect::Type::Float;
          IF_READU32(result, p_parser, p_node->word_offset + 2, p_type->traits.numeric.scalar.width);
        }
          break;

        case Op::OpTypeVector: {
          p_type->typeFlags |= Reflect::Type::Vector;
          u32 component_type_id = (u32)invalidValue;
          IF_READU32(result, p_parser, p_node->word_offset + 2, component_type_id);
          IF_READU32(result, p_parser, p_node->word_offset + 3, p_type->traits.numeric.vector.component_count);
          // Parse component type
          Node* p_next_node = FindNode(p_parser, component_type_id);
          if (IsNotNull(p_next_node)) {
            result = ParseType(p_parser, p_next_node, NULL, p_module, p_type);
          }
          else {
            result = Reflect::Result::ErrorSpirvInvalidIdReference;
          }
        }
          break;

        case Op::OpTypeMatrix: {
          p_type->typeFlags |= Reflect::Type::Matrix;
          u32 column_type_id = (u32)invalidValue;
          IF_READU32(result, p_parser, p_node->word_offset + 2, column_type_id);
          IF_READU32(result, p_parser, p_node->word_offset + 3, p_type->traits.numeric.matrix.column_count);
          Node* p_next_node = FindNode(p_parser, column_type_id);
          if (IsNotNull(p_next_node)) {
            result = ParseType(p_parser, p_next_node, NULL, p_module, p_type);
          }
          else {
            result = Reflect::Result::ErrorSpirvInvalidIdReference;
          }
          p_type->traits.numeric.matrix.row_count = p_type->traits.numeric.vector.component_count;
          p_type->traits.numeric.matrix.stride = p_node->decorations.matrix_stride;
          // NOTE: Matrix stride is decorated using OpMemberDecoreate - not OpDecoreate.
          if (IsNotNull(p_struct_member_decorations)) {
            p_type->traits.numeric.matrix.stride = p_struct_member_decorations->matrix_stride;
          }
        }
          break;

        case Op::OpTypeImage: {
          p_type->typeFlags |= Reflect::Type::ExternalImage;
          IF_READU32_CAST(result, p_parser, p_node->word_offset + 3, Dim, p_type->traits.image.dim);
          IF_READU32(result, p_parser, p_node->word_offset + 4, p_type->traits.image.depth);
          IF_READU32(result, p_parser, p_node->word_offset + 5, p_type->traits.image.arrayed);
          IF_READU32(result, p_parser, p_node->word_offset + 6, p_type->traits.image.ms);
          IF_READU32(result, p_parser, p_node->word_offset + 7, p_type->traits.image.sampled);
          IF_READU32_CAST(result, p_parser, p_node->word_offset + 8, ImageFormat, p_type->traits.image.image_format);
        }
          break;

        case Op::OpTypeSampler: {
          p_type->typeFlags |= Reflect::Type::ExternalSampler;
        }
          break;

        case Op::OpTypeSampledImage: {
          p_type->typeFlags |= Reflect::Type::ExternalSampledImage;
          u32 image_type_id = (u32)invalidValue;
          IF_READU32(result, p_parser, p_node->word_offset + 2, image_type_id);
          Node* p_next_node = FindNode(p_parser, image_type_id);
          if (IsNotNull(p_next_node)) {
            result = ParseType(p_parser, p_next_node, NULL, p_module, p_type);
          }
          else {
            result = Reflect::Result::ErrorSpirvInvalidIdReference;
          }
        }
          break;

        case Op::OpTypeArray: {
          p_type->typeFlags |= Reflect::Type::Array;
          if (result == Reflect::Result::Success) {
            u32 element_type_id = (u32)invalidValue;
            u32 length_id = (u32)invalidValue;
            IF_READU32(result, p_parser, p_node->word_offset + 2, element_type_id);
            IF_READU32(result, p_parser, p_node->word_offset + 3, length_id);
            // NOTE: Array stride is decorated using OpDecorate instead of
            //       OpMemberDecorate, even if the array is apart of a struct.
            p_type->traits.array.stride = p_node->decorations.array_stride;
            // Get length for current dimension
            Node* p_length_node = FindNode(p_parser, length_id);
            if (IsNotNull(p_length_node)) {
              if (p_length_node->op == Op::OpSpecConstant ||
                  p_length_node->op == Op::OpSpecConstantOp) {
                p_type->traits.array.dims[p_type->traits.array.dimsCount] = 0xFFFFFFFF;
                p_type->traits.array.dimsCount += 1;
              } else {
                u32 length = 0;
                IF_READU32(result, p_parser, p_length_node->word_offset + 3, length);
                if (result == Reflect::Result::Success) {
                  // Write the array dim and increment the count and offset
                  p_type->traits.array.dims[p_type->traits.array.dimsCount] = length;
                  p_type->traits.array.dimsCount += 1;
                } else {
                  result = Reflect::Result::ErrorSpirvInvalidIdReference;
                }
              }
              // Parse next dimension or element type
              Node* p_next_node = FindNode(p_parser, element_type_id);
              if (IsNotNull(p_next_node)) {
                result = ParseType(p_parser, p_next_node, NULL, p_module, p_type);
              }
            }
            else {
              result = Reflect::Result::ErrorSpirvInvalidIdReference;
            }
          }
        }
          break;

        case Op::OpTypeRuntimeArray: {
          u32 element_type_id = (u32)invalidValue;
          IF_READU32(result, p_parser, p_node->word_offset + 2, element_type_id);
          // Parse next dimension or element type
          Node* p_next_node = FindNode(p_parser, element_type_id);
          if (IsNotNull(p_next_node)) {
            result = ParseType(p_parser, p_next_node, NULL, p_module, p_type);
          }
          else {
            result = Reflect::Result::ErrorSpirvInvalidIdReference;
          }
        }
          break;

        case Op::OpTypeStruct: {
          p_type->typeFlags |= Reflect::Type::Struct;
          p_type->typeFlags |= Reflect::Type::ExternalBlock;
          u32 word_index = 2;
          u32 member_index = 0;
          for (; word_index < p_node->word_count; ++word_index, ++member_index) {
            u32 member_id = (u32)invalidValue;
            IF_READU32(result, p_parser, p_node->word_offset + word_index, member_id);
            // Find member node
            Node* p_member_node = FindNode(p_parser, member_id);
            if (IsNull(p_member_node)) {
              result = Reflect::Result::ErrorSpirvInvalidIdReference;
              break;
            }

            // Member decorations
            Decorations* p_member_decorations = &p_node->member_decorations[member_index];

            assert(member_index < p_type->member_count);
            // Parse member type
            Reflect::TypeDescription* p_member_type = &(p_type->members[member_index]);
            p_member_type->id = member_id;
            p_member_type->op = p_member_node->op;
            result = ParseType(p_parser, p_member_node, p_member_decorations, p_module, p_member_type);
            if (result != Reflect::Result::Success) {
              break;
            }
            // This looks wrong
            //p_member_type->type_name = p_member_node->name;
            p_member_type->structMemberName = p_node->member_names[member_index];
          }
        }
          break;

        case Op::OpTypeOpaque: break;

        case Op::OpTypePointer: {
          IF_READU32_CAST(result, p_parser, p_node->word_offset + 2, StorageClass, p_type->storageClass);
          u32 type_id = (u32)invalidValue;
          IF_READU32(result, p_parser, p_node->word_offset + 3, type_id);
          // Parse type
          Node* p_next_node = FindNode(p_parser, type_id);
          if (IsNotNull(p_next_node)) {
            result = ParseType(p_parser, p_next_node, NULL, p_module, p_type);
          }
          else {
            result = Reflect::Result::ErrorSpirvInvalidIdReference;
          }
        }
          break;
      }

      if (result == Reflect::Result::Success) {
        // Names get assigned on the way down. Guard against names
        // get overwritten on the way up.
        if (IsNull(p_type->typeName)) {
          p_type->typeName = p_node->name;
        }
      }
    }

    return result;
  }
  inline Reflect::Result ParseTypes(Parser* p_parser, Reflect::ShaderModuleStruct* p_module)
  {
    if (p_parser->type_count == 0) {
      return Reflect::Result::Success;
    }

    p_module->_internal->type_description_count = p_parser->type_count;
    p_module->_internal->type_descriptions = (Reflect::TypeDescription*)calloc(p_module->_internal->type_description_count,
                                                                               sizeof(*(p_module->_internal->type_descriptions)));
    if (IsNull(p_module->_internal->type_descriptions)) {
      return Reflect::Result::ErrorAllocFailed;
    }

    // Mark all types with an invalid state
    for (size_t i = 0; i < p_module->_internal->type_description_count; ++i) {
      Reflect::TypeDescription* p_type = &(p_module->_internal->type_descriptions[i]);
      p_type->id = (u32)invalidValue;
      p_type->op = (Op)invalidValue;
      p_type->storageClass = (StorageClass)invalidValue;
    }

    size_t type_index = 0;
    for (size_t i = 0; i < p_parser->node_count; ++i) {
      Node* p_node = &(p_parser->nodes[i]);
      if (! p_node->is_type) {
        continue;
      }

      Reflect::TypeDescription* p_type = &(p_module->_internal->type_descriptions[type_index]);
      Reflect::Result result = ParseType(p_parser, p_node, nullptr, p_module, p_type);
      if (result != Reflect::Result::Success) {
        return result;
      }
      ++type_index;
    }
    return Reflect::Result::Success;
  }
  inline int SortCompareDescriptorBinding(const void* a, const void* b)
  {
    const Reflect::DescriptorBinding* p_elem_a = (const Reflect::DescriptorBinding*)a;
    const Reflect::DescriptorBinding* p_elem_b = (const Reflect::DescriptorBinding*)b;
    int value = (int)(p_elem_a->binding) - (int)(p_elem_b->binding);
    if (value == 0) {
      // use spirv-id as a tiebreaker to ensure a stable ordering, as they're guaranteed
      // unique.
      assert(p_elem_a->spirv_id != p_elem_b->spirv_id);
      value = (int)(p_elem_a->spirv_id) - (int)(p_elem_b->spirv_id);
    }
    return value;
  }
  inline Reflect::Result ParseDescriptorBindings(Parser* p_parser, Reflect::ShaderModuleStruct* p_module)
  {
    p_module->descriptor_binding_count = 0;
    for (size_t i = 0; i < p_parser->node_count; ++i) {
      Node* p_node = &(p_parser->nodes[i]);
      if ((p_node->op != Op::OpVariable) ||
          ((p_node->storage_class != StorageClass::Uniform) && (p_node->storage_class != StorageClass::UniformConstant)))
      {
        continue;
      }
      if ((p_node->decorations.set.value == invalidValue) || (p_node->decorations.binding.value == invalidValue)) {
        continue;
      }

      p_module->descriptor_binding_count += 1;
    }

    if (p_module->descriptor_binding_count == 0) {
      return Reflect::Result::Success;
    }

    p_module->descriptor_bindings = (Reflect::DescriptorBinding*)calloc(p_module->descriptor_binding_count, sizeof(*(p_module->descriptor_bindings)));
    if (IsNull(p_module->descriptor_bindings)) {
      return Reflect::Result::ErrorAllocFailed;
    }

    // Mark all types with an invalid state
    for (u32 descriptor_index = 0; descriptor_index < p_module->descriptor_binding_count; ++descriptor_index) {
      Reflect::DescriptorBinding* p_descriptor = &(p_module->descriptor_bindings[descriptor_index]);
      p_descriptor->binding = (u32)invalidValue;
      p_descriptor->input_attachment_index = (u32)invalidValue;
      p_descriptor->set = (u32)invalidValue;
      p_descriptor->descriptor_type = (Reflect::DescriptorType)invalidValue;
      p_descriptor->uav_counter_id = (u32)invalidValue;
    }

    size_t descriptor_index = 0;
    for (size_t i = 0; i < p_parser->node_count; ++i) {
      Node* p_node = &(p_parser->nodes[i]);
      if ((p_node->op != Op::OpVariable) ||
          ((p_node->storage_class != StorageClass::Uniform) && (p_node->storage_class != StorageClass::UniformConstant)))\
    {
        continue;
      }
      if ((p_node->decorations.set.value == invalidValue) || (p_node->decorations.binding.value == invalidValue)) {
        continue;
      }

      Reflect::TypeDescription* p_type = FindType(p_module, p_node->type_id);
      if (IsNull(p_type)) {
        return Reflect::Result::ErrorSpirvInvalidIdReference;
      }
      // If the type is a pointer, resolve it
      if (p_type->op == Op::OpTypePointer) {
        // Find the type's node
        Node* p_type_node = FindNode(p_parser, p_type->id);
        if (IsNull(p_type_node)) {
          return Reflect::Result::ErrorSpirvInvalidIdReference;
        }
        // Should be the resolved type
        p_type = FindType(p_module, p_type_node->type_id);
        if (IsNull(p_type)) {
          return Reflect::Result::ErrorSpirvInvalidIdReference;
        }
      }

      Reflect::DescriptorBinding* p_descriptor = &p_module->descriptor_bindings[descriptor_index];
      p_descriptor->spirv_id = p_node->result_id;
      p_descriptor->name = p_node->name;
      p_descriptor->binding = p_node->decorations.binding.value;
      p_descriptor->input_attachment_index = p_node->decorations.input_attachment_index.value;
      p_descriptor->set = p_node->decorations.set.value;
      p_descriptor->count = 1;
      p_descriptor->uav_counter_id = p_node->decorations.uav_counter_buffer.value;
      p_descriptor->type_description = p_type;

      // Copy image traits
      if ((p_type->typeFlags & Reflect::Type::ExternalMask) == Reflect::Type::ExternalImage) {
        memcpy(&p_descriptor->image, &p_type->traits.image, sizeof(p_descriptor->image));
      }

      // This is a workaround for: https://github.com/KhronosGroup/glslang/issues/1096
      {
        const auto resource_mask = Reflect::Type(Reflect::Type::ExternalSampledImage) | Reflect::Type::ExternalImage;
        if ((p_type->typeFlags & resource_mask) == resource_mask) {
          memcpy(&p_descriptor->image, &p_type->traits.image, sizeof(p_descriptor->image));
        }
      }

      // Copy array traits
      if (p_type->traits.array.dimsCount > 0) {
        p_descriptor->array.dimsCount = p_type->traits.array.dimsCount;
        for (u32 dim_index = 0; dim_index < p_type->traits.array.dimsCount; ++dim_index) {
          u32 dim_value = p_type->traits.array.dims[dim_index];
          p_descriptor->array.dims[dim_index] = dim_value;
          p_descriptor->count *= dim_value;
        }
      }

      // Count


      p_descriptor->word_offset.binding = p_node->decorations.binding.word_offset;
      p_descriptor->word_offset.set = p_node->decorations.set.word_offset;

      ++descriptor_index;
    }

    if (p_module->descriptor_binding_count > 0) {
      qsort(p_module->descriptor_bindings,
            p_module->descriptor_binding_count,
            sizeof(*(p_module->descriptor_bindings)),
            SortCompareDescriptorBinding);
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseDescriptorType(Reflect::ShaderModuleStruct* p_module)
  {
    if (p_module->descriptor_binding_count == 0) {
      return Reflect::Result::Success;
    }

    for (u32 descriptor_index = 0; descriptor_index < p_module->descriptor_binding_count; ++descriptor_index) {
      Reflect::DescriptorBinding* p_descriptor = &(p_module->descriptor_bindings[descriptor_index]);
      Reflect::TypeDescription* p_type = p_descriptor->type_description;

      switch (p_type->typeFlags & Reflect::Type::ExternalMask) {
        default: assert(false && "unknown type flag"); break;

        case Reflect::Type::ExternalImage: {
          if (p_descriptor->image.dim == Dim::Buffer) {
            switch (p_descriptor->image.sampled) {
              default: assert(false && "unknown texel buffer sampled value"); break;
              case imageSampled: p_descriptor->descriptor_type = Reflect::DescriptorType::UniformTexelBuffer; break;
              case imageStorage: p_descriptor->descriptor_type = Reflect::DescriptorType::StorageTexelBuffer; break;
            }
          }
          else if(p_descriptor->image.dim == Dim::SubpassData) {
            p_descriptor->descriptor_type = Reflect::DescriptorType::InputAttachment;
          }
          else {
            switch (p_descriptor->image.sampled) {
              default: assert(false && "unknown image sampled value"); break;
              case imageSampled: p_descriptor->descriptor_type = Reflect::DescriptorType::SampledImage; break;
              case imageStorage: p_descriptor->descriptor_type = Reflect::DescriptorType::StorageImage; break;
            }
          }
        }
          break;

        case Reflect::Type::ExternalSampler: {
          p_descriptor->descriptor_type = Reflect::DescriptorType::Sampler;
        }
          break;

        case Bits::ReflectType((Reflect::Type::ExternalSampledImage | Reflect::Type::ExternalImage)): {
          // This is a workaround for: https://github.com/KhronosGroup/glslang/issues/1096
          if (p_descriptor->image.dim == Dim::Buffer) {
            switch (p_descriptor->image.sampled) {
              default: assert(false && "unknown texel buffer sampled value"); break;
              case imageSampled: p_descriptor->descriptor_type = Reflect::DescriptorType::UniformTexelBuffer; break;
              case imageStorage: p_descriptor->descriptor_type = Reflect::DescriptorType::StorageTexelBuffer; break;
            }
          }
          else {
            p_descriptor->descriptor_type = Reflect::DescriptorType::CombinedImageSampler;
          }
        }
          break;

        case Reflect::Type::ExternalBlock: {
          if (p_type->decorationFlags & Reflect::Decoration::Block) {
            p_descriptor->descriptor_type = Reflect::DescriptorType::UniformBuffer;
          }
          else if (p_type->decorationFlags & Reflect::Decoration::BufferBlock) {
            p_descriptor->descriptor_type = Reflect::DescriptorType::StorageBuffer;
          }
          else {
            assert(false && "unknown struct");
          }
        }
          break;
      }

      switch (p_descriptor->descriptor_type) {
        case Reflect::DescriptorType::Sampler                : p_descriptor->resource_type = Reflect::ResourceType::Sampler; break;
        case Reflect::DescriptorType::CombinedImageSampler : p_descriptor->resource_type = (Reflect::ResourceType)((u32)Reflect::ResourceType::Sampler | (u32)Reflect::ResourceType::Srv); break;
        case Reflect::DescriptorType::SampledImage          : /*p_descriptor->resource_type = Reflect::ResourceType::Srv; break;*/
        
        case Reflect::DescriptorType::UniformTexelBuffer   : p_descriptor->resource_type = Reflect::ResourceType::Srv; break;
        
        case Reflect::DescriptorType::UniformBuffer        : /*p_descriptor->resource_type = Reflect::ResourceType::Cbv; break;*/
        case Reflect::DescriptorType::UniformBufferDynamic : p_descriptor->resource_type = Reflect::ResourceType::Cbv; break;
        case Reflect::DescriptorType::StorageImage          : /*p_descriptor->resource_type = Reflect::ResourceType::Uav; break;*/
        case Reflect::DescriptorType::StorageTexelBuffer   : /*p_descriptor->resource_type = Reflect::ResourceType::Uav; break;*/
        case Reflect::DescriptorType::StorageBuffer        : /*p_descriptor->resource_type = Reflect::ResourceType::Uav; break;*/
        case Reflect::DescriptorType::StorageBufferDynamic : p_descriptor->resource_type = Reflect::ResourceType::Uav; break;

        case Reflect::DescriptorType::InputAttachment:
          break;
      }
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseUavCounterBindings(Reflect::ShaderModuleStruct* p_module)
  {
    char name[nodeNameLength];
    const utf8* k_count_tag = u8"@count";

    for (u32 descriptor_index = 0; descriptor_index < p_module->descriptor_binding_count; ++descriptor_index) {
      Reflect::DescriptorBinding* p_descriptor = &(p_module->descriptor_bindings[descriptor_index]);

      if (p_descriptor->descriptor_type != Reflect::DescriptorType::StorageBuffer) {
        continue;
      }

      Reflect::DescriptorBinding* p_counter_descriptor = NULL;
      // Use Uav counter buffer id if present...
      if (p_descriptor->uav_counter_id != UINT32_MAX) {
        for (u32 counter_descriptor_index = 0; counter_descriptor_index < p_module->descriptor_binding_count; ++counter_descriptor_index) {
          Reflect::DescriptorBinding* p_test_counter_descriptor = &(p_module->descriptor_bindings[counter_descriptor_index]);
          if (p_test_counter_descriptor->descriptor_type != Reflect::DescriptorType::StorageBuffer) {
            continue;
          }
          if (p_descriptor->uav_counter_id == p_test_counter_descriptor->spirv_id) {
            p_counter_descriptor = p_test_counter_descriptor;
            break;
          }
        }
      }
        // ...otherwise use old @count convention.
      else {
        const size_t descriptor_name_length = p_descriptor->name ? std::char_traits<char8_t>::length(p_descriptor->name) : 0;

        memset(name, 0, nodeNameLength);
        memcpy(name, p_descriptor->name, descriptor_name_length);
#if defined(WIN32)
        strcat_s((char*)name, nodeNameLength, (const char*)k_count_tag);
#else
        strcat(name, k_count_tag);
#endif

        for (u32 counter_descriptor_index = 0; counter_descriptor_index < p_module->descriptor_binding_count; ++counter_descriptor_index) {
          Reflect::DescriptorBinding* p_test_counter_descriptor = &(p_module->descriptor_bindings[counter_descriptor_index]);
          if (p_test_counter_descriptor->descriptor_type != Reflect::DescriptorType::StorageBuffer) {
            continue;
          }
          if (p_test_counter_descriptor->name && strcmp((const char*)name, (const char*)p_test_counter_descriptor->name) == 0) {
            p_counter_descriptor = p_test_counter_descriptor;
            break;
          }
        }
      }

      if (p_counter_descriptor != NULL) {
        p_descriptor->uav_counter_binding = p_counter_descriptor;
      }
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseDescriptorBlockVariable(
      Parser*                     p_parser,
      Reflect::ShaderModuleStruct*     p_module,
      Reflect::TypeDescription*  p_type,
      Reflect::BlockVariable*    p_var
  )
  {
    bool has_non_writable = false;

    if (IsNotNull(p_type->members) && (p_type->member_count > 0)) {
      p_var->member_count = p_type->member_count;
      p_var->members = (Reflect::BlockVariable*)calloc(p_var->member_count, sizeof(*p_var->members));
      if (IsNull(p_var->members)) {
        return Reflect::Result::ErrorAllocFailed;
      }

      Node* p_type_node = FindNode(p_parser, p_type->id);
      if (IsNull(p_type_node)) {
        return Reflect::Result::ErrorSpirvInvalidIdReference;
      }
      // Resolve to element type if current type is array or run time array
      if (p_type_node->op == Op::OpTypeArray) {
        while (p_type_node->op == Op::OpTypeArray) {
          p_type_node = FindNode(p_parser, p_type_node->array_traits.element_type_id);
          if (IsNull(p_type_node)) {
            return Reflect::Result::ErrorSpirvInvalidIdReference;
          }
        }
      }
      else if(p_type_node->op == Op::OpTypeRuntimeArray) {
        // Element type description
        p_type = FindType(p_module, p_type_node->array_traits.element_type_id);
        if (IsNull(p_type)) {
          return Reflect::Result::ErrorSpirvInvalidIdReference;
        }
        // Element type node
        p_type_node = FindNode(p_parser, p_type->id);
        if (IsNull(p_type_node)) {
          return Reflect::Result::ErrorSpirvInvalidIdReference;
        }
      }

      // Parse members
      for (u32 member_index = 0; member_index < p_type->member_count; ++member_index) {
        Reflect::TypeDescription* p_member_type = &p_type->members[member_index];
        Reflect::BlockVariable* p_member_var = &p_var->members[member_index];
        bool is_struct = (p_member_type->typeFlags & Reflect::Type::Struct) == Reflect::Type::Struct;
        if (is_struct) {
          Reflect::Result result = ParseDescriptorBlockVariable(p_parser, p_module, p_member_type, p_member_var);
          if (result != Reflect::Result::Success) {
            return result;
          }
        }

        p_member_var->name = p_type_node->member_names[member_index];
        p_member_var->offset = p_type_node->member_decorations[member_index].offset.value;
        p_member_var->decoration_flags = ApplyDecorations(&p_type_node->member_decorations[member_index]);
        p_member_var->flags |= Reflect::Variable::Unused;
        if (!has_non_writable && (p_member_var->decoration_flags & Reflect::Decoration::NonWritable)) {
          has_non_writable = true;
        }
        ApplyNumericTraits(p_member_type, &p_member_var->numeric);
        if (p_member_type->op == Op::OpTypeArray) {
          ApplyArrayTraits(p_member_type, &p_member_var->array);
        }

        p_member_var->type_description = p_member_type;
      }
    }

    p_var->name = p_type->typeName;
    p_var->type_description = p_type;
    if (has_non_writable) {
      p_var->decoration_flags |= Reflect::Decoration::NonWritable;
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseDescriptorBlockVariableSizes(
      Parser*                   p_parser,
      Reflect::ShaderModuleStruct*   p_module,
      bool                      is_parent_root,
      bool                      is_parent_aos,
      bool                      is_parent_rta,
      Reflect::BlockVariable*  p_var
  )
  {
    if (p_var->member_count == 0) {
      return Reflect::Result::Success;
    }

    // Absolute offsets
    for (u32 member_index = 0; member_index < p_var->member_count; ++member_index) {
      Reflect::BlockVariable* p_member_var = &p_var->members[member_index];
      if (is_parent_root) {
        p_member_var->absolute_offset = p_member_var->offset;
      }
      else {
        p_member_var->absolute_offset = is_parent_aos ? 0 : p_member_var->offset + p_var->absolute_offset;
      }
    }

    // Size
    for (u32 member_index = 0; member_index < p_var->member_count; ++member_index) {
      Reflect::BlockVariable* p_member_var = &p_var->members[member_index];
      Reflect::TypeDescription* p_member_type = p_member_var->type_description;

      switch (p_member_type->op) {
        case Op::OpTypeBool: {
          p_member_var->size = wordSize;
        }
          break;

        case Op::OpTypeInt:
        case Op::OpTypeFloat: {
          p_member_var->size = p_member_type->traits.numeric.scalar.width / byteWidth;
        }
          break;

        case Op::OpTypeVector: {
          u32 size = p_member_type->traits.numeric.vector.component_count *
                     (p_member_type->traits.numeric.scalar.width / byteWidth);
          p_member_var->size = size;
        }
          break;

        case Op::OpTypeMatrix: {
          if (p_member_var->decoration_flags & Reflect::Decoration::ColumnMajor) {
            p_member_var->size = p_member_var->numeric.matrix.column_count * p_member_var->numeric.matrix.stride;
          }
          else if (p_member_var->decoration_flags & Reflect::Decoration::RowMajor) {
            p_member_var->size = p_member_var->numeric.matrix.row_count * p_member_var->numeric.matrix.stride;
          }
        }
          break;

        case Op::OpTypeArray: {
          // If array of structs, parse members first...
          bool is_struct = (p_member_type->typeFlags & Reflect::Type::Struct) == Reflect::Type::Struct;
          if (is_struct) {
            Reflect::Result result = ParseDescriptorBlockVariableSizes(p_parser, p_module, false, true, is_parent_rta, p_member_var);
            if (result != Reflect::Result::Success) {
              return result;
            }
          }
          // ...then array
          u32 element_count = (p_member_var->array.dimsCount > 0 ? 1 : 0);
          for (u32 i = 0; i < p_member_var->array.dimsCount; ++i) {
            element_count *= p_member_var->array.dims[i];
          }
          p_member_var->size = element_count * p_member_var->array.stride;
        }
          break;

        case Op::OpTypeRuntimeArray: {
          bool is_struct = (p_member_type->typeFlags & Reflect::Type::Struct) == Reflect::Type::Struct;
          if (is_struct) {
            Reflect::Result result = ParseDescriptorBlockVariableSizes(p_parser, p_module, false, true, true, p_member_var);
            if (result != Reflect::Result::Success) {
              return result;
            }
          }
        }
          break;

        case Op::OpTypeStruct: {
          Reflect::Result result = ParseDescriptorBlockVariableSizes(p_parser, p_module, false, is_parent_aos, is_parent_rta, p_member_var);
          if (result != Reflect::Result::Success) {
            return result;
          }
        }
          break;

        default:
          break;
      }
    }

    // Parse padded size using offset difference for all member except for the last entry...
    for (u32 member_index = 0; member_index < (p_var->member_count - 1); ++member_index) {
      Reflect::BlockVariable* p_member_var = &p_var->members[member_index];
      Reflect::BlockVariable* p_next_member_var = &p_var->members[member_index + 1];
      p_member_var->padded_size = p_next_member_var->offset - p_member_var->offset;
      if (p_member_var->size > p_member_var->padded_size) {
        p_member_var->size = p_member_var->padded_size;
      }
      if (is_parent_rta) {
        p_member_var->padded_size = p_member_var->size;
      }
    }
    // ...last entry just gets rounded up to near multiple of dataAlignment, which is 16 and
    // subtract the offset.
    if (p_var->member_count > 0) {
      Reflect::BlockVariable* p_member_var = &p_var->members[p_var->member_count - 1];
      p_member_var->padded_size = RoundUp(p_member_var->offset  + p_member_var->size, dataAlignment) - p_member_var->offset;
      if (p_member_var->size > p_member_var->padded_size) {
        p_member_var->size = p_member_var->padded_size;
      }
      if (is_parent_rta) {
        p_member_var->padded_size = p_member_var->size;
      }
    }

    // @TODO validate this with assertion
    p_var->size = p_var->members[p_var->member_count - 1].offset +
                  p_var->members[p_var->member_count - 1].padded_size;
    p_var->padded_size = p_var->size;

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseDescriptorBlockVariableUsage(
      Parser*                  p_parser,
      Reflect::ShaderModuleStruct*  p_module,
      AccessChain*             p_access_chain,
      u32                 index_index,
      Op                    override_op_type,
      Reflect::BlockVariable* p_var
  )
  {
    (void)p_parser;
    (void)p_access_chain;
    (void)p_var;

    // Clear the current variable's USED flag
    p_var->flags &= ~Reflect::Variable::Unused;

    // Parsing arrays requires overriding the op type for
    // for the lowest dim's element type.
    Op op_type = p_var->type_description->op;
    if (override_op_type != (Op)invalidValue) {
      op_type = override_op_type;
    }

    switch (op_type) {
      default: break;

      case Op::OpTypeArray: {
        // Parse through array's type hierarchy to find the actual/non-array element type
        Reflect::TypeDescription* p_type = p_var->type_description;
        while ((p_type->op == Op::OpTypeArray) && (index_index < p_access_chain->index_count)) {
          // Find the array element type id
          Node* p_node = FindNode(p_parser, p_type->id);
          if (p_node == NULL) {
            return Reflect::Result::ErrorSpirvInvalidIdReference;
          }
          u32 element_type_id = p_node->array_traits.element_type_id;
          // Get the array element type
          p_type = FindType(p_module, element_type_id);
          if (p_type == NULL) {
            return Reflect::Result::ErrorSpirvInvalidIdReference;
          }
          // Next access index
          index_index += 1;
        }
        // Parse current var again with a type override and advanced index index
        Reflect::Result result = ParseDescriptorBlockVariableUsage(
            p_parser,
            p_module,
            p_access_chain,
            index_index,
            p_type->op,
            p_var);
        if (result != Reflect::Result::Success) {
          return result;
        }
      }
        break;

      case Op::OpTypeStruct: {
        assert(p_var->member_count > 0);
        if (p_var->member_count == 0) {
          return Reflect::Result::ErrorSpirvUnexpectedBlockData;
        }

        u32 index = p_access_chain->indexes[index_index];

        if (index >= p_var->member_count) {
          return Reflect::Result::ErrorSpirvInvalidBlockMemberReference;
        }

        Reflect::BlockVariable* p_member_var = &p_var->members[index];
        if (index_index < p_access_chain->index_count) {
          Reflect::Result result = ParseDescriptorBlockVariableUsage(
              p_parser,
              p_module,
              p_access_chain,
              index_index + 1,
              (Op)invalidValue,
              p_member_var);
          if (result != Reflect::Result::Success) {
            return result;
          }
        }
      }
        break;
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseDescriptorBlocks(Parser* p_parser, Reflect::ShaderModuleStruct* p_module)
  {
    if (p_module->descriptor_binding_count == 0) {
      return Reflect::Result::Success;
    }

    for (u32 descriptor_index = 0; descriptor_index < p_module->descriptor_binding_count; ++descriptor_index) {
      Reflect::DescriptorBinding* p_descriptor = &(p_module->descriptor_bindings[descriptor_index]);
      Reflect::TypeDescription* p_type = p_descriptor->type_description;
      if ((p_descriptor->descriptor_type != Reflect::DescriptorType::UniformBuffer) &&
          (p_descriptor->descriptor_type != Reflect::DescriptorType::StorageBuffer) )
      {
        continue;
      }

      // Mark UNUSED
      p_descriptor->block.flags |= Reflect::Variable::Unused;
      // Parse descriptor block
      Reflect::Result result = ParseDescriptorBlockVariable(p_parser, p_module, p_type, &p_descriptor->block);
      if (result != Reflect::Result::Success) {
        return result;
      }

      for (u32 access_chain_index = 0; access_chain_index < p_parser->access_chain_count; ++access_chain_index) {
        AccessChain* p_access_chain = &(p_parser->access_chains[access_chain_index]);
        // Skip any access chains that aren't touching this descriptor block
        if (p_descriptor->spirv_id != p_access_chain->base_id) {
          continue;
        }
        result = ParseDescriptorBlockVariableUsage(
            p_parser,
            p_module,
            p_access_chain,
            0,
            (Op)invalidValue,
            &p_descriptor->block);
        if (result != Reflect::Result::Success) {
          return result;
        }
      }

      p_descriptor->block.name = p_descriptor->name;

      bool is_parent_rta = (p_descriptor->descriptor_type == Reflect::DescriptorType::StorageBuffer);
      result = ParseDescriptorBlockVariableSizes(p_parser, p_module, true, false, is_parent_rta, &p_descriptor->block);
      if (result != Reflect::Result::Success) {
        return result;
      }

      if (is_parent_rta) {
        p_descriptor->block.size = 0;
        p_descriptor->block.padded_size = 0;
      }
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseFormat(
      const Reflect::TypeDescription*  p_type,
      Reflect::Format*                 p_format
  )
  {
    Reflect::Result result = Reflect::Result::ErrorInternalError;
    bool signedness = (p_type->traits.numeric.scalar.signedness != 0);
    if (p_type->typeFlags & Reflect::Type::Vector) {
      u32 component_count = p_type->traits.numeric.vector.component_count;
      if (p_type->typeFlags & Reflect::Type::Float) {
        switch (component_count) {
          case 2: *p_format = Reflect::Format::R32G32SFloat; break;
          case 3: *p_format = Reflect::Format::R32G32B32SFloat; break;
          case 4: *p_format = Reflect::Format::R32G32B32A32SFloat; break;
        }
        result = Reflect::Result::Success;
      }
      else if (p_type->typeFlags & (Reflect::Type::Int | Reflect::Type::Bool)) {
        switch (component_count) {
          case 2: *p_format = signedness ? Reflect::Format::R32G32SInt : Reflect::Format::R32G32UInt; break;
          case 3: *p_format = signedness ? Reflect::Format::R32G32B32SInt : Reflect::Format::R32G32B32UInt; break;
          case 4: *p_format = signedness ? Reflect::Format::R32G32B32A32SInt : Reflect::Format::R32G32B32A32UInt; break;
        }
        result = Reflect::Result::Success;
      }
    }
    else if (p_type->typeFlags & Reflect::Type::Float) {
      *p_format = Reflect::Format::R32SFloat;
      result = Reflect::Result::Success;
    }
    else if (p_type->typeFlags & (Reflect::Type::Int | Reflect::Type::Bool)) {
      if (signedness) {
        *p_format = Reflect::Format::R32SInt;
        result = Reflect::Result::Success;
      }
      else {
        *p_format = Reflect::Format::R32UInt;
        result = Reflect::Result::Success;
      }
    }
    else if (p_type->typeFlags & Reflect::Type::Struct) {
      *p_format = Reflect::Format::Undefined;
      result = Reflect::Result::Success;
    }
    return result;
  }
  inline Reflect::Result ParseInterfaceVariable(
      Parser*                      p_parser,
      const Decorations*           p_type_node_decorations,
      Reflect::ShaderModuleStruct*      p_module,
      Reflect::TypeDescription*   p_type,
      Reflect::InterfaceVariable* p_var,
      bool*                        p_has_built_in
  )
  {
    Node* p_type_node = FindNode(p_parser, p_type->id);
    if (IsNull(p_type_node)) {
      return Reflect::Result::ErrorSpirvInvalidIdReference;
    }

    if (p_type->member_count > 0) {
      p_var->member_count = p_type->member_count;
      p_var->members = (Reflect::InterfaceVariable*)calloc(p_var->member_count, sizeof(*p_var->members));
      if (IsNull(p_var->members)) {
        return Reflect::Result::ErrorAllocFailed;
      }

      for (u32 member_index = 0; member_index < p_type_node->member_count; ++member_index) {
        Decorations* p_member_decorations = &p_type_node->member_decorations[member_index];
        Reflect::TypeDescription* p_member_type = &p_type->members[member_index];
        Reflect::InterfaceVariable* p_member_var = &p_var->members[member_index];
        Reflect::Result result = ParseInterfaceVariable(p_parser, p_member_decorations, p_module, p_member_type, p_member_var, p_has_built_in);
        if (result != Reflect::Result::Success) {
          return result;
        }
      }
    }

    p_var->name = p_type_node->name;
    p_var->decoration_flags = ApplyDecorations(p_type_node_decorations);
    p_var->built_in = p_type_node_decorations->built_in;
    ApplyNumericTraits(p_type, &p_var->numeric);
    if (p_type->op == Op::OpTypeArray) {
      ApplyArrayTraits(p_type, &p_var->array);
    }

    p_var->type_description = p_type;

    *p_has_built_in |= p_type_node_decorations->is_built_in;

    Reflect::Result result = ParseFormat(p_var->type_description, &p_var->format);
    if (result != Reflect::Result::Success) {
      return result;
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result ParseInterfaceVariables(
      Parser*                 p_parser,
      Reflect::ShaderModuleStruct* p_module,
      Reflect::EntryPoint*   p_entry,
      size_t                 io_var_count,
      u32*                   io_vars
  )
  {
    if (io_var_count == 0) {
      return Reflect::Result::Success;
    }

    p_entry->input_variable_count = 0;
    p_entry->output_variable_count = 0;
    for (size_t i = 0; i < io_var_count; ++i) {
      u32 var_result_id = *(io_vars + i);
      Node* p_node = FindNode(p_parser, var_result_id);
      if (IsNull(p_node)) {
        return Reflect::Result::ErrorSpirvInvalidIdReference;
      }

      if (p_node->storage_class == StorageClass::Input) {
        p_entry->input_variable_count += 1;
      }
      else if (p_node->storage_class == StorageClass::Output) {
        p_entry->output_variable_count += 1;
      }
    }

    if (p_entry->input_variable_count > 0) {
      p_entry->input_variables = (Reflect::InterfaceVariable*)calloc(p_entry->input_variable_count, sizeof(*(p_entry->input_variables)));
      if (IsNull(p_entry->input_variables)) {
        return Reflect::Result::ErrorAllocFailed;
      }
    }


    if (p_entry->output_variable_count > 0) {
      p_entry->output_variables = (Reflect::InterfaceVariable*)calloc(p_entry->output_variable_count, sizeof(*(p_entry->output_variables)));
      if (IsNull(p_entry->output_variables)) {
        return Reflect::Result::ErrorAllocFailed;
      }
    }

    size_t input_index = 0;
    size_t output_index = 0;
    for (size_t i = 0; i < io_var_count; ++i) {
      u32 var_result_id = *(io_vars + i);
      Node* p_node = FindNode(p_parser, var_result_id);
      if (IsNull(p_node)) {
        return Reflect::Result::ErrorSpirvInvalidIdReference;
      }

      Reflect::TypeDescription* p_type = FindType(p_module, p_node->type_id);
      if (IsNull(p_node)) {
        return Reflect::Result::ErrorSpirvInvalidIdReference;
      }
      // If the type is a pointer, resolve it
      if (p_type->op == Op::OpTypePointer) {
        // Find the type's node
        Node* p_type_node = FindNode(p_parser, p_type->id);
        if (IsNull(p_type_node)) {
          return Reflect::Result::ErrorSpirvInvalidIdReference;
        }
        // Should be the resolved type
        p_type = FindType(p_module, p_type_node->type_id);
        if (IsNull(p_type)) {
          return Reflect::Result::ErrorSpirvInvalidIdReference;
        }
      }

      Node* p_type_node = FindNode(p_parser, p_type->id);
      if (IsNull(p_type_node)) {
        return Reflect::Result::ErrorSpirvInvalidIdReference;
      }

      Reflect::InterfaceVariable* p_var = NULL;
      if (p_node->storage_class == StorageClass::Input) {
        p_var = &(p_entry->input_variables[input_index]);
        p_var->storage_class = StorageClass::Input;
        ++input_index;
      }
      else if (p_node->storage_class == StorageClass::Output) {
        p_var = &(p_entry->output_variables[output_index]);
        p_var->storage_class = StorageClass::Output;
        ++output_index;
      } else {
        // interface variables can only have input or output storage classes;
        // anything else is either a new addition or an error.
        assert(false && "Unsupported storage class for interface variable");
        return Reflect::Result::ErrorSpirvInvalidStorageClass;
      }

      bool has_built_in = p_node->decorations.is_built_in;
      Reflect::Result result = ParseInterfaceVariable(
          p_parser,
          &p_type_node->decorations,
          p_module,
          p_type,
          p_var,
          &has_built_in);
      if (result != Reflect::Result::Success) {
        return result;
      }

      // SPIR-V result id
      p_var->spirv_id = p_node->result_id;
      // Name
      p_var->name = p_node->name;
      // Semantic
      p_var->semantic = p_node->decorations.semantic.value;

      // Decorate with built-in if any member is built-in
      if (has_built_in) {
        p_var->decoration_flags |= Reflect::Decoration::BuiltIn;
      }

      // Location is decorated on OpVariable node, not the type node.
      p_var->location = p_node->decorations.location.value;
      p_var->word_offset.location = p_node->decorations.location.word_offset;

      // Built in
      if (p_node->decorations.is_built_in) {
        p_var->built_in = p_node->decorations.built_in;
      }
    }

    return Reflect::Result::Success;
  }
  inline Reflect::Result EnumerateAllPushConstants(
      Reflect::ShaderModuleStruct* p_module,
      size_t*                 p_push_constant_count,
      u32**              p_push_constants
  )
  {
    *p_push_constant_count = p_module->push_constant_block_count;
    if (*p_push_constant_count == 0) {
      return Reflect::Result::Success;
    }
    *p_push_constants = (u32*)calloc(*p_push_constant_count, sizeof(**p_push_constants));

    if (IsNull(*p_push_constants)) {
      return Reflect::Result::ErrorAllocFailed;
    }

    for (size_t i = 0; i < *p_push_constant_count; ++i) {
      (*p_push_constants)[i] = p_module->push_constant_blocks[i].spirv_id;
    }
    qsort(*p_push_constants, *p_push_constant_count, sizeof(**p_push_constants),
          SortCompareUint32);
    return Reflect::Result::Success;
  }
  inline Reflect::Result TraverseCallGraph(
      Parser*   p_parser,
      Function* p_func,
      size_t*   p_func_count,
      u32* p_func_ids,
      u32  depth
  )
  {
    if (depth > p_parser->function_count) {
      // Vulkan does not permit recursion (Vulkan spec Appendix A):
      //   "Recursion: The static function-call graph for an entry point must not
      //    contain cycles."
      return Reflect::Result::ErrorSpirvRecursion;
    }
    if (IsNotNull(p_func_ids)) {
      p_func_ids[(*p_func_count)++] = p_func->id;
    } else {
      ++*p_func_count;
    }
    for (size_t i = 0; i < p_func->callee_count; ++i) {
      Reflect::Result result = TraverseCallGraph(
          p_parser, p_func->callee_ptrs[i], p_func_count, p_func_ids, depth + 1);
      if (result != Reflect::Result::Success) {
        return result;
      }
    }
    return Reflect::Result::Success;
  }
  
  inline Reflect::Result ParseStaticallyUsedResources(
      Parser*                 p_parser,
      Reflect::ShaderModuleStruct* p_module,
      Reflect::EntryPoint*   p_entry,
      size_t                  uniform_count,
      u32*               uniforms,
      size_t                  push_constant_count,
      u32*               push_constants
  )
  {
    // Find function with the right id
    Function* p_func = NULL;
    for (size_t i = 0; i < p_parser->function_count; ++i) {
      if (p_parser->functions[i].id == p_entry->id) {
        p_func = &(p_parser->functions[i]);
        break;
      }
    }
    if (p_func == NULL) {
      return Reflect::Result::ErrorSpirvInvalidIdReference;
    }

    size_t called_function_count = 0;
    Reflect::Result result = TraverseCallGraph(
        p_parser,
        p_func,
        &called_function_count,
        NULL,
        0);
    if (result != Reflect::Result::Success) {
      return result;
    }

    u32* p_called_functions = NULL;
    if (called_function_count > 0) {
      p_called_functions = (u32*)calloc(called_function_count, sizeof(*p_called_functions));
      if (IsNull(p_called_functions)) {
        return Reflect::Result::ErrorAllocFailed;
      }
    }

    called_function_count = 0;
    result = TraverseCallGraph(
        p_parser,
        p_func,
        &called_function_count,
        p_called_functions,
        0);
    if (result != Reflect::Result::Success) {
      return result;
    }

    if (called_function_count > 0) {
      qsort(
          p_called_functions,
          called_function_count,
          sizeof(*p_called_functions),
          SortCompareUint32);
    }
    called_function_count = DedupSortedUint32(p_called_functions, called_function_count);

    u32 used_variable_count = 0;
    for (size_t i = 0, j = 0; i < called_function_count; ++i) {
      // No need to bounds check j because a missing ID issue would have been
      // found during TraverseCallGraph
      while (p_parser->functions[j].id != p_called_functions[i]) {
        ++j;
      }
      used_variable_count += p_parser->functions[j].accessed_ptr_count;
    }
    u32* used_variables = NULL;
    if (used_variable_count > 0) {
      used_variables = (u32*)calloc(used_variable_count,
                                    sizeof(*used_variables));
      if (IsNull(used_variables)) {
        SafeFree(p_called_functions);
        return Reflect::Result::ErrorAllocFailed;
      }
    }
    used_variable_count = 0;
    for (size_t i = 0, j = 0; i < called_function_count; ++i) {
      while (p_parser->functions[j].id != p_called_functions[i]) {
        ++j;
      }

      memcpy(&used_variables[used_variable_count],
             p_parser->functions[j].accessed_ptrs,
             p_parser->functions[j].accessed_ptr_count * sizeof(*used_variables));
      used_variable_count += p_parser->functions[j].accessed_ptr_count;
    }
    SafeFree(p_called_functions);

    if (used_variable_count > 0) {
      qsort(used_variables, used_variable_count, sizeof(*used_variables),
            SortCompareUint32);
    }
    used_variable_count = (u32)DedupSortedUint32(used_variables,
                                                 used_variable_count);

    // Do set intersection to find the used uniform and push constants
    size_t used_uniform_count = 0;
    //
    Reflect::Result result0 = IntersectSortedUint32(
        used_variables,
        used_variable_count,
        uniforms,
        uniform_count,
        &p_entry->used_uniforms,
        &used_uniform_count);

    size_t used_push_constant_count = 0;
    //
    Reflect::Result result1 = IntersectSortedUint32(
        used_variables,
        used_variable_count,
        push_constants,
        push_constant_count,
        &p_entry->used_push_constants,
        &used_push_constant_count);

    for (u32 j = 0; j < p_module->descriptor_binding_count; ++j) {
      Reflect::DescriptorBinding* p_binding = &p_module->descriptor_bindings[j];
      bool found = SearchSortedUint32(
          used_variables,
          used_variable_count,
          p_binding->spirv_id);
      if (found) {
        p_binding->accessed = 1;
      }
    }

    SafeFree(used_variables);
    if (result0 != Reflect::Result::Success) {
      return result0;
    }
    if (result1 != Reflect::Result::Success) {
      return result1;
    }

    p_entry->used_uniform_count = (u32)used_uniform_count;
    p_entry->used_push_constant_count = (u32)used_push_constant_count;

    return Reflect::Result::Success;
  }
  
  inline Reflect::Result ParseEntryPoints(Parser* p_parser, Reflect::ShaderModuleStruct* p_module)
  {
    if (p_parser->entry_point_count == 0) {
      return Reflect::Result::Success;
    }

    p_module->entry_point_count = p_parser->entry_point_count;
    p_module->entry_points = (Reflect::EntryPoint*)calloc(p_module->entry_point_count,
                                                          sizeof(*(p_module->entry_points)));
    if (IsNull(p_module->entry_points)) {
      return Reflect::Result::ErrorAllocFailed;
    }

    Reflect::Result result;
    size_t uniform_count = 0;
    u32* uniforms = NULL;
    if ((result = EnumerateAllUniforms(p_module, &uniform_count, &uniforms)) !=
        Reflect::Result::Success) {
      return result;
    }
    size_t push_constant_count = 0;
    u32* push_constants = NULL;
    if ((result = EnumerateAllPushConstants(p_module, &push_constant_count, &push_constants)) !=
        Reflect::Result::Success) {
      return result;
    }

    size_t entry_point_index = 0;
    for (size_t i = 0; entry_point_index < p_parser->entry_point_count && i < p_parser->node_count; ++i) {
      Node* p_node = &(p_parser->nodes[i]);
      if (p_node->op != Op::OpEntryPoint) {
        continue;
      }

      Reflect::EntryPoint* p_entry_point = &(p_module->entry_points[entry_point_index]);
      CHECKED_READU32_CAST(p_parser, p_node->word_offset + 1, ExecutionModel, p_entry_point->spirv_execution_model);
      CHECKED_READU32(p_parser, p_node->word_offset + 2, p_entry_point->id);

      switch (p_entry_point->spirv_execution_model) {
        default: break;
        case ExecutionModel::Vertex                 : p_entry_point->shader_stage = Reflect::ShaderStage::Vertex; break;
        case ExecutionModel::TessellationControl    : p_entry_point->shader_stage = Reflect::ShaderStage::TessellationControl; break;
        case ExecutionModel::TessellationEvaluation : p_entry_point->shader_stage = Reflect::ShaderStage::TessellationEvaluation; break;
        case ExecutionModel::Geometry               : p_entry_point->shader_stage = Reflect::ShaderStage::Geometry; break;
        case ExecutionModel::Fragment               : p_entry_point->shader_stage = Reflect::ShaderStage::Fragment; break;
        case ExecutionModel::GLCompute              : p_entry_point->shader_stage = Reflect::ShaderStage::Compute; break;
      }

      ++entry_point_index;

      // Name length is required to calculate next operand
      u32 name_start_word_offset = 3;
      u32 name_length_with_terminator = 0;
      result = ReadStr(p_parser, p_node->word_offset + name_start_word_offset, 0, p_node->word_count, &name_length_with_terminator, NULL);
      if (result != Reflect::Result::Success) {
        return result;
      }
      p_entry_point->name = (const utf8*)(p_parser->spirv_code + p_node->word_offset + name_start_word_offset);

      u32 name_word_count = RoundUp(name_length_with_terminator, wordSize) / wordSize;
      size_t interface_variable_count = (p_node->word_count - (name_start_word_offset + name_word_count));
      u32* interface_variables = NULL;
      if (interface_variable_count > 0) {
        interface_variables = (u32*)calloc(interface_variable_count, sizeof(*(interface_variables)));
        if (IsNull(interface_variables)) {
          return Reflect::Result::ErrorAllocFailed;
        }
      }

      for (u32 var_index = 0; var_index < interface_variable_count; ++var_index) {
        u32 var_result_id = (u32)invalidValue;
        u32 offset = name_start_word_offset + name_word_count + var_index;
        CHECKED_READU32(p_parser, p_node->word_offset + offset, var_result_id);
        interface_variables[var_index] = var_result_id;
      }

      result = ParseInterfaceVariables(
          p_parser,
          p_module,
          p_entry_point,
          interface_variable_count,
          interface_variables);
      if (result != Reflect::Result::Success) {
        return result;
      }
      SafeFree(interface_variables);

      result = ParseStaticallyUsedResources(
          p_parser,
          p_module,
          p_entry_point,
          uniform_count,
          uniforms,
          push_constant_count,
          push_constants);
      if (result != Reflect::Result::Success) {
        return result;
      }
    }

    SafeFree(uniforms);
    SafeFree(push_constants);

    return Reflect::Result::Success;
  }
  
  inline Reflect::Result ParseExecutionModes(Parser* p_parser, Reflect::ShaderModuleStruct* p_module)
  {
    assert(IsNotNull(p_parser));
    assert(IsNotNull(p_parser->nodes));
    assert(IsNotNull(p_module));

    if (IsNotNull(p_parser) && IsNotNull(p_parser->spirv_code) && IsNotNull(p_parser->nodes)) {
      for (size_t node_idx = 0; node_idx < p_parser->node_count; ++node_idx) {
        Node* p_node = &(p_parser->nodes[node_idx]);
        if (p_node->op != Op::OpExecutionMode) {
          continue;
        }

        // Read entry point id
        u32 entry_point_id = 0;
        CHECKED_READU32(p_parser, p_node->word_offset + 1, entry_point_id);

        // Find entry point
        Reflect::EntryPoint* p_entry_point = NULL;
        for (size_t entry_point_idx = 0; entry_point_idx < p_module->entry_point_count; ++entry_point_idx) {
          if (p_module->entry_points[entry_point_idx].id == entry_point_id) {
            p_entry_point = &p_module->entry_points[entry_point_idx];
            break;
          }
        }
        // Bail if entry point is null
        if (IsNull(p_entry_point)) {
          return Reflect::Result::ErrorSpirvInvalidEntryPoint;
        }

        // Read execution mode
        auto execution_mode = (ExecutionMode)invalidValue;
        CHECKED_READU32(p_parser, p_node->word_offset + 2, execution_mode);

        // Parse execution mode
        switch (execution_mode) {
          default: {
            return Reflect::Result::ErrorSpirvInvalidExecutionMode;
          }
            break;

          case ExecutionMode::Invocations:
          case ExecutionMode::SpacingEqual:
          case ExecutionMode::SpacingFractionalEven:
          case ExecutionMode::SpacingFractionalOdd:
          case ExecutionMode::VertexOrderCw:
          case ExecutionMode::VertexOrderCcw:
          case ExecutionMode::PixelCenterInteger:
          case ExecutionMode::OriginUpperLeft:
          case ExecutionMode::OriginLowerLeft:
          case ExecutionMode::EarlyFragmentTests:
          case ExecutionMode::PointMode:
          case ExecutionMode::Xfb:
          case ExecutionMode::DepthReplacing:
          case ExecutionMode::DepthGreater:
          case ExecutionMode::DepthLess:
          case ExecutionMode::DepthUnchanged:
            break;

          case ExecutionMode::LocalSize: {
            CHECKED_READU32(p_parser, p_node->word_offset + 3, p_entry_point->local_size.x);
            CHECKED_READU32(p_parser, p_node->word_offset + 4, p_entry_point->local_size.y);
            CHECKED_READU32(p_parser, p_node->word_offset + 5, p_entry_point->local_size.z);
          }
            break;

          case ExecutionMode::LocalSizeHint:
          case ExecutionMode::InputPoints:
          case ExecutionMode::InputLines:
          case ExecutionMode::InputLinesAdjacency:
          case ExecutionMode::Triangles:
          case ExecutionMode::InputTrianglesAdjacency:
          case ExecutionMode::Quads:
          case ExecutionMode::Isolines:
          case ExecutionMode::OutputVertices:
          case ExecutionMode::OutputPoints:
          case ExecutionMode::OutputLineStrip:
          case ExecutionMode::OutputTriangleStrip:
          case ExecutionMode::VecTypeHint:
          case ExecutionMode::ContractionOff:
          case ExecutionMode::Initializer:
          case ExecutionMode::Finalizer:
          case ExecutionMode::SubgroupSize:
          case ExecutionMode::SubgroupsPerWorkgroup:
          case ExecutionMode::SubgroupsPerWorkgroupId:
          case ExecutionMode::LocalSizeId:
          case ExecutionMode::LocalSizeHintId:
          case ExecutionMode::PostDepthCoverage:
          case ExecutionMode::StencilRefReplacingEXT:
            break;
        }
      }
    }
    return Reflect::Result::Success;
  }
  
  inline Reflect::Result ParsePushConstantBlocks(Parser* p_parser, Reflect::ShaderModuleStruct* p_module)
  {
    for (size_t i = 0; i < p_parser->node_count; ++i) {
      Node* p_node = &(p_parser->nodes[i]);
      if ((p_node->op != Op::OpVariable) || (p_node->storage_class != StorageClass::PushConstant)) {
        continue;
      }

      p_module->push_constant_block_count += 1;
    }

    if (p_module->push_constant_block_count == 0) {
      return Reflect::Result::Success;
    }

    p_module->push_constant_blocks = (Reflect::BlockVariable*)calloc(p_module->push_constant_block_count, sizeof(*p_module->push_constant_blocks));
    if (IsNull(p_module->push_constant_blocks)) {
      return Reflect::Result::ErrorAllocFailed;
    }

    u32 push_constant_index = 0;
    for (size_t i = 0; i < p_parser->node_count; ++i) {
      Node* p_node = &(p_parser->nodes[i]);
      if ((p_node->op != Op::OpVariable) || (p_node->storage_class != StorageClass::PushConstant)) {
        continue;
      }

      Reflect::TypeDescription* p_type = FindType(p_module, p_node->type_id);
      if (IsNull(p_node)) {
        return Reflect::Result::ErrorSpirvInvalidIdReference;
      }
      // If the type is a pointer, resolve it
      if (p_type->op == Op::OpTypePointer) {
        // Find the type's node
        Node* p_type_node = FindNode(p_parser, p_type->id);
        if (IsNull(p_type_node)) {
          return Reflect::Result::ErrorSpirvInvalidIdReference;
        }
        // Should be the resolved type
        p_type = FindType(p_module, p_type_node->type_id);
        if (IsNull(p_type)) {
          return Reflect::Result::ErrorSpirvInvalidIdReference;
        }
      }

      Node* p_type_node = FindNode(p_parser, p_type->id);
      if (IsNull(p_type_node)) {
        return Reflect::Result::ErrorSpirvInvalidIdReference;
      }

      Reflect::BlockVariable* p_push_constant = &p_module->push_constant_blocks[push_constant_index];
      p_push_constant->spirv_id = p_node->result_id;
      Reflect::Result result = ParseDescriptorBlockVariable(p_parser, p_module, p_type, p_push_constant);
      if (result != Reflect::Result::Success) {
        return result;
      }
      result = ParseDescriptorBlockVariableSizes(p_parser, p_module, true, false, false, p_push_constant);
      if (result != Reflect::Result::Success) {
        return result;
      }

      ++push_constant_index;
    }

    return Reflect::Result::Success;
  }
  
  inline int SortCompareDescriptorSet(const void* a, const void* b) {
    const Reflect::DescriptorSet* p_elem_a = (const Reflect::DescriptorSet*)a;
    const Reflect::DescriptorSet* p_elem_b = (const Reflect::DescriptorSet*)b;
    int value = (int)(p_elem_a->set) - (int)(p_elem_b->set);
    // We should never see duplicate descriptor set numbers in a shader; if so, a tiebreaker
    // would be needed here.
    assert(value != 0);
    return value;
  }
  
  inline Reflect::Result ParseEntrypointDescriptorSets(Reflect::ShaderModuleStruct* p_module) {
    // Update the entry point's sets
    for (u32 i = 0; i < p_module->entry_point_count; ++i) {
      Reflect::EntryPoint* p_entry = &p_module->entry_points[i];
      for (u32 j = 0; j < p_entry->descriptor_set_count; ++j) {
        SafeFree(p_entry->descriptor_sets[j].bindings);
      }
      SafeFree(p_entry->descriptor_sets);
      p_entry->descriptor_set_count = 0;
      for (u32 j = 0; j < p_module->descriptor_set_count; ++j) {
        const Reflect::DescriptorSet* p_set = &p_module->descriptor_sets[j];
        for (u32 k = 0; k < p_set->binding_count; ++k) {
          bool found = SearchSortedUint32(
              p_entry->used_uniforms,
              p_entry->used_uniform_count,
              p_set->bindings[k]->spirv_id);
          if (found) {
            ++p_entry->descriptor_set_count;
            break;
          }
        }
      }

      p_entry->descriptor_sets = NULL;
      if (p_entry->descriptor_set_count > 0) {
        p_entry->descriptor_sets = (Reflect::DescriptorSet*)calloc(p_entry->descriptor_set_count,
                                                                   sizeof(*p_entry->descriptor_sets));
        if (IsNull(p_entry->descriptor_sets)) {
          return Reflect::Result::ErrorAllocFailed;
        }
      }
      p_entry->descriptor_set_count = 0;
      for (u32 j = 0; j < p_module->descriptor_set_count; ++j) {
        const Reflect::DescriptorSet* p_set = &p_module->descriptor_sets[j];
        u32 count = 0;
        for (u32 k = 0; k < p_set->binding_count; ++k) {
          bool found = SearchSortedUint32(
              p_entry->used_uniforms,
              p_entry->used_uniform_count,
              p_set->bindings[k]->spirv_id);
          if (found) {
            ++count;
          }
        }
        if (count == 0) {
          continue;
        }
        Reflect::DescriptorSet* p_entry_set = &p_entry->descriptor_sets[
            p_entry->descriptor_set_count++];
        p_entry_set->set = p_set->set;
        p_entry_set->bindings = (Reflect::DescriptorBinding**)calloc(count,
                                                                     sizeof(*p_entry_set->bindings));
        if (IsNull(p_entry_set->bindings)) {
          return Reflect::Result::ErrorAllocFailed;
        }
        for (u32 k = 0; k < p_set->binding_count; ++k) {
          bool found = SearchSortedUint32(
              p_entry->used_uniforms,
              p_entry->used_uniform_count,
              p_set->bindings[k]->spirv_id);
          if (found) {
            p_entry_set->bindings[p_entry_set->binding_count++] = p_set->bindings[k];
          }
        }
      }
    }

    return Reflect::Result::Success;
  }
  
  inline Reflect::Result ParseDescriptorSets(Reflect::ShaderModuleStruct* p_module)
  {
    // Count the descriptors in each set
    for (u32 i = 0; i < p_module->descriptor_binding_count; ++i) {
      Reflect::DescriptorBinding* p_descriptor = &(p_module->descriptor_bindings[i]);

      // Look for a target set using the descriptor's set number
      Reflect::DescriptorSet* p_target_set = NULL;
      for (u32 j = 0; j < Reflect::maxDescriptorSets; ++j) {
        Reflect::DescriptorSet* p_set = &p_module->descriptor_sets[j];
        if (p_set->set == p_descriptor->set) {
          p_target_set = p_set;
          break;
        }
      }

      // If a target set isn't found, find the first available one.
      if (IsNull(p_target_set)) {
        for (u32 j = 0; j < Reflect::maxDescriptorSets; ++j) {
          Reflect::DescriptorSet* p_set = &p_module->descriptor_sets[j];
          if (p_set->set == (u32)invalidValue) {
            p_target_set = p_set;
            p_target_set->set = p_descriptor->set;
            break;
          }
        }
      }

      if (IsNull(p_target_set)) {
        return Reflect::Result::ErrorInternalError;
      }

      p_target_set->binding_count += 1;
    }

    // Count the descriptor sets
    for (u32 i = 0; i < Reflect::maxDescriptorSets; ++i) {
      const Reflect::DescriptorSet* p_set = &p_module->descriptor_sets[i];
      if (p_set->set != (u32)invalidValue) {
        p_module->descriptor_set_count += 1;
      }
    }

    // Sort the descriptor sets based on numbers
    if (p_module->descriptor_set_count > 0) {
      qsort(p_module->descriptor_sets,
            p_module->descriptor_set_count,
            sizeof(*(p_module->descriptor_sets)),
            SortCompareDescriptorSet);
    }

    // Build descriptor pointer array
    for (u32 i = 0; i <p_module->descriptor_set_count; ++i) {
      Reflect::DescriptorSet* p_set = &(p_module->descriptor_sets[i]);
      p_set->bindings = (Reflect::DescriptorBinding **)calloc(p_set->binding_count, sizeof(*(p_set->bindings)));

      u32 descriptor_index = 0;
      for (u32 j = 0; j < p_module->descriptor_binding_count; ++j) {
        Reflect::DescriptorBinding* p_descriptor = &(p_module->descriptor_bindings[j]);
        if (p_descriptor->set == p_set->set) {
          assert(descriptor_index < p_set->binding_count);
          p_set->bindings[descriptor_index] = p_descriptor;
          ++descriptor_index;
        }
      }
    }

    return ParseEntrypointDescriptorSets(p_module);
  }

  inline Reflect::Result DisambiguateStorageBufferSrvUav(Reflect::ShaderModuleStruct* p_module)
  {
    if (p_module->descriptor_binding_count == 0) {
      return Reflect::Result::Success;
    }

    for (u32 descriptor_index = 0; descriptor_index < p_module->descriptor_binding_count; ++descriptor_index) {
      Reflect::DescriptorBinding* p_descriptor = &(p_module->descriptor_bindings[descriptor_index]);
      // Skip everything that isn't a StorageBufferdescriptor
      if (p_descriptor->descriptor_type != Reflect::DescriptorType::StorageBuffer) {
        continue;
      }

      //
      // Vulkan doesn't disambiguate between Srvs and Uavs so they
      // come back as StorageBuffer. The block parsing process will
      // mark a block as non-writable should any member of the block
      // or its descendants are non-writable.
      //
      if (p_descriptor->block.decoration_flags & Reflect::Decoration::NonWritable) {
        p_descriptor->resource_type = Reflect::ResourceType::Srv;
      }
    }

    return Reflect::Result::Success;
  }

  inline Reflect::Result SynchronizeDescriptorSets(Reflect::ShaderModuleStruct* p_module)
  {
    // Free and reset all descriptor set numbers
    for (u32 i = 0; i < Reflect::maxDescriptorSets; ++i) {
      Reflect::DescriptorSet* p_set = &p_module->descriptor_sets[i];
      SafeFree(p_set->bindings);
      p_set->binding_count = 0;
      p_set->set = (u32)invalidValue;
    }
    // Set descriptor set count to zero
    p_module->descriptor_set_count = 0;

    Reflect::Result result = ParseDescriptorSets(p_module);
    return result;
  }

  inline void SafeFreeTypes(Reflect::TypeDescription* p_type)
  {
    if (IsNull(p_type)) {
      return;
    }

    if (IsNotNull(p_type->members)) {
      for (size_t i = 0; i < p_type->member_count; ++i) {
        Reflect::TypeDescription* p_member = &p_type->members[i];
        SafeFreeTypes(p_member);
      }

      SafeFree(p_type->members);
      p_type->members = NULL;
    }
  }
  inline void SafeFreeBlockVariables(Reflect::BlockVariable* p_block)
  {
    if (IsNull(p_block)) {
      return;
    }

    if (IsNotNull(p_block->members)) {
      for (size_t i = 0; i < p_block->member_count; ++i) {
        Reflect::BlockVariable* p_member = &p_block->members[i];
        SafeFreeBlockVariables(p_member);
      }

      SafeFree(p_block->members);
      p_block->members = NULL;
    }
  }
  inline void SafeFreeInterfaceVariable(Reflect::InterfaceVariable* p_interface)
  {
    if (IsNull(p_interface)) {
      return;
    }

    if (IsNotNull(p_interface->members)) {
      for (size_t i = 0; i < p_interface->member_count; ++i) {
        Reflect::InterfaceVariable* p_member = &p_interface->members[i];
        SafeFreeInterfaceVariable(p_member);
      }

      SafeFree(p_interface->members);
      p_interface->members = NULL;
    }
  }

  inline void destroyShaderModuleStruct(Reflect::ShaderModuleStruct* p_module) {
    if (IsNull(p_module->_internal)) {
      return;
    }

    // Descriptor set bindings
    for (size_t i = 0; i < p_module->descriptor_set_count; ++i) {
      Reflect::DescriptorSet* p_set = &p_module->descriptor_sets[i];
      free(p_set->bindings);
    }

    // Descriptor binding blocks
    for (size_t i = 0; i < p_module->descriptor_binding_count; ++i) {
      Reflect::DescriptorBinding* p_descriptor = &p_module->descriptor_bindings[i];
      SafeFreeBlockVariables(&p_descriptor->block);
    }
    SafeFree(p_module->descriptor_bindings);

    // Entry points
    for (size_t i = 0; i < p_module->entry_point_count; ++i) {
      Reflect::EntryPoint* p_entry = &p_module->entry_points[i];
      for (size_t j = 0; j < p_entry->input_variable_count; j++) {
        SafeFreeInterfaceVariable(&p_entry->input_variables[j]);
      }
      for (size_t j = 0; j < p_entry->output_variable_count; j++) {
        SafeFreeInterfaceVariable(&p_entry->output_variables[j]);
      }
      for (u32 j = 0; j < p_entry->descriptor_set_count; ++j) {
        SafeFree(p_entry->descriptor_sets[j].bindings);
      }
      SafeFree(p_entry->descriptor_sets);
      SafeFree(p_entry->input_variables);
      SafeFree(p_entry->output_variables);
      SafeFree(p_entry->used_uniforms);
      SafeFree(p_entry->used_push_constants);
    }
    SafeFree(p_module->entry_points);

    // Push constants
    for (size_t i = 0; i < p_module->push_constant_block_count; ++i) {
      SafeFreeBlockVariables(&p_module->push_constant_blocks[i]);
    }
    SafeFree(p_module->push_constant_blocks);

    // Type infos
    for (size_t i = 0; i < p_module->_internal->type_description_count; ++i) {
      Reflect::TypeDescription* p_type = &p_module->_internal->type_descriptions[i];
      if (IsNotNull(p_type->members)) {
        SafeFreeTypes(p_type);
      }
      SafeFree(p_type->members);
    }
    SafeFree(p_module->_internal->type_descriptions);

    // Free SPIR-V code
    SafeFree(p_module->_internal->spirv_code);
    // Free internal
    SafeFree(p_module->_internal);
  }
}

Reflect::ShaderModule::ShaderModule(size_t size, const void *p_code) noexcept {
  // Initialize all module fields to zero
  memset(&m_module, 0, sizeof(m_module));

  // Allocate module internals
#ifdef __cplusplus
  m_module._internal = (Reflect::ShaderModuleStruct::Internal*)calloc(1, sizeof(*(m_module._internal)));
#else
  m_module._internal = calloc(1, sizeof(*(m_module._internal)));
#endif
  if (IsNull(m_module._internal)) {
    m_result = Reflect::Result::ErrorAllocFailed;
    return;
  }

  // Allocate SPIR-V code storage
  m_module._internal->spirv_size = size;
  m_module._internal->spirv_code = (u32*)calloc(1, m_module._internal->spirv_size);
  m_module._internal->spirv_word_count = (u32)(size / wordSize);
  if (IsNull(m_module._internal->spirv_code)) {
    SafeFree(m_module._internal);
    m_result = Reflect::Result::ErrorAllocFailed;
    return;
  }
  memcpy(m_module._internal->spirv_code, p_code, size);

  Parser parser = { 0 };
  m_result = CreateParser(m_module._internal->spirv_size,
                                        m_module._internal->spirv_code,
                                        &parser);

  // Generator
  {
    const u32* p_ptr = (const u32*)m_module._internal->spirv_code;
    m_module.generator = (Reflect::Generator)((*(p_ptr + 2) & 0xFFFF0000) >> 16);
  }

  if (m_result == Reflect::Result::Success) {
    m_result = ParseNodes(&parser);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseStrings(&parser);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseSource(&parser, &m_module);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseFunctions(&parser);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseMemberCounts(&parser);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseNames(&parser);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseDecorations(&parser);
  }

  // Start of reflection data parsing
  if (m_result == Reflect::Result::Success) {
    m_module.source_language = parser.source_language;
    m_module.source_language_version = parser.source_language_version;

    // Zero out descriptor set data
    m_module.descriptor_set_count = 0;
    memset(m_module.descriptor_sets, 0, Reflect::maxDescriptorSets * sizeof(*m_module.descriptor_sets));
    // Initialize descriptor set numbers
    for (u32 set_number = 0; set_number < Reflect::maxDescriptorSets; ++set_number) {
      m_module.descriptor_sets[set_number].set = (u32)invalidValue;
    }
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseTypes(&parser, &m_module);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseDescriptorBindings(&parser, &m_module);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseDescriptorType(&m_module);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseUavCounterBindings(&m_module);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseDescriptorBlocks(&parser, &m_module);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParsePushConstantBlocks(&parser, &m_module);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseEntryPoints(&parser, &m_module);
  }
  if (m_result == Reflect::Result::Success && m_module.entry_point_count > 0) {
    Reflect::EntryPoint* p_entry = &(m_module.entry_points[0]);
    m_module.entry_point_name = p_entry->name;
    m_module.entry_point_id = p_entry->id;
    m_module.spirv_execution_model = p_entry->spirv_execution_model;
    m_module.shader_stage = p_entry->shader_stage;
    m_module.input_variable_count = p_entry->input_variable_count;
    m_module.input_variables = p_entry->input_variables;
    m_module.output_variable_count = p_entry->output_variable_count;
    m_module.output_variables = p_entry->output_variables;
  }
  if (m_result == Reflect::Result::Success) {
    m_result = DisambiguateStorageBufferSrvUav(&m_module);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = SynchronizeDescriptorSets(&m_module);
  }
  if (m_result == Reflect::Result::Success) {
    m_result = ParseExecutionModes(&parser, &m_module);
  }

  // Destroy module if parse was not successful
  if (m_result != Reflect::Result::Success) {
    destroyShaderModuleStruct(&m_module);
  }

  DestroyParser(&parser);
}
Reflect::ShaderModule::~ShaderModule() {
  destroyShaderModuleStruct(&m_module);
}

Reflect::Result Reflect::ShaderModule::getStatus() const noexcept {
  return m_result;
}
valkyrie::Core::Span<const u32> Reflect::ShaderModule::getCode() const noexcept{
  return {m_module._internal->spirv_code, m_module._internal->spirv_size};
}

u32 Reflect::ShaderModule::getEntryPointCount() const noexcept {
  return m_module.entry_point_count;
}
valkyrie::Core::StringView Reflect::ShaderModule::getEntryPointName() const noexcept {
  return this->getEntryPointName(0);
}
valkyrie::Core::StringView Reflect::ShaderModule::getEntryPointName(u32 index) const {
  return m_module.entry_points[index].name;
}
valkyrie::Core::StringView Reflect::ShaderModule::getSourceFile() const noexcept {
  return m_module.source_file;
}
Reflect::ShaderStage Reflect::ShaderModule::getShaderStage() const noexcept {
  return m_module.shader_stage;
}

Reflect::ResultSpan<const Reflect::DescriptorBinding> Reflect::ShaderModule::enumerateDescriptorBindings() const {
  return {m_module.descriptor_bindings, m_module.descriptor_binding_count};
}
Reflect::ResultSpan<const Reflect::DescriptorBinding> Reflect::ShaderModule::enumerateEntryPointDescriptorBindings(Core::StringView entry_point) const {

  /*for (u32 entryIndex = 0; entryIndex < m_module.entry_point_count; ++entryIndex) {
    if (m_module.entry_points[entryIndex].name == entry_point)
      return {m_module.entry_points[entryIndex].};
  }*/
  //TODO: Implement....
}
Reflect::ResultSpan<const Reflect::DescriptorSet> Reflect::ShaderModule::enumerateDescriptorSets() const {
  return { m_module.descriptor_sets, m_module.descriptor_set_count };
}
Reflect::ResultSpan<const Reflect::InterfaceVariable> Reflect::ShaderModule::enumerateInputVariables() const {
  return {m_module.input_variables, m_module.input_variable_count};
}
Reflect::ResultSpan<const Reflect::InterfaceVariable> Reflect::ShaderModule::enumerateOutputVariables() const {
  return {m_module.output_variables, m_module.output_variable_count};
}
Reflect::ResultSpan<const Reflect::BlockVariable> Reflect::ShaderModule::enumeratePushConstantBlocks() const {
  return { m_module.push_constant_blocks, m_module.push_constant_block_count };
}



/*

const Reflect::EntryPoint* spvReflectGetEntryPoint(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   entry_point
) {
  if (IsNull(p_module) || IsNull(entry_point)) {
    return NULL;
  }

  for (u32 i = 0; i < p_module->entry_point_count; ++i) {
    if (strcmp(p_module->entry_points[i].name, entry_point) == 0) {
      return &p_module->entry_points[i];
    }
  }
  return NULL;
}

Reflect::Result spvReflectEnumerateDescriptorBindings(
    const Reflect::ShaderModuleStruct*  p_module,
    u32*                      p_count,
    Reflect::DescriptorBinding**  pp_bindings
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_count)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }

  if (IsNotNull(pp_bindings)) {
    if (*p_count != p_module->descriptor_binding_count) {
      return Reflect::Result::ErrorCountMismatch;
    }

    for (u32 index = 0; index < *p_count; ++index) {
      Reflect::DescriptorBinding* p_bindings = (Reflect::DescriptorBinding*)&p_module->descriptor_bindings[index];
      pp_bindings[index] = p_bindings;
    }
  }
  else {
    *p_count = p_module->descriptor_binding_count;
  }

  return Reflect::Result::Success;
}

Reflect::Result spvReflectEnumerateEntryPointDescriptorBindings(
    const Reflect::ShaderModuleStruct*  p_module,
    const utf8*                    entry_point,
    u32*                      p_count,
    Reflect::DescriptorBinding**  pp_bindings
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_count)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }

  const Reflect::EntryPoint* p_entry =
      spvReflectGetEntryPoint(p_module, entry_point);
  if (IsNull(p_entry)) {
    return Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
  }

  u32 count = 0;
  for (u32 i = 0; i < p_module->descriptor_binding_count; ++i) {
    bool found = SearchSortedUint32(
        p_entry->used_uniforms,
        p_entry->used_uniform_count,
        p_module->descriptor_bindings[i].spirv_id);
    if (found) {
      if (IsNotNull(pp_bindings)) {
        if (count >= *p_count) {
          return Reflect::Result::ErrorCountMismatch;
        }
        pp_bindings[count++] = (Reflect::DescriptorBinding*)&p_module->descriptor_bindings[i];
      } else {
        ++count;
      }
    }
  }
  if (IsNotNull(pp_bindings)) {
    if (count != *p_count) {
      return Reflect::Result::ErrorCountMismatch;
    }
  } else {
    *p_count = count;
  }
  return Reflect::Result::Success;
}

Reflect::Result spvReflectEnumerateDescriptorSets(
    const Reflect::ShaderModuleStruct* p_module,
    u32*                     p_count,
    Reflect::DescriptorSet**     pp_sets
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_count)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }

  if (IsNotNull(pp_sets)) {
    if (*p_count != p_module->descriptor_set_count) {
      return Reflect::Result::ErrorCountMismatch;
    }

    for (u32 index = 0; index < *p_count; ++index) {
      Reflect::DescriptorSet* p_set = (Reflect::DescriptorSet*)&p_module->descriptor_sets[index];
      pp_sets[index] = p_set;
    }
  }
  else {
    *p_count = p_module->descriptor_set_count;
  }

  return Reflect::Result::Success;
}

Reflect::Result spvReflectEnumerateEntryPointDescriptorSets(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   entry_point,
    u32*                     p_count,
    Reflect::DescriptorSet**     pp_sets
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_count)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }

  const Reflect::EntryPoint* p_entry =
      spvReflectGetEntryPoint(p_module, entry_point);
  if (IsNull(p_entry)) {
    return Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
  }

  if (IsNotNull(pp_sets)) {
    if (*p_count != p_entry->descriptor_set_count) {
      return Reflect::Result::ErrorCountMismatch;
    }

    for (u32 index = 0; index < *p_count; ++index) {
      Reflect::DescriptorSet* p_set = (Reflect::DescriptorSet*)&p_entry->descriptor_sets[index];
      pp_sets[index] = p_set;
    }
  }
  else {
    *p_count = p_entry->descriptor_set_count;
  }

  return Reflect::Result::Success;
}

Reflect::Result spvReflectEnumerateInputVariables(
    const Reflect::ShaderModuleStruct* p_module,
    u32*                     p_count,
    Reflect::InterfaceVariable** pp_variables
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_count)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }

  if (IsNotNull(pp_variables)) {
    if (*p_count != p_module->input_variable_count) {
      return Reflect::Result::ErrorCountMismatch;
    }

    for (u32 index = 0; index < *p_count; ++index) {
      Reflect::InterfaceVariable* p_var = (Reflect::InterfaceVariable*)&p_module->input_variables[index];
      pp_variables[index] = p_var;
    }
  }
  else {
    *p_count = p_module->input_variable_count;
  }

  return Reflect::Result::Success;
}

Reflect::Result spvReflectEnumerateEntryPointInputVariables(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   entry_point,
    u32*                     p_count,
    Reflect::InterfaceVariable** pp_variables
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_count)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }

  const Reflect::EntryPoint* p_entry =
      spvReflectGetEntryPoint(p_module, entry_point);
  if (IsNull(p_entry)) {
    return Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
  }

  if (IsNotNull(pp_variables)) {
    if (*p_count != p_entry->input_variable_count) {
      return Reflect::Result::ErrorCountMismatch;
    }

    for (u32 index = 0; index < *p_count; ++index) {
      Reflect::InterfaceVariable* p_var = (Reflect::InterfaceVariable*)&p_entry->input_variables[index];
      pp_variables[index] = p_var;
    }
  }
  else {
    *p_count = p_entry->input_variable_count;
  }

  return Reflect::Result::Success;
}

Reflect::Result spvReflectEnumerateOutputVariables(
    const Reflect::ShaderModuleStruct* p_module,
    u32*                     p_count,
    Reflect::InterfaceVariable** pp_variables
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_count)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }

  if (IsNotNull(pp_variables)) {
    if (*p_count != p_module->output_variable_count) {
      return Reflect::Result::ErrorCountMismatch;
    }

    for (u32 index = 0; index < *p_count; ++index) {
      Reflect::InterfaceVariable* p_var = (Reflect::InterfaceVariable*)&p_module->output_variables[index];
      pp_variables[index] = p_var;
    }
  }
  else {
    *p_count = p_module->output_variable_count;
  }

  return Reflect::Result::Success;
}

Reflect::Result spvReflectEnumerateEntryPointOutputVariables(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   entry_point,
    u32*                     p_count,
    Reflect::InterfaceVariable** pp_variables
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_count)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }

  const Reflect::EntryPoint* p_entry =
      spvReflectGetEntryPoint(p_module, entry_point);
  if (IsNull(p_entry)) {
    return Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
  }

  if (IsNotNull(pp_variables)) {
    if (*p_count != p_entry->output_variable_count) {
      return Reflect::Result::ErrorCountMismatch;
    }

    for (u32 index = 0; index < *p_count; ++index) {
      Reflect::InterfaceVariable* p_var = (Reflect::InterfaceVariable*)&p_entry->output_variables[index];
      pp_variables[index] = p_var;
    }
  }
  else {
    *p_count = p_entry->output_variable_count;
  }

  return Reflect::Result::Success;
}

Reflect::Result spvReflectEnumeratePushConstantBlocks(
    const Reflect::ShaderModuleStruct* p_module,
    u32*                     p_count,
    Reflect::BlockVariable**     pp_blocks
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_count)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }

  if (pp_blocks != NULL) {
    if (*p_count != p_module->push_constant_block_count) {
      return Reflect::Result::ErrorCountMismatch;
    }

    for (u32 index = 0; index < *p_count; ++index) {
      Reflect::BlockVariable* p_push_constant_blocks = (Reflect::BlockVariable*)&p_module->push_constant_blocks[index];
      pp_blocks[index] = p_push_constant_blocks;
    }
  }
  else {
    *p_count = p_module->push_constant_block_count;
  }

  return Reflect::Result::Success;
}
Reflect::Result spvReflectEnumeratePushConstants(
    const Reflect::ShaderModuleStruct* p_module,
    u32*                     p_count,
    Reflect::BlockVariable**     pp_blocks
)
{
  return spvReflectEnumeratePushConstantBlocks(p_module, p_count, pp_blocks);
}

Reflect::Result spvReflectEnumerateEntryPointPushConstantBlocks(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   entry_point,
    u32*                     p_count,
    Reflect::BlockVariable**     pp_blocks
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_count)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }


  const Reflect::EntryPoint* p_entry =
      spvReflectGetEntryPoint(p_module, entry_point);
  if (IsNull(p_entry)) {
    return Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
  }

  u32 count = 0;
  for (u32 i = 0; i < p_module->push_constant_block_count; ++i) {
    bool found = SearchSortedUint32(p_entry->used_push_constants,
                                    p_entry->used_push_constant_count,
                                    p_module->push_constant_blocks[i].spirv_id);
    if (found) {
      if (IsNotNull(pp_blocks)) {
        if (count >= *p_count) {
          return Reflect::Result::ErrorCountMismatch;
        }
        pp_blocks[count++] = (Reflect::BlockVariable*)&p_module->push_constant_blocks[i];
      } else {
        ++count;
      }
    }
  }
  if (IsNotNull(pp_blocks)) {
    if (count != *p_count) {
      return Reflect::Result::ErrorCountMismatch;
    }
  } else {
    *p_count = count;
  }
  return Reflect::Result::Success;
}

const Reflect::DescriptorBinding* spvReflectGetDescriptorBinding(
    const Reflect::ShaderModuleStruct* p_module,
    u32                      binding_number,
    u32                      set_number,
    Reflect::Result::*             p_result
)
{
  const Reflect::DescriptorBinding* p_descriptor = NULL;
  if (IsNotNull(p_module)) {
    for (u32 index = 0; index < p_module->descriptor_binding_count; ++index) {
      const Reflect::DescriptorBinding* p_potential = &p_module->descriptor_bindings[index];
      if ((p_potential->binding == binding_number) && (p_potential->set == set_number)) {
        p_descriptor = p_potential;
        break;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_descriptor)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_descriptor;
}

const Reflect::DescriptorBinding* spvReflectGetEntryPointDescriptorBinding(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   entry_point,
    u32                      binding_number,
    u32                      set_number,
    Reflect::Result::*             p_result
)
{
  const Reflect::EntryPoint* p_entry =
      spvReflectGetEntryPoint(p_module, entry_point);
  if (IsNull(p_entry)) {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
    }
    return NULL;
  }
  const Reflect::DescriptorBinding* p_descriptor = NULL;
  if (IsNotNull(p_module)) {
    for (u32 index = 0; index < p_module->descriptor_binding_count; ++index) {
      const Reflect::DescriptorBinding* p_potential = &p_module->descriptor_bindings[index];
      bool found = SearchSortedUint32(
          p_entry->used_uniforms,
          p_entry->used_uniform_count,
          p_potential->spirv_id);
      if ((p_potential->binding == binding_number) && (p_potential->set == set_number) && found) {
        p_descriptor = p_potential;
        break;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_descriptor)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_descriptor;
}

const Reflect::DescriptorSet* spvReflectGetDescriptorSet(
    const Reflect::ShaderModuleStruct* p_module,
    u32                      set_number,
    Reflect::Result::*             p_result
)
{
  const Reflect::DescriptorSet* p_set = NULL;
  if (IsNotNull(p_module)) {
    for (u32 index = 0; index < p_module->descriptor_set_count; ++index) {
      const Reflect::DescriptorSet* p_potential = &p_module->descriptor_sets[index];
      if (p_potential->set == set_number) {
        p_set = p_potential;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_set)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_set;
}

const Reflect::DescriptorSet* spvReflectGetEntryPointDescriptorSet(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   entry_point,
    u32                      set_number,
    Reflect::Result::*             p_result)
{
  const Reflect::DescriptorSet* p_set = NULL;
  if (IsNotNull(p_module)) {
    const Reflect::EntryPoint* p_entry = spvReflectGetEntryPoint(p_module, entry_point);
    if (IsNull(p_entry)) {
      if (IsNotNull(p_result)) {
        *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
      }
      return NULL;
    }
    for (u32 index = 0; index < p_entry->descriptor_set_count; ++index) {
      const Reflect::DescriptorSet* p_potential = &p_entry->descriptor_sets[index];
      if (p_potential->set == set_number) {
        p_set = p_potential;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_set)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_set;
}


const Reflect::InterfaceVariable* spvReflectGetInputVariableByLocation(
    const Reflect::ShaderModuleStruct* p_module,
    u32                      location,
    Reflect::Result::*             p_result
)
{
  if (location == invalidValue) {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
    }
    return NULL;
  }
  const Reflect::InterfaceVariable* p_var = NULL;
  if (IsNotNull(p_module)) {
    for (u32 index = 0; index < p_module->input_variable_count; ++index) {
      const Reflect::InterfaceVariable* p_potential = &p_module->input_variables[index];
      if (p_potential->location == location) {
        p_var = p_potential;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_var)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_var;
}
const Reflect::InterfaceVariable* spvReflectGetInputVariable(
    const Reflect::ShaderModuleStruct* p_module,
    u32                      location,
    Reflect::Result::*             p_result
)
{
  return spvReflectGetInputVariableByLocation(p_module, location, p_result);
}

const Reflect::InterfaceVariable* spvReflectGetEntryPointInputVariableByLocation(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   entry_point,
    u32                      location,
    Reflect::Result::*             p_result
)
{
  if (location == invalidValue) {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
    }
    return NULL;
  }

  const Reflect::InterfaceVariable* p_var = NULL;
  if (IsNotNull(p_module)) {
    const Reflect::EntryPoint* p_entry =
        spvReflectGetEntryPoint(p_module, entry_point);
    if (IsNull(p_entry)) {
      if (IsNotNull(p_result)) {
        *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
      }
      return NULL;
    }
    for (u32 index = 0; index < p_entry->input_variable_count; ++index) {
      const Reflect::InterfaceVariable* p_potential = &p_entry->input_variables[index];
      if (p_potential->location == location) {
        p_var = p_potential;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_var)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_var;
}

const Reflect::InterfaceVariable* spvReflectGetInputVariableBySemantic(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   semantic,
    Reflect::Result::*             p_result
)
{
  if (IsNull(semantic)) {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_NULL_POINTER;
    }
    return NULL;
  }
  if (semantic[0] == '\0') {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
    }
    return NULL;
  }
  const Reflect::InterfaceVariable* p_var = NULL;
  if (IsNotNull(p_module)) {
    for (u32 index = 0; index < p_module->input_variable_count; ++index) {
      const Reflect::InterfaceVariable* p_potential = &p_module->input_variables[index];
      if (p_potential->semantic != NULL && strcmp(p_potential->semantic, semantic) == 0) {
        p_var = p_potential;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_var)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_var;
}

const Reflect::InterfaceVariable* spvReflectGetEntryPointInputVariableBySemantic(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   entry_point,
    const utf8*                   semantic,
    Reflect::Result::*             p_result
)
{
  if (IsNull(semantic)) {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_NULL_POINTER;
    }
    return NULL;
  }
  if (semantic[0] == '\0') {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
    }
    return NULL;
  }
  const Reflect::InterfaceVariable* p_var = NULL;
  if (IsNotNull(p_module)) {
    const Reflect::EntryPoint* p_entry = spvReflectGetEntryPoint(p_module, entry_point);
    if (IsNull(p_entry)) {
      if (IsNotNull(p_result)) {
        *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
      }
      return NULL;
    }
    for (u32 index = 0; index < p_entry->input_variable_count; ++index) {
      const Reflect::InterfaceVariable* p_potential = &p_entry->input_variables[index];
      if (p_potential->semantic != NULL && strcmp(p_potential->semantic, semantic) == 0) {
        p_var = p_potential;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_var)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_var;
}

const Reflect::InterfaceVariable* spvReflectGetOutputVariableByLocation(
    const Reflect::ShaderModuleStruct*  p_module,
    u32                       location,
    Reflect::Result::*              p_result
)
{
  if (location == invalidValue) {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
    }
    return NULL;
  }
  const Reflect::InterfaceVariable* p_var = NULL;
  if (IsNotNull(p_module)) {
    for (u32 index = 0; index < p_module->output_variable_count; ++index) {
      const Reflect::InterfaceVariable* p_potential = &p_module->output_variables[index];
      if (p_potential->location == location) {
        p_var = p_potential;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_var)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_var;
}
const Reflect::InterfaceVariable* spvReflectGetOutputVariable(
    const Reflect::ShaderModuleStruct*  p_module,
    u32                       location,
    Reflect::Result::*              p_result
)
{
  return spvReflectGetOutputVariableByLocation(p_module, location, p_result);
}

const Reflect::InterfaceVariable* spvReflectGetEntryPointOutputVariableByLocation(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   entry_point,
    u32                      location,
    Reflect::Result::*             p_result
)
{
  if (location == invalidValue) {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
    }
    return NULL;
  }

  const Reflect::InterfaceVariable* p_var = NULL;
  if (IsNotNull(p_module)) {
    const Reflect::EntryPoint* p_entry = spvReflectGetEntryPoint(p_module, entry_point);
    if (IsNull(p_entry)) {
      if (IsNotNull(p_result)) {
        *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
      }
      return NULL;
    }
    for (u32 index = 0; index < p_entry->output_variable_count; ++index) {
      const Reflect::InterfaceVariable* p_potential = &p_entry->output_variables[index];
      if (p_potential->location == location) {
        p_var = p_potential;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_var)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_var;
}

const Reflect::InterfaceVariable* spvReflectGetOutputVariableBySemantic(
    const Reflect::ShaderModuleStruct*  p_module,
    const utf8*                    semantic,
    Reflect::Result::*              p_result
)
{
  if (IsNull(semantic)) {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_NULL_POINTER;
    }
    return NULL;
  }
  if (semantic[0] == '\0') {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
    }
    return NULL;
  }
  const Reflect::InterfaceVariable* p_var = NULL;
  if (IsNotNull(p_module)) {
    for (u32 index = 0; index < p_module->output_variable_count; ++index) {
      const Reflect::InterfaceVariable* p_potential = &p_module->output_variables[index];
      if (p_potential->semantic != NULL && strcmp(p_potential->semantic, semantic) == 0) {
        p_var = p_potential;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_var)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_var;
}

const Reflect::InterfaceVariable* spvReflectGetEntryPointOutputVariableBySemantic(
    const Reflect::ShaderModuleStruct* p_module,
    const utf8*                   entry_point,
    const utf8*                   semantic,
    Reflect::Result::*             p_result)
{
  if (IsNull(semantic)) {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_NULL_POINTER;
    }
    return NULL;
  }
  if (semantic[0] == '\0') {
    if (IsNotNull(p_result)) {
      *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
    }
    return NULL;
  }
  const Reflect::InterfaceVariable* p_var = NULL;
  if (IsNotNull(p_module)) {
    const Reflect::EntryPoint* p_entry = spvReflectGetEntryPoint(p_module, entry_point);
    if (IsNull(p_entry)) {
      if (IsNotNull(p_result)) {
        *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
      }
      return NULL;
    }
    for (u32 index = 0; index < p_entry->output_variable_count; ++index) {
      const Reflect::InterfaceVariable* p_potential = &p_entry->output_variables[index];
      if (p_potential->semantic != NULL && strcmp(p_potential->semantic, semantic) == 0) {
        p_var = p_potential;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_var)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_var;
}

const Reflect::BlockVariable* spvReflectGetPushConstantBlock(
    const Reflect::ShaderModuleStruct* p_module,
    u32                      index,
    Reflect::Result::*             p_result
)
{
  const Reflect::BlockVariable* p_push_constant = NULL;
  if (IsNotNull(p_module)) {
    if (index < p_module->push_constant_block_count) {
      p_push_constant = &p_module->push_constant_blocks[index];
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_push_constant)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_push_constant;
}
const Reflect::BlockVariable* spvReflectGetPushConstant(
    const Reflect::ShaderModuleStruct* p_module,
    u32                      index,
    Reflect::Result::*             p_result
)
{
  return spvReflectGetPushConstantBlock(p_module, index, p_result);
}

const Reflect::BlockVariable* spvReflectGetEntryPointPushConstantBlock(
    const Reflect::ShaderModuleStruct*  p_module,
    const utf8*                    entry_point,
    Reflect::Result::*              p_result)
{
  const Reflect::BlockVariable* p_push_constant = NULL;
  if (IsNotNull(p_module)) {
    const Reflect::EntryPoint* p_entry =
        spvReflectGetEntryPoint(p_module, entry_point);
    if (IsNull(p_entry)) {
      if (IsNotNull(p_result)) {
        *p_result = Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
      }
      return NULL;
    }
    for (u32 i = 0; i < p_module->push_constant_block_count; ++i) {
      bool found = SearchSortedUint32(
          p_entry->used_push_constants,
          p_entry->used_push_constant_count,
          p_module->push_constant_blocks[i].spirv_id);
      if (found) {
        p_push_constant = &p_module->push_constant_blocks[i];
        break;
      }
    }
  }
  if (IsNotNull(p_result)) {
    *p_result = IsNotNull(p_push_constant)
                ?  Reflect::Result::Success
                : (IsNull(p_module) ? Reflect::Result::ERROR_NULL_POINTER
                                    : Reflect::Result::ERROR_ELEMENT_NOT_FOUND);
  }
  return p_push_constant;
}

Reflect::Result spvReflectChangeDescriptorBindingNumbers(
    Reflect::ShaderModuleStruct*            p_module,
    const Reflect::DescriptorBinding* p_binding,
    u32                           new_binding_number,
    u32                           new_set_binding
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_binding)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }

  Reflect::DescriptorBinding* p_target_descriptor = NULL;
  for (u32 index = 0; index < p_module->descriptor_binding_count; ++index) {
    if(&p_module->descriptor_bindings[index] == p_binding) {
      p_target_descriptor = &p_module->descriptor_bindings[index];
      break;
    }
  }

  if (IsNotNull(p_target_descriptor)) {
    if (p_target_descriptor->word_offset.binding > (p_module->_internal->spirv_word_count - 1)) {
      return Reflect::Result::ErrorRangeExceeded;
    }
    // Binding number
    if (new_binding_number != (u32)Reflect::bindingNumberDontChange) {
      u32* p_code = p_module->_internal->spirv_code + p_target_descriptor->word_offset.binding;
      *p_code = new_binding_number;
      p_target_descriptor->binding = new_binding_number;
    }
    // Set number
    if (new_set_binding != (u32)Reflect::setNumberDontChange) {
      u32* p_code = p_module->_internal->spirv_code + p_target_descriptor->word_offset.set;
      *p_code = new_set_binding;
      p_target_descriptor->set = new_set_binding;
    }
  }

  Reflect::Result result = Reflect::Result::Success;
  if (new_set_binding != (u32)Reflect::setNumberDontChange) {
    result = SynchronizeDescriptorSets(p_module);
  }
  return result;
}
Reflect::Result spvReflectChangeDescriptorBindingNumber(
    Reflect::ShaderModuleStruct*            p_module,
    const Reflect::DescriptorBinding* p_descriptor_binding,
    u32                           new_binding_number,
    u32                           optional_new_set_number
)
{
  return spvReflectChangeDescriptorBindingNumbers(
      p_module,p_descriptor_binding,
      new_binding_number,
      optional_new_set_number);
}

Reflect::Result spvReflectChangeDescriptorSetNumber(
    Reflect::ShaderModuleStruct*        p_module,
    const Reflect::DescriptorSet* p_set,
    u32                       new_set_number
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_set)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  Reflect::DescriptorSet* p_target_set = NULL;
  for (u32 index = 0; index < Reflect::maxDescriptorSetsS; ++index) {
    // The descriptor sets for specific entry points might not be in this set,
    // so just match on set index.
    if (p_module->descriptor_sets[index].set == p_set->set) {
      p_target_set = (Reflect::DescriptorSet*)p_set;
      break;
    }
  }

  Reflect::Result result = Reflect::Result::Success;
  if (IsNotNull(p_target_set) && new_set_number != (u32)Reflect::setNumberDontChange) {
    for (u32 index = 0; index < p_target_set->binding_count; ++index) {
      Reflect::DescriptorBinding* p_descriptor = p_target_set->bindings[index];
      if (p_descriptor->word_offset.set > (p_module->_internal->spirv_word_count - 1)) {
        return Reflect::Result::ErrorRangeExceeded;
      }

      u32* p_code = p_module->_internal->spirv_code + p_descriptor->word_offset.set;
      *p_code = new_set_number;
      p_descriptor->set = new_set_number;
    }

    result = SynchronizeDescriptorSets(p_module);
  }

  return result;
}

static Reflect::Result ChangeVariableLocation(
    Reflect::ShaderModuleStruct*      p_module,
    Reflect::InterfaceVariable* p_variable,
    u32                     new_location
)
{
  if (p_variable->word_offset.location > (p_module->_internal->spirv_word_count - 1)) {
    return Reflect::Result::ErrorRangeExceeded;
  }
  u32* p_code = p_module->_internal->spirv_code + p_variable->word_offset.location;
  *p_code = new_location;
  p_variable->location = new_location;
  return Reflect::Result::Success;
}

Reflect::Result spvReflectChangeInputVariableLocation(
    Reflect::ShaderModuleStruct*            p_module,
    const Reflect::InterfaceVariable* p_input_variable,
    u32                           new_location
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_input_variable)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  for (u32 index = 0; index < p_module->input_variable_count; ++index) {
    if(&p_module->input_variables[index] == p_input_variable) {
      return ChangeVariableLocation(p_module, &p_module->input_variables[index], new_location);
    }
  }
  return Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
}

Reflect::Result spvReflectChangeOutputVariableLocation(
    Reflect::ShaderModuleStruct*             p_module,
    const Reflect::InterfaceVariable*  p_output_variable,
    u32                            new_location
)
{
  if (IsNull(p_module)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  if (IsNull(p_output_variable)) {
    return Reflect::Result::ERROR_NULL_POINTER;
  }
  for (u32 index = 0; index < p_module->output_variable_count; ++index) {
    if(&p_module->output_variables[index] == p_output_variable) {
      return ChangeVariableLocation(p_module, &p_module->output_variables[index], new_location);
    }
  }
  return Reflect::Result::ERROR_ELEMENT_NOT_FOUND;
}

const utf8* spvReflectSourceLanguage(SourceLanguage source_lang)
{
  switch (source_lang) {
    case SourceLanguage::Unknown    : return "Unknown";
    case SourceLanguage::ESSL       : return "ESSL";
    case SourceLanguage::GLSL       : return "GLSL";
    case SourceLanguage::OpenCL_C   : return "OpenCL_C";
    case SourceLanguage::OpenCL_CPP : return "OpenCL_CPP";
    case SourceLanguage::HLSL       : return "HLSL";

    case SourceLanguage::Max:
      break;
  }
  return "";
}
*/
