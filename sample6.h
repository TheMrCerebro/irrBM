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

#ifndef SAMPLE7_H_INCLUDED
#define SAMPLE7_H_INCLUDED

//===============================================
// RME4 - Crater
// Creado por patu en 2021-03-31
// https://www.shadertoy.com/view/MlSBDt
//===============================================

namespace sample6
{


f32 hash12(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1f, 311.7f))) * 43758.5453123f);
}

f32 fbm(vec3 x)
{
    f32 r = 0.0f, w = 1.0f, s = 1.0f;
    for (u32 i=0; i<7; i++)
    {
        w *= 0.5f;
        s *= 2.0f;

        vec3 g = floor(s * x);
        vec3 f = fract(s * x);
        vec3 u = f * f * (vec3(3.0f) - vec3(2.0f) * f);

        vec2 ii = vec2(g.v[0],g.v[1]) + g.v[2] * vec2(5.0f);
        f32 a = hash12( ii + vec2(0.0f, 0.0f) );
        f32 b = hash12( ii + vec2(1.0f, 0.0f) );
        f32 c = hash12( ii + vec2(0.0f, 1.0f) );
        f32 d = hash12( ii + vec2(1.0f, 1.0f) );
        f32 v1 = mix(mix(a, b, u.v[0]), mix(c, d, u.v[0]), u.v[1]);

        ii += vec2(5.0f);
        a = hash12( ii + vec2(0.0f, 0.0f) );
        b = hash12( ii + vec2(1.0f, 0.0f) );
        c = hash12( ii + vec2(0.0f, 1.0f) );
        d = hash12( ii + vec2(1.0f, 1.0f) );
        f32 v2 = mix(mix(a, b, u.v[0]), mix(c, d, u.v[0]), u.v[1]);

        r += w * max(mix(v1, v2, u.v[2]), 0.0f);
    }
    return r;
}

f32 saturate(f32 a)
{
    return clamp(a, 0.0f, 1.0f);
}

f32 smin( f32 a, f32 b, f32 k )
{
    f32 res = exp( -k*a ) + exp( -k*b );
    return -log( res )/k ;
}

struct geometry
{
    f32 dist;
    f32 specular;
    f32 diffuse;
    vec3 color;
};

geometry scene2(vec3 p)
{
    geometry plane;

    f32 localNoise = fbm(p / 10.0f) * 2.0f;
    p.v[1] -= localNoise * 0.2f;
    plane.dist = p.v[1];
    p.v[1] *= 3.5f;

    plane.dist = smin(plane.dist, length(p) - 25.0f, 0.15f + localNoise * 0.2f);
    plane.dist = max(plane.dist, -length(p) + 29.0f  + localNoise);

    plane.color = vec3(1.0f, 0.2f, 0.0f);
    plane.diffuse = 0.0f;
    plane.specular = 22.1f;

    return plane;
}

geometry raymarch(vec3 o, vec3 d)
{
    geometry mp;

    f32 l = -0.1f;

    for (u32 i=0; i<30; i++)
    {
        if (abs(l) < 0.1f || l > 130.0f)
            break;

        mp = scene2(o + d * l);
        l += mp.dist;
    }

    mp.dist = l;
    return mp;
}

f32 softShadow(vec3 ro, vec3 lp, f32 k)
{
    vec3 rd = (lp - ro);

    f32 shade = 1.0f;
    f32 dist = 1.0f;
    f32 end = max(length(rd), 0.01f);
    f32 stepDist = end / 125.0f;

    f32 tb = (8.0f - ro.v[1]) / normalize(rd).v[1];
    if( tb>0.0f )
        end = min( end, tb );

    rd /= end;
    for (u32 i=0; i<125; i++)
    {
        f32 h = scene2(ro + rd * dist).dist;
        shade = min(shade, smoothstep(0.0f, 1.0f, k * h / dist));
        dist += min(h, stepDist * 2.0f);
        if (h < 0.001f || dist > end)
            break;
    }
    return min(max(shade, 0.3f), 1.0f);
}

vec3 getNormal(vec3 pos)
{
	f32 d = scene2(pos).dist;
	return normalize(vec3(scene2( pos + vec3(0.001f, 0.0f, 0.0f)).dist - d, scene2(pos + vec3(0.0f, 0.001f, 0.0f)).dist - d, scene2(pos + vec3(0.0f, 0.0f, 0.001f)).dist - d));
}

f32 getAO(vec3 hitp, vec3 normal, f32 dist)
{
    f32 sdist = scene2(hitp + normal * dist).dist;
    return clamp(sdist / dist, 0.4f, 1.0f);
}

vec3 Sky(vec3 rd, bool showSun, vec3 lightDir)
{
    vec3 lightColour = normalize(vec3(1.8f, 1.0f, 0.3f));
   	f32 sunSize = 1.0f;
   	f32 sunAmount = max(dot(rd, lightDir), 0.1f);
   	f32 v = pow(1.2f - max(rd.v[1], 0.5f), 1.1f);
   	vec3 sky = mix(vec3(1.0f, 0.78f, 0.39f), vec3(1.1f, 1.2f, 1.3f) / 10.0f, v);
   	sky += lightColour * sunAmount * sunAmount * 1.0f + lightColour * min(pow(sunAmount, 1e4)* sunSize, 1233.0f * sunSize);
   	return clamp(sky, 0.0f, 1.0f);
}

vec3 doColor(vec3 sp, vec3 rd, vec3 sn, vec3 lp, geometry obj)
{
    vec3 ld = sp + lp - sp;
    ld /= max(length(ld / 2.0f), 0.001f);
	f32 diff = max(dot(sn, ld), obj.diffuse);
    f32 spec = max(dot(reflect(-ld, sn), -rd), obj.specular / 2.0f);
    return (obj.color * (diff + 0.15f) * spec * 0.1f);
}

vec3 test(u32 x, u32 y)
{
    vec2 uv = vec2(x,y) / iResolution - 0.5f;
    uv.v[1] *= 1.2f;

    vec3 light = vec3(0.5f, 7.0f, 100.0f);
    vec3 ro = vec3(cos(-iTime * 0.1f) * 48.0f, 18.0f, sin(-iTime * 0.1f) * 48.0f);
    vec3 vpn = normalize(-ro);

    vec3 u = normalize(cross(vec3(0.0f, 1.0f, 0.0f), vpn));
    vec3 v = cross(vpn, u);
    vec3 vcv = ro + vpn;
    vec3 scrCoord = (vcv + uv.v[0] * u * iResolution.v[0]/iResolution.v[1] + uv.v[1] * v);
    vec3 rd = normalize(scrCoord - ro);
    vec3 sky = Sky(rd, true, normalize(light)) * 1.0f;

    vec3 sceneColor;

    geometry tr = raymarch(ro, rd);
    if (tr.dist < 512.0f)
    {
        vec3 hit = ro + rd * tr.dist;
        vec3 sn = getNormal(hit);
        sceneColor = (doColor(hit, rd, sn, light, tr) * 1.0f) * 1.0f;
        sceneColor *= getAO(hit, sn, 10.2f);
        sceneColor *= softShadow(hit, hit + light, 8.2f);
        sceneColor = mix(sceneColor, sky, saturate(tr.dist * 4.5f / 512.0f));
    }
    else
        sceneColor = sky;

    vec3 col = vec3(clamp(sceneColor * (1.0f - length(uv) / 3.5f), 0.0f, 1.0f));
    return pow(col, 1.0f / vec3(1.2f));
}

}

#endif // SAMPLE7_H_INCLUDED
