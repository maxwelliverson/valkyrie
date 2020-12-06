//
// Created by Maxwell on 2020-11-06.
//

#ifndef VULKAN_WRAPPER_VK_PP_H
#define VULKAN_WRAPPER_VK_PP_H


#define PP_VK_impl_STRINGIFY_redirect(x) #x
#define PP_VK_impl_STRINGIFY PP_VK_impl_STRINGIFY_redirect
#define PP_VK_impl_DOUBLE_ARG(arg) arg , arg
#define PP_VK_impl_ARG_32_TIMES(x) PP_VK_impl_DOUBLE_ARG(PP_VK_impl_DOUBLE_ARG(PP_VK_impl_DOUBLE_ARG(PP_VK_impl_DOUBLE_ARG(PP_VK_impl_DOUBLE_ARG(x)))))
#define PP_VK_impl_FALSE_32_TIMES PP_VK_impl_ARG_32_TIMES(false)
#define PP_VK_impl_GET_NINTH_MEM(a, b, c, d, e, f, g, h, i, j, ...) j
#define PP_VK_impl_GET_32ND_MEM(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, ab, ac, ad, ae, af, ag, ah, ...) ah
#define PP_VK_impl_GET_32ND_MEM_IMPL PP_VK_impl_GET_32ND_MEM
#define PP_VK_impl_IS_PACK_EMPTY_IMPL(...) PP_VK_impl_GET_32ND_MEM_IMPL(__VA_ARGS__)
#define PP_VK_impl_GET_LIST_LENGTH_POSTFIX(...) PP_VK_impl_GET_NINTH_MEM(,##__VA_ARGS__ , _8, _7, _6, _5, _4, _3, _2, _1, _0)

#define PP_VK_impl_NEXT_0 _1
#define PP_VK_impl_NEXT_1 _2
#define PP_VK_impl_NEXT_2 _3
#define PP_VK_impl_NEXT_3 _4
#define PP_VK_impl_NEXT_4 _5
#define PP_VK_impl_NEXT_5 _6
#define PP_VK_impl_NEXT_6 _7
#define PP_VK_impl_NEXT_7 _8
#define PP_VK_impl_NEXT_8 _9
#define PP_VK_impl_NEXT_9 _10
#define PP_VK_impl_NEXT_10 _11
#define PP_VK_impl_NEXT_11 _12
#define PP_VK_impl_NEXT_12 _13
#define PP_VK_impl_NEXT_VALUE_redirect(Val) PP_VK_impl_NEXT##Val
#define PP_VK_impl_NEXT_VALUE PP_VK_impl_NEXT_VALUE_redirect
#define PP_VK_impl_PREVIOUS_1 _0
#define PP_VK_impl_PREVIOUS_2 _1
#define PP_VK_impl_PREVIOUS_3 _2
#define PP_VK_impl_PREVIOUS_4 _3
#define PP_VK_impl_PREVIOUS_5 _4
#define PP_VK_impl_PREVIOUS_6 _5
#define PP_VK_impl_PREVIOUS_7 _6
#define PP_VK_impl_PREVIOUS_8 _7
#define PP_VK_impl_PREVIOUS_9 _8
#define PP_VK_impl_PREVIOUS_10 _9
#define PP_VK_impl_PREVIOUS_11 _10
#define PP_VK_impl_PREVIOUS_12 _11
#define PP_VK_impl_PREVIOUS_VALUE_redirect(Val) PP_VK_impl_PREVIOUS##Val
#define PP_VK_impl_PREVIOUS_VALUE PP_VK_impl_PREVIOUS_VALUE_redirect

#define VK_next(...) PP_VK_impl_NEXT_VALUE(__VA_ARGS__)
#define VK_previous(...) PP_VK_impl_PREVIOUS_VALUE(__VA_ARGS__)





#define PP_VK_impl_FOR_EACH_34(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_33(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_33(Macro, Dlm, x, ...) Macro(x) Dlm, PP_VK_impl_FOR_EACH_32(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_32(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_31(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_31(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_30(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_30(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_29(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_29(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_28(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_28(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_27(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_27(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_26(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_26(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_25(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_25(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_24(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_24(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_23(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_23(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_22(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_22(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_21(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_21(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_20(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_20(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_19(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_19(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_18(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_18(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_17(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_17(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_16(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_16(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_15(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_15(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_14(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_14(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_13(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_13(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_12(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_12(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_11(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_11(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_10(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_10(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_9(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_9(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_8(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_8(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_7(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_7(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_6(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_6(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_5(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_5(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_4(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_4(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_3(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_3(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_2(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_2(Macro, Dlm, x, ...) Macro(x) Dlm PP_VK_impl_FOR_EACH_1(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_1(Macro, Dlm, x) Macro(x)
#define PP_VK_impl_FOR_EACH_0(Macro, Dlm)


/*#define PP_VK_impl_GET_THIRD_MEM(a, b, c, d, ...) d
#define PP_VK_impl_GET_PARAM_TYPE(...) GET_NINTH_MEM(, ##__VA_ARGS__ , _PACK, _PACK, _PACK, _PACK, _PACK, _PACK, _PACK, _NOT_PACK, _NOT_PACK)
#define PP_VK_impl_GET_OPT(...) GET_NINTH_MEM(, ##__VA_ARGS__ , YES_OPT, YES_OPT, YES_OPT, YES_OPT, YES_OPT, YES_OPT, YES_OPT, YES_OPT, NO_OPT)
#define PP_VK_impl_VARARGS_YES_OPT(...) __VA_ARGS__
#define PP_VK_impl_VARARGS_NO_OPT(...)
#define __VA_OPT(x, ...)  CONCAT(PP_VK_impl_VARARGS_, GET_OPT(__VA_ARGS__)) x

#define PP_VK_impl_THIRD_ARG(a,b,c,...) c
#define PP_VK_impl_VA_OPT_SUPPORTED_I(...) PP_VK_impl_THIRD_ARG(__VA_OPT__(,),true,false,)
#define PP_VK_impl_VA_OPT_SUPPORTED PP_VK_impl_VA_OPT_SUPPORTED_I(?)*/

#define PP_VK_impl_CONCAT_redirect(x, y) x##y
#define PP_VK_impl_CONCAT PP_VK_impl_CONCAT_redirect
#define PP_VK_impl_CONCAT_INNER_redirect(x, y) x##y
#define PP_VK_impl_CONCAT_INNER PP_VK_impl_CONCAT_INNER_redirect
#define PP_VK_impl_LPAREN() (
#define PP_VK_impl_RPAREN() )
#define PP_VK_impl_LAZY_EVAL_redirect(...) __VA_ARGS__
#define PP_VK_impl_LAZY_EVAL PP_VK_impl_LAZY_EVAL_redirect
#define PP_VK_impl_DISCARD_CONDITION(Cond) VK_concat(PP_VK_impl_DISCARD_CONDITION_, VK_unwrap(Cond))
#define PP_VK_impl_DISCARD_CONDITION_true
#define PP_VK_impl_DISCARD_CONDITION_false
#define PP_VK_impl_TAKE_X_redirect(x, ...) x
#define PP_VK_impl_TAKE_X_ PP_VK_impl_TAKE_X_redirect
#define PP_VK_impl_TAKE_X(...) PP_VK_impl_TAKE_X_(__VA_ARGS__)
#define PP_VK_impl_TAKE_Y_redirect(x, ...) __VA_ARGS__
#define PP_VK_impl_TAKE_Y_ PP_VK_impl_TAKE_Y_redirect
#define PP_VK_impl_TAKE_Y(...) PP_VK_impl_TAKE_Y_(__VA_ARGS__)
#define PP_VK_impl_IF_CONCAT_redirect_I(x, y) x##y
#define PP_VK_impl_IF_CONCAT_redirect PP_VK_impl_IF_CONCAT_redirect_I
#define PP_VK_impl_IF_CONCAT(...) PP_VK_impl_IF_CONCAT_redirect(__VA_ARGS__)
#define PP_VK_impl_ELSE_IF_CONCAT_redirect_I(x, y) x##y
#define PP_VK_impl_ELSE_IF_CONCAT_redirect PP_VK_impl_ELSE_IF_CONCAT_redirect_I
#define PP_VK_impl_ELSE_IF_CONCAT(...) PP_VK_impl_ELSE_IF_CONCAT_redirect(__VA_ARGS__)
#define PP_VK_impl_ISOLATE_CONDITION(Cond) PP_VK_impl_TAKE_X(PP_VK_impl_IF_CONCAT(PP_VK_impl_ISOLATE_, VK_unwrap(Cond)))
#define PP_VK_impl_ISOLATE_CLAUSES(Cond) PP_VK_impl_TAKE_Y(PP_VK_impl_IF_CONCAT(PP_VK_impl_ISOLATE_, VK_unwrap(Cond)))
#define PP_VK_impl_ISOLATE_true true ,
#define PP_VK_impl_ISOLATE_false false ,
#define PP_VK_impl_IF_true VK_lazy_take_first
#define PP_VK_impl_IF_false VK_lazy_discard_first
#define PP_VK_impl_INSERT_COMMA_redirect(...)  (__VA_ARGS__) ,
#define PP_VK_impl_INSERT_COMMA PP_VK_impl_INSERT_COMMA_redirect
#define PP_VK_impl_TAKE_FIRST_AGAIN_redirect(x, ...) VK_unwrap_inner(x)
#define PP_VK_impl_TAKE_FIRST_AGAIN PP_VK_impl_TAKE_FIRST_AGAIN_redirect
#define PP_VK_impl_TAKE_FIRST_redirect(x, ...) PP_VK_impl_TAKE_FIRST_AGAIN x
#define PP_VK_impl_TAKE_FIRST PP_VK_impl_TAKE_FIRST_redirect
#define PP_VK_impl_DISCARD_FIRST_AGAIN_redirect(x, ...) VK_unwrap_inner(__VA_ARGS__)
#define PP_VK_impl_DISCARD_FIRST_AGAIN PP_VK_impl_DISCARD_FIRST_AGAIN_redirect
#define PP_VK_impl_DISCARD_FIRST_redirect(x, ...) PP_VK_impl_DISCARD_FIRST_AGAIN x
#define PP_VK_impl_DISCARD_FIRST PP_VK_impl_DISCARD_FIRST_redirect
#define PP_VK_impl_AND(A) VK_concat(PP_VK_impl_AND_, redirect)(A)
#define PP_VK_impl_AND_redirect(A) VK_lazy(PP_VK_impl_AND_redirect_##A)
#define PP_VK_impl_AND_redirect_true(B) VK_lazy(B)
#define PP_VK_impl_AND_redirect_false false VK_lazy_discard
#define PP_VK_impl_OR(A) VK_concat(PP_VK_impl_OR_, redirect)(A)
#define PP_VK_impl_OR_redirect(A) VK_lazy(PP_VK_impl_OR_redirect_##A)
#define PP_VK_impl_OR_redirect_true true VK_lazy_discard
#define PP_VK_impl_OR_redirect_false(B) VK_lazy(B)
#define PP_VK_impl_XOR_IMPL(A) VK_concat(PP_VK_impl_XOR_, IMPL_A)(A)
#define PP_VK_impl_XOR_IMPL_A(A) VK_lazy(PP_VK_impl_XOR_A_##A)
#define PP_VK_impl_XOR_A_true(B) VK_not(B)
#define PP_VK_impl_XOR_A_false(B) VK_lazy(B)
#define PP_VK_impl_NOT_redirect(A) VK_lazy(PP_VK_impl_NOT_##A)
#define PP_VK_impl_NOT_true false
#define PP_VK_impl_NOT_false true
#define PP_VK_impl_TRY_UNWRAP(x)
#define PP_VK_impl_UNWRAP_CONCAT_redirect_I(x, y) x##y
#define PP_VK_impl_UNWRAP_CONCAT_redirect PP_VK_impl_UNWRAP_CONCAT_redirect_I
#define PP_VK_impl_UNWRAP_CONCAT(...) PP_VK_impl_UNWRAP_CONCAT_redirect(__VA_ARGS__)
#define PP_VK_impl_TRY_UNWRAP_I(...) PP_VK_impl_TRY_UNWRAP_SUCCESS(__VA_ARGS__)
#define PP_VK_impl_TRY_UNWRAP_ PP_VK_impl_TRY_UNWRAP_I
#define result_PP_VK_impl_TRY_UNWRAP_SUCCESS PP_VK_impl_UNWRAP_success
#define result_PP_VK_impl_TRY_UNWRAP_I PP_VK_impl_UNWRAP_failure
#define PP_VK_impl_UNWRAP_INNER_CONCAT_redirect_I(x, y) x##y
#define PP_VK_impl_UNWRAP_INNER_CONCAT_redirect PP_VK_impl_UNWRAP_INNER_CONCAT_redirect_I
#define PP_VK_impl_UNWRAP_INNER_CONCAT(...) PP_VK_impl_UNWRAP_INNER_CONCAT_redirect(__VA_ARGS__)
#define PP_VK_impl_UNWRAP_failure
#define PP_VK_impl_UNWRAP_success(...) __VA_ARGS__
#define PP_VK_impl_TRY_UNWRAP_INNER(x)
#define PP_VK_impl_TRY_UNWRAP_INNER_I(...) PP_VK_impl_TRY_UNWRAP_INNER_SUCCESS(__VA_ARGS__)
#define PP_VK_impl_TRY_UNWRAP_INNER_ PP_VK_impl_TRY_UNWRAP_INNER_I
#define result_PP_VK_impl_TRY_UNWRAP_INNER_SUCCESS PP_VK_impl_UNWRAP_INNER_success
#define result_PP_VK_impl_TRY_UNWRAP_INNER_I PP_VK_impl_UNWRAP_INNER_failure
#define PP_VK_impl_UNWRAP_INNER_failure
#define PP_VK_impl_UNWRAP_INNER_success(...) __VA_ARGS__
#define PP_VK_impl_PACK_IS_EMPTY_redirect(...) PP_VK_impl_IS_PACK_EMPTY_IMPL(, ##__VA_ARGS__, PP_VK_impl_FALSE_32_TIMES, true)
#define PP_VK_impl_PACK_IS_EMPTY PP_VK_impl_PACK_IS_EMPTY_redirect




// Public facing macro functions

#define VK_stringify(...) PP_VK_impl_STRINGIFY(__VA_ARGS__)

#define VK_count_args(...) PP_VK_impl_GET_32ND_MEM_IMPL(, ##__VA_ARGS__, _32, _31, _30, _29, _28, _27, _26, _25, _24, _23, _22, _21, _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, _0)
//#define VK_pack_is_empty(...) PP_VK_impl_IS_PACK_EMPTY_IMPL(, ##__VA_ARGS__, PP_VK_impl_FALSE_32_TIMES, true)
#define VK_pack_is_empty(...) PP_VK_impl_PACK_IS_EMPTY(__VA_ARGS__)

#define VK_for_each(Macro, ...) VK_for_each_delimit(Macro, , ##__VA_ARGS__)
#define VK_for_each_delimit(Macro, Delimiter, ...) VK_concat(PP_VK_impl_FOR_EACH, VK_count_args(__VA_ARGS__))(Macro, Delimiter, ##__VA_ARGS__)


#define VK_concat(x, y) PP_VK_impl_CONCAT(x, y)
#define VK_lazy_concat VK_lazy(PP_VK_impl##_CONCAT)
#define VK_concat_inner(x, y) PP_VK_impl_CONCAT_INNER(x, y)
#define VK_lazy_concat_inner VK_lazy(PP_VK_impl##_CONCAT_INNER)


#define VK_lparen PP_VK_impl_LPAREN()
#define VK_rparen PP_VK_impl_RPAREN()

#define VK_lazy(...) PP_VK_impl_LAZY_EVAL(__VA_ARGS__)

#define VK_discard(...)
#define VK_lazy_discard VK_discard
#define VK_no_discard(...) __VA_ARGS__
#define VK_lazy_no_discard VK_no_discard


#define VK_else
#define VK_else_if(Condition) PP_VK_impl_ELSE_IF_CONCAT(PP_VK_impl_IF_, PP_VK_impl_ISOLATE_CONDITION(Condition))(PP_VK_impl_ISOLATE_CLAUSES(Condition))
#define VK_if(Condition) VK_concat(PP_VK_impl_IF_, PP_VK_impl_ISOLATE_CONDITION(Condition))(PP_VK_impl_ISOLATE_CLAUSES(Condition))


#define VK_take_first(...) PP_VK_impl_TAKE_FIRST((PP_VK_impl_INSERT_COMMA __VA_ARGS__), LMAO)
#define VK_discard_first(...) PP_VK_impl_DISCARD_FIRST((PP_VK_impl_INSERT_COMMA __VA_ARGS__), LMAO)
#define VK_lazy_take_first VK_take_first
#define VK_lazy_discard_first VK_discard_first



#define VK_and(A, B) PP_VK_impl_AND(A)(B)
#define VK_or(A, B) PP_VK_impl_OR(A)(B)
#define VK_xor(A, B) PP_VK_impl_XOR_IMPL(A)(B)
#define VK_not(A) VK_concat(PP_VK_impl_NOT_, redirect)(A)
#define VK_implies(A, B) VK_or(VK_not(A), B)


#define VK_unwrap_inner(x) PP_VK_impl_UNWRAP_INNER_CONCAT(result_, PP_VK_impl_TRY_UNWRAP_INNER_ x)
#define VK_unwrap(x) PP_VK_impl_UNWRAP_CONCAT(result_, PP_VK_impl_TRY_UNWRAP_ x)

#define PP_VK_impl_TAGGED_MACRO(Tag) VK_##Tag
#define PP_VK_impl_TAGGED PP_VK_impl_TAGGED_MACRO
#define VK_tagged(...) VK_for_each(PP_VK_impl_TAGGED, ##__VA_ARGS__)

#define PP_VK_impl_VAR_CONCAT_redirect(x, y, ...) x##y , ##__VA_ARGS__
#define PP_VK_impl_VAR_CONCAT PP_VK_impl_VAR_CONCAT_redirect
#define VK_variadic_concat(...) PP_VK_impl_VAR_CONCAT(__VA_ARGS__)












#endif//VULKAN_WRAPPER_VK_PP_H
