/*
 *
 * Copyright (c) 2022 TheMrCerebro
 *
 * irrBM - Zlib license.
 *
 * This software is provided 'as-is', without any express or
 * implied warranty. In no event will the authors be held
 * liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented;
 * you must not claim that you wrote the original software.
 * If you use this software in a product, an acknowledgment
 * in the product documentation would be appreciated but
 * is not required.
 *
 * 2. Altered source versions must be plainly marked as such,
 * and must not be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any
 * source distribution.
 *
*/

#ifndef SAMPLE9_H_INCLUDED
#define SAMPLE9_H_INCLUDED

//===============================================
// pemptos
// Creado por lorenzowy en 2021-12-07
// https://www.shadertoy.com/view/stdXz7
//===============================================

namespace sample8
{

vec2 rot2d(vec2 p, f32 r)
{
    return vec2(p.v[0] * cos(r) - p.v[1] * sin(r), p.v[0] * sin(r) + p.v[1] * cos(r));
}

vec2 pmod(vec2 p, f32 r)
{
    f32 a = atan(p.v[0], p.v[1]) + PI/r;
    f32 n = (PI*2.0f) / r;
    a = floor(a/n) * n;
    return vec2(p.v[0] * cos(-a) + p.v[1] * sin(-a),-(p.v[0] * sin(-a)) + p.v[1] * cos(-a));
}

f32 box( vec3 p, vec3 b )
{
    vec3 d = abs(p) - b;
    return min(max(d.v[0], max(d.v[1], d.v[2])), 0.0f) + length(max(d, -0.1f));
}

f32 ifsBox(vec3 p)
{
    vec2 rot;
    for (u32 i=1; i<3; i++)
    {
        p = abs(p) - 1.0f;

        rot = rot2d(vec2(p.v[0],p.v[1]), iTime * 0.3f);
        p.v[0] = rot.v[0];
        p.v[1] = rot.v[1];

        rot = rot2d(vec2(p.v[0],p.v[2]), iTime * 0.1f);
        p.v[0] = rot.v[0];
        p.v[2] = rot.v[1];
    }

    rot = rot2d(vec2(p.v[0],p.v[2]), iTime);
    p.v[0] = rot.v[0];
    p.v[2] = rot.v[1];

    return box(p, vec3(0.4f, 0.8f, 0.3f));
}

f32 map(vec3 p, vec3 cPos)
{
    vec3 p1 = p;
    p1.v[0] = mod(p1.v[0] - 5.0f, 10.0f) - 5.0f;
    p1.v[1] = mod(p1.v[1] - 5.0f, 10.0f) - 5.0f;
    p1.v[2] = mod(p1.v[2], 16.0f) - 8.0f;

    vec2 p1xy = pmod(vec2(p1.v[0], p1.v[1]), 5.0f);
    p1.v[0] = p1xy.v[0];
    p1.v[1] = p1xy.v[1];

    return ifsBox(p1);
}

vec3 test(u32 x, u32 y)
{
    vec2 p = (vec2(x,y) * 2.0f - iResolution) / min(iResolution.v[0], iResolution.v[1]);

    vec3 cPos = vec3(0.0f, 0.0f, -3.0f * iTime);
    vec3 cDir = normalize(vec3(0.0f, 0.0f, -1.0f));
    vec3 cUp  = vec3(cos(iTime), 1.0f, 0.0f);
    vec3 cSide = cross(cDir, cUp);
    vec3 ray = normalize(cSide * p.v[0] + cUp * p.v[1] + cDir);

    f32 acc = 0.0f, acc2 = 0.0f, t = 0.0f;
    for (u32 i=0; i<99; i++)
    {
        vec3 pos = cPos + ray * t;
        f32 dist = map(pos, cPos);
        dist = max(abs(dist), 0.02f);
        f32 a = exp(-dist * 3.0f);
        if (mod(length(pos) + 24.0f * iTime, 30.0f) < 3.0f)
        {
            a *= 2.0f;
            acc2 += a;
        }
        acc += a;
        t += dist * 0.5f;
    }

    vec3 col = vec3(acc * 0.01f, acc * 0.011f + acc2 * 0.01f, acc * 0.012f + acc2 * 0.015f);

    return col;
}

}

#endif // SAMPLE9_H_INCLUDED
