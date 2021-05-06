//
// Created by maxwe on 2021-04-03.
//

#include <valkyrie/graphics/engine/obj.hpp>


#include <cassert>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <limits>
#include <sstream>
#include <utility>


#ifdef __clang__
#pragma clang diagnostic push
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif

#pragma clang diagnostic ignored "-Wpadded"
#endif

using namespace valkyrie;
namespace engine   = graphics::engine;
namespace obj      = engine::obj;

#pragma region Illumination Models
const obj::illumination_model obj::illumination_models[11]{
    {
        // [0]
        .diffuse     = false,
        .specular    = false,
        .reflection  = false,
        .fresnel     = false,
        .refraction  = false,
        .ray_tracing = false,
        .shadowmatte = false
    },
    {
        // [1]
        .diffuse     = true,
        .specular    = false,
        .reflection  = false,
        .fresnel     = false,
        .refraction  = false,
        .ray_tracing = false,
        .shadowmatte = false
    },
    {
        // [2]
        .diffuse     = true,
        .specular    = true,
        .reflection  = false,
        .fresnel     = false,
        .refraction  = false,
        .ray_tracing = false,
        .shadowmatte = false
    },
    {
        // [3]
        .diffuse     = true,
        .specular    = true,
        .reflection  = true,
        .fresnel     = false,
        .refraction  = false,
        .ray_tracing = true,
        .shadowmatte = false
    },
    {
        // [4]
        .diffuse     = true,
        .specular    = true,
        .reflection  = true,
        .fresnel     = false,
        .refraction  = false,
        .ray_tracing = true,
        .shadowmatte = false
    },
    {
        // [5]
        .diffuse     = true,
        .specular    = true,
        .reflection  = true,
        .fresnel     = true,
        .refraction  = false,
        .ray_tracing = true,
        .shadowmatte = false
    },
    {
        // [6]
        .diffuse     = true,
        .specular    = true,
        .reflection  = true,
        .fresnel     = false,
        .refraction  = true,
        .ray_tracing = true,
        .shadowmatte = false
    },
    {
        // [7]
        .diffuse     = true,
        .specular    = true,
        .reflection  = true,
        .fresnel     = true,
        .refraction  = true,
        .ray_tracing = true,
        .shadowmatte = false
    },
    {
        // [8]
        .diffuse     = true,
        .specular    = true,
        .reflection  = true,
        .fresnel     = false,
        .refraction  = false,
        .ray_tracing = false,
        .shadowmatte = false
    },
    {
        // [9]
        .diffuse     = true,
        .specular    = true,
        .reflection  = true,
        .fresnel     = false,
        .refraction  = false,
        .ray_tracing = false,
        .shadowmatte = false
    },
    {
        // [10]
        .diffuse     = false,
        .specular    = false,
        .reflection  = false,
        .fresnel     = false,
        .refraction  = false,
        .ray_tracing = false,
        .shadowmatte = true
    }
};
#pragma endregion

#pragma region Implementation Details
namespace obj{
  namespace {
    struct vertex_index {
      int v_idx, vt_idx, vn_idx;
      vertex_index() : v_idx(-1), vt_idx(-1), vn_idx(-1) {}
      explicit vertex_index(int idx) : v_idx(idx), vt_idx(idx), vn_idx(idx) {}
      vertex_index(int vidx, int vtidx, int vnidx)
          : v_idx(vidx), vt_idx(vtidx), vn_idx(vnidx) {}
    };

// Internal data structure for face representation
// index + smoothing group.
    struct face {
      u32 smoothing_group_id;  // smoothing group id. 0 = smoothing groupd is off.
      // i32 pad_;
      vector<vertex_index> vertex_indices;  // face vertex indices.

      face() : smoothing_group_id(0), pad_(0) {}
    };

// Internal data structure for line representation
    struct obj_line_t {
      // In the specification, line primitrive does not have normal index, but
      // TinyObjLoader allow it
      std::vector<vertex_index> vertex_indices;
    };

// Internal data structure for points representation
    struct obj_points_t {
      // In the specification, point primitrive does not have normal index and
      // texture coord index, but TinyObjLoader allow it.
      std::vector<vertex_index> vertex_indices;
    };

    struct tag_sizes {
      tag_sizes() : num_ints(0), num_reals(0), num_strings(0) {}
      i32 num_ints;
      i32 num_reals;
      i32 num_strings;
    };

    struct obj_shape {
      vector<f32> v;
      vector<f32> vn;
      vector<f32> vt;
    };

//
// Manages group of primitives(face, line, points, ...)
    struct primitive_group {
      vector<face> face_group;
      vector<obj_line_t> line_group;
      vector<obj_points_t> points_group;

      void clear() {
        face_group.clear();
        line_group.clear();
        points_group.clear();
      }

      bool is_empty() const {
        return face_group.empty() && line_group.empty() && points_group.empty();
      }

      // TODO(syoyo): bspline, surface, ...
    };


    // See
// http://stackoverflow.com/questions/6089231/getting-std-ifstream-to-handle-lf-cr-and-crlf
    std::istream &safe_get_line(std::istream &is, string &t) {
      t.clear();

      // The characters in the stream are read one-by-one using a std::streambuf.
      // That is faster than reading them one-by-one using the std::istream.
      // code that uses streambuf this way must be guarded by a sentry object.
      // The sentry object performs various tasks,
      // such as thread synchronization and updating the stream state.

      std::istream::sentry se(is, true);
      std::streambuf *sb = is.rdbuf();

      if (se) {
        for (;;) {
          int c = sb->sbumpc();
          switch (c) {
            case '\n':
              return is;
            case '\r':
              if (sb->sgetc() == '\n') sb->sbumpc();
              return is;
            case EOF:
              // Also handle the case when the last line has no line ending
              if (t.empty()) is.setstate(std::ios::eofbit);
              return is;
            default:
              t += static_cast<char>(c);
          }
        }
      }

      return is;
    }

#define IS_SPACE(x) (((x) == ' ') || ((x) == '\t'))
#define IS_DIGIT(x) \
  (static_cast<unsigned int>((x) - '0') < static_cast<unsigned int>(10))
#define IS_NEW_LINE(x) (((x) == '\r') || ((x) == '\n') || ((x) == '\0'))


    // Make index zero-base, and also support relative index.
    inline bool fix_index(i32 idx, i32 n, i32 *ret) {
      if (!ret) {
        return false;
      }

      if (idx > 0) {
        (*ret) = idx - 1;
        return true;
      }

      if (idx == 0) {
        // zero is not allowed according to the spec.
        return false;
      }

      if (idx < 0) {
        (*ret) = n + idx;  // negative value = relative
        return true;
      }

      VK_unreachable;
      VK_assert(false);
      //return false;  // never reach here.
    }

    inline string parse_string(const char **token) {
      string s;
      (*token) += strspn((*token), " \t");
      size_t e = strcspn((*token), " \t\r");
      s = std::string((*token), &(*token)[e]);
      (*token) += e;
      return s;
    }
    inline i32    parse_int(const char **token) {
      (*token) += strspn((*token), " \t");
      int i = atoi((*token));
      (*token) += strcspn((*token), " \t\r");
      return i;
    }

    /*
     * Tries to parse a floating point number located at s.
     * s_end should be a location in the string where reading should absolutely
     * stop. For example at the end of the string, to prevent buffer overflows.
     * 
     * Parses the following EBNF grammar:
     *   sign    = "+" | "-" ;
     *   END     = ? anything not in digit ?
     *   digit   = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
     *   integer = [sign] , digit , {digit} ;
     *   decimal = integer , ["." , integer] ;
     *   float   = ( decimal , END ) | ( decimal , ("E" | "e") , integer , END ) ;
     *
     *  Valid strings are for example:
     *   -0  +3.1417e+2  -0.0E-3  1.0324  -1.41   11e2
     *
     * If the parsing is a success, result is set to the parsed value and true
     * is returned.
     *
     * The function is greedy and will parse until any of the following happens:
     *  - a non-conforming character is encountered.
     *  - s_end is reached.
     *
     * The following situations triggers a failure:
     *  - s >= s_end.
     *  - parse failure.
     *
     */
    bool try_parse_f64(const char *s, const char *s_end, f64 *result) {
      if (s >= s_end) {
        return false;
      }

      f64 mantissa = 0.0;
      // This exponent is base 2 rather than 10.
      // However the exponent we parse is supposed to be one of ten,
      // thus we must take care to convert the exponent/and or the
      // mantissa to a * 2^E, where a is the mantissa and E is the
      // exponent.
      // To get the final f64 we will use ldexp, it requires the
      // exponent to be in base 2.
      int exponent = 0;

      // NOTE: THESE MUST BE DECLARED HERE SINCE WE ARE NOT ALLOWED
      // TO JUMP OVER DEFINITIONS.
      char sign = '+';
      char exp_sign = '+';
      char const *curr = s;

      // How many characters were read in a loop.
      int read = 0;
      // Tells whether a loop terminated due to reaching s_end.
      bool end_not_reached = false;
      bool leading_decimal_dots = false;


      // Find out what sign we've got.
      if (*curr == '+' || *curr == '-') {
        sign = *curr;
        curr++;
        if ((curr != s_end) && (*curr == '.')) {
          // accept. Somethig like `.7e+2`, `-.5234`
          leading_decimal_dots = true;
        }
      } else if (IS_DIGIT(*curr)) {
      } else if (*curr == '.') {
        // accept. Somethig like `.7e+2`, `-.5234`
        leading_decimal_dots = true;
      } else {
        goto fail;
      }

      // Read the integer part.
      end_not_reached = (curr != s_end);
      if (!leading_decimal_dots) {
        while (end_not_reached && IS_DIGIT(*curr)) {
          mantissa *= 10;
          mantissa += static_cast<int>(*curr - 0x30);
          curr++;
          read++;
          end_not_reached = (curr != s_end);
        }

        // We must make sure we actually got something.
        if (read == 0) goto fail;
      }

      // We allow numbers of form "#", "###" etc.
      if (!end_not_reached) 
        goto assemble;

      // Read the decimal part.
      if (*curr == '.') {
        curr++;
        read = 1;
        end_not_reached = (curr != s_end);
        while (end_not_reached && IS_DIGIT(*curr)) {
          static const f64 pow_lut[] = {
              1.0, 0.1, 0.01, 0.001, 0.0001, 0.00001, 0.000001, 0.0000001,
          };
          const int lut_entries = sizeof pow_lut / sizeof pow_lut[0];

          // NOTE: Don't use powf here, it will absolutely murder precision.
          mantissa += static_cast<int>(*curr - 0x30) *
                      (read < lut_entries ? pow_lut[read] : std::pow(10.0, -read));
          read++;
          curr++;
          end_not_reached = (curr != s_end);
        }
      } else if (*curr == 'e' || *curr == 'E') {
      } else {
        goto assemble;
      }

      if (!end_not_reached) 
        goto assemble;

      // Read the exponent part.
      if (*curr == 'e' || *curr == 'E') {
        curr++;
        // Figure out if a sign is present and if it is.
        end_not_reached = (curr != s_end);
        if (end_not_reached && (*curr == '+' || *curr == '-')) {
          exp_sign = *curr;
          curr++;
        } else if (IS_DIGIT(*curr)) {
        } else {
          // Empty E is not allowed.
          goto fail;
        }

        read = 0;
        end_not_reached = (curr != s_end);
        while (end_not_reached && IS_DIGIT(*curr)) {
          exponent *= 10;
          exponent += static_cast<int>(*curr - 0x30);
          curr++;
          read++;
          end_not_reached = (curr != s_end);
        }
        exponent *= (exp_sign == '+' ? 1 : -1);
        if (read == 0) goto fail;
      }

      assemble:
      *result = (sign == '+' ? 1 : -1) *
                (exponent ? std::ldexp(mantissa * std::pow(5.0, exponent), exponent)
                          : mantissa);
      return true;
      fail:
        return false;
    }
    
    inline f32  parse_f32(const char **token, f64 default_value = 0.0) {
      (*token) += strspn((*token), " \t");
      const char *end = (*token) + strcspn((*token), " \t\r");
      f64 val = default_value;
      try_parse_f64((*token), end, &val);
      f32 f = static_cast<f32>(val);
      (*token) = end;
      return f;
    }
    inline bool parse_f32(const char **token, f32 *out) {
      (*token) += strspn((*token), " \t");
      const char *end = (*token) + strcspn((*token), " \t\r");
      f64 val;
      bool ret = try_parse_f64((*token), end, &val);
      if (ret) {
        f32 f = static_cast<f32>(val);
        (*out) = f;
      }
      (*token) = end;
      return ret;
    }

    inline void parse_f32_x2(f32 *x, f32 *y, const char **token,
                                  const f64 default_x = 0.0,
                                  const f64 default_y = 0.0) {
      (*x) = parse_f32(token, default_x);
      (*y) = parse_f32(token, default_y);
    }

    inline void parse_f32_x3(f32 *x, f32 *y, f32 *z,
                                  const char **token, const f64 default_x = 0.0,
                                  const f64 default_y = 0.0,
                                  const f64 default_z = 0.0) {
      (*x) = parse_f32(token, default_x);
      (*y) = parse_f32(token, default_y);
      (*z) = parse_f32(token, default_z);
    }

    inline void parse_f32_x4(f32 *x, f32 *y, f32 *z, f32 *w,
                              const char **token, const f64 default_x = 0.0,
                              const f64 default_y = 0.0,
                              const f64 default_z = 0.0,
                              const f64 default_w = 1.0) {
      (*x) = parse_f32(token, default_x);
      (*y) = parse_f32(token, default_y);
      (*z) = parse_f32(token, default_z);
      (*w) = parse_f32(token, default_w);
    }


    // Extension: parse vertex with colors(6 items)
    inline bool parse_vertex_w_color(f32 *x, f32 *y, f32 *z,
                                            f32 *r, f32 *g, f32 *b,
                                            const char **token,
                                            const f64 default_x = 0.0,
                                            const f64 default_y = 0.0,
                                            const f64 default_z = 0.0) {
      (*x) = parse_f32(token, default_x);
      (*y) = parse_f32(token, default_y);
      (*z) = parse_f32(token, default_z);

      const bool found_color =
          parse_f32(token, r) && parse_f32(token, g) && parse_f32(token, b);

      if (!found_color) {
        (*r) = (*g) = (*b) = 1.0;
      }

      return found_color;
    }

    inline bool parse_on_off(const char **token, bool default_value = true) {
      (*token) += strspn((*token), " \t");
      const char *end = (*token) + strcspn((*token), " \t\r");

      bool ret = default_value;
      if ((0 == strncmp((*token), "on", 2))) {
        ret = true;
      } else if ((0 == strncmp((*token), "off", 3))) {
        ret = false;
      }

      (*token) = end;
      return ret;
    }

    inline texture_type parse_texture_type(const char **token, texture_type default_value = texture_type::none) {
      (*token) += strspn((*token), " \t");
      const char *end = (*token) + strcspn((*token), " \t\r");
      texture_type ty = default_value;

      if ((0 == strncmp((*token), "cube_top", strlen("cube_top")))) {
        ty = texture_type::cube_top;
      } else if ((0 == strncmp((*token), "cube_bottom", strlen("cube_bottom")))) {
        ty = texture_type::cube_bottom;
      } else if ((0 == strncmp((*token), "cube_left", strlen("cube_left")))) {
        ty = texture_type::cube_left;
      } else if ((0 == strncmp((*token), "cube_right", strlen("cube_right")))) {
        ty = texture_type::cube_right;
      } else if ((0 == strncmp((*token), "cube_front", strlen("cube_front")))) {
        ty = texture_type::cube_front;
      } else if ((0 == strncmp((*token), "cube_back", strlen("cube_back")))) {
        ty = texture_type::cube_back;
      } else if ((0 == strncmp((*token), "sphere", strlen("sphere")))) {
        ty = texture_type::sphere;
      }

      (*token) = end;
      return ty;
    }

    tag_sizes parse_tag_triple(const char **token) {
      tag_sizes ts;

      (*token) += strspn((*token), " \t");
      ts.num_ints = atoi((*token));
      (*token) += strcspn((*token), "/ \t\r");
      if ((*token)[0] != '/') {
        return ts;
      }

      (*token)++;  // Skip '/'

      (*token) += strspn((*token), " \t");
      ts.num_reals = atoi((*token));
      (*token) += strcspn((*token), "/ \t\r");
      if ((*token)[0] != '/') {
        return ts;
      }
      (*token)++;  // Skip '/'

      ts.num_strings = parse_int(token);

      return ts;
    }

// Parse triples with index offsets: i, i/j/k, i//k, i/j
    bool parse_triple(const char **token, 
                      int vsize, 
                      int vnsize, 
                      int vtsize,
                      vertex_index *ret) {
      if (!ret) {
        return false;
      }

      vertex_index vi(-1);

      if (!fix_index(atoi((*token)), vsize, &(vi.v_idx))) {
        return false;
      }

      (*token) += strcspn((*token), "/ \t\r");
      if ((*token)[0] != '/') {
        (*ret) = vi;
        return true;
      }
      (*token)++;

      // i//k
      if ((*token)[0] == '/') {
        (*token)++;
        if (!fix_index(atoi((*token)), vnsize, &(vi.vn_idx))) {
          return false;
        }
        (*token) += strcspn((*token), "/ \t\r");
        (*ret) = vi;
        return true;
      }

      // i/j/k or i/j
      if (!fix_index(atoi((*token)), vtsize, &(vi.vt_idx))) {
        return false;
      }

      (*token) += strcspn((*token), "/ \t\r");
      if ((*token)[0] != '/') {
        (*ret) = vi;
        return true;
      }

      // i/j/k
      (*token)++;  // skip '/'
      if (!fix_index(atoi((*token)), vnsize, &(vi.vn_idx))) {
        return false;
      }
      (*token) += strcspn((*token), "/ \t\r");

      (*ret) = vi;

      return true;
    }

// Parse raw triples: i, i/j/k, i//k, i/j
    vertex_index parse_raw_triple(const char **token) {
      vertex_index vi(static_cast<int>(0));  // 0 is an invalid index in OBJ

      vi.v_idx = atoi((*token));
      (*token) += strcspn((*token), "/ \t\r");
      if ((*token)[0] != '/') {
        return vi;
      }
      (*token)++;

      // i//k
      if ((*token)[0] == '/') {
        (*token)++;
        vi.vn_idx = atoi((*token));
        (*token) += strcspn((*token), "/ \t\r");
        return vi;
      }

      // i/j/k or i/j
      vi.vt_idx = atoi((*token));
      (*token) += strcspn((*token), "/ \t\r");
      if ((*token)[0] != '/') {
        return vi;
      }

      // i/j/k
      (*token)++;  // skip '/'
      vi.vn_idx = atoi((*token));
      (*token) += strcspn((*token), "/ \t\r");
      return vi;
    }

    
    void InitTexOpt(texture_option_t *texopt, const bool is_bump) {
      if (is_bump) {
        texopt->imfchan = 'l';
      } else {
        texopt->imfchan = 'm';
      }
      texopt->bump_multiplier = static_cast<f32>(1.0);
      texopt->clamp = false;
      texopt->blendu = true;
      texopt->blendv = true;
      texopt->sharpness = static_cast<f32>(1.0);
      texopt->brightness = static_cast<f32>(0.0);
      texopt->contrast = static_cast<f32>(1.0);
      texopt->origin_offset[0] = static_cast<f32>(0.0);
      texopt->origin_offset[1] = static_cast<f32>(0.0);
      texopt->origin_offset[2] = static_cast<f32>(0.0);
      texopt->scale[0] = static_cast<f32>(1.0);
      texopt->scale[1] = static_cast<f32>(1.0);
      texopt->scale[2] = static_cast<f32>(1.0);
      texopt->turbulence[0] = static_cast<f32>(0.0);
      texopt->turbulence[1] = static_cast<f32>(0.0);
      texopt->turbulence[2] = static_cast<f32>(0.0);
      texopt->texture_resolution = -1;
      texopt->type = texture_type::none;
    }
    void InitMaterial(material_t *material) {
      InitTexOpt(&material->ambient_texopt,  false);
      InitTexOpt(&material->diffuse_texopt,  false);
      InitTexOpt(&material->specular_texopt,  false);
      InitTexOpt(&material->specular_highlight_texopt,  false);
      InitTexOpt(&material->bump_texopt,  true);
      InitTexOpt(&material->displacement_texopt,  false);
      InitTexOpt(&material->alpha_texopt,  false);
      InitTexOpt(&material->reflection_texopt,  false);
      InitTexOpt(&material->roughness_texopt,  false);
      InitTexOpt(&material->metallic_texopt,  false);
      InitTexOpt(&material->sheen_texopt,  false);
      InitTexOpt(&material->emissive_texopt,  false);
      InitTexOpt(&material->normal_texopt,
                 false);  // @fixme { is_bump will be true? }
      material->name = "";
      material->ambient_texname = "";
      material->diffuse_texname = "";
      material->specular_texname = "";
      material->specular_highlight_texname = "";
      material->bump_texname = "";
      material->displacement_texname = "";
      material->reflection_texname = "";
      material->alpha_texname = "";
      for (int i = 0; i < 3; i++) {
        material->ambient[i] = static_cast<f32>(0.0);
        material->diffuse[i] = static_cast<f32>(0.0);
        material->specular[i] = static_cast<f32>(0.0);
        material->transmittance[i] = static_cast<f32>(0.0);
        material->emission[i] = static_cast<f32>(0.0);
      }
      material->illum = 0;
      material->dissolve = static_cast<f32>(1.0);
      material->shininess = static_cast<f32>(1.0);
      material->ior = static_cast<f32>(1.0);

      material->roughness = static_cast<f32>(0.0);
      material->metallic = static_cast<f32>(0.0);
      material->sheen = static_cast<f32>(0.0);
      material->clearcoat_thickness = static_cast<f32>(0.0);
      material->clearcoat_roughness = static_cast<f32>(0.0);
      material->anisotropy_rotation = static_cast<f32>(0.0);
      material->anisotropy = static_cast<f32>(0.0);
      material->roughness_texname = "";
      material->metallic_texname = "";
      material->sheen_texname = "";
      material->emissive_texname = "";
      material->normal_texname = "";

      material->unknown_parameter.clear();
    }

// code from https://wrf.ecse.rpi.edu//Research/Short_Notes/pnpoly.html
    template <typename T>
    i32 pnpoly(int nvert, T *vertx, T *verty, T testx, T testy) {
      int i, j, c = 0;
      for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        if (((verty[i] > testy) != (verty[j] > testy)) &&
            (testx <
             (vertx[j] - vertx[i]) * (testy - verty[i]) / (verty[j] - verty[i]) +
             vertx[i]))
          c = !c;
      }
      return c;
    }


    // TODO(syoyo): refactor function.
    bool export_groups_to_shape(shape *shape, 
                             const primitive_group &prim_group,
                             span<const tag> tags,
                             const i32 material_id, 
                             string_view name,
                             bool triangulate,
                             span<const f32> v) {
      if (prim_group.is_empty()) {
        return false;
      }

      shape->name = name;

      // polygon
      if (!prim_group.face_group.empty()) {
        // Flatten vertices and indices
        for (size_t i = 0; i < prim_group.face_group.size(); i++) {
          const face &face = prim_group.face_group[i];

          size_t npolys = face.vertex_indices.size();

          if (npolys < 3) {
            // Face must have 3+ vertices.
            continue;
          }

          vertex_index i0 = face.vertex_indices[0];
          vertex_index i1(-1);
          vertex_index i2 = face.vertex_indices[1];

          if (triangulate) {
            // find the two axes to work in
            size_t axes[2] = {1, 2};
            for (size_t k = 0; k < npolys; ++k) {
              i0 = face.vertex_indices[(k + 0) % npolys];
              i1 = face.vertex_indices[(k + 1) % npolys];
              i2 = face.vertex_indices[(k + 2) % npolys];
              size_t vi0 = size_t(i0.v_idx);
              size_t vi1 = size_t(i1.v_idx);
              size_t vi2 = size_t(i2.v_idx);

              if (((3 * vi0 + 2) >= v.size()) || ((3 * vi1 + 2) >= v.size()) ||
                  ((3 * vi2 + 2) >= v.size())) {
                // Invalid triangle.
                // FIXME(syoyo): Is it ok to simply skip this invalid triangle?
                continue;
              }
              f32 v0x = v[vi0 * 3 + 0];
              f32 v0y = v[vi0 * 3 + 1];
              f32 v0z = v[vi0 * 3 + 2];
              f32 v1x = v[vi1 * 3 + 0];
              f32 v1y = v[vi1 * 3 + 1];
              f32 v1z = v[vi1 * 3 + 2];
              f32 v2x = v[vi2 * 3 + 0];
              f32 v2y = v[vi2 * 3 + 1];
              f32 v2z = v[vi2 * 3 + 2];
              f32 e0x = v1x - v0x;
              f32 e0y = v1y - v0y;
              f32 e0z = v1z - v0z;
              f32 e1x = v2x - v1x;
              f32 e1y = v2y - v1y;
              f32 e1z = v2z - v1z;
              f32 cx = std::fabs(e0y * e1z - e0z * e1y);
              f32 cy = std::fabs(e0z * e1x - e0x * e1z);
              f32 cz = std::fabs(e0x * e1y - e0y * e1x);
              const f32 epsilon = std::numeric_limits<f32>::epsilon();
              if (cx > epsilon || cy > epsilon || cz > epsilon) {
                // found a corner
                if (cx > cy && cx > cz) {
                } else {
                  axes[0] = 0;
                  if (cz > cx && cz > cy) axes[1] = 1;
                }
                break;
              }
            }

            f32 area = 0;
            for (size_t k = 0; k < npolys; ++k) {
              i0 = face.vertex_indices[(k + 0) % npolys];
              i1 = face.vertex_indices[(k + 1) % npolys];
              size_t vi0 = size_t(i0.v_idx);
              size_t vi1 = size_t(i1.v_idx);
              if (((vi0 * 3 + axes[0]) >= v.size()) ||
                  ((vi0 * 3 + axes[1]) >= v.size()) ||
                  ((vi1 * 3 + axes[0]) >= v.size()) ||
                  ((vi1 * 3 + axes[1]) >= v.size())) {
                // Invalid index.
                continue;
              }
              f32 v0x = v[vi0 * 3 + axes[0]];
              f32 v0y = v[vi0 * 3 + axes[1]];
              f32 v1x = v[vi1 * 3 + axes[0]];
              f32 v1y = v[vi1 * 3 + axes[1]];
              area += (v0x * v1y - v0y * v1x) * static_cast<f32>(0.5);
            }

            face remainingFace = face;  // copy
            size_t guess_vert = 0;
            vertex_index ind[3];
            f32 vx[3];
            f32 vy[3];

            // How many iterations can we do without decreasing the remaining
            // vertices.
            size_t remainingIterations = face.vertex_indices.size();
            size_t previousRemainingVertices = remainingFace.vertex_indices.size();

            while (remainingFace.vertex_indices.size() > 3 &&
                   remainingIterations > 0) {
              npolys = remainingFace.vertex_indices.size();
              if (guess_vert >= npolys) {
                guess_vert -= npolys;
              }

              if (previousRemainingVertices != npolys) {
                // The number of remaining vertices decreased. Reset counters.
                previousRemainingVertices = npolys;
                remainingIterations = npolys;
              } else {
                // We didn't consume a vertex on previous iteration, reduce the
                // available iterations.
                remainingIterations--;
              }

              for (size_t k = 0; k < 3; k++) {
                ind[k] = remainingFace.vertex_indices[(guess_vert + k) % npolys];
                size_t vi = size_t(ind[k].v_idx);
                if (((vi * 3 + axes[0]) >= v.size()) ||
                    ((vi * 3 + axes[1]) >= v.size())) {
                  // ???
                  vx[k] = static_cast<f32>(0.0);
                  vy[k] = static_cast<f32>(0.0);
                } else {
                  vx[k] = v[vi * 3 + axes[0]];
                  vy[k] = v[vi * 3 + axes[1]];
                }
              }
              f32 e0x = vx[1] - vx[0];
              f32 e0y = vy[1] - vy[0];
              f32 e1x = vx[2] - vx[1];
              f32 e1y = vy[2] - vy[1];
              f32 cross = e0x * e1y - e0y * e1x;
              // if an internal angle
              if (cross * area < static_cast<f32>(0.0)) {
                guess_vert += 1;
                continue;
              }

              // check all other verts in case they are inside this triangle
              bool overlap = false;
              for (size_t otherVert = 3; otherVert < npolys; ++otherVert) {
                size_t idx = (guess_vert + otherVert) % npolys;

                if (idx >= remainingFace.vertex_indices.size()) {
                  // ???
                  continue;
                }

                size_t ovi = size_t(remainingFace.vertex_indices[idx].v_idx);

                if (((ovi * 3 + axes[0]) >= v.size()) ||
                    ((ovi * 3 + axes[1]) >= v.size())) {
                  // ???
                  continue;
                }
                f32 tx = v[ovi * 3 + axes[0]];
                f32 ty = v[ovi * 3 + axes[1]];
                if (pnpoly(3, vx, vy, tx, ty)) {
                  overlap = true;
                  break;
                }
              }

              if (overlap) {
                guess_vert += 1;
                continue;
              }

              // this triangle is an ear
              {
                index_t idx0, idx1, idx2;
                idx0.vertex_index = ind[0].v_idx;
                idx0.normal_index = ind[0].vn_idx;
                idx0.texcoord_index = ind[0].vt_idx;
                idx1.vertex_index = ind[1].v_idx;
                idx1.normal_index = ind[1].vn_idx;
                idx1.texcoord_index = ind[1].vt_idx;
                idx2.vertex_index = ind[2].v_idx;
                idx2.normal_index = ind[2].vn_idx;
                idx2.texcoord_index = ind[2].vt_idx;

                shape->mesh.indices.push_back(idx0);
                shape->mesh.indices.push_back(idx1);
                shape->mesh.indices.push_back(idx2);

                shape->mesh.num_face_vertices.push_back(3);
                shape->mesh.material_ids.push_back(material_id);
                shape->mesh.smoothing_group_ids.push_back(face.smoothing_group_id);
              }

              // remove v1 from the list
              size_t removed_vert_index = (guess_vert + 1) % npolys;
              while (removed_vert_index + 1 < npolys) {
                remainingFace.vertex_indices[removed_vert_index] =
                    remainingFace.vertex_indices[removed_vert_index + 1];
                removed_vert_index += 1;
              }
              remainingFace.vertex_indices.pop_back();
            }

            if (remainingFace.vertex_indices.size() == 3) {
              i0 = remainingFace.vertex_indices[0];
              i1 = remainingFace.vertex_indices[1];
              i2 = remainingFace.vertex_indices[2];
              {
                index_t idx0, idx1, idx2;
                idx0.vertex_index = i0.v_idx;
                idx0.normal_index = i0.vn_idx;
                idx0.texcoord_index = i0.vt_idx;
                idx1.vertex_index = i1.v_idx;
                idx1.normal_index = i1.vn_idx;
                idx1.texcoord_index = i1.vt_idx;
                idx2.vertex_index = i2.v_idx;
                idx2.normal_index = i2.vn_idx;
                idx2.texcoord_index = i2.vt_idx;

                shape->mesh.indices.push_back(idx0);
                shape->mesh.indices.push_back(idx1);
                shape->mesh.indices.push_back(idx2);

                shape->mesh.num_face_vertices.push_back(3);
                shape->mesh.material_ids.push_back(material_id);
                shape->mesh.smoothing_group_ids.push_back(face.smoothing_group_id);
              }
            }
          } else {
            for (size_t k = 0; k < npolys; k++) {
              index_t idx;
              idx.vertex_index = face.vertex_indices[k].v_idx;
              idx.normal_index = face.vertex_indices[k].vn_idx;
              idx.texcoord_index = face.vertex_indices[k].vt_idx;
              shape->mesh.indices.push_back(idx);
            }

            shape->mesh.num_face_vertices.push_back(
                static_cast<unsigned char>(npolys));
            shape->mesh.material_ids.push_back(material_id);  // per face
            shape->mesh.smoothing_group_ids.push_back(
                face.smoothing_group_id);  // per face
          }
        }

        shape->mesh.tags = tags;
      }

      // line
      if (!prim_group.line_group.empty()) {
        // Flatten indices
        for (size_t i = 0; i < prim_group.line_group.size(); i++) {
          for (size_t j = 0; j < prim_group.line_group[i].vertex_indices.size();
               j++) {
            const vertex_index &vi = prim_group.line_group[i].vertex_indices[j];

            index_t idx;
            idx.vertex_index = vi.v_idx;
            idx.normal_index = vi.vn_idx;
            idx.texcoord_index = vi.vt_idx;

            shape->lines.indices.push_back(idx);
          }

          shape->lines.num_line_vertices.push_back(
              int(prim_group.line_group[i].vertex_indices.size()));
        }
      }

      // points
      if (!prim_group.points_group.empty()) {
        // Flatten & convert indices
        for (size_t i = 0; i < prim_group.points_group.size(); i++) {
          for (size_t j = 0; j < prim_group.points_group[i].vertex_indices.size();
               j++) {
            const vertex_index &vi = prim_group.points_group[i].vertex_indices[j];

            index_t idx;
            idx.vertex_index = vi.v_idx;
            idx.normal_index = vi.vn_idx;
            idx.texcoord_index = vi.vt_idx;

            shape->points.indices.push_back(idx);
          }
        }
      }

      return true;
    }


    // Split a string with specified delimiter character and escape character.
// https://rosettacode.org/wiki/Tokenize_a_string_with_escaping#C.2B.2B
    void SplitString(string_view s,
                     char delim,
                     char escape,
                     vector<string_view> &elems) {
      std::string token;

      bool escaping = false;
      for (int i = 0; i < s.size(); ++i) {
        char ch = s[i];
        if (escaping) {
          escaping = false;
        } else if (ch == escape) {
          escaping = true;
          continue;
        } else if (ch == delim) {
          if (!token.empty()) {
            elems.push_back(token);
          }
          token.clear();
          continue;
        }
        token += ch;
      }

      elems.push_back(token);
    }

    string JoinPath(const std::string &dir,
                                const std::string &filename) {
      if (dir.empty()) {
        return filename;
      } else {
        // check '/'
        char lastChar = *dir.rbegin();
        if (lastChar != '/') {
          return dir + std::string("/") + filename;
        } else {
          return dir + filename;
        }
      }
    }

  }
}
#pragma endregion

bool obj::parse_texture_name_and_option(string *texname,
                                        texture_option *texopt,
                                        const char *linebuf) {
  // @todo { write more robust lexer and parser. }
  bool found_texname = false;
  std::string texture_name;

  const char *token = linebuf;  // Assume line ends with nullptr

  while (!IS_NEW_LINE((*token))) {
    token += strspn(token, " \t");  // skip space
    if ((0 == strncmp(token, "-blendu", 7)) && IS_SPACE((token[7]))) {
      token += 8;
      texopt->blendu = parse_on_off(&token,  true);
    } else if ((0 == strncmp(token, "-blendv", 7)) && IS_SPACE((token[7]))) {
      token += 8;
      texopt->blendv = parse_on_off(&token,  true);
    } else if ((0 == strncmp(token, "-clamp", 6)) && IS_SPACE((token[6]))) {
      token += 7;
      texopt->clamp = parse_on_off(&token,  true);
    } else if ((0 == strncmp(token, "-boost", 6)) && IS_SPACE((token[6]))) {
      token += 7;
      texopt->sharpness = parse_f32(&token, 1.0);
    } else if ((0 == strncmp(token, "-bm", 3)) && IS_SPACE((token[3]))) {
      token += 4;
      texopt->bump_multiplier = parse_f32(&token, 1.0);
    } else if ((0 == strncmp(token, "-o", 2)) && IS_SPACE((token[2]))) {
      token += 3;
      parse_f32_x3(&(texopt->origin_offset[0]), &(texopt->origin_offset[1]),
                   &(texopt->origin_offset[2]), &token);
    } else if ((0 == strncmp(token, "-s", 2)) && IS_SPACE((token[2]))) {
      token += 3;
      parse_f32_x3(&(texopt->scale[0]), &(texopt->scale[1]), &(texopt->scale[2]),
                   &token, 1.0, 1.0, 1.0);
    } else if ((0 == strncmp(token, "-t", 2)) && IS_SPACE((token[2]))) {
      token += 3;
      parse_f32_x3(&(texopt->turbulence[0]), &(texopt->turbulence[1]),
                   &(texopt->turbulence[2]), &token);
    } else if ((0 == strncmp(token, "-type", 5)) && IS_SPACE((token[5]))) {
      token += 5;
      texopt->type = parse_texture_type((&token), texture_type::none);
    } else if ((0 == strncmp(token, "-texres", 7)) && IS_SPACE((token[7]))) {
      token += 7;
      // TODO(syoyo): Check if arg is int type.
      texopt->texture_resolution = parse_int(&token);
    } else if ((0 == strncmp(token, "-imfchan", 8)) && IS_SPACE((token[8]))) {
      token += 9;
      token += strspn(token, " \t");
      const char *end = token + strcspn(token, " \t\r");
      if ((end - token) == 1) {  // Assume one char for -imfchan
        texopt->imfchan = (*token);
      }
      token = end;
    } else if ((0 == strncmp(token, "-mm", 3)) && IS_SPACE((token[3]))) {
      token += 4;
      parse_f32_x2(&(texopt->brightness), &(texopt->contrast), &token, 0.0, 1.0);
    } else if ((0 == strncmp(token, "-colorspace", 11)) &&
               IS_SPACE((token[11]))) {
      token += 12;
      texopt->colorspace = parse_string(&token);
    } else {
// Assume texture filename
#if 0
      size_t len = strcspn(token, " \t\r");  // untile next space
      texture_name = std::string(token, token + len);
      token += len;

      token += strspn(token, " \t");  // skip space
#else
      // Read filename until line end to parse filename containing whitespace
      // TODO(syoyo): Support parsing texture option flag after the filename.
      texture_name = std::string(token);
      token += texture_name.length();
#endif

      found_texname = true;
    }
  }

  if (found_texname) {
    (*texname) = texture_name;
    return true;
  } else {
    return false;
  }
}

void obj::load_material(dictionary<int> *material_map,
                        vector<material> *materials,
                        std::istream *inStream,
                        string *warning,
                        string *err) {
  (void)err;

  // Create a default material anyway.
  material_t material;
  InitMaterial(&material);

  // Issue 43. `d` wins against `Tr` since `Tr` is not in the MTL specification.
  bool has_d = false;
  bool has_tr = false;

  // has_kd is used to set a default diffuse value when map_Kd is present
  // and Kd is not.
  bool has_kd = false;

  std::stringstream warn_ss;

  size_t line_no = 0;
  std::string linebuf;
  while (inStream->peek() != -1) {
    safe_get_line(*inStream, linebuf);
    line_no++;

    // Trim trailing whitespace.
    if (linebuf.size() > 0) {
      linebuf = linebuf.substr(0, linebuf.find_last_not_of(" \t") + 1);
    }

    // Trim newline '\r\n' or '\n'
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\n')
        linebuf.erase(linebuf.size() - 1);
    }
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\r')
        linebuf.erase(linebuf.size() - 1);
    }

    // Skip if empty line.
    if (linebuf.empty()) {
      continue;
    }

    // Skip leading space.
    const char *token = linebuf.c_str();
    token += strspn(token, " \t");

    assert(token);
    if (token[0] == '\0') continue;  // empty line

    if (token[0] == '#') continue;  // comment line

    // new mtl
    if ((0 == strncmp(token, "newmtl", 6)) && IS_SPACE((token[6]))) {
      // flush previous material.
      if (!material.name.empty()) {
        material_map->insert(std::pair<std::string, int>(
            material.name, static_cast<int>(materials->size())));
        materials->push_back(material);
      }

      // initial temporary material
      InitMaterial(&material);

      has_d = false;
      has_tr = false;

      // set new mtl name
      token += 7;
      {
        std::stringstream sstr;
        sstr << token;
        material.name = sstr.str();
      }
      continue;
    }

    // ambient
    if (token[0] == 'K' && token[1] == 'a' && IS_SPACE((token[2]))) {
      token += 2;
      f32 r, g, b;
      parse_f32_x3(&r, &g, &b, &token);
      material.ambient[0] = r;
      material.ambient[1] = g;
      material.ambient[2] = b;
      continue;
    }

    // diffuse
    if (token[0] == 'K' && token[1] == 'd' && IS_SPACE((token[2]))) {
      token += 2;
      f32 r, g, b;
      parse_f32_x3(&r, &g, &b, &token);
      material.diffuse[0] = r;
      material.diffuse[1] = g;
      material.diffuse[2] = b;
      has_kd = true;
      continue;
    }

    // specular
    if (token[0] == 'K' && token[1] == 's' && IS_SPACE((token[2]))) {
      token += 2;
      f32 r, g, b;
      parse_f32_x3(&r, &g, &b, &token);
      material.specular[0] = r;
      material.specular[1] = g;
      material.specular[2] = b;
      continue;
    }

    // transmittance
    if ((token[0] == 'K' && token[1] == 't' && IS_SPACE((token[2]))) ||
        (token[0] == 'T' && token[1] == 'f' && IS_SPACE((token[2])))) {
      token += 2;
      f32 r, g, b;
      parse_f32_x3(&r, &g, &b, &token);
      material.transmittance[0] = r;
      material.transmittance[1] = g;
      material.transmittance[2] = b;
      continue;
    }

    // ior(index of refraction)
    if (token[0] == 'N' && token[1] == 'i' && IS_SPACE((token[2]))) {
      token += 2;
      material.ior = parse_f32(&token);
      continue;
    }

    // emission
    if (token[0] == 'K' && token[1] == 'e' && IS_SPACE(token[2])) {
      token += 2;
      f32 r, g, b;
      parse_f32_x3(&r, &g, &b, &token);
      material.emission[0] = r;
      material.emission[1] = g;
      material.emission[2] = b;
      continue;
    }

    // shininess
    if (token[0] == 'N' && token[1] == 's' && IS_SPACE(token[2])) {
      token += 2;
      material.shininess = parse_f32(&token);
      continue;
    }

    // illum model
    if (0 == strncmp(token, "illum", 5) && IS_SPACE(token[5])) {
      token += 6;
      material.illum = parse_int(&token);
      continue;
    }

    // dissolve
    if ((token[0] == 'd' && IS_SPACE(token[1]))) {
      token += 1;
      material.dissolve = parse_f32(&token);

      if (has_tr) {
        warn_ss << "Both `d` and `Tr` parameters defined for \""
                << material.name
                << "\". Use the value of `d` for dissolve (line " << line_no
                << " in .mtl.)" << std::endl;
      }
      has_d = true;
      continue;
    }
    if (token[0] == 'T' && token[1] == 'r' && IS_SPACE(token[2])) {
      token += 2;
      if (has_d) {
        // `d` wins. Ignore `Tr` value.
        warn_ss << "Both `d` and `Tr` parameters defined for \""
                << material.name
                << "\". Use the value of `d` for dissolve (line " << line_no
                << " in .mtl.)" << std::endl;
      } else {
        // We invert value of Tr(assume Tr is in range [0, 1])
        // NOTE: Interpretation of Tr is application(exporter) dependent. For
        // some application(e.g. 3ds max obj exporter), Tr = d(Issue 43)
        material.dissolve = static_cast<f32>(1.0) - parse_f32(&token);
      }
      has_tr = true;
      continue;
    }

    // PBR: roughness
    if (token[0] == 'P' && token[1] == 'r' && IS_SPACE(token[2])) {
      token += 2;
      material.roughness = parse_f32(&token);
      continue;
    }

    // PBR: metallic
    if (token[0] == 'P' && token[1] == 'm' && IS_SPACE(token[2])) {
      token += 2;
      material.metallic = parse_f32(&token);
      continue;
    }

    // PBR: sheen
    if (token[0] == 'P' && token[1] == 's' && IS_SPACE(token[2])) {
      token += 2;
      material.sheen = parse_f32(&token);
      continue;
    }

    // PBR: clearcoat thickness
    if (token[0] == 'P' && token[1] == 'c' && IS_SPACE(token[2])) {
      token += 2;
      material.clearcoat_thickness = parse_f32(&token);
      continue;
    }

    // PBR: clearcoat roughness
    if ((0 == strncmp(token, "Pcr", 3)) && IS_SPACE(token[3])) {
      token += 4;
      material.clearcoat_roughness = parse_f32(&token);
      continue;
    }

    // PBR: anisotropy
    if ((0 == strncmp(token, "aniso", 5)) && IS_SPACE(token[5])) {
      token += 6;
      material.anisotropy = parse_f32(&token);
      continue;
    }

    // PBR: anisotropy rotation
    if ((0 == strncmp(token, "anisor", 6)) && IS_SPACE(token[6])) {
      token += 7;
      material.anisotropy_rotation = parse_f32(&token);
      continue;
    }

    // ambient texture
    if ((0 == strncmp(token, "map_Ka", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.ambient_texname),
                                &(material.ambient_texopt), token);
      continue;
    }

    // diffuse texture
    if ((0 == strncmp(token, "map_Kd", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.diffuse_texname),
                                &(material.diffuse_texopt), token);

      // Set a decent diffuse default value if a diffuse texture is specified
      // without a matching Kd value.
      if (!has_kd) {
        material.diffuse[0] = static_cast<f32>(0.6);
        material.diffuse[1] = static_cast<f32>(0.6);
        material.diffuse[2] = static_cast<f32>(0.6);
      }

      continue;
    }

    // specular texture
    if ((0 == strncmp(token, "map_Ks", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.specular_texname),
                                &(material.specular_texopt), token);
      continue;
    }

    // specular highlight texture
    if ((0 == strncmp(token, "map_Ns", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.specular_highlight_texname),
                                &(material.specular_highlight_texopt), token);
      continue;
    }

    // bump texture
    if ((0 == strncmp(token, "map_bump", 8)) && IS_SPACE(token[8])) {
      token += 9;
      ParseTextureNameAndOption(&(material.bump_texname),
                                &(material.bump_texopt), token);
      continue;
    }

    // bump texture
    if ((0 == strncmp(token, "map_Bump", 8)) && IS_SPACE(token[8])) {
      token += 9;
      ParseTextureNameAndOption(&(material.bump_texname),
                                &(material.bump_texopt), token);
      continue;
    }

    // bump texture
    if ((0 == strncmp(token, "bump", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(&(material.bump_texname),
                                &(material.bump_texopt), token);
      continue;
    }

    // alpha texture
    if ((0 == strncmp(token, "map_d", 5)) && IS_SPACE(token[5])) {
      token += 6;
      material.alpha_texname = token;
      ParseTextureNameAndOption(&(material.alpha_texname),
                                &(material.alpha_texopt), token);
      continue;
    }

    // displacement texture
    if ((0 == strncmp(token, "disp", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(&(material.displacement_texname),
                                &(material.displacement_texopt), token);
      continue;
    }

    // reflection map
    if ((0 == strncmp(token, "refl", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(&(material.reflection_texname),
                                &(material.reflection_texopt), token);
      continue;
    }

    // PBR: roughness texture
    if ((0 == strncmp(token, "map_Pr", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.roughness_texname),
                                &(material.roughness_texopt), token);
      continue;
    }

    // PBR: metallic texture
    if ((0 == strncmp(token, "map_Pm", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.metallic_texname),
                                &(material.metallic_texopt), token);
      continue;
    }

    // PBR: sheen texture
    if ((0 == strncmp(token, "map_Ps", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.sheen_texname),
                                &(material.sheen_texopt), token);
      continue;
    }

    // PBR: emissive texture
    if ((0 == strncmp(token, "map_Ke", 6)) && IS_SPACE(token[6])) {
      token += 7;
      ParseTextureNameAndOption(&(material.emissive_texname),
                                &(material.emissive_texopt), token);
      continue;
    }

    // PBR: normal map texture
    if ((0 == strncmp(token, "norm", 4)) && IS_SPACE(token[4])) {
      token += 5;
      ParseTextureNameAndOption(&(material.normal_texname),
                                &(material.normal_texopt), token);
      continue;
    }

    // unknown parameter
    const char *_space = strchr(token, ' ');
    if (!_space) {
      _space = strchr(token, '\t');
    }
    if (_space) {
      ptrdiff_t len = _space - token;
      std::string key(token, static_cast<size_t>(len));
      std::string value = _space + 1;
      material.unknown_parameter.insert(
          std::pair<std::string, std::string>(key, value));
    }
  }
  // flush last material.
  material_map->insert(std::pair<std::string, int>(
      material.name, static_cast<int>(materials->size())));
  materials->push_back(material);

  if (warning) {
    (*warning) = warn_ss.str();
  }
}


bool obj::material_file_reader::load(string_view matId,
                                     vector<material> *mat,
                                     dictionary<int> *matMap,
                                     string *warn,
                                     string *err){
  if (!m_mtlBaseDir.empty()) {
#ifdef _WIN32
    char sep = ';';
#else
    char sep = ':';
#endif

    // https://stackoverflow.com/questions/5167625/splitting-a-c-stdstring-using-tokens-e-g
    std::vector<std::string> paths;
    std::istringstream f(m_mtlBaseDir);

    std::string s;
    while (getline(f, s, sep)) {
      paths.push_back(s);
    }

    for (size_t i = 0; i < paths.size(); i++) {
      std::string filepath = JoinPath(paths[i], matId);

      std::ifstream matIStream(filepath.c_str());
      if (matIStream) {
        LoadMtl(matMap, materials, &matIStream, warn, err);

        return true;
      }
    }

    std::stringstream ss;
    ss << "Material file [ " << matId
       << " ] not found in a path : " << m_mtlBaseDir << std::endl;
    if (warn) {
      (*warn) += ss.str();
    }
    return false;

  } else {
    std::string filepath = matId;
    std::ifstream matIStream(filepath.c_str());
    if (matIStream) {
      LoadMtl(matMap, materials, &matIStream, warn, err);

      return true;
    }

    std::stringstream ss;
    ss << "Material file [ " << filepath
       << " ] not found in a path : " << m_mtlBaseDir << std::endl;
    if (warn) {
      (*warn) += ss.str();
    }

    return false;
  }
}

bool obj::material_stream_reader::load(string_view matId,
                                       vector<material> *mat,
                                       dictionary<int> *matMap,
                                       string *warn,
                                       string *err){
  (void)err;
  (void)matId;
  if (!m_inStream) {
    std::stringstream ss;
    ss << "Material stream in error state. " << std::endl;
    if (warn) {
      (*warn) += ss.str();
    }
    return false;
  }

  LoadMtl(matMap, materials, &m_inStream, warn, err);

  return true;
}


bool obj::load_object(vertex_attributes *attrib,
                      vector<shape> *shapes,
                      vector<material> *materials,
                      string *warn,
                      string *err,
                      string_view filename,
                      const loader_config &config){

  /*{
  std::string mtl_search_path;

  if (config.mtl_search_path.empty()) {
    filename.stem();
  }
  else {
    mtl_search_path = config.mtl_search_path;
  }

  valid_ = LoadObj(&attrib_,
                   &shapes_, &materials_, &warning_, &error_,
                   filename.c_str(), mtl_search_path.c_str(),
                   config.triangulate, config.vertex_color);

  return valid_;
}*/


  attrib->vertices.clear();
  attrib->normals.clear();
  attrib->texcoords.clear();
  attrib->colors.clear();
  shapes->clear();

  std::stringstream errss;

  std::ifstream ifs(filename);
  if (!ifs) {
    errss << "Cannot open file [" << filename << "]" << std::endl;
    if (err) {
      (*err) = errss.str();
    }
    return false;
  }

  /*{
  std::string mtl_search_path;

  if (config.mtl_search_path.empty()) {
    filename.stem();
  }
  else {
    mtl_search_path = config.mtl_search_path;
  }

  valid_ = LoadObj(&attrib_,
                   &shapes_, &materials_, &warning_, &error_,
                   filename.c_str(), mtl_search_path.c_str(),
                   config.triangulate, config.vertex_color);

  return valid_;
}*/

  material_file_reader mat_reader(config.material_search_path);

  return obj::load_object(attrib,
                          shapes,
                          materials,
                          warn,
                          err,
                          &ifs,
                          &mat_reader,
                          trianglulate,
                          default_vcols_fallback);
}


bool obj::load_object(vertex_attributes *attrib,
                      vector<shape> *shapes,
                      vector<material> *materials,
                      string *warn,
                      string *err,
                      std::istream *in,
                      material_reader *read_material_fn,
                      bool triangulate,
                      bool default_vcols_fallback) {
  std::stringstream errss;

  std::vector<f32> v;
  std::vector<f32> vn;
  std::vector<f32> vt;
  std::vector<f32> vc;
  std::vector<skin_weight_t> vw;
  std::vector<tag_t> tags;
  primitive_group prim_group;
  std::string name;

  // material
  std::map<std::string, int> material_map;
  int material = -1;

  // smoothing group id
  unsigned int current_smoothing_id =
      0;  // Initial value. 0 means no smoothing.

  int greatest_v_idx = -1;
  int greatest_vn_idx = -1;
  int greatest_vt_idx = -1;

  shape_t shape;

  bool found_all_colors = true;

  size_t line_num = 0;
  std::string linebuf;
  while (inStream->peek() != -1) {
    safe_get_line(*inStream, linebuf);

    line_num++;

    // Trim newline '\r\n' or '\n'
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\n')
        linebuf.erase(linebuf.size() - 1);
    }
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\r')
        linebuf.erase(linebuf.size() - 1);
    }

    // Skip if empty line.
    if (linebuf.empty()) {
      continue;
    }

    // Skip leading space.
    const char *token = linebuf.c_str();
    token += strspn(token, " \t");

    assert(token);
    if (token[0] == '\0') continue;  // empty line

    if (token[0] == '#') continue;  // comment line

    // vertex
    if (token[0] == 'v' && IS_SPACE((token[1]))) {
      token += 2;
      f32 x, y, z;
      f32 r, g, b;

      found_all_colors &= parse_vertex_w_color(&x, &y, &z, &r, &g, &b, &token);

      v.push_back(x);
      v.push_back(y);
      v.push_back(z);

      if (found_all_colors || default_vcols_fallback) {
        vc.push_back(r);
        vc.push_back(g);
        vc.push_back(b);
      }

      continue;
    }

    // normal
    if (token[0] == 'v' && token[1] == 'n' && IS_SPACE((token[2]))) {
      token += 3;
      f32 x, y, z;
      parse_f32_x3(&x, &y, &z, &token);
      vn.push_back(x);
      vn.push_back(y);
      vn.push_back(z);
      continue;
    }

    // texcoord
    if (token[0] == 'v' && token[1] == 't' && IS_SPACE((token[2]))) {
      token += 3;
      f32 x, y;
      parse_f32_x2(&x, &y, &token);
      vt.push_back(x);
      vt.push_back(y);
      continue;
    }

    // skin weight. tinyobj extension
    if (token[0] == 'v' && token[1] == 'w' && IS_SPACE((token[2]))) {
      token += 3;

      // vw <vid> <joint_0> <weight_0> <joint_1> <weight_1> ...
      // example:
      // vw 0 0 0.25 1 0.25 2 0.5

      // TODO(syoyo): Add syntax check
      int vid = 0;
      vid = parse_int(&token);

      skin_weight_t sw;

      sw.vertex_id = vid;

      while (!IS_NEW_LINE(token[0])) {
        f32 j, w;
        // joint_id should not be negative, weight may be negative
        // TODO(syoyo): # of elements check
        parse_f32_x2(&j, &w, &token, -1.0);

        if (j < 0.0) {
          if (err) {
            std::stringstream ss;
            ss << "Failed parse `vw' line. joint_id is negative. "
                  "line "
               << line_num << ".)\n";
            (*err) += ss.str();
          }
          return false;
        }

        joint_and_weight_t jw;

        jw.joint_id = int(j);
        jw.weight = w;

        sw.weightValues.push_back(jw);

        size_t n = strspn(token, " \t\r");
        token += n;
      }

      vw.push_back(sw);
    }

    // line
    if (token[0] == 'l' && IS_SPACE((token[1]))) {
      token += 2;

      obj_line_t line;

      while (!IS_NEW_LINE(token[0])) {
        vertex_index vi;
        if (!parse_triple(&token, static_cast<int>(v.size() / 3),
                          static_cast<int>(vn.size() / 3),
                          static_cast<int>(vt.size() / 2), &vi)) {
          if (err) {
            std::stringstream ss;
            ss << "Failed parse `l' line(e.g. zero value for vertex index. "
                  "line "
               << line_num << ".)\n";
            (*err) += ss.str();
          }
          return false;
        }

        line.vertex_indices.push_back(vi);

        size_t n = strspn(token, " \t\r");
        token += n;
      }

      prim_group.line_group.push_back(line);

      continue;
    }

    // points
    if (token[0] == 'p' && IS_SPACE((token[1]))) {
      token += 2;

      obj_points_t pts;

      while (!IS_NEW_LINE(token[0])) {
        vertex_index vi;
        if (!parse_triple(&token, static_cast<int>(v.size() / 3),
                          static_cast<int>(vn.size() / 3),
                          static_cast<int>(vt.size() / 2), &vi)) {
          if (err) {
            std::stringstream ss;
            ss << "Failed parse `p' line(e.g. zero value for vertex index. "
                  "line "
               << line_num << ".)\n";
            (*err) += ss.str();
          }
          return false;
        }

        pts.vertex_indices.push_back(vi);

        size_t n = strspn(token, " \t\r");
        token += n;
      }

      prim_group.points_group.push_back(pts);

      continue;
    }

    // face
    if (token[0] == 'f' && IS_SPACE((token[1]))) {
      token += 2;
      token += strspn(token, " \t");

      face face;

      face.smoothing_group_id = current_smoothing_id;
      face.vertex_indices.reserve(3);

      while (!IS_NEW_LINE(token[0])) {
        vertex_index vi;
        if (!parse_triple(&token, static_cast<int>(v.size() / 3),
                          static_cast<int>(vn.size() / 3),
                          static_cast<int>(vt.size() / 2), &vi)) {
          if (err) {
            std::stringstream ss;
            ss << "Failed parse `f' line(e.g. zero value for face index. line "
               << line_num << ".)\n";
            (*err) += ss.str();
          }
          return false;
        }

        greatest_v_idx = greatest_v_idx > vi.v_idx ? greatest_v_idx : vi.v_idx;
        greatest_vn_idx =
            greatest_vn_idx > vi.vn_idx ? greatest_vn_idx : vi.vn_idx;
        greatest_vt_idx =
            greatest_vt_idx > vi.vt_idx ? greatest_vt_idx : vi.vt_idx;

        face.vertex_indices.push_back(vi);
        size_t n = strspn(token, " \t\r");
        token += n;
      }

      // replace with emplace_back + std::move on C++11
      prim_group.face_group.push_back(face);

      continue;
    }

    // use mtl
    if ((0 == strncmp(token, "usemtl", 6))) {
      token += 6;
      std::string namebuf = parse_string(&token);

      int newMaterialId = -1;
      std::map<std::string, int>::const_iterator it = material_map.find(namebuf);
      if (it != material_map.end()) {
        newMaterialId = it->second;
      } else {
        // { error!! material not found }
        if (warn) {
          (*warn) += "material [ '" + namebuf + "' ] not found in .mtl\n";
        }
      }

      if (newMaterialId != material) {
        // Create per-face material. Thus we don't add `shape` to `shapes` at
        // this time.
        // just clear `face_group` after `export_groups_to_shape()` call.
        export_groups_to_shape(&shape, prim_group, tags, material, name,
                               triangulate, v);
        prim_group.face_group.clear();
        material = newMaterialId;
      }

      continue;
    }

    // load mtl
    if ((0 == strncmp(token, "mtllib", 6)) && IS_SPACE((token[6]))) {
      if (readMatFn) {
        token += 7;

        std::vector<std::string> filenames;
        SplitString(std::string(token), ' ', '\\', filenames);

        if (filenames.empty()) {
          if (warn) {
            std::stringstream ss;
            ss << "Looks like empty filename for mtllib. Use default "
                  "material (line "
               << line_num << ".)\n";

            (*warn) += ss.str();
          }
        } else {
          bool found = false;
          for (size_t s = 0; s < filenames.size(); s++) {
            std::string warn_mtl;
            std::string err_mtl;
            bool ok = (*readMatFn)(filenames[s].c_str(), materials,
                                   &material_map, &warn_mtl, &err_mtl);
            if (warn && (!warn_mtl.empty())) {
              (*warn) += warn_mtl;
            }

            if (err && (!err_mtl.empty())) {
              (*err) += err_mtl;
            }

            if (ok) {
              found = true;
              break;
            }
          }

          if (!found) {
            if (warn) {
              (*warn) +=
                  "Failed to load material file(s). Use default "
                  "material.\n";
            }
          }
        }
      }

      continue;
    }

    // group name
    if (token[0] == 'g' && IS_SPACE((token[1]))) {
      // flush previous face group.
      bool ret = export_groups_to_shape(&shape, prim_group, tags, material, name,
                                        triangulate, v);
      (void)ret;  // return value not used.

      if (shape.mesh.indices.size() > 0) {
        shapes->push_back(shape);
      }

      shape = shape_t();

      // material = -1;
      prim_group.clear();

      std::vector<std::string> names;

      while (!IS_NEW_LINE(token[0])) {
        std::string str = parse_string(&token);
        names.push_back(str);
        token += strspn(token, " \t\r");  // skip tag
      }

      // names[0] must be 'g'

      if (names.size() < 2) {
        // 'g' with empty names
        if (warn) {
          std::stringstream ss;
          ss << "Empty group name. line: " << line_num << "\n";
          (*warn) += ss.str();
          name = "";
        }
      } else {
        std::stringstream ss;
        ss << names[1];

        // tinyobjloader does not support multiple groups for a primitive.
        // Currently we concatinate multiple group names with a space to get
        // single group name.

        for (size_t i = 2; i < names.size(); i++) {
          ss << " " << names[i];
        }

        name = ss.str();
      }

      continue;
    }

    // object name
    if (token[0] == 'o' && IS_SPACE((token[1]))) {
      // flush previous face group.
      bool ret = export_groups_to_shape(&shape, prim_group, tags, material, name,
                                        triangulate, v);
      (void)ret;  // return value not used.

      if (shape.mesh.indices.size() > 0 || shape.lines.indices.size() > 0 ||
          shape.points.indices.size() > 0) {
        shapes->push_back(shape);
      }

      // material = -1;
      prim_group.clear();
      shape = shape_t();

      // @todo { multiple object name? }
      token += 2;
      std::stringstream ss;
      ss << token;
      name = ss.str();

      continue;
    }

    if (token[0] == 't' && IS_SPACE(token[1])) {
      const int max_tag_nums = 8192;  // FIXME(syoyo): Parameterize.
      tag_t tag;

      token += 2;

      tag.name = parse_string(&token);

      tag_sizes ts = parse_tag_triple(&token);

      if (ts.num_ints < 0) {
        ts.num_ints = 0;
      }
      if (ts.num_ints > max_tag_nums) {
        ts.num_ints = max_tag_nums;
      }

      if (ts.num_reals < 0) {
        ts.num_reals = 0;
      }
      if (ts.num_reals > max_tag_nums) {
        ts.num_reals = max_tag_nums;
      }

      if (ts.num_strings < 0) {
        ts.num_strings = 0;
      }
      if (ts.num_strings > max_tag_nums) {
        ts.num_strings = max_tag_nums;
      }

      tag.intValues.resize(static_cast<size_t>(ts.num_ints));

      for (size_t i = 0; i < static_cast<size_t>(ts.num_ints); ++i) {
        tag.intValues[i] = parse_int(&token);
      }

      tag.floatValues.resize(static_cast<size_t>(ts.num_reals));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_reals); ++i) {
        tag.floatValues[i] = parse_f32(&token);
      }

      tag.stringValues.resize(static_cast<size_t>(ts.num_strings));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_strings); ++i) {
        tag.stringValues[i] = parse_string(&token);
      }

      tags.push_back(tag);

      continue;
    }

    if (token[0] == 's' && IS_SPACE(token[1])) {
      // smoothing group id
      token += 2;

      // skip space.
      token += strspn(token, " \t");  // skip space

      if (token[0] == '\0') {
        continue;
      }

      if (token[0] == '\r' || token[1] == '\n') {
        continue;
      }

      if (strlen(token) >= 3 && token[0] == 'o' && token[1] == 'f' &&
          token[2] == 'f') {
        current_smoothing_id = 0;
      } else {
        // assume number
        int smGroupId = parse_int(&token);
        if (smGroupId < 0) {
          // parse error. force set to 0.
          // FIXME(syoyo): Report warning.
          current_smoothing_id = 0;
        } else {
          current_smoothing_id = static_cast<unsigned int>(smGroupId);
        }
      }

      continue;
    }  // smoothing group id

    // Ignore unknown command.
  }

  // not all vertices have colors, no default colors desired? -> clear colors
  if (!found_all_colors && !default_vcols_fallback) {
    vc.clear();
  }

  if (greatest_v_idx >= static_cast<int>(v.size() / 3)) {
    if (warn) {
      std::stringstream ss;
      ss << "Vertex indices out of bounds (line " << line_num << ".)\n"
         << std::endl;
      (*warn) += ss.str();
    }
  }
  if (greatest_vn_idx >= static_cast<int>(vn.size() / 3)) {
    if (warn) {
      std::stringstream ss;
      ss << "Vertex normal indices out of bounds (line " << line_num << ".)\n"
         << std::endl;
      (*warn) += ss.str();
    }
  }
  if (greatest_vt_idx >= static_cast<int>(vt.size() / 2)) {
    if (warn) {
      std::stringstream ss;
      ss << "Vertex texcoord indices out of bounds (line " << line_num << ".)\n"
         << std::endl;
      (*warn) += ss.str();
    }
  }

  bool ret = export_groups_to_shape(&shape, prim_group, tags, material, name,
                                    triangulate, v);
  // export_groups_to_shape return false when `usemtl` is called in the last
  // line.
  // we also add `shape` to `shapes` when `shape.mesh` has already some
  // faces(indices)
  if (ret || shape.mesh.indices
                  .size()) {  // FIXME(syoyo): Support other prims(e.g. lines)
    shapes->push_back(shape);
  }
  prim_group.clear();  // for safety

  if (err) {
    (*err) += errss.str();
  }

  attrib->vertices.swap(v);
  attrib->vertex_weights.swap(v);
  attrib->normals.swap(vn);
  attrib->texcoords.swap(vt);
  attrib->texcoord_ws.swap(vt);
  attrib->colors.swap(vc);
  attrib->skin_weights.swap(vw);

  return true;
}


bool LoadObjWithCallback(std::istream &inStream,
                         const callback_t &callback,
                         void *user_data ,
                         MaterialReader *readMatFn ,
                         std::string *warn,
                         std::string *err )


bool ObjReader::ParseFromFile(const std::string &filename,
                              const ObjReaderConfig &config) {

}




bool obj::loader::load_from_file(const fs::path& filename,
                                 const loader_config &config)  {
  std::stringstream errss;

  // material
  std::map<std::string, int> material_map;
  int material_id = -1;  // -1 = invalid

  std::vector<index_t> indices;
  std::vector<material_t> materials;
  std::vector<std::string> names;
  names.reserve(2);
  std::vector<const char *> names_out;

  std::string linebuf;
  while (inStream.peek() != -1) {
    safe_get_line(inStream, linebuf);

    // Trim newline '\r\n' or '\n'
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\n')
        linebuf.erase(linebuf.size() - 1);
    }
    if (linebuf.size() > 0) {
      if (linebuf[linebuf.size() - 1] == '\r')
        linebuf.erase(linebuf.size() - 1);
    }

    // Skip if empty line.
    if (linebuf.empty()) {
      continue;
    }

    // Skip leading space.
    const char *token = linebuf.c_str();
    token += strspn(token, " \t");

    assert(token);
    if (token[0] == '\0') continue;  // empty line

    if (token[0] == '#') continue;  // comment line

    // vertex
    if (token[0] == 'v' && IS_SPACE((token[1]))) {
      token += 2;
      // TODO(syoyo): Support parsing vertex color extension.
      f32 x, y, z, w;  // w is optional. default = 1.0
      parse_f32_x4(&x, &y, &z, &w, &token);
      if (callback.vertex_cb) {
        callback.vertex_cb(user_data, x, y, z, w);
      }
      continue;
    }

    // normal
    if (token[0] == 'v' && token[1] == 'n' && IS_SPACE((token[2]))) {
      token += 3;
      f32 x, y, z;
      parse_f32_x3(&x, &y, &z, &token);
      if (callback.normal_cb) {
        callback.normal_cb(user_data, x, y, z);
      }
      continue;
    }

    // texcoord
    if (token[0] == 'v' && token[1] == 't' && IS_SPACE((token[2]))) {
      token += 3;
      f32 x, y, z;  // y and z are optional. default = 0.0
      parse_f32_x3(&x, &y, &z, &token);
      if (callback.texcoord_cb) {
        callback.texcoord_cb(user_data, x, y, z);
      }
      continue;
    }

    // face
    if (token[0] == 'f' && IS_SPACE((token[1]))) {
      token += 2;
      token += strspn(token, " \t");

      indices.clear();
      while (!IS_NEW_LINE(token[0])) {
        vertex_index vi = parse_raw_triple(&token);

        index_t idx;
        idx.vertex_index = vi.v_idx;
        idx.normal_index = vi.vn_idx;
        idx.texcoord_index = vi.vt_idx;

        indices.push_back(idx);
        size_t n = strspn(token, " \t\r");
        token += n;
      }

      if (callback.index_cb && indices.size() > 0) {
        callback.index_cb(user_data, &indices.at(0),
                          static_cast<int>(indices.size()));
      }

      continue;
    }

    // use mtl
    if ((0 == strncmp(token, "usemtl", 6)) && IS_SPACE((token[6]))) {
      token += 7;
      std::stringstream ss;
      ss << token;
      std::string namebuf = ss.str();

      int newMaterialId = -1;
      std::map<std::string, int>::const_iterator it = material_map.find(namebuf);
      if (it != material_map.end()) {
        newMaterialId = it->second;
      } else {
        // { warn!! material not found }
        if (warn && (!callback.usemtl_cb)) {
          (*warn) += "material [ " + namebuf + " ] not found in .mtl\n";
        }
      }

      if (newMaterialId != material_id) {
        material_id = newMaterialId;
      }

      if (callback.usemtl_cb) {
        callback.usemtl_cb(user_data, namebuf.c_str(), material_id);
      }

      continue;
    }

    // load mtl
    if ((0 == strncmp(token, "mtllib", 6)) && IS_SPACE((token[6]))) {
      if (readMatFn) {
        token += 7;

        std::vector<std::string> filenames;
        SplitString(std::string(token), ' ', '\\', filenames);

        if (filenames.empty()) {
          if (warn) {
            (*warn) +=
                "Looks like empty filename for mtllib. Use default "
                "material. \n";
          }
        } else {
          bool found = false;
          for (size_t s = 0; s < filenames.size(); s++) {
            std::string warn_mtl;
            std::string err_mtl;
            bool ok = (*readMatFn)(filenames[s].c_str(), &materials,
                                   &material_map, &warn_mtl, &err_mtl);

            if (warn && (!warn_mtl.empty())) {
              (*warn) += warn_mtl;  // This should be warn message.
            }

            if (err && (!err_mtl.empty())) {
              (*err) += err_mtl;
            }

            if (ok) {
              found = true;
              break;
            }
          }

          if (!found) {
            if (warn) {
              (*warn) +=
                  "Failed to load material file(s). Use default "
                  "material.\n";
            }
          } else {
            if (callback.mtllib_cb) {
              callback.mtllib_cb(user_data, &materials.at(0),
                                 static_cast<int>(materials.size()));
            }
          }
        }
      }

      continue;
    }

    // group name
    if (token[0] == 'g' && IS_SPACE((token[1]))) {
      names.clear();

      while (!IS_NEW_LINE(token[0])) {
        std::string str = parse_string(&token);
        names.push_back(str);
        token += strspn(token, " \t\r");  // skip tag
      }

      assert(names.size() > 0);

      if (callback.group_cb) {
        if (names.size() > 1) {
          // create const char* array.
          names_out.resize(names.size() - 1);
          for (size_t j = 0; j < names_out.size(); j++) {
            names_out[j] = names[j + 1].c_str();
          }
          callback.group_cb(user_data, &names_out.at(0),
                            static_cast<int>(names_out.size()));

        } else {
          callback.group_cb(user_data, nullptr, 0);
        }
      }

      continue;
    }

    // object name
    if (token[0] == 'o' && IS_SPACE((token[1]))) {
      // @todo { multiple object name? }
      token += 2;

      std::stringstream ss;
      ss << token;
      std::string object_name = ss.str();

      if (callback.object_cb) {
        callback.object_cb(user_data, object_name.c_str());
      }

      continue;
    }

#if 0  // @todo
    if (token[0] == 't' && IS_SPACE(token[1])) {
      tag_t tag;

      token += 2;
      std::stringstream ss;
      ss << token;
      tag.name = ss.str();

      token += tag.name.size() + 1;

      tag_sizes ts = parse_tag_triple(&token);

      tag.intValues.resize(static_cast<size_t>(ts.num_ints));

      for (size_t i = 0; i < static_cast<size_t>(ts.num_ints); ++i) {
        tag.intValues[i] = atoi(token);
        token += strcspn(token, "/ \t\r") + 1;
      }

      tag.floatValues.resize(static_cast<size_t>(ts.num_reals));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_reals); ++i) {
        tag.floatValues[i] = parse_f32(&token);
        token += strcspn(token, "/ \t\r") + 1;
      }

      tag.stringValues.resize(static_cast<size_t>(ts.num_strings));
      for (size_t i = 0; i < static_cast<size_t>(ts.num_strings); ++i) {
        std::stringstream ss;
        ss << token;
        tag.stringValues[i] = ss.str();
        token += tag.stringValues[i].size() + 1;
      }

      tags.push_back(tag);
    }
#endif

    // Ignore unknown command.
  }

  if (err) {
    (*err) += errss.str();
  }

  return true;
}

bool obj::loader::parse_from_string(string_view obj_text,
                                    string_view mtl_text,
                                    const loader_config &config) {
  std::stringbuf obj_buf(std::string(obj_text.c_str(), obj_text.size()));
  std::stringbuf mtl_buf(std::string(mtl_text.c_str(), mtl_text.size()));

  std::istream obj_ifs(&obj_buf);
  std::istream mtl_ifs(&mtl_buf);

  material_stream_reader mtl_ss(mtl_ifs);

  valid_ = obj::load_object(&attrib_,
                            &shapes_,
                            &materials_,
                            &warning_,
                            &error_,
                            &obj_ifs,
                            &mtl_ss,
                            config.triangulate,
                            config.vertex_color);

  return valid_;
}

bool obj::loader::load_object(std::istream &in){ }







void obj::loader::anchor() {}


#ifdef __clang__
#pragma clang diagnostic pop
#endif

