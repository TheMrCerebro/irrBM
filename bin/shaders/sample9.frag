//===============================================
// Fork Fork fracr
// Creado por NikolaErceg en 2021-05-16
// https://www.shadertoy.com/view/NsjSDy
//===============================================

uniform float iTime;
uniform vec2 iResolution;

vec2 rot2d(vec2 p, float r)
{
    	return vec2(p.x * cos(r) + p.y * sin(r), -(p.x * sin(r)) + p.y * cos(r));
}

vec3 palette(float d)
{
	return mix(vec3(2.0, 2.02, 1.24), vec3(0.0, 0.01, 1.2), d);
}

float map(vec3 p)
{
    	for(int i=0; i<43; ++i)
    	{
        	float t = iTime * 0.01;

        	vec2 pxz = rot2d(vec2(p.x, p.z), t);
        	p.x = pxz.x;
        	p.z = pxz.y;

        	vec2 pxy = rot2d(vec2(p.x, p.y), t * 5.0);
        	p.x = pxy.x;
        	p.y = pxy.y;

       	 	vec2 pxza = abs(vec2(p.x, p.z));
        	p.x = pxza.x;
        	p.z = pxza.y;

        	p.x -= 0.11;
        	p.z -= 0.11;

	}
	return dot(sign(p), p) / 6.0;
}

vec3 rm(vec3 ro, vec3 rd)
{
    	float t = 1.0;
    	vec3 col = vec3(0.006);
    	float d;
    	for(int i=0; i<36; i++)
    	{
		vec3 p = ro + rd * t;
        	d = map(p) * 0.98;

        	if(d<0.07)
            		break;

        	if(d>22.0)
        		break;

        	col += palette(length(p) * 0.1) / (222.0 * (d));
        	t += d;
    	}
    	return col;
}

void main()
{
    	vec2 uv = (gl_FragCoord - (iResolution / 2.0)) / iResolution.x;
	vec3 ro = vec3(55.0, 55.0, -13.0);
    	vec2 roxz = rot2d(vec2(ro.x, ro.z), iTime);
    	ro.x = roxz.x;
    	ro.z = roxz.y;
    	vec3 cf = normalize(-ro);
	vec3 cs = normalize(cross(cf, vec3(-17.0, 0.0, 17.0)));
    	vec3 cu = normalize(cross(cf, cs));

    	vec3 uuv = ro + cf * 3.0 + uv.x * cs + uv.y * cu;

    	vec3 rd = normalize(uuv - ro);

	gl_FragColor = vec4(rm(ro, rd), 1.0); 
}