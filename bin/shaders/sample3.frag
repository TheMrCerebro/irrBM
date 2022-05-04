//===============================================
// Mobius trans in hyper 3-space
// Creado por neozhaoliang en 2018-05-09
// https://www.shadertoy.com/view/4scfR2
//===============================================

uniform float iTime;
uniform vec2 iResolution;

#define PI  3.1415926536
#define E_  2.7182818285

// Raymarching constants
#define MIN_TRACE_DIST   0.01
#define MAX_TRACE_STEPS  255
#define PRECISION        1e-5
#define FAR              100.

// grid and cone size
const vec2 polar_grid = vec2(0.4, PI / 7.0);
const vec2 cone_angle = normalize(vec2(1.5, 1.0));

// Intensity constants
const float intensity_divisor = 40000.;
const float intensity_factor_max = 7.2;
const float center_intensity = 12.0;
const float ppow = 1.9;

// Color constants
const float center_hue = 0.5;
const float center_sat = 0.18;

// shape constants
const float dist_factor = 3.0;
const float strong_factor = 7.0;
const float weak_factor = 1.0;
const vec2 star_hv_factor = vec2(30, 1);
const vec2 star_diag_factor = vec2(30, 1);

// hsv to rgb conversion
vec3 hsv2rgb(vec3 hsv)
{
    const vec3 p = vec3(0.0, 2.0/3.0, 1.0/3.0);
    vec2 hsvyz = clamp(vec2(hsv.y,hsv.z), 0.0, 1.0);
    hsv.y = hsvyz.x;
    hsv.z = hsvyz.y;
    return hsv.z*(0.63*hsv.y*(cos(2.*PI*(hsv.x + p)) - 1.0) + 1.0);
}

// 2d rotation
vec2 rot2d(vec2 p, float a) { return cos(a) * p + sin(a) * vec2(p.y, -p.x); }

// 2d polar grids
vec2 polarGrid(vec2 p, vec2 size)
{
    float theta = atan(p.y, p.x);
    float r = log(length(p));
    return mod(vec2(r, theta) + 0.5 * size, size) - 0.5 * size;
}

//Quaternion arithmetic
vec4 qmul(vec4 p, vec4 q)
{
    return vec4(p.x * q.x - dot(p.yzw, q.yzw), p.x * q.yzw + q.x * p.yzw + cross(p.yzw, q.yzw));
}

float applyMobius(vec3 p)
{
    vec4 pt = vec4(p, 0.0);

    vec4 p1 = qmul(vec4(vec2(-1.0, 0.0), 0.0, 0.0), pt) + vec4(vec2(1.2, 0.0), 0.0, 0.0);
    vec4 p2 = qmul(vec4(vec2(-1.0, 0.0), 0.0, 0.0), pt) + vec4(vec2(-1.2, 0.0), 0.0, 0.0);

    vec4 xyz = qmul(p1, vec4(p2.x, -vec3(p2.y,p2.z,p2.w)) / dot(p2, p2));
    p.x = xyz.x;
    p.y = xyz.y;
    p.z = xyz.z;
    float scale = length(p);
    return scale > 1.0 ? 1.0 / scale : scale;
}

// A Mobius transformation of hyperbolic type is conjugate to a pure scaling
void trans_hyperbolic(vec2 p)
{
    float d = log(length(p)) - (iTime * 0.5) * polar_grid.x;
    d = mod(d + 0.5 * polar_grid.x, polar_grid.x) - 0.5 * polar_grid.x;
    p = normalize(p) * pow(E_,d);
}

// A Mobius transformation of elliptic type is conjugate to a pure rotation
void trans_elliptic(vec2 p)
{
    p = rot2d(p, (iTime * 0.5) * polar_grid.y);
}

float sdSphere2(vec3 p, float r)
{
	p.y -= r;
	return length(p) - r;
}

// signed distance function for parabolic case
float sdScene1(vec3 p)
{
	return min(p.y, sdSphere2(p, 1.0));
}

vec3 getColor(vec2 p, float pint)
{
    float sat = 0.75 / pow(pint, 2.5) + center_sat;
    float hue2 = (iTime * 0.3) - log(length(p)) / 7.0;
    float hue = center_hue + hue2;
    return hsv2rgb(vec3(hue, sat, pint)) + pint / 3.0;
}

float getIntensity2(vec2 p)
{
    float angle = atan(polar_grid.x, polar_grid.y);
    float dist  = length(p);
    float disth = length(p * star_hv_factor);
    float distv = length(p * star_hv_factor.yx);

    vec2 q1 = rot2d(p, angle);
    float dist1 = length(q1 * star_diag_factor);
    vec2 q2 = rot2d(p, -angle);
    float dist2 = length(q2 * star_diag_factor);

    float pint1 = 1. / (dist * dist_factor  + .5);

	pint1 += weak_factor / (distv * dist_factor + 0.01) +
            strong_factor / (disth * dist_factor + 0.01) +
            weak_factor / (dist1 * dist_factor + 0.01) +
            weak_factor / (dist2 * dist_factor + 0.01);

    return intensity_factor_max * pow(pint1, ppow) / intensity_divisor * center_intensity * 3.;
}

vec3 getNormal(vec3 p)
{
    vec2 e = vec2(0.003, 0.0);

    float d1 = sdScene1(p + e.xyy), d2 = sdScene1(p - e.xyy);
    float d3 = sdScene1(p + e.yxy), d4 = sdScene1(p - e.yxy);
    float d5 = sdScene1(p + e.yyx), d6 = sdScene1(p - e.yyx);

    return normalize(vec3(d1 - d2, d3 - d4, d5 - d6));
}

float softShadow(vec3 ro, vec3 rd, float tmin, float tmax, float k)
{
    float res = 1.0;
    float t = tmin;
    for (int i=0; i<20; i++)
    {
        float h = sdScene1(ro + rd * t);
        res = min(res, smoothstep(0.0, 1.0, k * h / t));
        t += clamp(h, 0.01, 0.2);
        if (abs(h) < 1e-4 || t > tmax)
            break;
    }
    return clamp(res + 0.15, 0.0, 1.0);
}

float calcAO(vec3 p, vec3 n)
{
    float occ = 0.0, sca = 1.0;
    for (int i = 0; i<5; i++)
    {
        float h = 0.01 + 0.15 * float(i) / 4.0;
        float d = sdScene1(p + h * n);
        occ += (h - d) * sca;
        sca *= 0.7;
    }
    return clamp(1.0 - 3.0 * occ, 0.0, 1.0);
}

float trace(vec3 ro, vec3 rd, out vec2 p, out float pint)
{
    float depth = MIN_TRACE_DIST, dist;
    vec3 pos;
    for (int i = 0; i < MAX_TRACE_STEPS; i++)
    {
        pos = ro + rd * depth;
        dist = sdScene1(pos);

        if (dist < PRECISION || depth >= FAR)
            break;

        depth += dist;
    }

	applyMobius(pos);
    p.x = pos.x;
    p.y = pos.z;
    trans_hyperbolic(vec2(pos.x,pos.z));
    trans_elliptic(vec2(pos.x,pos.z));
    vec2 polxz = polarGrid(vec2(pos.x,pos.z), polar_grid);
    pos.x = polxz.x;
    pos.z = polxz.y;
    pint = getIntensity2(vec2(pos.x,pos.z));

    return depth;
}

vec3 tonemap(vec3 color)
{
    return (color * (2.51 * color + 0.03)) / (color * (2.43 * color + 0.59) + 0.14);
}

void main()
{
    vec3 ro = vec3(-2.4, 4.8, 7.0);
    vec2 roxz = rot2d(vec2(ro.x, ro.z), iTime * 0.3);
    ro.x = roxz.x;
    ro.z = roxz.y;
    vec3 lookat = vec3(0.0, 0.6, 0.0);
    vec3 up = vec3(0.0, 1.0, 0.0);
    vec3 f = normalize(lookat - ro);
    vec3 r = normalize(cross(f, up));
    vec3 u = normalize(cross(r, f));
    vec3 tot = vec3(0);
    vec3 lp = ro + vec3(.2, .8, -0.2);
   
	vec2 uv = gl_FragCoord / iResolution.xy;
	uv = 2.0 * uv - 1.0;
	uv.x *= iResolution.x / iResolution.y;
	vec3 rd = normalize(uv.x * r + uv.y * u + 4.0 * f);
	vec2 p;
	float pint;

	float t = trace(ro, rd, p, pint);
	if (t >= 0.0)
	{            
		vec3 col = tonemap(4.0 * getColor(p, pint));
		vec3 pos = ro + rd * t;
		vec3 nor = getNormal(pos);
		vec3 ld = lp - pos;
		float dist = max(length(ld), 0.001);
		ld /= dist;
		float at = 2.2/(1. + dist*.1 + dist*dist*.05); 
		float ao = calcAO(pos, nor);
		float sh = softShadow(pos, ld, 0.04, dist, 8.);
		float diff = clamp(dot(nor, ld), 0.0, 1.0);
		float spec = max( 0.0, dot( reflect(-ld, nor), -rd));
		spec = pow(spec, 50.0);
		tot += diff * 2.5 * col + vec3(0.6, 0.8, 0.8) * spec * 2.;
		tot *= ao * sh * at;
	}
	if(t >= FAR)
		lp = normalize(lp - ro - rd*FAR);
 
	vec3 bg = mix(vec3(0.5, 0.7, 1), vec3(1, 0.5, 0.6), 0.5 - 0.5*lp.y) * 0.3;
	tot = mix(clamp(tot, 0.0, 1.0), bg, smoothstep(0.0, FAR-2.0, t));

	gl_FragColor = vec4(sqrt(clamp(tot, 0.0, 1.0)), 1.0);
}