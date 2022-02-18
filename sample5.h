/*
 *
 * irrBM (irrlight BenckMarck)
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

#ifndef SAMPLE6_H_INCLUDED
#define SAMPLE6_H_INCLUDED

//===============================================
// another synthwave sunset thing
// Creado por stduhpf en 2020-04-16
// https://www.shadertoy.com/view/tsScRK
//===============================================

namespace sample5
{

f32 jiT;

f32 pow512(f32 a)
{
    for(u32 i=0; i<8; i++)
        a*=a;
    return a*a;
}

f32 pow1d5(f32 a)
{
    return a*sqrt(a);
}

f32 hash21(vec2 co)
{
    return fract(sin(dot(vec2(co.v[0], co.v[1]), vec2(1.9898f, 7.233f))) * 45758.5433f);
}

f32 hash(vec2 uv)
{
    f32 a = smoothstep(1.0f, 8.0f, abs(uv.v[0]));
    f32 w = a > 0.0f ? (1.0f - 0.4f * pow512(0.51f + 0.49f * sin((0.02f * (uv.v[1] + 0.5f * uv.v[0])-jiT) * 2.0f))) : 0.0f;
    return a > 0.0f ? a * pow1d5(hash21(uv)) * w : 0.0f;
}

vec2 trinoise(vec2 uv)
{
    const f32 sq = sqrt(3.0f / 2.0f);
    uv.v[0] *= sq;
    uv.v[1] -= 0.5f * uv.v[0];
    vec2 d = fract(uv);
    uv -= d;
    if(dot(d, vec2(1.0f)) < 1.0f)
    {
        f32 n1 = hash(uv);
        f32 n2 = hash(uv + vec2(1.0f, 0.0f));
        f32 n3 = hash(uv + vec2(0.0f, 1.0f));
        f32 nmid = mix(n2, n3, d.v[1]);
        f32 ng = mix(n1, n3, d.v[1]);
        f32 dx = d.v[0] / (1.0f - d.v[1]);
        return vec2(mix(ng, nmid, dx), min(min((1.0f - dx) * (1.0f - d.v[1]), d.v[0]), d.v[1]));
	}
	else
    {
    	f32 n2 = hash(uv+vec2(1.0f, 0.0f));
        f32 n3 = hash(uv+vec2(0.0f, 1.0f));
        f32 n4 = hash(uv+1.0f);
        f32 nmid = mix(n2, n3, d.v[1]);
        f32 nd = mix(n2, n4, d.v[1]);
        f32 dx = (1.0f - d.v[0]) / d.v[1];
        return vec2(mix(nd, nmid, dx), min(min((1.0f - dx) * d.v[1], 1.0f - d.v[0]), 1.0f - d.v[1]));
	}
    return vec2(0.0f);
}


vec2 map(vec3 p)
{
    vec2 n = trinoise(vec2(p.v[0], p.v[2]));
    return vec2(p.v[1] - 2.0f * n.v[0], n.v[1]);
}

vec3 grad(vec3 p)
{
    const vec2 e = vec2(0.005f, 0.0f);
    f32 a = map(p).v[0];

    vec3 xyy = vec3(e.v[0],e.v[1],e.v[1]);
    vec3 yxy = vec3(e.v[1],e.v[0],e.v[1]);
    vec3 yyx = vec3(e.v[1],e.v[1],e.v[0]);

    return vec3(map(p+xyy).v[0]-a,map(p+yxy).v[0]-a,map(p+yyx).v[0]-a)/e.v[0];
}

vec2 intersect(vec3 ro,vec3 rd)
{
    f32 d = 0.0f, h = 0.0f;
    for(u32 i=0; i<500; i++)
    {
        vec3 p = ro + d * rd;
        vec2 s = map(p);
        h = s.v[0];
        d += h * 0.5f;

        if(abs(h)<0.003f*d)
            return vec2(d,s.v[1]);

        if(d>150.0f || p.v[1]>2.0f)
            break;
    }

    return vec2(-1.0);
}


vec3 sun(vec3 rd, vec3 ld, vec3 base)
{
	f32 sun = smoothstep(0.21f, 0.2f, distance(rd, ld));
    if(sun>0.0f)
    {
        f32 yd = (rd.v[1] - ld.v[1]);
        f32 a = sin(3.1f * exp(-(yd) * 14.0f));
        sun *= smoothstep(-0.8f, 0.0f, a);
        base = mix(base, vec3(1.0f, 0.8f, 0.4f) * 0.75f, sun);
    }
    return base;
}
vec3 gsky(vec3 rd, vec3 ld, bool mask)
{
    f32 haze = exp2(-5.0f * (abs(rd.v[1]) - 0.2f * dot(rd, ld)));
    vec2 xy(rd.v[0], rd.v[1]);
    f32 st = mask ? pow512(hash21((xy + vec2(300.1f, 100.0f) * rd.v[2]) * 10.0f)) * (1.0f - min(haze, 1.0f)) : 0.0f;
    vec3 col = clamp(mix(vec3(0.4f, 0.1f, 0.7f), vec3(0.7f, 0.1f, 0.4f), haze) + st, 0.0f, 1.0f);
    return mask ? sun(rd, ld, col) : col;
}

vec3 test(u32 x, u32 y)
{
    vec2 uv = (vec2(2.0f) * (vec2(x,y))- iResolution) / iResolution.v[0];

    f32 dt = fract(hash21( 1.0f * vec2(x,y) ) + iTime);
    jiT = mod(iTime - dt * 0.001f * 0.25f, 4000.0f);
    vec3 ro = vec3(0.0f, 1.0f,(-2000.0f + jiT) * 10.0f);

    vec3 rd = normalize(vec3(uv, 0.75f));

    vec2 i = intersect(ro, rd);
    f32 d = i.v[0];

    vec3 ld = normalize(vec3(0.0f, 0.125f + 0.05f * sin(0.1f * jiT), 1.0f));

    f32 fog = d > 0.0f ? exp2(-d * 0.14f) : 0.0f;
    vec3 sky = gsky(rd, ld, d < 0.0f);

    vec3 p = ro + d * rd;
    vec3 n = normalize(grad(p));

    f32 diff = dot(n, ld) + 0.1f * n.v[1];
    vec3 col = vec3(0.1f, 0.11f, 0.18f) * diff;

    vec3 rfd = reflect(rd, n);
    vec3 rfcol = gsky(rfd, ld, true);

    col = mix(col, rfcol, 0.05f + 0.95f * pow(max(1.0f + dot(rd, n), 0.0f), 5.0f));
    col = mix(col, vec3(0.8f, 0.1f, 0.92f),smoothstep(0.05f, 0.0f, i.v[1]));
    col = mix(sky, col, fog);

    //no gamma for that old cg look
    if(d < 0.0f)
        d = 1e6;

    d = min(d, 10.0f);

    return clamp(col, 0.0f, 1.0f);
}

}

#endif // SAMPLE6_H_INCLUDED
