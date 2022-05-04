//===============================================
// RME4 - Crater
// Creado por patu en 2021-03-31
// https://www.shadertoy.com/view/MlSBDt
//===============================================

uniform float iTime;
uniform vec2 iResolution;

#define FAR 570.
#define INFINITY 1e32

#define FOV 110.0
#define FOG 1.

#define PI 3.14159265
#define TAU (2*PI)
#define PHI (1.618033988749895)

float hash12(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

float fbm(vec3 x)
{
    float r = 0.0, w = 1.0, s = 1.0;
    for (int i=0; i<7; i++)
    {
        w *= 0.5;
        s *= 2.0;

        vec3 g = floor(s * x);
        vec3 f = fract(s * x);
        vec3 u = f * f * (vec3(3.0) - vec3(2.0) * f);

        vec2 ii = vec2(g.x,g.y) + g.z * vec2(5.0);
        float a = hash12( ii + vec2(0.0,0.0) );
        float b = hash12( ii + vec2(1.0,0.0) );
        float c = hash12( ii + vec2(0.0,1.0) );
        float d = hash12( ii + vec2(1.0,1.0) );
        float v1 = mix(mix(a,b,u.x), mix(c,d,u.x), u.y);

        ii += vec2(5.0);
        a = hash12( ii + vec2(0.0,0.0) );
        b = hash12( ii + vec2(1.0,0.0) );
        c = hash12( ii + vec2(0.0,1.0) );
        d = hash12( ii + vec2(1.0,1.0) );
        float v2 = mix(mix(a,b,u.x), mix(c,d,u.x), u.y);

        r += w * max(mix(v1,v2,u.z),0.0);
    }
    return r;
}

float saturate(float a)
{
	return clamp(a, 0.0, 1.0);
}

float smin( float a, float b, float k )
{
    float res = exp( -k*a ) + exp( -k*b );
    return -log( res )/k ;
}

struct geometry
{
    float dist;
    float specular;
    float diffuse;
    vec3 color;
};

geometry scene2(vec3 p)
{
    geometry plane;

    float localNoise = fbm(p / 10.) * 2.;;
    p.y -= localNoise * .2;
    plane.dist = p.y;
    p.y *= 3.5;

    plane.dist = smin(plane.dist, length(p) - 25., .15 + localNoise * .2);
    plane.dist = max(plane.dist, -length(p) + 29.  + localNoise);

    plane.color = vec3(1., .2, .0);
    plane.diffuse = 0.;
    plane.specular = 22.1;
    
    return plane;
}

geometry raymarch(vec3 o, vec3 d)
{
    geometry mp;
    
    float l = -0.1;
    
    for (int i = 0; i < 30; i++)
	{
        if (abs(l) < 0.1 || l > 130.)
            break;
        
        mp = scene2(o + d * l);
        l += mp.dist;
    }
    
    mp.dist = l;
    return mp;
}

float softShadow(vec3 ro, vec3 lp, float k)
{
    vec3 rd = (lp - ro); // Unnormalized direction ray.

    float shade = 1.0;
    float dist = 1.0;
    float end = max(length(rd), 0.01);
    float stepDist = end / 125.0;
    
    float tb = (8.0-ro.y)/normalize(rd).y; 
    if( tb>0.0 ) end = min( end, tb );
    
    rd /= end;
    for (int i = 0; i < 125.0; i++)
	{
        float h = scene2(ro + rd * dist).dist;
        shade = min(shade, smoothstep(0.0, 1.0, k * h / dist)); 
        dist += min(h, stepDist * 2.); 
        if (h < 0.001 || dist > end)
			break;
    }
    return min(max(shade, 0.3), 1.0);
}

vec3 getNormal(vec3 pos)
{
	float d=scene2(pos).dist;
	return normalize(vec3(scene2( pos+vec3(0.001, 0.0, 0.0)).dist-d, scene2(pos+vec3(0.0, 0.001, 0.0)).dist-d, scene2(pos+vec3(0.0, 0.0, 0.001)).dist-d));
}

float getAO(vec3 hitp, vec3 normal, float dist)
{
    float sdist = scene2(hitp + normal * dist).dist;
    return clamp(sdist / dist, 0.4, 1.0);
}

vec3 Sky(in vec3 rd, bool showSun, vec3 lightDir)
{
    vec3 lightColour = normalize(vec3(1.8, 1.0, 0.3));
   	float sunSize = 1.;
   	float sunAmount = max(dot(rd, lightDir), .1);
   	float v = pow(1.2 - max(rd.y, .5), 1.1);
   	vec3 sky = mix(vec3(1.0, 0.78, 0.39), vec3(1.1, 1.2, 1.3) / 10.0, v);
   	sky += lightColour * sunAmount * sunAmount * 1.0 + lightColour * min(pow(sunAmount, 1e4)* sunSize,1233.0 * sunSize);

   	return clamp(sky, 0.0, 1.0);
}

vec3 doColor( in vec3 sp, in vec3 rd, in vec3 sn, in vec3 lp, geometry obj)
{
	vec3 ld = sp + lp - sp;
    ld /= max(length(ld / 2.0), 0.001);
	float diff = max(dot(sn, ld), obj.diffuse);
    float spec = max(dot(reflect(-ld, sn), -rd), obj.specular / 2.0);
    return (obj.color * (diff + 0.15) * spec * 0.1);
}


void main()
{
    vec2 uv = gl_FragCoord / iResolution - 0.5;
    uv.y *= 1.2;

    vec3 light = vec3(0.5, 7.0, 100.0);
    vec3 ro = vec3(cos(-iTime * 0.1) * 48.0, 18.0, sin(-iTime * 0.1) * 48.0);
    vec3 vpn = normalize(-ro);

    vec3 u = normalize(cross(vec3(0.0, 1.0, 0.0), vpn));
    vec3 v = cross(vpn, u);
    vec3 vcv = (ro + vpn);
    vec3 scrCoord = (vcv + uv.x * u * iResolution.x/iResolution.y + uv.y * v);
    vec3 rd = normalize(scrCoord - ro);
    vec3 sky = Sky(rd, true, normalize(light)) * 1.0;

    vec3 sceneColor = vec3(0.0);

    geometry tr = raymarch(ro, rd);
    if (tr.dist < FAR)
	{ 
        vec3 hit = ro + rd * tr.dist;
        vec3 sn = getNormal(hit);
        sceneColor = (doColor(hit, rd, sn, light, tr) * 1.0) * 1.0;
        sceneColor *= getAO(hit, sn, 10.2);
        sceneColor *= softShadow(hit, hit + light, 8.2);
        sceneColor = mix(sceneColor, sky, saturate(tr.dist * 4.5 / FAR));
    }
	else
	{
        sceneColor = sky;
    }

    gl_FragColor = vec4(clamp(sceneColor * (1. - length(uv) / 3.5), 0.0, 1.0), 1.0);
    gl_FragColor = pow(gl_FragColor, 1./vec4(1.2));
}