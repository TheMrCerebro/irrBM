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

#ifndef SAMPLE8_H_INCLUDED
#define SAMPLE8_H_INCLUDED

//===============================================
// PsychedelicSakura
// Creado por Reva en 2020-02-21
// https://www.shadertoy.com/view/wlGXRD
//===============================================

namespace sample7
{

f32 plot(f32 r, f32 pct)
{
    return smoothstep( pct - 0.2f, pct, r) - smoothstep( pct, pct + 0.2f, r);
}

vec3 pal( f32 t, vec3 a, vec3 b, vec3 c, vec3 d )
{
    return a + b * cos( 6.28318f * (c*t+d) );
}

vec3 test(u32 x, u32 y)
{
    vec2 uv = vec2(x,y)/iResolution;
    vec3 col = vec3(1.0f);
    vec2 pos = vec2(0.5f) - uv;

    pos.v[0] *= iResolution.v[0] / iResolution.v[1];
    pos *= cosf(iTime) * 1.0f + 1.5f;

    f32 r = length(pos) * 2.0f;
    f32 a = atan(pos.v[1], pos.v[0]);

    f32 f = abs(cos(a * 2.5f + iTime * 0.5f)) * sin(iTime * 2.0f) * 0.698f + cos(iTime) - 4.0f;
    f32 d = f-r;

    col = (vec3(smoothstep(fract(d), fract(d) + -0.200f, 0.160f)) - vec3(smoothstep(fract(d), fract(d) + -1.184f, 0.160f)) )
	* pal(f, vec3(0.725f, 0.475f, 0.440f), vec3(0.605f, 0.587f, 0.007f), vec3(1.0f), vec3(0.310f, 0.410f, 0.154f) );

    f32 pct = plot(r * 0.272f, fract(d * (sin(iTime) * 0.45f + 0.5f)));

    col += pct * pal(r, vec3(0.750f, 0.360f, 0.352f), vec3(0.450f, 0.372f, 0.271f), vec3(0.540f, 0.442f, 0.264f), vec3(0.038f, 0.350f, 0.107f) );

    return col;
}

}

#endif // SAMPLE8_H_INCLUDED
