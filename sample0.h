/*
 *
 * irrBM (Irrlicht Benchmark)
 * Copyright (C) 2022 TheMrCerebro
 *
 * This file is part of irrBM.
 *
 * irrBM is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * irrBM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/
#ifndef SAMPLE0_H_INCLUDED
#define SAMPLE0_H_INCLUDED

//===============================================
// Apollian with a twist
// Creado por mrange en 2021-02-03
// https://www.shadertoy.com/view/Wl3fzM
//===============================================

namespace sample0
{

#define TAU             (2.0f*PI)
#define L2(x)           dot(x, x)
#define PSIN(x)         (0.5f+0.5f*sin(x))
#define lp1             vec3(0.5f, 1.25f, 0.5f)
#define lp2             vec3(-0.5f, 1.25f, 0.5f)
#define ss              15.0f
#define zoom            0.5f
#define K               vec4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f)

vec2 rot2d(vec2 p, f32 r)
{
    return vec2(p.v[0] * cos(r) - p.v[1] * sin(r),p.v[0] * sin(r) + p.v[1] * cos(r));
}

vec3 hsv2rgb(vec3 c)
{
  vec3 p = abs(fract(vec3(c.v[0]) + vec3(K.v[0],K.v[1],K.v[2])) * 6.0f - vec3(K.v[3]));
  return c.v[2] * mix(vec3(K.v[0]), clamp(p - vec3(K.v[0]), 0.0f, 1.0f), c.v[1]);
}

f32 apollian(vec4 p, f32 s)
{
    f32 scale = 1.0f;

    for(u32 i=0; i<7; ++i)
    {
        p= -1.0f + 2.0f * fract(0.5f * p + 0.5f);
        f32 r2 = dot(p,p);
        f32 k = s/r2;
        p *= k;
        scale *= k;
    }

    return abs(p.v[1])/scale;
}

f32 weird(vec2 p)
{
    f32 z = 4.0f, r = 0.5f;
    f32 tm = 0.2f * iTime;

    vec4 off = vec4(r * PSIN(tm * sqrt(3.0f)), r * PSIN(tm * sqrt(1.5f)), r * PSIN(tm * sqrt(2.0f)), 0.0f);

    p = rot2d(p, iTime*0.1);
    vec4 pp = vec4(p.v[0], p.v[1], 0.0f, 0.0f) + off;
    pp.v[3] = 0.125f * (1.0f-tanh(length(vec3(pp.v[0],pp.v[1],pp.v[2]))));

    vec2 ppyz = rot2d(vec2(pp.v[1],pp.v[2]), tm);
    pp.v[1] = ppyz.v[0];
    pp.v[2] = ppyz.v[1];

    vec2 ppxz = rot2d(vec2(pp.v[0],pp.v[2]),tm * sqrt(0.5f));
    pp.v[0] = ppxz.v[0];
    pp.v[2] = ppxz.v[1];

    pp /= z;
    f32 d = apollian(pp, 1.2f);
    return d*z;
}

f32 df(vec2 p)
{
  p /= zoom;
  f32 d0 = weird(p);
  return d0*zoom;
}

vec3 postProcess(vec3 col, vec2 q)
{
  col = pow(clamp(col, 0.0f, 1.0f),vec3(1.0f/2.2f));
  col = col * 0.6f + 0.4f * col;
  col = mix(col, vec3(dot(col, vec3(0.33f))), -0.4f);  // saturation
  col *= 0.5f + 0.5f * pow(19.0f * q.v[0] * q.v[1] * (1.0f-q.v[0]) * (1.0f-q.v[1]), 0.7f);  // vigneting
  return col;
}

vec3 test(u32 x, u32 y)
{
    vec2 q = vec2(x,y) / iResolution;
    vec2 p = -1.0f + 2.0f * q;
    p.v[0] *= iResolution.v[0]/iResolution.v[1];

    f32 aa = 2.0f/iResolution.v[1];

    f32 d = df(p);

    f32 b = -0.125f;
    f32 t = 10.0f;

    vec3 ro = vec3(0.0f, t, 0.0f);
    vec3 pp = vec3(p.v[0], 0.0f, p.v[1]);

    vec3 rd = normalize(pp - ro);

    f32 bt = -(t-b)/rd.v[1];

    vec3 bp   = ro + bt * rd;

    vec3 srd1 = normalize(lp1-bp);
    vec3 srd2 = normalize(lp2-bp);
    f32 bl21 = L2(lp1-bp);
    f32 bl22 = L2(lp2-bp);

    f32 st1 = -b/srd1.v[1];

    vec3 sp1 = bp + srd1 * st1;
    vec3 sp2 = bp + srd2 * st1;
    f32 sd1 = df(vec2(sp1.v[0], sp1.v[2]));
    f32 sd2 = df(vec2(sp2.v[0], sp2.v[2]));

    vec3 col = vec3(1.0f) * (1.0f - exp(-ss * (max(sd1, 0.0f)))) / bl21;
    col += vec3(0.5f) * (1.0f - exp(-ss * (max(sd2, 0.0f)))) / bl22;

    f32 l = length(p);
    f32 hue = fract(0.75f * l - 0.3f * iTime) + 0.3f + 0.15f;
    f32 sat = 0.75f * tanh(2.0f * l);
    vec3 hsv = vec3(hue, sat, 1.0f);
    vec3 bcol = hsv2rgb(hsv);
    col *= (1.0f - tanh(0.75f * l)) * 0.5f;
    col = mix(col, bcol, smoothstep(-aa, aa, -d));
    col += 0.5f * sqrt(vec3(bcol.v[2], bcol.v[0], bcol.v[1])) * (exp(-(10.0f + 100.0f * tanh(l)) * max(d, 0.0f)));

    col = postProcess(col, q);

    return col;
}

}

#endif // SAMPLE0_H_INCLUDED
