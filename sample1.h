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

#ifndef SAMPLE1_H_INCLUDED
#define SAMPLE1_H_INCLUDED

//===============================================
// Fractal Trip
// Creado por namanonamako en 2022-01-27
// https://www.shadertoy.com/view/fd2yRw
//===============================================

namespace sample1
{

f32 speed;

vec3 spherePos1;
vec3 spherePos2;

const vec3 lightCol1 = vec3(0.8f, 0.5f, 0.2f);
const vec3 lightCol2 = vec3(0.2f, 0.2f, 0.8f);

vec3 pal(f32 t, vec3 a, vec3 b, vec3 c, vec3 d )
{
    return a + b * cos(6.28318f * (c * t + d));
}

vec3 cam(f32 t)
{
    return vec3(sin(t * 0.3f) * 1.0f, cos(t * 0.5f + 1.5f), t);
}

f32 sdSphere(vec3 p, f32 r)
{
    return length(p) - r;
}

f32 sdCross(vec3 p, f32 c)
{
	p = abs(p);
	vec3 d = max(vec3(p.v[0],p.v[1],p.v[2]), vec3(p.v[1],p.v[2],p.v[0]));
	return min(min(d.v[0], d.v[1]), d.v[2]) - c;
}

f32 sdMenger(vec3 p, f32 size, f32 width)
{
	f32 s = 1.0f, d = 0.0f;
	for(int i=0; i<4; i++)
    {
		vec3 q = mod(p * s, 2.0f) - 1.0f;
		s *= size;
		q = 1.0f - size * abs(q);
		f32 c = sdCross(q, width) / s;
		d = max(d, c);
	}
	return d;
}

f32 map(vec3 p)
{
	f32 d1 = sdMenger(p, 3.8f, 0.7f);
	vec3 xy = cam(p.v[2]);
	f32 d2 = length(vec2(p.v[0], p.v[1]) - vec2(xy.v[0], xy.v[1])) - 0.2f;
	return max(d1, -d2);
}

vec4 spheres(vec3 p)
{
	spherePos1 = cam(iTime * speed + 1.0f) + vec3(cos(iTime * 1.3f) * 0.6f, sin(iTime) * 0.6f, exp(sin(iTime)) * 0.5f);
	f32 d3 = sdSphere(p - spherePos1, 0.0f);
	vec3 col = 3.0f * (exp(-d3 * 3.0f) * lightCol1);
	return vec4(col, d3);
}

vec3 genNormal(vec3 p)
{
	vec2 d = vec2(0.001f, 0.0f);
	vec3 xyy = vec3(d.v[0],d.v[1],d.v[1]);
	vec3 yxy = vec3(d.v[1],d.v[0],d.v[1]);
	vec3 yyx = vec3(d.v[1],d.v[1],d.v[0]);
	return normalize(vec3(map(p + xyy) - map(p - xyy),map(p + yxy) - map(p - yxy),map(p + yyx) - map(p - yyx)));
}

vec3 test(u32 x, u32 y)
{
	vec2 p = ( vec2(x,y) * 2.0f - iResolution ) / min(iResolution.v[0], iResolution.v[1]);

	vec3 color = vec3(0.0f);

	speed = iTime;

	vec3 cPos = vec3(0.0f, 0.0f, -4.0f + speed);
	cPos = cam(iTime);
	vec3 t = vec3(0.0f, 0.0f, 0.0f + speed);
	t = cam(iTime + 0.5f);
	vec3 fwd = normalize(t - cPos);
	vec3 side = normalize(cross(vec3(sin(iTime * 0.6f) * 0.5f, 1.0f, cos(iTime * 0.3f + 1.6f) * 0.4f), fwd));
	vec3 up = normalize(cross(fwd, side));
	vec3 rd = normalize(p.v[0] * side + p.v[1] * up + fwd * (1.0f + 0.3f * (1.0f - dot(p,p))));

	f32 d = 0.0f, dd = 0.0f;
	vec3 ac;
	u32 k=0;

	for(u32 i=0; i<100; i++)
    {
		vec4 s = spheres(cPos + d * rd);
		dd = map(cPos + d * rd);

		if(dd < 0.001f)
			break;

		ac += vec3(s.v[0],s.v[1],s.v[2]);
		dd = min(dd, s.v[3]);
		k = i;
		d += dd;
	}

	vec3 ip = cPos + d * rd;

	if(dd < 0.001f)
    {
		vec3 normal = genNormal(ip);
		f32 ao = 1.0f - (f32(k) + dd / 0.001f) / 100.0f;
		f32 diff1 = clamp(dot(normalize(spherePos1 - ip), normal), 0.0f, 1.0f) / pow(length(spherePos1 - ip), 3.0f);
		color += diff1 * lightCol1;
		color *= ao;
	}

	color += ac * 0.03f;

	p = vec2(x,y) / iResolution;
	color = sqrt(color);
	color *= pow(p.v[0] * p.v[1] * (1.0f - p.v[0]) * (1.0f - p.v[1]) * 16.0f, 0.5f);

	return color;
}

}

#endif // SAMPLE1_H_INCLUDED
