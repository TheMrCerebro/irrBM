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

#ifndef SAMPLE2_H_INCLUDED
#define SAMPLE2_H_INCLUDED

//===============================================
// synthetic aperture
// Creado por FabriceNeyret2 en 2014-07-13
// https://www.shadertoy.com/view/ldlSzX
//===============================================

namespace sample2
{

#define N 23    	// number of sources

f32 rnd(f32 i)
{
	return mod(4000.0f * sin(23464.345f * i + 45.345f), 1.0f);
}

f32 srnd(f32 i)
{
    return 2.0f * rnd(i) - 1.0f;
}

vec3 test(u32 x, u32 y)
{
    vec2 uv = (2.0f * vec2(x,y) - iResolution ) / iResolution.v[1];

	vec2 mouse = vec2(1.5f * cos(0.2345f * iTime) - 0.7f * sin(iTime), sin(0.3214f * iTime) + 0.5f * cos(1.234f * iTime)) / 1.5f;

	f32 xo = -0.75f, xt = xo + 0.03f;

	f32 Phi[N],D2[N];
	for (u32 i=0; i<N; i++)
    {
		vec2 P = 0.99f * vec2(sin(4.0f * xt), -cos(4.0f * xt));
		xt += (1.54f / f32(N)) * (1.0f + 0.7f * srnd(f32(i)));
		f32 dm = length(mouse-P), phim = dm, d = length(uv-P), phi  = d - 0.1f * iTime;
		Phi[i] = (2.0f * PI / 0.04f) * (phi - phim);
		D2[i] = pow(d, 1.0f);

		if (d<0.01f)
            return vec3(0.0f, 0.0f, 1.0f);
	}

	// combines waves or energy
	f32 v = 0.0f;
    for (u32 i=0; i<N; i++)
    {
        for (u32 j=0; j<N; j++)
            v += cos(Phi[j] - Phi[i]) / (D2[i] * D2[j]);
    }

    v = sqrt(v / 2.0f);

	v = v * 4.5f / f32(N);

	return v * vec3(1.0f, 0.5f, 0.25f);
}

}

#endif // SAMPLE2_H_INCLUDED
