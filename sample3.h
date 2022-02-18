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

#ifndef SAMPLE3_H_INCLUDED
#define SAMPLE3_H_INCLUDED

//===============================================
// Mobius trans in hyper 3-space
// Creado por neozhaoliang en 2018-05-09
// https://www.shadertoy.com/view/4scfR2
//===============================================

namespace sample3
{

#define E_  2.7182818285f

// Raymarching constants
#define MIN_TRACE_DIST   0.01f
#define MAX_TRACE_STEPS  255
#define PRECISION        1e-5

// grid and cone size
#define polar_grid vec2(0.4f, PI / 7.0f)
#define cone_angle normalize(vec2(1.5f, 1.0f));

// Intensity constants
#define intensity_divisor 40000.0f
#define intensity_factor_max 7.2f
#define center_intensity 12.0f
#define ppow 1.9f

// Color constants
#define center_hue 0.5f
#define center_sat 0.18f

// shape constants
#define dist_factor 3.0f
#define strong_factor 7.0f
#define weak_factor 1.0f
#define star_hv_factor vec2(30.0f, 1.0f)
#define star_diag_factor vec2(30.0f, 1.0f)

// hsv to rgb conversion
vec3 hsv2rgb(vec3 hsv)
{
    const vec3 p = vec3(0.0f, 2.0f/3.0f, 1.0f/3.0f);
    vec2 hsvyz = clamp(vec2(hsv.v[1],hsv.v[2]), 0.0f, 1.0f);
    hsv.v[1] = hsvyz.v[0];
    hsv.v[2] = hsvyz.v[1];
    return hsv.v[2] * (0.63f * hsv.v[1] * (cos(2.0f * PI * (hsv.v[0] + p)) - 1.0f) + 1.0f);
}

// 2d rotation
vec2 rot2d(vec2 p, f32 a) { return cos(a) * p + sin(a) * vec2(p.v[1], -p.v[0]); }

// 2d polar grids
vec2 polarGrid(vec2 p, vec2 size)
{
    f32 theta = atan2(p.v[1], p.v[0]);
    f32 r = log(length(p));
    return mod(vec2(r, theta) + 0.5f * size, size) - 0.5f * size;
}

//Quaternion arithmetic
vec4 qmul(vec4 p, vec4 q)
{
    return vec4(p.v[0] * q.v[0] - dot(vec3(p.v[1],p.v[2],p.v[3]), vec3(q.v[1],q.v[2],q.v[3])), p.v[0] * vec3(q.v[1],q.v[2],q.v[3]) + q.v[0] * vec3(p.v[1],p.v[2],p.v[3]) + cross(vec3(p.v[1],p.v[2],p.v[3]), vec3(q.v[1],q.v[2],q.v[3])));
}

// Apply Mobius transformation on upper half space as quaternions
f32 applyMobius(vec3 p)
{
    vec4 pt = vec4(p, 0.0);

    vec4 p1 = qmul(vec4(vec2(-1.0f, 0.0f), 0.0f, 0.0f), pt) + vec4(vec2(1.2f, 0.0f), 0.0f, 0.0f);
    vec4 p2 = qmul(vec4(vec2(-1.0f, 0.0f), 0.0f, 0.0f), pt) + vec4(vec2(-1.2f, 0.0f), 0.0f, 0.0f);

    vec4 xyz = qmul(p1, vec4(p2.v[0], -vec3(p2.v[1],p2.v[2],p2.v[3])) / dot(p2, p2));
    p.v[0] = xyz.v[0];
    p.v[1] = xyz.v[1];
    p.v[2] = xyz.v[2];
    f32 scale = length(p);
    return scale > 1.0f ? 1.0f / scale : scale;
}

// A Mobius transformation of hyperbolic type is conjugate to a pure scaling
void trans_hyperbolic(vec2 p)
{
    f32 d = log(length(p)) - (iTime * 0.5f) * polar_grid.v[0];
    d = mod(d + 0.5f * polar_grid.v[0], polar_grid.v[0]) - 0.5f * polar_grid.v[0];
    p = normalize(p) * pow(E_,d);
}

// A Mobius transformation of elliptic type is conjugate to a pure rotation
void trans_elliptic(vec2 p)
{
    p = rot2d(p, (iTime * 0.5f) * polar_grid.v[1]);
}

// signed distance function for sphere kissing at y=0 with radius r
f32 sdSphere2(vec3 p, f32 r)
{
    p.v[1] -= r;
    return length(p) - r;
}

// signed distance function for parabolic case
f32 sdScene1(vec3 p)
{
    return min(p.v[1], sdSphere2(p, 1.0f));
}

vec3 getColor(vec2 p, f32 pint)
{
    f32 sat = 0.75f / pow(pint, 2.5f) + center_sat;
    f32 hue2 = (iTime * 0.3f) - log(length(p)) / 7.0f;
    f32 hue = center_hue + hue2;
    return hsv2rgb(vec3(hue, sat, pint)) + pint / 3.0f;
}

f32 getIntensity2(vec2 p)
{
    f32 angle = atan2f(polar_grid.v[0], polar_grid.v[1]);
    f32 dist  = length(p);
    f32 disth = length(p * star_hv_factor);
    f32 distv = length(p * vec2(star_hv_factor.v[1],star_hv_factor.v[0]));

    vec2 q1 = rot2d(p, angle);
    f32 dist1 = length(q1 * star_diag_factor);
    vec2 q2 = rot2d(p, -angle);
    f32 dist2 = length(q2 * star_diag_factor);

    f32 pint1 = 1.0f / (dist * dist_factor + 0.5f);

    pint1 += weak_factor / (distv * dist_factor + 0.01f) +
        strong_factor / (disth * dist_factor + 0.01f) +
        weak_factor / (dist1 * dist_factor + 0.01f) +
        weak_factor / (dist2 * dist_factor + 0.01f);

    return intensity_factor_max * pow(pint1, ppow) / intensity_divisor * center_intensity * 3.0f;
}

vec3 getNormal(vec3 p)
{
    const vec2 e = vec2(0.003f, 0.0f);

    vec3 xyy = vec3(e.v[0],e.v[1],e.v[1]);
    vec3 yxy = vec3(e.v[1],e.v[0],e.v[1]);
    vec3 yyx = vec3(e.v[1],e.v[1],e.v[0]);

    f32 d1 = sdScene1(p + xyy), d2 = sdScene1(p - xyy);
    f32 d3 = sdScene1(p + yxy), d4 = sdScene1(p - yxy);
    f32 d5 = sdScene1(p + yyx), d6 = sdScene1(p - yyx);

    return normalize(vec3(d1 - d2, d3 - d4, d5 - d6));
}

f32 softShadow(vec3 ro, vec3 rd, f32 tmin, f32 tmax, f32 k)
{
    f32 res = 1.0f, t = tmin;
    for (int i=0; i<20; i++)
    {
        f32 h = sdScene1(ro + rd * t);
        res = min(res, smoothstep(0.0f, 1.0f, k * h / t));
        t += clamp(h, 0.01f, 0.2f);
        if (abs(h) < 1e-4 || t > tmax)
            break;
    }
    return clamp(res + 0.15f, 0.0f, 1.0f);
}

f32 calcAO(vec3 p, vec3 n)
{
    f32 occ = 0.0f, sca = 1.0f;
    for (int i=0; i<5; i++)
    {
        f32 h = 0.01f + 0.15f * f32(i) / 4.0f;
        f32 d = sdScene1(p + h * n);
        occ += (h - d) * sca;
        sca *= 0.7f;
    }
    return clamp(1.0f - 3.0f * occ, 0.0f, 1.0f);
}

f32 trace(vec3 ro, vec3 rd, vec2& p, f32& pint)
{
    f32 depth = MIN_TRACE_DIST, dist;
    vec3 pos;
    for (u32 i=0; i<MAX_TRACE_STEPS; i++)
    {
        pos = ro + rd * depth;
        dist = sdScene1(pos);

        if (dist < PRECISION || depth >= 100.0f)
            break;

        depth += dist;
    }

    applyMobius(pos);
    p.v[0] = pos.v[0];
    p.v[1] = pos.v[2];
    trans_hyperbolic(vec2(pos.v[0],pos.v[2]));
    trans_elliptic(vec2(pos.v[0],pos.v[2]));
    vec2 polxz = polarGrid(vec2(pos.v[0],pos.v[2]), polar_grid);
    pos.v[0] = polxz.v[0];
    pos.v[2] = polxz.v[1];
    pint = getIntensity2(vec2(pos.v[0],pos.v[2]));

    return depth;
}

vec3 tonemap(vec3 color)
{
    return (color * (2.51f * color + 0.03f)) / (color * (2.43f * color + 0.59f) + 0.14f);
}

vec3 test(u32 x, u32 y)
{
    vec3 ro = vec3(-2.4f, 4.8f, 7.0f);
    vec2 roxz = rot2d(vec2(ro.v[0], ro.v[2]), iTime * 0.3f);
    ro.v[0] = roxz.v[0];
    ro.v[2] = roxz.v[1];
    vec3 lookat = vec3(0.0f, 0.6f, 0.0f);
    vec3 up = vec3(0.0f, 1.0f, 0.0f);
    vec3 f = normalize(lookat - ro);
    vec3 r = normalize(cross(f, up));
    vec3 u = normalize(cross(r, f));
    vec3 tot = vec3(0.0f);
    vec3 lp = ro + vec3(0.2f, 0.8f, -0.2f);

    vec2 uv = vec2(x,y) / iResolution;
    uv = 2.0f * uv - 1.0f;
    uv.v[0] *= iResolution.v[0] / iResolution.v[1];
    vec3 rd = normalize(uv.v[0] * r + uv.v[1] * u + 4.0f * f);
    vec2 p;
    f32 pint;

    f32 t = trace(ro, rd, p, pint);
    if (t >= 0.0f)
    {
        vec3 col = tonemap(4.0f * getColor(p, pint));
        vec3 pos = ro + rd * t;
        vec3 nor = getNormal(pos);
        vec3 ld = lp - pos;
        f32 dist = max(length(ld), 0.001f);
        ld /= dist;
        f32 at = 2.2f / (1.0f + dist * 0.1f + dist * dist * 0.05f);
        f32 ao = calcAO(pos, nor);
        f32 sh = softShadow(pos, ld, 0.04f, dist, 8.0f);
        f32 diff = clamp(dot(nor, ld), 0.0f, 1.0f);
        f32 spec = max( 0.0f, dot( reflect(-ld, nor), -rd));
        spec = powf(spec, 50.0f);
        tot += diff * 2.5f * col + vec3(0.6f, 0.8f, 0.8f) * spec * 2.0f;
        tot *= ao * sh * at;
    }

    if(t >= 100.0f)
        lp = normalize(lp - ro - rd * 100.0f);

    vec3 bg = mix(vec3(0.5f, 0.7f, 1.0f), vec3(1.0f, 0.5f, 0.6f), 0.5f - 0.5f * lp.v[1]) * 0.3f;
    tot = mix(clamp(tot, 0.0f, 1.0f), bg, smoothstep(0.0f, 100.0f - 2.0f, t));

    return sqrt(clamp(tot, 0.0f, 1.0f));
}

}

#endif // SAMPLE3_H_INCLUDED
