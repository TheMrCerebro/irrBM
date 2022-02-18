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

#ifndef FAKEGLSL_H_INCLUDED
#define FAKEGLSL_H_INCLUDED

template<u32 N>
class vec
{
public:

    vec<N>() { for(u32 i=0; i<N; i++) v[i]=0.0f; }
    vec<N>(const f32 n) { for(u32 i=0; i<N; i++) v[i]=n; }
    vec<N>(const f32 n0, const f32 n1) { v[0]=n0; v[1]=n1; }
    vec<N>(const f32 n0, const f32 n1, const f32 n2) { v[0]=n0; v[1]=n1; v[2]=n2; }
    vec<N>(const f32 n0, const f32 n1, const f32 n2, const f32 n3) { v[0]=n0; v[1]=n1; v[2]=n2; v[3]=n3; }
    vec<N>(const vec<N>& n) { for(u32 i=0; i<N; i++) v[i]=n.v[i]; }
    vec<N>(const vec<2>& n, const f32 n0) { v[0]=n.v[0]; v[1]=n.v[1]; v[2]=n0; }
    vec<N>(const vec<2>& n, const f32 n0, const f32 n1) { v[0]=n.v[0]; v[1]=n.v[1]; v[2]=n0; v[3]=n1; }
    vec<N>(const vec<3>& n, const f32 n0) { v[0]=n.v[0]; v[1]=n.v[1]; v[2]=n.v[2]; v[3]=n0; }
    vec<N>(const f32 n0, const vec<3>& n) { v[0]=n0; v[1]=n.v[0]; v[2]=n.v[1]; v[3]=n.v[2]; }

    vec<N> operator-() const{ vec<N> tmp; for(u32 i=0; i<N; i++) tmp.v[i] = -v[i]; return tmp; }

    vec<N>& operator=(const vec<N>& other) { for(u32 i=0; i<N; i++) v[i] = other.v[i]; return *this; }

    vec<N> operator+(const vec<N>& other) const { vec<N> tmp; for(u32 i=0; i<N; i++) tmp.v[i] = v[i] + other.v[i]; return tmp; }
    vec<N>& operator+=(const vec<N>& other) { for(u32 i=0; i<N; i++) v[i] += other.v[i]; return *this; }
    vec<N> operator+(const f32 val) const { vec<N> tmp; for(u32 i=0; i<N; i++) tmp.v[i] = v[i] + val; return tmp; }
    vec<N>& operator+=(const f32 val) { for(u32 i=0; i<N; i++) v[i] += val; return *this; }

    vec<N> operator-(const vec<N>& other) const { vec<N> tmp; for(u32 i=0; i<N; i++) tmp.v[i] = v[i] - other.v[i]; return tmp; }
    vec<N>& operator-=(const vec<N>& other) { for(u32 i=0; i<N; i++) v[i] -= other.v[i]; return *this; }
    vec<N> operator-(const f32 val) const { vec<N> tmp; for(u32 i=0; i<N; i++) tmp.v[i] = v[i] - val; return tmp; }
    vec<N>& operator-=(const f32 val) { for(u32 i=0; i<N; i++) v[i] -= val; return *this; }

    vec<N> operator*(const vec<N>& other) const { vec<N> tmp; for(u32 i=0; i<N; i++) tmp.v[i] = v[i] * other.v[i]; return tmp; }
    vec<N>& operator*=(const vec<N>& other) { for(u32 i=0; i<N; i++) v[i] *= other.v[i]; return *this; }
    vec<N> operator*(const f32 val) const { vec<N> tmp; for(u32 i=0; i<N; i++) tmp.v[i] = v[i] * val; return tmp; }
    vec<N>& operator*=(const f32 val) { for(u32 i=0; i<N; i++) v[i] *= val; return *this; }

    vec<N> operator/(const vec<N>& other) const { vec<N> tmp; for(u32 i=0; i<N; i++) tmp.v[i] = v[i] / other.v[i]; return tmp; }
    vec<N>& operator/=(const vec<N>& other) { for(u32 i=0; i<N; i++) v[i] /= other.v[i]; return *this; }
    vec<N> operator/(const f32 val) const { vec<N> tmp; for(u32 i=0; i<N; i++) tmp.v[i] = v[i] / val; return tmp; }
    vec<N>& operator/=(const f32 val) { for(u32 i=0; i<N; i++) v[i] /= val; return *this; }

    f32 v[N];
};

template<u32 N> vec<N> operator+(const f32 scalar, const vec<N>& vector) { return vector+scalar; }
template<u32 N> vec<N> operator-(const f32 scalar, const vec<N>& vector) { return vector-scalar; }
template<u32 N> vec<N> operator/(const f32 scalar, const vec<N>& vector) { return vector/scalar; }
template<u32 N> vec<N> operator*(const f32 scalar, const vec<N>& vector) { return vector*scalar; }

#define vec2 vec<2>
#define vec3 vec<3>
#define vec4 vec<4>

/* -----------------------------------------------------------------------
                    ANGLE & TRIGONOMETRY FUNCTIONS
   -----------------------------------------------------------------------*/

// Radians
// Converts degrees to radians.

template<class T> T radians(T degrees) { return (PI / 180.0f) * degrees; }

// Degrees
// Converts radians to degrees.

template<class T> T degrees(T radians) { return (180.0f / PI) * radians; }

// Sine
// Returns the sine of an angle in radians.

#define sin sinf
template<u32 N> vec<N> sin(vec<N> x)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = sin(x.v[i]);
    return out;
}

// Cosine
// Returns the cosine of an angle in radians.

#define cos cosf
template<u32 N> vec<N> cos(vec<N> x)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = cos(x.v[i]);
    return out;
}

// Tangent
// Returns the tangent of an angle in radians.

#define tan tanf
template<u32 N> vec<N> tan(vec<N> x)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = tan(x.v[i]);
    return out;
}

// Arcsine
// Returns the arcsine of an angle in radians.

#define asin asinf
template<u32 N> vec<N> asin(vec<N> x)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = asin(x.v[i]);
    return out;
}

// Arccosine
// Returns the arccosine of an angle in radians.

#define acos acosf
template<u32 N> vec<N> acos(vec<N> x)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = acos(x.v[i]);
    return out;
}

// Arctangent
// Returns the arctangent of an angle in radians.

#define atan atan2f
template<u32 N> vec<N> atan(vec<N> x, vec<N> y)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = atan(x.v[i], y.v[i]);
    return out;
}

/* -----------------------------------------------------------------------
                          EXPONENTIAL FUNCTIONS
   -----------------------------------------------------------------------*/

// Exponentiation
// Returns x raised to the power of y.

#define pow powf
template<u32 N> vec<N> pow(vec<N> x, vec<N> y)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = pow(x.v[i], y.v[i]);
    return out;
}

// Exponential function
// Returns the constant e raised to the power of x.

#define exp expf
template<u32 N> vec<N> exp(vec<N> p)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = exp(p.v[i]);
    return out;
}

// Natural logarithm
// Returns the power to which the constant e has to be raised to produce x.

#define log logf
template<u32 N> vec<N> log(vec<N> p)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = log(p.v[i]);
    return out;
}

// Exponential function (base 2)
// Returns 2 raised to the power of x.

#define exp2 exp2f
template<u32 N> vec<N> exp2(vec<N> p)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = exp2(p.v[i]);
    return out;
}

// Logarithm (base 2)
// Returns the power to which 2 has to be raised to produce x.

#define log2 log2f
template<u32 N> vec<N> log2(vec<N> p)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = log2(p.v[i]);
    return out;
}

// Square root
// Returns the square root of x.

#define sqrt sqrtf
template<u32 N> vec<N> sqrt(vec<N> p)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = sqrt(p.v[i]);
    return out;
}

// Inverse square root
// Returns the inverse square root of x.

template<class T>
T inversesqrt(T x) { return 1.0f/sqrt(x); }

/* -----------------------------------------------------------------------
                            COMMON FUNCTIONS
   -----------------------------------------------------------------------*/

// Abs
// Return the absolute value of the parameter.

#define abs fabsf
template<u32 N> vec<N> abs(vec<N> x)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = abs(x.v[i]);
    return out;
}

// Sign
// Returns 1.0 when x is positive, 0.0 when x is zero and -1.0 when x is negative.

f32 sign(f32 x) { return x < 0.0f ? -1.0f : x > 0.0f ? +1.0f : 0.0f; }

template<u32 N> vec<N> sign(vec<N> x)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = sign(x.v[i]);
    return out;
}

// Floor
// Returns the largest integer number that is smaller or equal to x.

#define floor floorf
template<u32 N> vec<N> floor(vec<N> x)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = floor(x.v[i]);
    return out;
}

// Ceiling
// Returns the smallest number that is larger or equal to x.


// Fractional part
// Returns the fractional part of x.

template<class T> T fract(T x) { return x - floor(x); }

// Modulo
// Returns x minus the product of y and floor(x/y).

template<class T, class S> T mod(T x, S y) { return x - y * (floor(x / y)); }

// Minimum
// Returns the smaller of the two arguments.

f32 min(f32 a, f32 b) { return a < b ? a : b; }

template<u32 N> vec<N> min(vec<N> a, f32 b)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = min(a.v[i], b);
    return out;
}

template<u32 N> vec<N> min(vec<N> a, vec<N> b)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = min(a.v[i], b.v[i]);
    return out;
}

// Maximum
// Returns the larger of the two arguments.

f32 max(f32 a, f32 b) { return a < b ? b : a; }

template<u32 N> vec<N> max(vec<N> a, f32 b)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = max(a.v[i], b);
    return out;
}

template<u32 N> vec<N> max(vec<N> a, vec<N> b)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = max(a.v[i], b.v[i]);
    return out;
}

// Clamp
// Returns x if it is larger than minVal and smaller than maxVal.

template<class T, class S, class U>
T clamp(T x, S minVal, U maxVal) { return min(max(x, minVal), maxVal); }

// Mix
// Returns the linear blend of x and y.

template<class T, class S, class U>
T mix(T x, S y, U a) { return x * (1.0f - a) + y * a; }

// Step
// Returns 0.0 if x is smaller than edge and otherwise 1.0.

f32 step(f32 edge, f32 x) { return x < edge ? 0.0f : 1.0f; }

template<u32 N> vec<N> step(vec<N> edge, vec<N> x)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = step(edge.v[i], x.v[i]);
    return out;
}

template<u32 N> vec<N> step(f32 edge, vec<N> x)
{
    vec<N> out;
    for(u32 i=0; i<N; i++) out.v[i] = step(edge, x.v[i]);
    return out;
}

// Smoothstep
// Returns 0.0 if x is smaller than edge0 and 1.0 if x is larger than edge1.

template<class T> T smoothstep(T edge0, T edge1, T x)
{
    T t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

/* -----------------------------------------------------------------------
                        GEOMETRIC FUNCTIONS
   -----------------------------------------------------------------------*/

// Length
// Returns the length of a vector defined by the Euclidean norm.

template<class T> f32 length(T p) { return sqrt(dot(p,p)); }

// Distance
// Returns the distance between two points.

template<class T> f32 distance(T p0, T p1) { return length(p1-p0); }

// Dot product
// Returns the dot product of the two input parameters.

f32 dot(f32 x, f32 y) { return x*y; }

template<u32 N> f32 dot(vec<N> x, vec<N> y)
{
    f32 out = 0.0f;
    for(u32 i=0; i<N; i++) out += x.v[i] * y.v[i];
    return out;
}

// Cross product
// Returns the cross product of the two input parameters.

vec3 cross(vec3 x, vec3 y) { return vec3(x.v[1] * y.v[2] - x.v[2] * y.v[1], x.v[2] * y.v[0] - x.v[0] * y.v[2], x.v[0] * y.v[1] - x.v[1] * y.v[0]); }

// Normalize
// Calculate the unit vector in the same direction as the input vector.

template<class T> T normalize(T p) { return p * (1.0f / length(p)); }

// Faceforward
// returns a vector that points in the same direction as a reference vector

template<class T> T faceforward(T n, T i, T nref) { return dot(nref, i) < T(0.0f) ? n : -n; }

// Reflect
// Returns a vector that points in the direction of reflection

template<class T> T reflect(T i, T n) { return i - (2.0f * dot(n, i)) * n; }

// Refract
// Returns a vector that points in the direction of refraction

template<class T> T refract(T i, T n, T eta)
{
    f32 n_i = dot(n, i);
    f32 k = 1.0f - eta * eta * (1.0f - n_i * n_i);
    if (k < 0.0f)
        return T(0.0f);
    else
        return eta * i - (eta * n_i + sqrt(k)) * n;
}

/* -----------------------------------------------------------------------
                    VECTOR RELATIONAL FUNCTIONS
   -----------------------------------------------------------------------*/



/* -----------------------------------------------------------------------
                        MATRIX FUNCTIONS
   -----------------------------------------------------------------------*/


#endif // FAKEGLSL_H_INCLUDED
