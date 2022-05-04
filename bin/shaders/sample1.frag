//===============================================
// Fractal Trip
// Creado por namanonamako en 2022-01-27
// https://www.shadertoy.com/view/fd2yRw
//===============================================

uniform float iTime;
uniform vec2 iResolution;

float speed;

vec3 spherePos1;
vec3 spherePos2;

vec3 lightCol1 = vec3(0.8, 0.5, 0.2);
vec3 lightCol2 = vec3(0.2, 0.2, 0.8);

vec3 pal(float t, vec3 a, vec3 b, vec3 c, vec3 d)
{
    return a + b * cos(6.28318 * (c * t + d) );
}

vec3 cam(float t)
{
	return vec3(sin(t * 0.3) * 1.0 , cos(t * 0.5 + 1.5), t); 
}

float sdSphere(vec3 p, float r)
{
	return length(p) - r;
}

float sdCross(vec3 p, float c)
{
	p = abs(p);
	vec3 d = max(vec3(p.x,p.y,p.z), vec3(p.y,p.z,p.x));
	return min(min(d.x, d.y), d.z) - c;
}

float sdMenger(vec3 p, float size, float width)
{
	float s = 1.0, d = 0.0;
	for(int i=0; i<4; i++)
	{
		vec3 q = mod(p * s, 2.0) - 1.0;
		s *= size;
		q = 1.0 - size * abs(q);
		float c = sdCross(q, width) / s;
		d = max(d, c);
	}
	return d;
}

float map(vec3 p)
{
	float d1 = sdMenger(p, 3.8, 0.7);
	vec3 xy = cam(p.z);
	float d2 = length(vec2(p.x,p.y) - vec2(xy.x, xy.y)) - 0.2;
	return max(d1, -d2);
}

vec4 spheres(vec3 p)
{
	spherePos1 = cam(iTime + 1.0) + vec3(cos(iTime * 1.3) * 0.6, sin(iTime) * 0.6, exp(sin(iTime)) * 0.5);
	float d3 = sdSphere(p - spherePos1, 0.0);
	vec3 col = 3.0 * (exp(-d3 * 3.0) * lightCol1);
	return vec4(col, d3);
}

vec3 genNormal(vec3 p)
{
	vec2 d = vec2(0.001, 0.0);
	return normalize(vec3(
		map(p + d.xyy) - map(p - d.xyy),
		map(p + d.yxy) - map(p - d.yxy),
		map(p + d.yyx) - map(p - d.yyx)
		));
}
	
void main()
{
	vec2 p = ( gl_FragCoord.xy * 2.0 - iResolution.xy ) / min(iResolution.x, iResolution.y);

	vec3 color = vec3(0);
	
	speed = iTime;
	
	vec3 cPos = vec3(0.0, 0.0, -4.0 + speed);
	cPos = cam(iTime);
	vec3 t = vec3(0.0, 0.0, 0.0 + speed);
	t = cam(iTime + 0.5);
	vec3 fwd = normalize(t - cPos);
	vec3 side = normalize(cross(vec3(sin(iTime*0.6) * 0.6, 1.0, cos(iTime * 0.3 + 1.6) * 0.4), fwd));
	vec3 up = normalize(cross(fwd, side));
	vec3 rd = normalize(p.x * side + p.y * up + fwd * (1.0 + 0.3 * (1.0 - dot(p,p))));
	
	float d = 0.0, dd = 0.0;
	vec3 ac;
	int k;
	
	for(int i = 0; i<100; i++)
	{
		vec4 s = spheres(cPos + d * rd);
		dd = map(cPos + d * rd);

		if(dd < 0.001)
			break;

		ac += vec3(s.x,s.y,s.z);
		dd = min(dd, s.w);
		k = i;
		d += dd;
	}
	
	vec3 ip = cPos + d * rd;

	if(dd < 0.001)
	{
		vec3 normal = genNormal(ip);
		float ao = 1.0 - (float(k) + dd / 0.001) / 100.0;
		float diff1 = clamp(dot(normalize(spherePos1 - ip), normal), 0.0, 1.0) / pow(length(spherePos1 - ip), 3.0);
		color += diff1 * lightCol1;
		color *= ao;
	}
	
	color += ac * 0.03;
	
	p = gl_FragCoord.xy / iResolution.xy;
	color = sqrt(color);
	color *= pow(p.x * p.y * (1.0 - p.x) * (1.0 - p.y) * 16.0, 0.5);
	
	gl_FragColor = vec4(color, 1.0 );
}