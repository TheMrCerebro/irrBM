//===============================================
// Reflecting Pseudo Kleinian
// Creado por Kamoshika en 2022-01-01
// https://www.shadertoy.com/view/ftKSzK
//===============================================

uniform float iTime;
uniform vec2 iResolution;

#define ihash(x) floor(fract(sin(x)*1426.1623)*3.-1.)*c*2.

const float pi=acos(-1.0);
const float c=0.9;
const float sp=1.0;
float ac,T,b;

void rot3d(inout vec3 v, float a, vec3 ax)
{
	ax = normalize(ax);
  	v = mix(dot(ax, v) * ax, v, cos(a)) - sin(a) * cross(ax, v);
}

float in1d(float x)
{
  	x /= c * 2.0;
  	float i = floor(x);
  	float s = 0.2;
  	float u = smoothstep(0.5 - s, 0.5 + s, fract(x));
  	return mix(ihash(i), ihash(i+1.0), u);
}

float map(vec3 p)
{
  	float d1, d2, a = 1.0;
  	vec3 p0 = p;
  	p = abs(mod(p, c * 2.0) - c);
  
  	for(int i=0; i<3; i++)
	{
    	p = 2.0 * clamp(p, -c, c) - p;
    	float k = max(1.0 / dot(p, p), 1.0);
    	p *= k;
    	a *= k;
	}

	d1 = (p.x + p.y + p.z - 1.54) / a * 0.5;
	d2 = p.y / a * 0.5;
  	float s = smoothstep(-2.0, 2.0, b - p0.z);

  	float Tm = mod(T + 0.5, 4.0);
  	if(Tm<1.0)
	{
    	d1 = mix(d1, d2, s);
    	ac += mix(a, 1.0, s);
  	}
	else if(Tm<2.)
	{
    	d1 = d2;
    	ac += 1.0;
	}
	else if(Tm<3.0)
	{
    	d1 = mix(d2, d1, s);
    	ac += mix(1.0, a, s);
	}
	else
	{
		ac += a;
	}
  
  return d1;
}

vec3 calcN(vec3 p)
{
  	vec2 e = vec2(0.001,0);
  	return normalize(vec3(map(p+e.xyy)-map(p-e.xyy),
 	 map(p+e.yxy)-map(p-e.yxy),
 	 map(p+e.yyx)-map(p-e.yyx)));
}

float fs(float f0,float c)
{
  	return f0 + (1.0 - f0) * pow(1.0 - c, 5.0);
}

vec3 hsv(float h,float s,float v)
{
	vec3 res = fract(h + vec3(0, 2, 1) / 3.0) * 6.0 - 3.0;
  	res = clamp(abs(res) - 1.0, 0.0, 1.0);
  	res = (res - 1.0) * s + 1.0;
  	res *= v;
  	return res;
}

vec3 getC(vec3 rp)
{
  	float i = floor(T + 0.5);
  	vec3 c1 = hsv(pi * 0.5 * i, 0.8, 1.0);
  	vec3 c2 = hsv(pi * 0.5 * (i + 1.0), 0.8, 1.0);
  	float v = smoothstep(-0.3, 0.3, b - rp.z);
  	return mix(c1, c2, v);
}

vec3 march(inout vec3 rp,inout vec3 rd,inout vec3 ra,vec3 cp)
{
  	vec3 col = vec3(0);
  	float d, t = 0.0;
  	ac = 0.0;
  	for(int i=0;i<100;i++)
	{
    	d = map(rp);
    	if(abs(d)<0.0001 || t>20.0)
			break;
    	t += d;
    	rp += rd * d;
  	}
  
  	vec3 n = calcN(rp);
  	vec3 ref = reflect(rd, n);
  	float fog = exp(-t * t * 0.01);
  	vec3 al = getC(rp);
  	float lp = 0.1 / abs(b - rp.z);
  
  	col += al * lp + ac * 0.0005;
  	col = mix(vec3(1), col, fog);
  
  	col *= ra;
  	ra *= al * fs(0.8, dot(ref, n)) * fog;
  
  	rp += 0.001*n;
  	rd = ref;
  
  	return col;
}

void main()
{
  	vec2 uv = vec2(gl_FragCoord.x / iResolution.x, gl_FragCoord.y / iResolution.y);
  	uv -= 0.5;
  	uv /= vec2(iResolution.y / iResolution.x, 1)*.5;
  	vec3 col = vec3(0);
  
  	T = iTime * sp / c * 0.5;
  
	vec3 cp = vec3(0, 0, -iTime * sp);
  	cp.x += in1d(cp.z * 0.5 - c);
  	cp.y += in1d(cp.z * 0.5 - c * 400.0);
  
  	b = cp.z + tan(T * pi);
  
  	vec3 rd = normalize(vec3(uv, -1));
  
	rot3d(rd,iTime*.5,vec3(1,2,9));
	rot3d(rd,pi/4.,vec3(0,1,0));
  
  	vec3 rp = cp;
  	vec3 ra = vec3(1);
  	col += march(rp,rd,ra,cp);
  	col += march(rp,rd,ra,cp);
  	col += march(rp,rd,ra,cp);
  
  	gl_FragColor = vec4(col,1.0);
}