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

#ifndef SAMPLE10_H_INCLUDED
#define SAMPLE10_H_INCLUDED

//===============================================
// Fork Fork fracr
// Creado por NikolaErceg en 2021-05-16
// https://www.shadertoy.com/view/NsjSDy
//===============================================

namespace sample9
{

vec2 rot2d(vec2 p, f32 r)
{
    return vec2(p.v[0] * cos(r) + p.v[1] * sin(r), -(p.v[0] * sin(r)) + p.v[1] * cos(r));
}

vec3 palette(f32 d)
{
	return mix(vec3(2.0f, 2.02f, 1.24f), vec3(0.0f, 0.01f, 1.2f), d);
}

f32 map(vec3 p)
{
    for(u32 i=0; i<43; ++i)
    {
        f32 t = iTime * 0.01f;

        vec2 pxz = rot2d(vec2(p.v[0], p.v[2]), t);
        p.v[0] = pxz.v[0];
        p.v[2] = pxz.v[1];

        vec2 pxy = rot2d(vec2(p.v[0], p.v[1]), t * 5.0f);
        p.v[0] = pxy.v[0];
        p.v[1] = pxy.v[1];

        vec2 pxza = abs(vec2(p.v[0],p.v[2]));
        p.v[0] = pxza.v[0];
        p.v[2] = pxza.v[1];

        p.v[0] -= 0.11f;
        p.v[2] -= 0.11f;

	}
	return dot(sign(p), p) / 6.0f;
}

vec3 rm(vec3 ro, vec3 rd)
{
    f32 t = 1.0f;
    vec3 col = vec3(0.006f);
    f32 d;
    for(u32 i=0; i<36; i++)
    {
		vec3 p = ro + rd * t;
        d = map(p) * 0.98f;

        if(d<0.07f)
            break;

        if(d>22.0f)
        	break;

        col += palette(length(p) * 0.1f) / (222.0f * (d));
        t += d;
    }
    return col;
}

vec3 test(u32 x, u32 y)
{
    vec2 uv = (vec2(x, y) - (iResolution / 2.0f)) / iResolution.v[0];
	vec3 ro = vec3(55.0f, 55.0f, -13.0f);
    vec2 roxz = rot2d(vec2(ro.v[0], ro.v[2]), iTime);
    ro.v[0] = roxz.v[0];
    ro.v[2] = roxz.v[1];
    vec3 cf = normalize(-ro);
    vec3 cs = normalize(cross(cf, vec3(-17.0f, 0.0f, 17.0f)));
    vec3 cu = normalize(cross(cf, cs));

    vec3 uuv = ro + cf * 3.0f + uv.v[0] * cs + uv.v[1] * cu;

    vec3 rd = normalize(uuv - ro);

    return rm(ro, rd);
}

}

#endif // SAMPLE10_H_INCLUDED
