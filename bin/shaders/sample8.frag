//===============================================
// pemptos
// Creado por lorenzowy en 2021-12-07
// https://www.shadertoy.com/view/stdXz7
//===============================================

uniform float iTime;
uniform vec2 iResolution;

#define PI 3.141592654
#define PI2 PI*2.0

vec2 rot2d(vec2 p, float r)
{
    return vec2(p.x * cos(r) - p.y * sin(r), p.x * sin(r) + p.y * cos(r));
}

vec2 pmod(vec2 p, float r)
{
    float a = atan(p.x, p.y) + PI/r;
    float n = PI2 / r;
    float k = a * n;
    a = floor(a/n)*n;
    return vec2(p.x * cos(-a) + p.y * sin(-a),-(p.x * sin(-a)) + p.y * cos(-a));
}

float box(vec3 p, vec3 b)
{
    vec3 d = abs(p) - b;
    return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,-0.1));
}

float ifsBox(vec3 p)
{
    vec2 rot;
    for (int i=1; i<3; i++)
	{
        p = abs(p) - 1.0;

        rot = rot2d(vec2(p.x,p.y), iTime * 0.3);
        p.x = rot.x;
        p.y = rot.y;

        rot = rot2d(vec2(p.x,p.z), iTime * 0.1);
        p.x = rot.x;
        p.z = rot.y;
    }

    rot = rot2d(vec2(p.x,p.z), iTime);
    p.x = rot.x;
    p.z = rot.y;

    return box(p, vec3(0.4,0.8,0.3));
}

float map(vec3 p, vec3 cPos)
{
    vec3 p1 = p;
    p1.x = mod(p1.x-5.0, 10.0) - 5.0;
    p1.y = mod(p1.y-5.0, 10.0) - 5.0;
    p1.z = mod(p1.z, 16.0)-8.0;

    vec2 p1xy = pmod(vec2(p1.x, p1.y), 5.0);
    p1.x = p1xy.x;
    p1.y = p1xy.y;

    return ifsBox(p1);
}

void main()
{
    vec2 p = (gl_FragCoord.xy * 2.0 - iResolution.xy) / min(iResolution.x, iResolution.y);

    vec3 cPos = vec3(0.0,0.0, -3.0 * iTime);
    vec3 cDir = normalize(vec3(0.0, 0.0, -1.0));
    vec3 cUp  = vec3(cos(iTime), 1.0, 0.0);
    vec3 cSide = cross(cDir, cUp);
    vec3 ray = normalize(cSide * p.x + cUp * p.y + cDir);

    float acc = 0.0, acc2 = 0.0, t = 0.0;
    for (int i = 0; i < 99; i++)
	{
        vec3 pos = cPos + ray * t;
        float dist = map(pos, cPos);
        dist = max(abs(dist), 0.02);
        float a = exp(-dist*3.0);
        if (mod(length(pos)+24.0*iTime, 30.0) < 3.0)
		{
            a *= 2.0;
            acc2 += a;
        }
        acc += a;
        t += dist * 0.5;
    }

    vec3 col = vec3(acc * 0.01, acc * 0.011 + acc2*0.01, acc * 0.012+ acc2*0.015);
    
	gl_FragColor = vec4(col, 1.0 - t * 0.3);
}