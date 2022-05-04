//===============================================
// Apollian with a twist
// Creado por mrange en 2021-02-03
// https://www.shadertoy.com/view/Wl3fzM
//===============================================

uniform float iTime;
uniform vec2 iResolution;

#define PI              3.141592654
#define TAU             (2.0*PI)
#define L2(x)           dot(x, x)
#define PSIN(x)         (0.5+0.5*sin(x))
#define lp1             vec3(0.5, 1.25, 0.5)
#define lp2             vec3(-0.5, 1.25, 0.5)
#define ss              15.0
#define zoom            0.5
#define K               vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0)

vec2 rot2d(vec2 p, float r)
{
    return vec2(p.x * cos(r) - p.y * sin(r),p.x * sin(r) + p.y * cos(r));
}

vec3 hsv2rgb(vec3 c)
{
	vec3 p = abs(fract(vec3(c.x) + vec3(K.x,K.y,K.z)) * 6.0 - vec3(K.w));
	return c.z * mix(vec3(K.x), clamp(p - vec3(K.x), 0.0, 1.0), c.y);
}

float apollian(vec4 p, float s)
{
	float scale = 1.0;

  	for(int i=0; i<7; ++i)
	{
    	p = -1.0 + 2.0*fract(0.5*p+0.5);
		float r2 = dot(p,p);
    	float k  = s/r2;
    	p *= k;
    	scale *= k;
	}
  
	return abs(p.y)/scale;
}

float weird(vec2 p)
{
    float z = 4.0, r = 0.5;
    float tm = 0.2*iTime;

    vec4 off = vec4(r * PSIN(tm * sqrt(3.0)), r * PSIN(tm * sqrt(1.5)), r * PSIN(tm * sqrt(2.0)), 0.0);

    p = rot2d(p, iTime*0.1);
    vec4 pp = vec4(p.x, p.y, 0.0, 0.0) + off;
    pp.w = 0.125 * (1.0-tanh(length(vec3(pp.x,pp.y,pp.z))));

    vec2 ppyz = rot2d(vec2(pp.y,pp.z), tm);
    pp.y = ppyz.x;
    pp.z = ppyz.y;

    vec2 ppxz = rot2d(vec2(pp.x,pp.z),tm * sqrt(0.5));
    pp.x = ppxz.x;
    pp.z = ppxz.y;

    pp /= z;
    float d = apollian(pp, 1.2);
    return d*z;
}

float df(vec2 p)
{
  p /= zoom;
  float d0 = weird(p);
  return d0*zoom;
}

vec3 postProcess(vec3 col, vec2 q)
{
  col = pow(clamp(col,0.0,1.0),vec3(1.0/2.2));
  col = col*0.6+0.4*col;
  col = mix(col, vec3(dot(col, vec3(0.33))), -0.4);  // saturation
  col *= 0.5+0.5*pow(19.0*q.x*q.y*(1.0-q.x)*(1.0-q.y),0.7);  // vigneting
  return col;
}

void main()
{
	vec2 q = gl_FragCoord.xy/iResolution.xy;
	vec2 p = -1.0 + 2.0 * q;
	p.x *= iResolution.x/iResolution.y;
  
	float aa = 2.0/iResolution.y;

	float d = df(p);

	float b = -0.125;
	float t = 10.0;

  	vec3 ro = vec3(0.0, t, 0.0);
  	vec3 pp = vec3(p.x, 0.0, p.y);

  	vec3 rd = normalize(pp - ro);

  	float bt = -(t-b)/rd.y;

  	vec3 bp   = ro + bt*rd;

  	vec3 srd1 = normalize(lp1-bp);
  	vec3 srd2 = normalize(lp2-bp);
  	float bl21= L2(lp1-bp);
  	float bl22= L2(lp2-bp);

  	float st1= -b/srd1.y;
	
	vec3 sp1 = bp + srd1 * st1;
  	vec3 sp2 = bp + srd2 * st1;
	float sd1= df(vec2(sp1.x,sp1.z));
    float sd2= df(vec2(sp2.x,sp2.z));
 
    vec3 col = vec3(1.0) * (1.0 - exp(-ss * (max(sd1, 0.0)))) / bl21;
    col += vec3(0.5) * (1.0 - exp(-ss * (max(sd2, 0.0)))) / bl22;
  
    float l = length(p);
    float hue = fract(0.75 * l - 0.3 * iTime) + 0.3 + 0.15;
    float sat = 0.75 * tanh(2.0 * l);
    vec3 hsv = vec3(hue, sat, 1.0);
    vec3 bcol = hsv2rgb(hsv);
    col *= (1.0 - tanh(0.75 * l)) * 0.5;
    col = mix(col, bcol, smoothstep(-aa, aa, -d));
    col += 0.5 * sqrt(vec3(bcol.z,bcol.x,bcol.y))*(exp(-(10.0 + 100.0 * tanh(l)) * max(d, 0.0)));

    col = postProcess(col, q);
  
	gl_FragColor = vec4(col, 1.0);
}