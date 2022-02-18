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

#ifndef SAMPLE5_H_INCLUDED
#define SAMPLE5_H_INCLUDED

//===============================================
// Reflecting Pseudo Kleinian
// Creado por Kamoshika en 2022-01-01
// https://www.shadertoy.com/view/ftKSzK
//===============================================

namespace sample4
{

#define ihash(x) floor(fract(sin(x)*1426.1623f)*3.0f-1.0f)*c*2.0f

const f32 c=0.9f;
const f32 sp=1.0f;
f32 ac,T,b;

void rot3d(vec3& v,f32 a,vec3 ax)
{
    ax = normalize(ax);
    v = mix(dot(ax,v) * ax, v, cos(a)) - sin(a) * cross(ax,v);
}

f32 in1d(f32 x)
{
    x /= c * 2.0f;
    f32 i = floor(x);
    f32 s = 0.2f;
    f32 u = smoothstep(0.5f-s, 0.5f+s, fract(x));
    return mix(ihash(i), ihash(i+1.0f), u);
}

f32 map(vec3 p)
{
    f32 d1 = 0.0f, d2 = 0.0f, a = 1.0f;
    vec3 p0 = p;
    p = abs(mod(p, c * 2.0f) - c);

    for(u32 i=0; i<3; i++)
    {
        p = 2.0f * clamp(p, -c, c) - p;
        f32 k = max(1.0f / dot(p, p), 1.0f);
        p*=k;
        a*=k;
    }

    d1 = (p.v[0] + p.v[1] + p.v[2] - 1.54f) / a * 0.5f;
    d2 = p.v[1] / a * 0.5f;
    f32 s = smoothstep(-2.0f, 2.0f, b-p0.v[2]);

    f32 Tm = mod(T+0.5f, 4.0f);
    if(Tm<1.0f)
    {
        d1 = mix(d1, d2, s);
        ac += mix(a, 1.0f, s);
    }
    else if(Tm<2.0f)
    {
        d1 = d2;
        ac += 1.0;
    }
    else if(Tm<3.0f)
    {
        d1 = mix(d2, d1, s);
        ac += mix(1.0f, a, s);
    }
    else
    {
        ac+=a;
    }

    return d1;
}

vec3 calcN(vec3 p)
{
    vec2 e = vec2(0.001f, 0.0f);

    vec3 xyy = vec3(e.v[0],e.v[1],e.v[1]);
    vec3 yxy = vec3(e.v[1],e.v[0],e.v[1]);
    vec3 yyx = vec3(e.v[1],e.v[1],e.v[0]);

    return normalize(vec3(map(p+xyy)-map(p-xyy),map(p+yxy)-map(p-yxy),map(p+yyx)-map(p-yyx)));
}

f32 fs(f32 f0, f32 c)
{
    return f0 + (1.0f - f0) * pow(1.0f - c, 5.0f);
}

vec3 hsv(f32 h,f32 s,f32 v)
{
    vec3 res = fract(h + vec3(0.0f, 2.0f, 1.0f) / 3.0f) * 6.0f - 3.0f;
    res = clamp(abs(res) - 1.0f, 0.0f, 1.0f);
    res = (res - 1.0f) * s + 1.0f;
    res *= v;
    return res;
}

vec3 getC(vec3 rp)
{
    f32 i = floor(T + 0.5f);
    vec3 c1 = hsv(PI * 0.5f * i, 0.8f, 1.0f);
    vec3 c2 = hsv(PI * 0.5f * (i + 1.0f), 0.8f, 1.0f);
    f32 v = smoothstep(-0.3f, 0.3f, b-rp.v[2]);
    return mix(c1, c2, v);
}

vec3 march(vec3& rp, vec3& rd, vec3& ra, vec3 cp)
{
    vec3 col = vec3(0.0f);
    f32 d=0.0f, t=0.0f;
    ac = 0.0f;
    for(u32 i=0; i<100; i++)
    {
        d = map(rp);
        if(abs(d) < 0.0001f || t > 20.0f)
            break;

        t += d;
        rp += rd*d;
    }

    vec3 n = calcN(rp);
    vec3 ref = reflect(rd, n);
    f32 fog = exp(-t * t * 0.01f);
    vec3 al = getC(rp);
    f32 lp = 0.1f/abs(b - rp.v[2]);

    col += al * lp + ac * 0.0005f;
    col = mix(vec3(1.0f), col, fog);

    col *= ra;
    ra *= al * fs(0.8f, dot(ref, n)) * fog;

    rp += 0.001f * n;
    rd = ref;

    return col;
}

vec3 test(u32 x, u32 y)
{
    vec2 uv = vec2(x / iResolution.v[0], y / iResolution.v[1]);
    uv -= 0.5f;
    uv /= vec2(iResolution.v[1] / iResolution.v[0], 1.0f) * 0.5f;
    vec3 col = vec3(0.0f);

    T = iTime * sp / c * 0.5f;

    vec3 cp = vec3(0.0f, 0.0f, -iTime * sp);
    cp.v[0] += in1d(cp.v[2] * 0.5f - c);
    cp.v[1] += in1d(cp.v[2] * 0.5f - c * 400.0f);

    b = cp.v[2] + tan(T * PI);

    vec3 rd = normalize(vec3(uv, -1.0f));

    rot3d(rd, iTime * 0.5f, vec3(1.0f, 2.0f, 9.0f));
    rot3d(rd, PI / 4.0f, vec3(0.0f, 1.0f, 0.0f));

    vec3 rp = cp;
    vec3 ra = vec3(1.0f);
    col += march(rp,rd,ra,cp);
    col += march(rp,rd,ra,cp);
    col += march(rp,rd,ra,cp);

    return col;
}

}

#endif // SAMPLE5_H_INCLUDED
