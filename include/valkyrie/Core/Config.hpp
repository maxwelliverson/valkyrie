//
// Created by Maxwell on 2020-11-09.
//

#ifndef VALKYRIE_CONFIG_HPP
#define VALKYRIE_CONFIG_HPP

#if defined(_WIN32)
#define VK_interface struct __declspec(novtable)
#define VK_api __declspec(dllimport)
#define VK_input_string _In_z_ const valkyrie::utf8*
#else
#define VK_interface struct
#define VK_api
#endif

#define PP_VK_impl_STRINGIFY_redirect(x) #x
#define PP_VK_impl_STRINGIFY PP_VK_impl_STRINGIFY_redirect
#define PP_VK_impl_DOUBLE_ARG(arg) arg , arg
#define PP_VK_impl_ARG_32_TIMES(x) PP_VK_impl_DOUBLE_ARG(PP_VK_impl_DOUBLE_ARG(PP_VK_impl_DOUBLE_ARG(PP_VK_impl_DOUBLE_ARG(PP_VK_impl_DOUBLE_ARG(x)))))
#define PP_VK_impl_FALSE_32_TIMES PP_VK_impl_ARG_32_TIMES(false)
#define PP_VK_impl_GET_NINTH_MEM(a, b, c, d, e, f, g, h, i, j, ...) j
#define PP_VK_impl_GET_32ND_MEM(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, aa, ab, ac, ad, ae, af, ag, ah, ...) ah
#define PP_VK_impl_GET_32ND_MEM_IMPL PP_VK_impl_GET_32ND_MEM
#define PP_VK_impl_GET_256TH_MEM_redirect_2(aa, ab, ac, ad, ae, af, ag, ah, ai, aj, ak, al, am, an, ao, ap, aq, ar, as, at, au, av, aw, ax, ay, az, \
                                          ba, bb, bc, bd, be, bf, bg, bh, bi, bj, bk, bl, bm, bn, bo, bp, bq, br, bs, bt, bu, bv, bw, bx, by, bz, \
                                          ca, cb, cc, cd, ce, cf, cg, ch, ci, cj, ck, cl, cm, cn, co, cp, cq, cr, cs, ct, cu, cv, cw, cx, cy, cz, \
                                          da, db, dc, dd, de, df, dg, dh, di, dj, dk, dl, dm, dn, _do, dp, dq, dr, ds, dt, du, dv, dw, dx, dy, dz, \
                                          ea, eb, ec, ed, ee, ef, eg, eh, ei, ej, ek, el, em, en, eo, ep, eq, er, es, et, eu, ev, ew, ex, ey, ez, \
                                          fa, fb, fc, fd, fe, ff, fg, fh, fi, fj, fk, fl, fm, fn, fo, fp, fq, fr, fs, ft, fu, fv, fw, fx, fy, fz, \
                                          ga, gb, gc, gd, ge, gf, gg, gh, gi, gj, gk, gl, gm, gn, go, gp, gq, gr, gs, gt, gu, gv, gw, gx, gy, gz, \
                                          ha, hb, hc, hd, he, hf, hg, hh, hi, hj, hk, hl, hm, hn, ho, hp, hq, hr, hs, ht, hu, hv, hw, hx, hy, hz, \
                                          ia, ib, ic, id, ie, _if, ig, ih, ii, ij, ik, il, im, in, io, ip, iq, ir, is, it, iu, iv, iw, ix, iy, iz, \
                                          ja, jb, jc, jd, je, jf, jg, jh, ji, jj, jk, jl, jm, jn, jo, jp, jq, jr, js, jt, ju, jv, jw, jx, ...) jx
#define PP_VK_impl_LIST_OF_256  256, 255, 254, 253, 252, 251, 250, \
                                249, 248, 247, 246, 245, 244, 243, 242, 241, 240,\
                                239, 238, 237, 236, 235, 234, 233, 232, 231, 230,\
                                229, 228, 227, 226, 225, 224, 223, 222, 221, 220,\
                                219, 218, 217, 216, 215, 214, 213, 212, 211, 210,\
                                209, 208, 207, 206, 205, 204, 203, 202, 201, 200,\
                                199, 198, 197, 196, 195, 194, 193, 192, 191, 190,\
                                189, 188, 187, 186, 185, 184, 183, 182, 181, 180,\
                                179, 178, 177, 176, 175, 174, 173, 172, 171, 170,\
                                169, 168, 167, 166, 165, 164, 163, 162, 161, 160,\
                                159, 158, 157, 156, 155, 154, 153, 152, 151, 150,\
                                149, 148, 147, 146, 145, 144, 143, 142, 141, 140,\
                                139, 138, 137, 136, 135, 134, 133, 132, 131, 130,\
                                129, 128, 127, 126, 125, 124, 123, 122, 121, 120,\
                                119, 118, 117, 116, 115, 114, 113, 112, 111, 110,\
                                109, 108, 107, 106, 105, 104, 103, 102, 101, 100,\
                                99, 98, 97, 96, 95, 94, 93, 92, 91, 90,\
                                89, 88, 87, 86, 85, 84, 83, 82, 81, 80,\
                                79, 78, 77, 76, 75, 74, 73, 72, 71, 70,\
                                69, 68, 67, 66, 65, 64, 63, 62, 61, 60,\
                                59, 58, 57, 56, 55, 54, 53, 52, 51, 50,\
                                49, 48, 47, 46, 45, 44, 43, 42, 41, 40,\
                                39, 38, 37, 36, 35, 34, 33, 32, 31, 30,\
                                29, 28, 27, 26, 25, 24, 23, 22, 21, 20,\
                                19, 18, 17, 16, 15, 14, 13, 12, 11, 10,\
                                9, 8, 7, 6, 5, 4, 3, 2, 1, 0


#define PP_VK_impl_COUNT_ARGS(...) PP_VK_impl_GET_256TH_MEM(, ##__VA_ARGS__, PP_VK_impl_LIST_OF_256)
#define PP_VK_impl_TUPLE_SIZE_redirect(...) PP_VK_impl_GET_256TH_MEM(, ##__VA_ARGS__, PP_VK_impl_LIST_OF_256)
#define PP_VK_impl_TUPLE_SIZE PP_VK_impl_TUPLE_SIZE_redirect

#define PP_VK_impl_GET_256TH_MEM_redirect PP_VK_impl_GET_256TH_MEM_redirect_2
#define PP_VK_impl_GET_256TH_MEM(...) PP_VK_impl_GET_256TH_MEM_redirect(__VA_ARGS__)
#define PP_VK_impl_IS_PACK_EMPTY_IMPL(...) PP_VK_impl_GET_32ND_MEM_IMPL(__VA_ARGS__)
#define PP_VK_impl_GET_LIST_LENGTH_POSTFIX(...) PP_VK_impl_GET_NINTH_MEM(,##__VA_ARGS__ , _8, _7, _6, _5, _4, _3, _2, _1, _0)

#define PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, i, x) PP_VK_impl_INVOKE_DISPATCH_redirect(PP_VK_impl_INVOKE_DISPATCH_COUNT(PP_VK_impl_INVOKE_TRY_UNWRAP(Macro)), PP_VK_impl_INVOKE_TRY_UNWRAP(Macro), i, x)

#define PP_VK_impl_INVOKE_FOREACH_INDEXED_256(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 255, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_255(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_255(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 254, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_254(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_254(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 253, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_253(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_253(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 252, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_252(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_252(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 251, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_251(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_251(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 250, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_250(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_250(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 249, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_249(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_249(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 248, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_248(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_248(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 247, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_247(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_247(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 246, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_246(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_246(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 245, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_245(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_245(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 244, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_244(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_244(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 243, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_243(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_243(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 242, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_242(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_242(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 241, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_241(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_241(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 240, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_240(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_240(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 239, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_239(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_239(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 238, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_238(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_238(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 237, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_237(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_237(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 236, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_236(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_236(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 235, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_235(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_235(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 234, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_234(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_234(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 233, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_233(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_233(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 232, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_232(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_232(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 231, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_231(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_231(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 230, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_230(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_230(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 229, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_229(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_229(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 228, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_228(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_228(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 227, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_227(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_227(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 226, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_226(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_226(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 225, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_225(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_225(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 224, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_224(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_224(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 223, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_223(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_223(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 222, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_222(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_222(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 221, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_221(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_221(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 220, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_220(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_220(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 219, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_219(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_219(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 218, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_218(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_218(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 217, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_217(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_217(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 216, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_216(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_216(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 215, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_215(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_215(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 214, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_214(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_214(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 213, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_213(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_213(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 212, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_212(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_212(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 211, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_211(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_211(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 210, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_210(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_210(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 209, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_209(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_209(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 208, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_208(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_208(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 207, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_207(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_207(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 206, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_206(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_206(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 205, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_205(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_205(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 204, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_204(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_204(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 203, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_203(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_203(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 202, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_202(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_202(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 201, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_201(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_201(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 200, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_200(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_200(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 199, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_199(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_199(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 198, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_198(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_198(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 197, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_197(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_197(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 196, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_196(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_196(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 195, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_195(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_195(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 194, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_194(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_194(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 193, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_193(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_193(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 192, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_192(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_192(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 191, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_191(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_191(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 190, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_190(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_190(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 189, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_189(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_189(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 188, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_188(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_188(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 187, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_187(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_187(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 186, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_186(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_186(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 185, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_185(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_185(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 184, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_184(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_184(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 183, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_183(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_183(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 182, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_182(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_182(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 181, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_181(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_181(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 180, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_180(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_180(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 179, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_179(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_179(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 178, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_178(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_178(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 177, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_177(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_177(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 176, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_176(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_176(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 175, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_175(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_175(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 174, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_174(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_174(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 173, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_173(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_173(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 172, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_172(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_172(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 171, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_171(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_171(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 170, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_170(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_170(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 169, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_169(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_169(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 168, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_168(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_168(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 167, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_167(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_167(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 166, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_166(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_166(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 165, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_165(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_165(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 164, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_164(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_164(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 163, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_163(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_163(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 162, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_162(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_162(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 161, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_161(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_161(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 160, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_160(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_160(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 159, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_159(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_159(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 158, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_158(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_158(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 157, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_157(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_157(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 156, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_156(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_156(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 155, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_155(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_155(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 154, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_154(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_154(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 153, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_153(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_153(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 152, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_152(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_152(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 151, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_151(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_151(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 150, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_150(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_150(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 149, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_149(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_149(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 148, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_148(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_148(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 147, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_147(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_147(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 146, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_146(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_146(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 145, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_145(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_145(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 144, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_144(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_144(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 143, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_143(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_143(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 142, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_142(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_142(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 141, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_141(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_141(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 140, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_140(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_140(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 139, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_139(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_139(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 138, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_138(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_138(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 137, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_137(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_137(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 136, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_136(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_136(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 135, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_135(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_135(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 134, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_134(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_134(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 133, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_133(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_133(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 132, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_132(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_132(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 131, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_131(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_131(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 130, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_130(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_130(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 129, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_129(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_129(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 128, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_128(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_128(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 127, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_127(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_127(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 126, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_126(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_126(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 125, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_125(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_125(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 124, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_124(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_124(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 123, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_123(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_123(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 122, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_122(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_122(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 121, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_121(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_121(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 120, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_120(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_120(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 119, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_119(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_119(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 118, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_118(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_118(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 117, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_117(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_117(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 116, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_116(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_116(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 115, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_115(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_115(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 114, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_114(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_114(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 113, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_113(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_113(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 112, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_112(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_112(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 111, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_111(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_111(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 110, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_110(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_110(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 109, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_109(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_109(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 108, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_108(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_108(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 107, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_107(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_107(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 106, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_106(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_106(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 105, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_105(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_105(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 104, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_104(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_104(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 103, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_103(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_103(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 102, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_102(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_102(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 101, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_101(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_101(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 100, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_100(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_100(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 99, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_99(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_99(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 98, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_98(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_98(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 97, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_97(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_97(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 96, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_96(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_96(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 95, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_95(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_95(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 94, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_94(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_94(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 93, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_93(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_93(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 92, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_92(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_92(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 91, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_91(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_91(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 90, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_90(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_90(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 89, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_89(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_89(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 88, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_88(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_88(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 87, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_87(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_87(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 86, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_86(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_86(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 85, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_85(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_85(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 84, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_84(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_84(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 83, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_83(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_83(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 82, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_82(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_82(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 81, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_81(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_81(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 80, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_80(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_80(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 79, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_79(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_79(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 78, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_78(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_78(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 77, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_77(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_77(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 76, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_76(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_76(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 75, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_75(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_75(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 74, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_74(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_74(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 73, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_73(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_73(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 72, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_72(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_72(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 71, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_71(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_71(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 70, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_70(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_70(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 69, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_69(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_69(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 68, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_68(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_68(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 67, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_67(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_67(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 66, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_66(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_66(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 65, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_65(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_65(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 64, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_64(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_64(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 63, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_63(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_63(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 62, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_62(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_62(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 61, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_61(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_61(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 60, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_60(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_60(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 59, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_59(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_59(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 58, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_58(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_58(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 57, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_57(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_57(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 56, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_56(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_56(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 55, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_55(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_55(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 54, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_54(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_54(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 53, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_53(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_53(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 52, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_52(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_52(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 51, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_51(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_51(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 50, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_50(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_50(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 49, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_49(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_49(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 48, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_48(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_48(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 47, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_47(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_47(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 46, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_46(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_46(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 45, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_45(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_45(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 44, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_44(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_44(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 43, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_43(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_43(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 42, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_42(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_42(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 41, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_41(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_41(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 40, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_40(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_40(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 39, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_39(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_39(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 38, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_38(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_38(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 37, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_37(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_37(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 36, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_36(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_36(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 35, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_35(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_35(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 34, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_34(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_34(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 33, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_33(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_33(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 32, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_32(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_32(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 31, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_31(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_31(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 30, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_30(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_30(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 29, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_29(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_29(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 28, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_28(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_28(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 27, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_27(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_27(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 26, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_26(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_26(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 25, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_25(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_25(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 24, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_24(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_24(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 23, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_23(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_23(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 22, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_22(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_22(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 21, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_21(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_21(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 20, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_20(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_20(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 19, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_19(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_19(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 18, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_18(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_18(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 17, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_17(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_17(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 16, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_16(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_16(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 15, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_15(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_15(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 14, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_14(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_14(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 13, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_13(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_13(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 12, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_12(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_12(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 11, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_11(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_11(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 10, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_10(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_10(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 9, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_9(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_9(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 8, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_8(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_8(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 7, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_7(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_7(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 6, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_6(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_6(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 5, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_5(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_5(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 4, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_4(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_4(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 3, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_3(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_3(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 2, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_2(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_2(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 1, x) dlm() PP_VK_impl_INVOKE_FOREACH_INDEXED_1(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_1(Macro, dlm, x)      PP_VK_impl_FOREACH_INNER_INVOKE_INDEXED(Macro, 0, x)
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_0(Macro, dlm)

#define PP_VK_impl_INVOKE_FOREACH_INDEXED_CONCAT_redirect_2(Count) PP_VK_impl_INVOKE_FOREACH_INDEXED_##Count
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_CONCAT_redirect PP_VK_impl_INVOKE_FOREACH_INDEXED_CONCAT_redirect_2
#define PP_VK_impl_INVOKE_FOREACH_INDEXED_CONCAT(Count) PP_VK_impl_INVOKE_FOREACH_INDEXED_CONCAT_redirect(Count)


#define PP_VK_impl_INVOKE_FOREACH_INDEXED(Macro, dlm, ...) PP_VK_impl_INVOKE_FOREACH_INDEXED_CONCAT(PP_VK_impl_GET_256TH_MEM(, ##__VA_ARGS__, PP_VK_impl_LIST_OF_256))(Macro, dlm, ##__VA_ARGS__)



#define PP_VK_impl_INVOKE_FOREACH_256(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_255(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_255(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_254(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_254(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_253(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_253(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_252(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_252(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_251(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_251(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_250(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_250(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_249(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_249(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_248(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_248(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_247(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_247(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_246(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_246(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_245(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_245(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_244(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_244(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_243(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_243(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_242(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_242(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_241(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_241(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_240(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_240(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_239(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_239(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_238(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_238(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_237(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_237(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_236(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_236(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_235(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_235(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_234(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_234(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_233(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_233(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_232(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_232(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_231(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_231(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_230(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_230(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_229(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_229(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_228(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_228(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_227(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_227(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_226(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_226(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_225(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_225(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_224(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_224(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_223(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_223(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_222(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_222(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_221(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_221(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_220(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_220(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_219(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_219(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_218(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_218(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_217(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_217(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_216(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_216(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_215(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_215(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_214(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_214(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_213(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_213(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_212(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_212(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_211(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_211(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_210(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_210(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_209(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_209(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_208(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_208(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_207(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_207(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_206(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_206(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_205(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_205(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_204(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_204(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_203(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_203(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_202(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_202(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_201(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_201(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_200(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_200(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_199(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_199(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_198(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_198(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_197(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_197(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_196(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_196(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_195(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_195(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_194(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_194(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_193(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_193(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_192(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_192(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_191(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_191(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_190(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_190(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_189(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_189(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_188(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_188(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_187(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_187(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_186(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_186(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_185(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_185(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_184(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_184(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_183(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_183(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_182(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_182(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_181(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_181(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_180(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_180(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_179(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_179(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_178(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_178(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_177(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_177(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_176(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_176(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_175(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_175(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_174(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_174(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_173(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_173(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_172(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_172(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_171(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_171(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_170(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_170(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_169(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_169(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_168(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_168(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_167(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_167(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_166(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_166(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_165(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_165(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_164(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_164(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_163(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_163(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_162(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_162(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_161(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_161(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_160(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_160(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_159(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_159(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_158(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_158(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_157(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_157(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_156(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_156(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_155(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_155(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_154(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_154(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_153(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_153(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_152(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_152(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_151(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_151(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_150(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_150(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_149(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_149(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_148(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_148(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_147(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_147(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_146(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_146(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_145(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_145(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_144(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_144(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_143(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_143(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_142(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_142(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_141(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_141(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_140(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_140(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_139(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_139(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_138(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_138(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_137(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_137(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_136(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_136(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_135(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_135(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_134(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_134(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_133(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_133(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_132(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_132(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_131(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_131(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_130(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_130(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_129(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_129(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_128(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_128(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_127(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_127(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_126(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_126(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_125(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_125(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_124(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_124(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_123(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_123(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_122(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_122(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_121(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_121(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_120(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_120(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_119(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_119(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_118(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_118(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_117(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_117(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_116(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_116(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_115(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_115(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_114(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_114(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_113(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_113(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_112(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_112(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_111(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_111(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_110(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_110(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_109(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_109(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_108(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_108(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_107(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_107(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_106(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_106(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_105(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_105(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_104(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_104(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_103(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_103(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_102(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_102(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_101(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_101(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_100(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_100(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_99(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_99(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_98(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_98(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_97(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_97(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_96(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_96(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_95(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_95(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_94(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_94(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_93(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_93(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_92(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_92(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_91(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_91(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_90(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_90(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_89(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_89(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_88(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_88(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_87(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_87(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_86(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_86(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_85(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_85(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_84(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_84(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_83(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_83(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_82(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_82(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_81(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_81(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_80(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_80(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_79(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_79(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_78(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_78(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_77(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_77(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_76(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_76(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_75(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_75(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_74(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_74(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_73(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_73(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_72(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_72(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_71(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_71(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_70(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_70(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_69(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_69(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_68(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_68(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_67(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_67(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_66(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_66(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_65(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_65(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_64(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_64(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_63(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_63(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_62(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_62(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_61(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_61(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_60(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_60(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_59(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_59(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_58(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_58(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_57(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_57(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_56(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_56(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_55(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_55(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_54(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_54(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_53(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_53(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_52(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_52(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_51(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_51(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_50(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_50(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_49(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_49(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_48(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_48(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_47(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_47(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_46(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_46(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_45(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_45(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_44(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_44(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_43(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_43(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_42(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_42(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_41(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_41(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_40(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_40(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_39(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_39(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_38(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_38(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_37(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_37(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_36(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_36(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_35(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_35(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_34(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_34(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_33(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_33(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_32(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_32(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_31(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_31(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_30(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_30(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_29(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_29(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_28(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_28(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_27(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_27(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_26(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_26(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_25(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_25(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_24(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_24(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_23(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_23(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_22(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_22(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_21(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_21(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_20(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_20(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_19(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_19(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_18(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_18(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_17(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_17(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_16(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_16(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_15(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_15(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_14(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_14(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_13(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_13(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_12(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_12(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_11(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_11(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_10(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_10(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_9(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_9(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_8(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_8(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_7(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_7(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_6(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_6(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_5(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_5(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_4(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_4(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_3(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_3(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_2(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_2(Macro, dlm, x, ...) PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) dlm() PP_VK_impl_INVOKE_FOREACH_1(Macro, dlm, __VA_ARGS__)
#define PP_VK_impl_INVOKE_FOREACH_1(Macro, dlm, x)      PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x)
#define PP_VK_impl_INVOKE_FOREACH_0(Macro, dlm)

#define PP_VK_impl_INVOKE_FOREACH_CONCAT_redirect_2(Count) PP_VK_impl_INVOKE_FOREACH_##Count
#define PP_VK_impl_INVOKE_FOREACH_CONCAT_redirect PP_VK_impl_INVOKE_FOREACH_CONCAT_redirect_2
#define PP_VK_impl_INVOKE_FOREACH_CONCAT(Count) PP_VK_impl_INVOKE_FOREACH_CONCAT_redirect(Count)

#define PP_VK_impl_FOREACH_INNER_INVOKE(Macro, x) PP_VK_impl_INVOKE_DISPATCH_redirect(PP_VK_impl_INVOKE_DISPATCH_COUNT(PP_VK_impl_INVOKE_TRY_UNWRAP(Macro)), PP_VK_impl_INVOKE_TRY_UNWRAP(Macro), x)


#define PP_VK_impl_INVOKE_FOREACH(Macro, dlm, ...) PP_VK_impl_INVOKE_FOREACH_CONCAT(PP_VK_impl_GET_256TH_MEM(, ##__VA_ARGS__, PP_VK_impl_LIST_OF_256))(Macro, dlm, ##__VA_ARGS__)

#define PP_VK_impl_GET_COUNT_256(...) PP_VK_impl_GET_256TH_MEM(, ##__VA_ARGS__, PP_VK_impl_LIST_OF_256)





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







#define PP_VK_impl_FOR_EACH_34(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_33(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_33(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_32(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_32(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_31(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_31(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_30(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_30(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_29(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_29(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_28(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_28(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_27(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_27(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_26(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_26(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_25(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_25(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_24(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_24(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_23(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_23(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_22(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_22(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_21(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_21(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_20(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_20(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_19(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_19(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_18(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_18(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_17(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_17(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_16(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_16(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_15(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_15(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_14(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_14(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_13(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_13(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_12(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_12(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_11(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_11(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_10(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_10(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_9(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_9(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_8(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_8(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_7(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_7(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_6(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_6(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_5(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_5(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_4(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_4(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_3(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_3(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_2(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_2(Macro, Dlm, x, ...) Macro(x) Dlm() PP_VK_impl_FOR_EACH_1(Macro, Dlm, __VA_ARGS__)
#define PP_VK_impl_FOR_EACH_1(Macro, Dlm, x) Macro(x)
#define PP_VK_impl_FOR_EACH_0(Macro, Dlm)

#define PP_VK_impl_COMMA_DELIMIT_redirect() ,
#define PP_VK_impl_COMMA_DELIMIT PP_VK_impl_COMMA_DELIMIT_redirect

#define PP_VK_impl_DEFAULT_DELIMIT_redirect()
#define PP_VK_impl_DEFAULT_DELIMIT PP_VK_impl_DEFAULT_DELIMIT_redirect


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
#define PP_VK_impl_IF_PP_VK_impl_ISOLATE_(...)
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
#define PP_VK_impl_RAW_STRING_redirect_2(...) u8R###__VA_ARGS__
#define PP_VK_impl_RAW_STRING_redirect PP_VK_impl_RAW_STRING_redirect_2
#define PP_VK_impl_RAW_STRING(...) PP_VK_impl_RAW_STRING_redirect(valk_STR_GUARD(__VA_ARGS__)valk_STR_GUARD)
#define PP_VK_impl_FAST_UNWRAP(...) __VA_ARGS__

#define PP_VK_impl_CONCAT_TUPLE(A, B) (PP_VK_impl_FAST_UNWRAP A VK_if(VK_not(VK_or(VK_pack_is_empty A, VK_pack_is_empty B))(,)) PP_VK_impl_FAST_UNWRAP B)
#define PP_VK_impl_TEST_BIND_UNWRAP_redirect_2(X) PP_VK_impl_TEST_UNWRAPPED_##X
#define PP_VK_impl_TEST_BIND_UNWRAP_redirect PP_VK_impl_TEST_BIND_UNWRAP_redirect_2
#define PP_VK_impl_TEST_BIND_UNWRAP(X) PP_VK_impl_TEST_BIND_UNWRAP_redirect(X)
#define PP_VK_impl_TEST_UNWRAPPED_PP_VK_impl_BIND_UNWRAP_ARGS
#define PP_VK_impl_BIND_UNWRAP_ARGS(...) BIND_UNWRAP_SUCCESS(__VA_ARGS__)
#define PP_VK_impl_TEST_UNWRAPPED_BIND_UNWRAP_SUCCESS(...) __VA_ARGS__
#define PP_VK_impl_BIND_TRY_UNWRAP(X) PP_VK_impl_TEST_BIND_UNWRAP(PP_VK_impl_BIND_UNWRAP_ARGS X)
#define PP_VK_impl_BIND_DISPATCH_CONCAT_redirect_2(A, B) A##B
#define PP_VK_impl_BIND_DISPATCH_CONCAT_redirect PP_VK_impl_BIND_DISPATCH_CONCAT_redirect_2
#define PP_VK_impl_BIND_DISPATCH_CONCAT(A, B) PP_VK_impl_BIND_DISPATCH_CONCAT_redirect(A, B)
#define PP_VK_impl_BIND_DISPATCH_redirect_2(Header, Count, ...) PP_VK_impl_BIND_DISPATCH_CONCAT(Header, Count)(__VA_ARGS__)
#define PP_VK_impl_BIND_DISPATCH_redirect PP_VK_impl_BIND_DISPATCH_redirect_2
#define PP_VK_impl_BIND_DISPATCH(Header, Macro, ...) PP_VK_impl_BIND_DISPATCH_redirect(Header, PP_VK_impl_BIND_DISPATCH_COUNT(PP_VK_impl_BIND_TRY_UNWRAP(Macro)), PP_VK_impl_BIND_TRY_UNWRAP(Macro), ##__VA_ARGS__)
#define PP_VK_impl_BIND_DISPATCH_GET_4TH_ITEM(a, b, c, d, ...) d
#define PP_VK_impl_BIND_DISPATCH_COUNT(...) PP_VK_impl_BIND_DISPATCH_GET_4TH_ITEM(__VA_ARGS__, _3, _2, _1, _0)
#define PP_VK_impl_BIND_FRONT_UNWRAPPED_1(Macro, ...) (Macro, (__VA_ARGS__), ())
#define PP_VK_impl_BIND_FRONT_UNWRAPPED_3(Macro, Front, Back, ...) (Macro, PP_VK_impl_CONCAT_TUPLE(Front, (__VA_ARGS__)), Back)
#define PP_VK_impl_BIND_BACK_UNWRAPPED_1(Macro, ...) (Macro, (), (__VA_ARGS__))
#define PP_VK_impl_BIND_BACK_UNWRAPPED_3(Macro, Front, Back, ...) (Macro, Front, PP_VK_impl_CONCAT_TUPLE((__VA_ARGS__), Back))
#define PP_VK_impl_LAZY_EVAL_MACRO_redirect_2(Macro, Args) Macro Args
#define PP_VK_impl_LAZY_EVAL_MACRO_redirect PP_VK_impl_LAZY_EVAL_MACRO_redirect_2
#define PP_VK_impl_LAZY_EVAL_MACRO(Macro, Args) PP_VK_impl_LAZY_EVAL_MACRO_redirect(Macro, Args)
#define PP_VK_impl_APPEND_BOUND_TO_BACK_redirect_2(Front, Back) PP_VK_impl_FAST_UNWRAP Front VK_if(VK_not(VK_or(VK_pack_is_empty Front, VK_pack_is_empty Back))(,)) PP_VK_impl_FAST_UNWRAP Back
#define PP_VK_impl_APPEND_BOUND_TO_BACK_redirect PP_VK_impl_APPEND_BOUND_TO_BACK_redirect_2
#define PP_VK_impl_APPEND_BOUND_TO_BACK(Front, Back) PP_VK_impl_APPEND_BOUND_TO_BACK_redirect(Front, Back)
#define PP_VK_impl_PP_INVOKE_WITH_BOUND_redirect_2(Macro, ...) Macro(__VA_ARGS__)
#define PP_VK_impl_PP_INVOKE_WITH_BOUND_redirect PP_VK_impl_PP_INVOKE_WITH_BOUND_redirect_2
#define PP_VK_impl_PP_INVOKE_WITH_BOUND(...) PP_VK_impl_PP_INVOKE_WITH_BOUND_redirect
#define PP_VK_impl_PP_INVOKE_UNWRAPPED_1(Macro, ...) Macro(__VA_ARGS__)
#define PP_VK_impl_PP_INVOKE_UNWRAPPED_3(Macro, Front, Back, ...) PP_VK_impl_LAZY_EVAL_MACRO(Macro, (PP_VK_impl_APPEND_BOUND_TO_BACK((PP_VK_impl_FAST_UNWRAP Front VK_if(VK_not(VK_or(VK_pack_is_empty Front, VK_pack_is_empty(__VA_ARGS__)))(,)) __VA_ARGS__), Back)))
#define PP_VK_impl_INVOKE_COUNT_ARGS_redirect_2(...) VK_count_args(__VA_ARGS__)
#define PP_VK_impl_INVOKE_COUNT_ARGS_redirect PP_VK_impl_INVOKE_COUNT_ARGS_redirect_2
#define PP_VK_impl_INVOKE_COUNT_ARGS(Macro) PP_VK_impl_INVOKE_COUNT_ARGS_redirect(VK_unwrap_inner(Macro))
#define PP_VK_impl_INVOKE_DISPATCH_GET_4TH_ITEM(a, b, c, d, ...) d
#define PP_VK_impl_INVOKE_DISPATCH_COUNT(...) PP_VK_impl_INVOKE_DISPATCH_GET_4TH_ITEM(__VA_ARGS__, _3, _2, _1, _0)
#define PP_VK_impl_TEST_INVOKE_UNWRAP_redirect_2(X) PP_VK_impl_TEST_UNWRAPPED_##X
#define PP_VK_impl_TEST_INVOKE_UNWRAP_redirect PP_VK_impl_TEST_INVOKE_UNWRAP_redirect_2
#define PP_VK_impl_TEST_INVOKE_UNWRAP(X) PP_VK_impl_TEST_INVOKE_UNWRAP_redirect(X)
#define PP_VK_impl_TEST_UNWRAPPED_PP_VK_impl_INVOKE_UNWRAP_ARGS
#define PP_VK_impl_INVOKE_UNWRAP_ARGS(...) INVOKE_UNWRAP_SUCCESS(__VA_ARGS__)
#define PP_VK_impl_TEST_UNWRAPPED_INVOKE_UNWRAP_SUCCESS(...) __VA_ARGS__
#define PP_VK_impl_INVOKE_TRY_UNWRAP(X) PP_VK_impl_TEST_INVOKE_UNWRAP(PP_VK_impl_INVOKE_UNWRAP_ARGS X)
#define PP_VK_impl_INVOKE_DISPATCH_CONCAT_redirect_2(A, B) A##B
#define PP_VK_impl_INVOKE_DISPATCH_CONCAT_redirect PP_VK_impl_INVOKE_DISPATCH_CONCAT_redirect_2
#define PP_VK_impl_INVOKE_DISPATCH_CONCAT(A, B) PP_VK_impl_INVOKE_DISPATCH_CONCAT_redirect(A, B)
#define PP_VK_impl_INVOKE_DISPATCH_redirect_2(Count, ...) PP_VK_impl_INVOKE_DISPATCH_CONCAT(PP_VK_impl_PP_INVOKE_UNWRAPPED, Count)(__VA_ARGS__)
#define PP_VK_impl_INVOKE_DISPATCH_redirect PP_VK_impl_INVOKE_DISPATCH_redirect_2
#define PP_VK_impl_INVOKE_DISPATCH(Macro, ...) PP_VK_impl_INVOKE_DISPATCH_redirect(PP_VK_impl_INVOKE_DISPATCH_COUNT(PP_VK_impl_INVOKE_TRY_UNWRAP(Macro)), PP_VK_impl_INVOKE_TRY_UNWRAP(Macro), ##__VA_ARGS__)

// Valkyrie Traits implementation
#define PP_VK_impl_EXPAND_TEMPLATE_PARAM_PACK_typename typename ...
#define PP_VK_impl_EXPAND_TEMPLATE_PARAM_PACK_auto auto ...
#define PP_VK_impl_EXPAND_TEMPLATE_PARAM_PACK_decltype(auto) decltype(auto) ...
#define PP_VK_impl_EXPAND_TEMPLATE_PARAM_PACK_size_t size_t ...

#define PP_VK_impl_EXPAND_TEMPLATE_PARAM_typename typename
#define PP_VK_impl_EXPAND_TEMPLATE_PARAM_auto auto
#define PP_VK_impl_EXPAND_TEMPLATE_PARAM_decltype(auto) decltype(auto)
#define PP_VK_impl_EXPAND_TEMPLATE_PARAM_size_t size_t
#define PP_VK_impl_EXPAND_TEMPLATE_PARAM_pack(arg) PP_VK_impl_EXPAND_TEMPLATE_PARAM_PACK_##arg

#define PP_VK_impl_EXPAND_TEMPLATE_ARG_typename
#define PP_VK_impl_EXPAND_TEMPLATE_ARG_auto
#define PP_VK_impl_EXPAND_TEMPLATE_ARG_decltype(auto)
#define PP_VK_impl_EXPAND_TEMPLATE_ARG_size_t
#define PP_VK_impl_EXPAND_TEMPLATE_ARG_pack(arg) PP_VK_impl_EXPAND_TEMPLATE_ARG_##arg ...

#define PP_VK_impl_EXPAND_TEMPLATE_ARGS_MACRO_redirect(arg) PP_VK_impl_EXPAND_TEMPLATE_ARG_##arg
#define PP_VK_impl_EXPAND_TEMPLATE_ARGS_MACRO PP_VK_impl_EXPAND_TEMPLATE_ARGS_MACRO_redirect
#define PP_VK_impl_EXPAND_TEMPLATE_PARAMS_MACRO_redirect(arg) PP_VK_impl_EXPAND_TEMPLATE_PARAM_##arg
#define PP_VK_impl_EXPAND_TEMPLATE_PARAMS_MACRO PP_VK_impl_EXPAND_TEMPLATE_PARAMS_MACRO_redirect
#define PP_VK_impl_EXPAND_TEMPLATE_ARGS_(...) VK_for_each_delimit(PP_VK_impl_EXPAND_TEMPLATE_ARGS_MACRO, VK_comma_delimiter, ##__VA_ARGS__)
#define PP_VK_impl_EXPAND_TEMPLATE_PARAMS_(...) VK_for_each_delimit(PP_VK_impl_EXPAND_TEMPLATE_PARAMS_MACRO, VK_comma_delimiter, ##__VA_ARGS__)
#define PP_VK_impl_EXPAND_TEMPLATE_ARGS(Args) PP_VK_impl_EXPAND_TEMPLATE_ARGS_ Args
#define PP_VK_impl_EXPAND_TEMPLATE_PARAMS(Args) PP_VK_impl_EXPAND_TEMPLATE_PARAMS_ Args


#define PP_VK_impl_TRAIT_CASE_STRUCT_LET(name_, args_, index_, x)                 \
  template <PP_VK_impl_EXPAND_TEMPLATE_PARAMS(args_)> requires PP_VK_impl_TRAIT_TAKE_REQUIREMENT(x)               \
  struct Trait<index_ + 1, PP_VK_impl_EXPAND_TEMPLATE_ARGS(args_)>{               \
     inline constexpr static decltype(auto) name_{PP_VK_impl_TRAIT_TAKE_VALUE(x)};\
  };                                                                              \
  template <PP_VK_impl_EXPAND_TEMPLATE_PARAMS(args_)>                             \
  struct Trait<index_ + 1, PP_VK_impl_EXPAND_TEMPLATE_ARGS(args_)>                \
      : Trait<index_, PP_VK_impl_EXPAND_TEMPLATE_ARGS(args_)>{};

#define PP_VK_impl_TRAIT_CASE_STRUCT_ALIAS(name_, args_, index_, x)               \
  template <PP_VK_impl_EXPAND_TEMPLATE_PARAMS(args_)> requires PP_VK_impl_TRAIT_TAKE_REQUIREMENT(x)               \
  struct Trait<index_ + 1, PP_VK_impl_EXPAND_TEMPLATE_ARGS(args_)>{               \
     using name_ = PP_VK_impl_TRAIT_TAKE_VALUE(x);                                \
  };                                                                              \
  template <PP_VK_impl_EXPAND_TEMPLATE_PARAMS(args_)>                             \
  struct Trait<index_ + 1, PP_VK_impl_EXPAND_TEMPLATE_ARGS(args_)>                \
      : Trait<index_, PP_VK_impl_EXPAND_TEMPLATE_ARGS(args_)>{};


#define PP_VK_impl_TRAIT_TAKE_REQUIREMENT_redirect_3(req, val) req
#define PP_VK_impl_TRAIT_TAKE_REQUIREMENT_redirect_2 PP_VK_impl_TRAIT_TAKE_REQUIREMENT_redirect_3
#define PP_VK_impl_TRAIT_TAKE_REQUIREMENT_redirect(...) PP_VK_impl_TRAIT_TAKE_REQUIREMENT_redirect_2(__VA_ARGS__)
#define PP_VK_impl_TRAIT_TAKE_REQUIREMENT(x) PP_VK_impl_TRAIT_TAKE_REQUIREMENT_redirect(PP_VK_impl_TRAIT_CASE_##x)

#define PP_VK_impl_TRAIT_TAKE_VALUE_redirect_3(req, val) VK_unwrap(val)
#define PP_VK_impl_TRAIT_TAKE_VALUE_redirect_2 PP_VK_impl_TRAIT_TAKE_VALUE_redirect_3
#define PP_VK_impl_TRAIT_TAKE_VALUE_redirect(...) PP_VK_impl_TRAIT_TAKE_VALUE_redirect_2(__VA_ARGS__)
#define PP_VK_impl_TRAIT_TAKE_VALUE(x) PP_VK_impl_TRAIT_TAKE_VALUE_redirect(PP_VK_impl_TRAIT_CASE_##x)

#define PP_VK_impl_TRAIT_BIND_given(...) (__VA_ARGS__)
#define PP_VK_impl_TRAIT_PARAMS_given(...) VK_for_each_delimit(PP_VK_impl_EXPAND_TEMPLATE_PARAMS_MACRO, VK_comma_delimiter, ##__VA_ARGS__)
#define PP_VK_impl_TRAIT_ARGS_given(...) VK_for_each_delimit(PP_VK_impl_EXPAND_TEMPLATE_ARGS_MACRO, VK_comma_delimiter, ##__VA_ARGS__)

#define PP_VK_impl_TRAIT_BODY_MACRO_let(name_) (PP_VK_impl_TRAIT_CASE_STRUCT_LET, (name_), ())
#define PP_VK_impl_TRAIT_BODY_MACRO_alias(name_) (PP_VK_impl_TRAIT_CASE_STRUCT_ALIAS, (name_), ())

#define PP_VK_impl_TRAIT_BODY_MACRO_redirect(Macro, args_, ...) VK_foreach_indexed(VK_bind_front(Macro, args_), ##__VA_ARGS__)
#define PP_VK_impl_TRAIT_BODY_MACRO PP_VK_impl_TRAIT_BODY_MACRO_redirect

#define PP_VK_impl_TRAIT_CASE_if(...) (__VA_ARGS__) ,
#define PP_VK_impl_TRAIT_CASE_typename(...) (requires{ typename __VA_ARGS__; }), (typename __VA_ARGS__)
#define PP_VK_impl_TRAIT_CASE_otherwise (true),
#define PP_VK_impl_TRAIT_CASE_member_type(name_, ...) (requires{ typename __VA_ARGS__::name_; }), (typename __VA_ARGS__::name_)
#define PP_VK_impl_TRAIT_CASE_member_value(name_, ...) (requires{ { __VA_ARGS__::name_ }; }), (__VA_ARGS__::name_)































// Public facing macro functions

#define VK_stringify(...) PP_VK_impl_STRINGIFY(__VA_ARGS__)

#define VK_get_arg_count PP_VK_impl_GET_COUNT_256
#define VK_count_args(...) PP_VK_impl_GET_32ND_MEM_IMPL(, ##__VA_ARGS__, _32, _31, _30, _29, _28, _27, _26, _25, _24, _23, _22, _21, _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9, _8, _7, _6, _5, _4, _3, _2, _1, _0)
//#define VK_pack_is_empty(...) PP_VK_impl_IS_PACK_EMPTY_IMPL(, ##__VA_ARGS__, PP_VK_impl_FALSE_32_TIMES, true)
#define VK_pack_is_empty(...) PP_VK_impl_PACK_IS_EMPTY(__VA_ARGS__)

#define VK_tuple_size(...) PP_VK_impl_TUPLE_SIZE(__VA_ARGS__)

#define VK_invoke(Macro, ...)    PP_VK_impl_INVOKE_DISPATCH(Macro, ##__VA_ARGS__)
#define VK_foreach_indexed(Macro, ...) PP_VK_impl_INVOKE_FOREACH_INDEXED(Macro, VK_default_delimiter, ##__VA_ARGS__)
#define VK_foreach_indexed_delimit(Macro, Delimiter, ...) PP_VK_impl_INVOKE_FOREACH_INDEXED(Macro, Delimiter, ##__VA_ARGS__)
#define VK_foreach(Macro, ...) PP_VK_impl_INVOKE_FOREACH(Macro, VK_default_delimiter, ##__VA_ARGS__)
#define VK_foreach_delimit(Macro, Delimiter, ...) PP_VK_impl_INVOKE_FOREACH(Macro, Delimiter, ##__VA_ARGS__)
#define VK_bind_front(Macro, ...) PP_VK_impl_BIND_DISPATCH(PP_VK_impl_BIND_FRONT_UNWRAPPED, Macro, ##__VA_ARGS__)
#define VK_bind_back(Macro, ...)  PP_VK_impl_BIND_DISPATCH(PP_VK_impl_BIND_BACK_UNWRAPPED, Macro, ##__VA_ARGS__)

#define VK_next(...) PP_VK_impl_NEXT_VALUE(__VA_ARGS__)
#define VK_previous(...) PP_VK_impl_PREVIOUS_VALUE(__VA_ARGS__)

#define VK_for_each(Macro, ...) VK_for_each_delimit(Macro, VK_default_delimiter, ##__VA_ARGS__)
#define VK_for_each_delimit(Macro, Delimiter, ...) VK_concat(PP_VK_impl_FOR_EACH, VK_count_args(__VA_ARGS__))(Macro, Delimiter, ##__VA_ARGS__)
#define VK_comma_delimiter PP_VK_impl_COMMA_DELIMIT
#define VK_default_delimiter PP_VK_impl_DEFAULT_DELIMIT

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

/*#define PP_VK_impl_TAGGED_MACRO(Tag) VK_##Tag
#define PP_VK_impl_TAGGED PP_VK_impl_TAGGED_MACRO
#define VK_tagged(...) VK_for_each(PP_VK_impl_TAGGED, ##__VA_ARGS__)*/

#define VK_param(...) VK_for_each(PP_VK_impl_PARAM, ##__VA_ARGS__)
#define PP_VK_impl_PARAM PP_VK_impl_PARAM_redirect
#define PP_VK_impl_PARAM_redirect(arg) PP_VK_impl_PARAM_##arg

#if defined(_WIN32)
#define PP_VK_impl_PARAM_in _In_
#define PP_VK_impl_PARAM_out _Out_
#define PP_VK_impl_PARAM_inout _Inout_
#define PP_VK_impl_PARAM_nonnull _Nonnull_
#define PP_VK_impl_PARAM_nullable _Maybenull_
#define PP_VK_impl_PARAM_in_string
#else
#define PP_VK_impl_PARAM_in
#define PP_VK_impl_PARAM_out
#define PP_VK_impl_PARAM_inout
#define PP_VK_impl_PARAM_nonnull _Nonnull
#define PP_VK_impl_PARAM_nullable _Nullable
#endif

#define PP_VK_impl_VAR_CONCAT_redirect(x, y, ...) x##y , ##__VA_ARGS__
#define PP_VK_impl_VAR_CONCAT PP_VK_impl_VAR_CONCAT_redirect
#define VK_variadic_concat(...) PP_VK_impl_VAR_CONCAT(__VA_ARGS__)

#define VK_fallthrough [[fallthrough]]
#define VK_nodiscard [[nodiscard]]
#define VK_noreturn [[noreturn]]
#define VK_empty_bases VK_if(VK_compiler_msvc(__declspec(empty_bases)))
#define VK_likely VK_if(VK_not(VK_compiler_clang)([[likely]]))
#define VK_unlikely VK_if(VK_not(VK_compiler_clang)([[unlikely]]))
#define VK_restrict __restrict
#if defined(__CUDACC__)
#define VK_kernel __global__ void
#define VK_gpu __host__ __device__
#define VK_inline __forceinline__
#elif defined(_MSC_VER)
#define VK_kernel void
#define VK_gpu
#define VK_inline __forceinline
#else
#define VK_kernel __attribute__((kernel)) void
#define VK_gpu
#define VK_inline inline
#endif
#define VK_gpu_inline VK_gpu VK_inline

#if defined(_CPPUNWIND) || defined(__EXCEPTIONS)
#define VK_exceptions_enabled true
#else
#define VK_exceptions_enabled false
#endif

#define VK_noexcept VK_if(VK_exceptions_enabled(noexcept))


#if defined(NDEBUG)
#define VK_debug_build false
#else
#define VK_debug_build true
#endif

#if defined(_WIN64) || defined(_WIN32)
#define VK_system_windows true
#define VK_system_linux false
#define VK_system_macos false
#if defined(_WIN64)
#define VK_64bit true
#define VK_32bit false
#else
#define VK_64bit false
#define VK_32bit true
#endif
#else
#define VK_system_windows false
#if defined(__LP64__)
#define VK_64bit true
#define VK_32bit false
#else
#define VK_64bit false
#define VK_32bit true
#endif
#if defined(__linux__)

#define VK_system_linux true
#define VK_system_macos false
#elif defined(__OSX__)
#define VK_system_linux false
#define VK_system_macos true
#else
#define VK_system_linux false
#define VK_system_macos false
#endif
#endif


#if defined(_MSC_VER)
#define VK_compiler_msvc true
#else
#define VK_compiler_msvc false
#endif


#if defined(__clang__)
#define VK_compiler_clang true
#else
#define VK_compiler_clang false
#endif
#if defined(__GNUC__)
#define VK_compiler_gcc true
#else
#define VK_compiler_gcc false
#endif


#if defined(__CUDACC__)
#define VK_language_cuda true
#else
#define VK_language_cuda false
#endif

#if defined(__cplusplus)
#define VK_language_cxx true
#define VK_language_c false
#else
#define VK_language_cxx false
#define VK_language_c true
#endif

#if VK_language_cxx
#define VK_std_version VK_if(VK_compiler_msvc(_MSVC_LANG)VK_else(__cplusplus))
#if VK_std_version >= 201103L
#define VK_cxx11 true
#if VK_std_version >= 201402L
#define VK_cxx14 true
#if VK_std_version >= 201703L
#define VK_cxx17 true
#if VK_std_version > 201703L
#define VK_cxx20 true
#else
#define VK_cxx20 false
#endif
#else
#define VK_cxx17 false
#define VK_cxx20 false
#endif
#else
#define VK_cxx14 false
#define VK_cxx17 false
#define VK_cxx20 false
#endif
#else
#define VK_cxx11 false
#define VK_cxx14 false
#define VK_cxx17 false
#define VK_cxx20 false
#endif
#endif




#define VK_begin_c_namespace VK_if(VK_language_cxx(extern "C" {) VK_else())
#define VK_end_c_namespace VK_if(VK_language_cxx(})VK_else())

#define VK_function_name VK_if(VK_compiler_msvc(__FUNCSIG__)VK_else(__PRETTY_FUNCTION__))
#define VK_filename __FILE__
#define VK_line_number __LINE__
#define VK_has_include(...) __has_include(__VA_ARGS__)
//#define VK_raw_string(...) PP_VK_impl_STRING(""valk_GUARD(__VA_ARGS__)"valk_GUARD")
#define VK_raw_string(...) PP_VK_impl_RAW_STRING(__VA_ARGS__)
#define VK_string(...) u8##__VA_ARGS__
#define VK_empty_string VK_string("")
#define VK_constexpr_error(Msg) VK_if(VK_and(VK_compiler_msvc, VK_not(VK_compiler_clang))(throw Vk::ConstantEvaluationException(Msg))VK_else((void)(Msg, 1 / 0)))


#define VK_throws VK_if(VK_exceptions_enabled()VK_else(noexcept))

#define VK_constant inline constexpr static
#define VK_consteval_block if(std::is_constant_evaluated())
#define VK_runtime_block if(!std::is_constant_evaluated())



#define VK_pragma(...) VK_if(VK_and(VK_compiler_msvc, VK_not(VK_compiler_clang))(__pragma(__VA_ARGS__))VK_else_if(VK_compiler_clang(_Pragma(("clang " VK_stringify(__VA_ARGS__))))VK_else(_Pragma("GCC " VK_stringify(__VA_ARGS__)))));
#define VK_msvc_pragma(...) VK_if(VK_compiler_msvc(__pragma(__VA_ARGS__)))
#define VK_diagnostic_push VK_if(VK_compiler_msvc(__pragma(warning(push)))VK_else_if(VK_compiler_clang(_Pragma("clang diagnostic push"))VK_else(_Pragma("GCC diagnostic push"))));
#define VK_msvc_warning(...) VK_if(VK_compiler_msvc(__pragma(warning(__VA_ARGS__));))
#define VK_diagnostic_pop VK_if(VK_compiler_msvc(__pragma(warning(pop)))VK_else_if(VK_compiler_clang(_Pragma("clang diagnostic pop"))VK_else(_Pragma("GCC diagnostic pop"))));




#define VK_compiler_print(Msg) VK_pragma(VK_if(VK_and(VK_compiler_msvc, VK_not(VK_compiler_clang))(message(Msg))VK_else(message Msg)))

#define VK_inspect_macro(...) VK_compiler_print(VK_stringify(Macro expression #__VA_ARGS__ evaluates to VK_stringify(__VA_ARGS__)))

#include <cassert>


#define VK_unreachable VK_if(VK_and(VK_compiler_msvc, VK_not(VK_compiler_clang))(__assume(0))VK_else(__builtin_unreachable()))
#define VK_assert_bool(Expr, Msg) VK_if(VK_not(VK_debug_build)(true)VK_else_if(VK_compiler_msvc(((bool)(VK_unwrap_inner(Expr)) || ((_wassert(L"" Msg, L"" VK_filename, unsigned(VK_line_number))), false)))VK_else((static_cast<bool>(Expr) ? (void)0 : __assert_fail(Msg, VK_filename, VK_line_number, VK_function_name)))))
#define VK_assert_msg(Expr, Msg) (void)(VK_assert_bool(Expr, Msg))
#define VK_assert(Expr) VK_assert_msg(Expr, #Expr)
#define VK_assume(Expr) VK_if(VK_compiler_msvc(__assume(Expr))VK_else_if(VK_compiler_clang(__builtin_assume(Expr))))
#define VK_axiom(...) { bool __value_of_axiom(__VA_ARGS__); (void)(VK_assert_bool(__value_of_axiom, #__VA_ARGS__)); VK_assume(__value_of_axiom); }
#define VK_no_default default: VK_unreachable


#define VK_constexpr_assert_msg(Expr, Msg) VK_consteval_block{ VK_diagnostic_push VK_msvc_warning(disable:4297) if(!(VK_unwrap(Expr))) VK_constexpr_error(Msg); VK_diagnostic_pop } else { VK_assert_msg(VK_unwrap(Expr), Msg); } (void)0
#define VK_constexpr_assert(...) VK_constexpr_assert_msg((__VA_ARGS__), #__VA_ARGS__)

#if VK_compiler_msvc
#include <exception>

namespace Vk{
  class ConstantEvaluationException : public std::exception{
  public:
    template <size_t N>
    constexpr explicit ConstantEvaluationException(const char(&Msg)[N]) noexcept : std::exception(){}

    VK_nodiscard const char * what() const override{ return nullptr; }
  };
}
#endif


#endif//VALKYRIE_CONFIG_HPP
