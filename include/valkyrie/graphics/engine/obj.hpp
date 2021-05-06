//
// Created by Maxwell on 2020-11-23.
//

#ifndef VALKYRIE_GRAPHICS_ENGINE_OBJ_HPP
#define VALKYRIE_GRAPHICS_ENGINE_OBJ_HPP

/*
The MIT License (MIT)
Copyright (c) 2012-2018 Syoyo Fujita and many contributors.
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

//
// version 2.0.0 : Add new object oriented api. 1.x api is still provided.
//                 * Support line primitive.
//                 * Support points primitive.
//                 * Support multiple search path for .mtl(v1 api).
//                 * Support vertex weight `vw`(as an tinyobj extension)
//                 * Support escaped whitespece in mtllib
// version 1.4.0 : Modifed ParseTextureNameAndOption api
// version 1.3.1 : Make ParseTextureNameAndOption api public
// version 1.3.0 : Separate warning and error message(breaking api of LoadObj)
// version 1.2.3 : Added color space extension('-colorspace') to tex opts.
// version 1.2.2 : Parse multiple group names.
// version 1.2.1 : Added initial support for line('l') primitive(PR #178)
// version 1.2.0 : Hardened implementation(#175)
// version 1.1.1 : Support smoothing groups(#162)
// version 1.1.0 : Support parsing vertex color(#144)
// version 1.0.8 : Fix parsing `g` tag just after `usemtl`(#138)
// version 1.0.7 : Support multiple tex options(#126)
// version 1.0.6 : Add TINYOBJLOADER_USE_DOUBLE option(#124)
// version 1.0.5 : Ignore `Tr` when `d` exists in MTL(#43)
// version 1.0.4 : Support multiple filenames for 'mtllib'(#112)
// version 1.0.3 : Support parsing texture options(#85)
// version 1.0.2 : Improve parsing speed by about a factor of 2 for large
// files(#105)
// version 1.0.1 : Fixes a shape is lost if obj ends with a 'usemtl'(#104)
// version 1.0.0 : Change data structure. Change license from BSD to MIT.
//

//
// Use this in *one* .cc
//   #define TINYOBJLOADER_IMPLEMENTATION
//   #include "tiny_obj_loader.h"
//


#include <valkyrie/adt/small_vector.hpp>
#include <valkyrie/adt/string.hpp>
#include <valkyrie/adt/array_ref.hpp>
#include <valkyrie/adt/dictionary.hpp>

#include <valkyrie/utility/filesystem.hpp>



#include <map>
#include <string>
#include <vector>



// https://en.wikipedia.org/wiki/Wavefront_.obj_file says ...
//
//  -blendu on | off                       # set horizontal texture blending
//  (default on)
//  -blendv on | off                       # set vertical texture blending
//  (default on)
//  -boost real_value                      # boost mip-map sharpness
//  -mm base_value gain_value              # modify texture map values (default
//  0 1)
//                                         #     base_value = brightness,
//                                         gain_value = contrast
//  -o u [v [w]]                           # Origin offset             (default
//  0 0 0)
//  -s u [v [w]]                           # Scale                     (default
//  1 1 1)
//  -t u [v [w]]                           # Turbulence                (default
//  0 0 0)
//  -texres resolution                     # texture resolution to create
//  -clamp on | off                        # only render texels in the clamped
//  0-1 range (default off)
//                                         #   When unclamped, textures are
//                                         repeated across a surface,
//                                         #   when clamped, only texels which
//                                         fall within the 0-1
//                                         #   range are rendered.
//  -bm mult_value                         # bump multiplier (for bump maps
//  only)
//
//  -imfchan r | g | b | m | l | z         # specifies which channel of the file
//  is used to
//                                         # create a scalar or bump texture.
//                                         r:red, g:green,
//                                         # b:blue, m:matte, l:luminance,
//                                         z:z-depth..
//                                         # (the default for bump is 'l' and
//                                         for decal is 'm')
//  bump -imfchan r bumpmap.tga            # says to use the red channel of
//  bumpmap.tga as the bumpmap
//
// For reflection maps...
//
//   -type sphere                           # specifies a sphere for a "refl"
//   reflection map
//   -type cube_top    | cube_bottom |      # when using a cube map, the texture
//   file for each
//         cube_front  | cube_back   |      # side of the cube is specified
//         separately
//         cube_left   | cube_right
//
// TinyObjLoader extension.
//
//   -colorspace SPACE                      # Color space of the texture. e.g.
//   'sRGB` or 'linear'
//

namespace valkyrie::graphics::engine::obj{

/*#ifdef TINYOBJLOADER_USE_DOUBLE
  //#pragma message "using double"
typedef double real_t;
#else
//#pragma message "using float"
  typedef float real_t;
#endif*/

  enum class texture_type{
    none,
    sphere,
    cube_top,
    cube_bottom,
    cube_front,
    cube_back,
    cube_left,
    cube_right
  };
  /*0 Color on and Ambient off
  1 Color on and Ambient on
  2 Highlight on
  3 Reflection on and Ray trace on
  4 Transparency: Glass on
  Reflection: Ray trace on
  5 Reflection: Fresnel on and Ray trace on
  6 Transparency: Refraction on
  Reflection: Fresnel off and Ray trace on
  7 Transparency: Refraction on
  Reflection: Fresnel on and Ray trace on
  8 Reflection on and Ray trace off
  9 Transparency: Glass on
  Reflection: Ray trace off
  10 Casts shadows onto invisible surfaces*/



  enum class illumination_model_type{
    color_only,
    diffuse,
    specular,
    reflection,
    glass,
    fresnel_reflection,
    refraction,
    fresnel_refraction,
    reflection_no_tracing,
    glass_no_tracing,
    shadowmatte
  };
  struct illumination_model{
    bool diffuse;
    bool specular;
    bool reflection;
    bool fresnel;
    bool refraction;
    bool ray_tracing;
    bool shadowmatte;
  };
  
  extern const illumination_model illumination_models[11];

  

  struct texture_option{
    texture_type type;

    f32 sharpness;
    f32 brightness;
    f32 contrast;
    f32 origin_offset[3];
    f32 scale[3];
    f32 turbulence[3];
    int texture_resolution;

    bool clamp;
    char imfchan;
    bool blendu;
    bool blendv;
    f32  bump_multiplier;

    string colorspace;
  };
  struct material{
    string name;

    f32    ambient[3];
    f32    diffuse[3];
    f32    specular[3];
    f32    transmittance[3];
    f32    emission[3];
    f32    shininess;
    f32    ior;
    f32    dissolve;

    illumination_model_type illum;

    string ambient_texname;             // map_Ka
    string diffuse_texname;             // map_Kd
    string specular_texname;            // map_Ks
    string specular_highlight_texname;  // map_Ns
    string bump_texname;                // map_bump, map_Bump, bump
    string displacement_texname;        // disp
    string alpha_texname;               // map_d
    string reflection_texname;          // refl


    texture_option ambient_texopt;
    texture_option diffuse_texopt;
    texture_option specular_texopt;
    texture_option specular_highlight_texopt;
    texture_option bump_texopt;
    texture_option displacement_texopt;
    texture_option alpha_texopt;
    texture_option reflection_texopt;

    // PBR extension
    // http://exocortex.com/blog/extending_wavefront_mtl_to_support_pbr
    f32 roughness;            // [0, 1] default 0
    f32 metallic;             // [0, 1] default 0
    f32 sheen;                // [0, 1] default 0
    f32 clearcoat_thickness;  // [0, 1] default 0
    f32 clearcoat_roughness;  // [0, 1] default 0
    f32 anisotropy;           // aniso. [0, 1] default 0
    f32 anisotropy_rotation;  // anisor. [0, 1] default 0

    string roughness_texname;  // map_Pr
    string metallic_texname;   // map_Pm
    string sheen_texname;      // map_Ps
    string emissive_texname;   // map_Ke
    string normal_texname;     // norm. For normal mapping.

    texture_option roughness_texopt;
    texture_option metallic_texopt;
    texture_option sheen_texopt;
    texture_option emissive_texopt;
    texture_option normal_texopt;

    dictionary<string> unknown_parameter;
  };
  struct tag{
    string         name;

    vector<i32>    i32_values;
    vector<f32>    f32_values;
    vector<string> string_values;
  };
  struct joint_and_weight{
    i32 joint_id;
    f32 weight;
  };
  struct skin_weight{
    i32 vertex_id;

    vector<joint_and_weight> weight_values;
  };
  struct index{
    i32 vertex;
    i32 normal;
    i32 texcoord;
  };
  struct mesh{
    vector<index> indices;
    vector<u8>  num_face_vertices;          // The number of vertices per
    // face. 3 = triangle, 4 = quad,
    // ... Up to 255 vertices per face.
    vector<i32> material_ids;  // per-face material ID
    vector<u32> smoothing_group_ids;  // per-face smoothing group
    // ID(0 = off. positive value
    // = group id)
    vector<tag> tags;                        // SubD tag
  };

  struct lines{
    // Linear flattened indices.
    vector<index> indices;        // indices for vertices(poly lines)
    vector<i32>   num_line_vertices;  // The number of vertices per line.
  };
  struct points{
    vector<index> indices;  // indices for points
  };
  struct shape {
    string name;
    mesh   mesh;
    lines  lines;
    points points;
  };

// Vertex attributes
  struct vertex_attributes{
    vector<f32> vertices;  // 'v'(xyz)

    // For backward compatibility, we store vertex weight in separate array.
    vector<f32> vertex_weights;  // 'v'(w)
    vector<f32> normals;         // 'vn'
    vector<f32> texcoords;       // 'vt'(uv)

    // For backward compatibility, we store texture coordinate 'w' in separate
    // array.
    vector<f32> texcoord_ws;  // 'vt'(w)
    vector<f32> colors;       // extension: vertex colors

    //
    // TinyObj extension.
    //

    // NOTE(syoyo): array index is based on the appearance order.
    // To get a corresponding skin weight for a specific vertex id `vid`,
    // Need to reconstruct a look up table: `skin_weight_t::vertex_id` == `vid`
    // (e.g. using std::map, std::unordered_map)
    vector<skin_weight> skin_weights;
  };


  VK_interface material_reader{

    virtual ~material_reader() = default;


    virtual bool load(string_view matId,
                      vector<material>* mat,
                      dictionary<int> *matMap,
                      string* warn,
                      string* err) = 0;
  };

  class material_file_reader   : public material_reader{
  public:
    // Path could contain separator(';' in Windows, ':' in Posix)
    explicit material_file_reader(const fs::path& mtl_basedir) : m_mtlBaseDir(mtl_basedir) {}
    bool load(string_view matId,
              vector<material>* mat,
              dictionary<int> *matMap,
              string* warn,
              string* err) override;

  private:
    fs::path m_mtlBaseDir;
  };
  class material_stream_reader : public material_reader{
  public:

    explicit material_stream_reader(std::istream &inStream) noexcept : m_in(inStream){}

    bool load(string_view matId,
              vector<material>* mat,
              dictionary<int> *matMap,
              string* warn,
              string* err) override;

  private:
    std::istream & m_in;
  };

  VK_interface loader_operations{
    virtual void vertex(f32 x, f32 y, f32 z, f32 w)   noexcept = 0;
    virtual void normal(f32 x, f32 y, f32 z)          noexcept = 0;

    virtual void texcoord(f32 x, f32 y, f32 z)        noexcept = 0;

    virtual void indices(span<index> i)               noexcept = 0;

    virtual void usemat(string_view name, int id)     noexcept = 0;
    virtual void matlib(span<material> materials)     noexcept = 0;

    virtual void group(span<const char*> group_names) noexcept = 0;
    virtual void object(const char* name)             noexcept = 0;
  };

  struct loader_config {
    bool triangulate      = true;  // triangulate polygon?

    /// Parse vertex color.
    /// If vertex color is not present, its filled with default value.
    /// false = no vertex color
    /// This will increase memory of parsed .obj
    bool vertex_color     = true;

    ///
    /// Search path to .mtl file.
    /// Default = "" = search from the same directory of .obj file.
    /// Valid only when loading .obj from a file.
    ///
    fs::path material_search_path = {};

    loader_config() : triangulate(true), vertex_color(true) {}
  };

  class loader{
  public:
    virtual ~loader() = default;


    bool load_from_file(const fs::path& filename, const loader_config &config = {});
    bool parse_from_string(string_view obj_text, string_view mtl_text, const loader_config &config = {});
    bool load_object(std::istream& in) noexcept;


    virtual const vertex_attributes& get_vertex_attributes() const noexcept = 0;
    virtual span<const shape>        get_shapes() const noexcept = 0;
    virtual span<const material>     get_materials() const noexcept = 0;



    bool valid() const noexcept { return valid_; }
    string_view warning() const noexcept { return warning_; }
    string_view error() const noexcept { return error_; }

  private:

    void do_load_object(std::istream& in,
                        material_reader& read_material_fn,
                        bool triangulate,
                        bool default_vcols_fallback);

    bool do_load_materials(std::istream& inStream);

    virtual loader_operations*     get_operations() const noexcept = 0;
    virtual material_reader*       get_material_reader() const noexcept = 0;

    virtual void anchor();

  protected:
    string warning_;
    string error_;

  private:
    bool   valid_;
  };

  /*struct callbacks {
    // W is optional and set to 1 if there is no `w` item in `v` line
    void (*vertex_cb)(void *user_data, real_t x, real_t y, real_t z, real_t w);
    void (*normal_cb)(void *user_data, real_t x, real_t y, real_t z);

    // y and z are optional and set to 0 if there is no `y` and/or `z` item(s) in
    // `vt` line.
    void (*texcoord_cb)(void *user_data, real_t x, real_t y, real_t z);

    // called per 'f' line. num_indices is the number of face indices(e.g. 3 for
    // triangle, 4 for quad)
    // 0 will be passed for undefined index in index_t members.
    void (*index_cb)(void *user_data, index *indices, int num_indices);
    // `name` obj_material name, `material_id` = the array index of material_t[]. -1
    // if
    // a obj_material not found in .mtl
    void (*usemtl_cb)(void *user_data, const char *name, int material_id);
    // `materials` = parsed obj_material data.
    void (*mtllib_cb)(void *user_data, const material_t *materials,
                      int num_materials);
    // There may be multiple group names
    void (*group_cb)(void *user_data, const char **names, int num_names);
    void (*object_cb)(void *user_data, const char *name);

    callbacks()
        : vertex_cb(nullptr),
          normal_cb(nullptr),
          texcoord_cb(nullptr),
          index_cb(nullptr),
          usemtl_cb(nullptr),
          mtllib_cb(nullptr),
          group_cb(nullptr),
          object_cb(nullptr) {}
  };*/



// v2 api

  

  /*class reader{
  public:

    bool load_from_file(string_view filename, const loader_config &config = {});

    ///
    /// Parse .obj from a text string.
    /// Need to supply .mtl text string by `mtl_text`.
    /// This function ignores `mtllib` line in .obj text.
    ///
    /// @param[in] text wavefront .obj filename
    /// @param[in] mtl_text wavefront .mtl filename
    /// @param[in] config Reader configuration
    ///
    bool parse_from_string(string_view obj_text, string_view mtl_text, const loader_config &config = {});

    bool valid() const noexcept { return valid_; }

    const vertex_attributes& get_vertex_attributes() const noexcept { return attrib_; }
    span<const shape> get_shapes() const noexcept { return shapes_; }
    span<const material> get_materials() const noexcept { return materials_; }


    string_view warning() const noexcept { return warning_; }
    string_view error() const noexcept { return error_; }


  private:

    bool valid_ = false;

    vertex_attributes       attrib_;
    vector<shape>    shapes_;
    vector<material> materials_;

    string warning_;
    string error_;
  };*/


/// ==>>========= Legacy v1 api =============================================

/// Loads .obj from a file.
/// 'attrib', 'shapes' and 'materials' will be filled with parsed shape data
/// 'shapes' will be filled with parsed shape data
/// Returns true when loading .obj become success.
/// Returns warning message into `warn`, and error message into `err`
/// 'mtl_basedir' is optional, and used for base directory for .mtl file.
/// In default(`nullptr'), .mtl file is searched from an application's working
/// directory.
/// 'triangulate' is optional, and used whether triangulate polygon face in .obj
/// or not.
/// Option 'default_vcols_fallback' specifies whether vertex colors should
/// always be defined, even if no colors are given (fallback to white).
  bool load_object(vertex_attributes *attrib,
                   vector<shape> *shapes,
                   vector<material> *materials,
                   string *warn,
                   string *err,
                   string_view filename,
                   const loader_config& config = {});

  bool load_object(vertex_attributes *attrib,
                   vector<shape> *shapes,
                   vector<material> *materials,
                   string *warn,
                   string *err,
                   std::istream *in,
                   material_reader* read_material_fn = nullptr,
                   bool triangulate = true,
                   bool default_vcols_fallback = true);


/// Loads .obj from a file with custom user callback.
/// .mtl is loaded as usual and parsed material_t data will be passed to
/// `callback.mtllib_cb`.
/// Returns true when loading .obj/.mtl become success.
/// Returns warning message into `warn`, and error message into `err`
/// See `examples/callback_api/` for how to use this function.
  /*bool load_object(std::istream &inStream,
                   const callback_t &callback,
                           void *user_data = nullptr,
                           MaterialReader *readMatFn = nullptr,
                           std::string *warn = nullptr, std::string *err = nullptr);*/

/// Loads object from a std::istream, uses `readMatFn` to retrieve
/// std::istream for materials.
/// Returns true when loading .obj become success.
/// Returns warning and error message into `err`


/// Loads materials into std::map
  void load_material(dictionary<int> *material_map,
                     vector<material> *materials,
                     std::istream *inStream,
                     string *warning, 
                     string *err);

///
/// Parse texture name and texture option for custom texture parameter through
/// material::unknown_parameter
///
/// @param[out] texname Parsed texture name
/// @param[out] texopt Parsed texopt
/// @param[in] linebuf Input string
///
  bool parse_texture_name_and_option(string * texname, 
                                     texture_option *texopt,
                                     const char *linebuf);

/// =<<========== Legacy v1 api =============================================

}  // namespace valkyrie::graphics::engine



#endif//VALKYRIE_GRAPHICS_ENGINE_OBJ_HPP
