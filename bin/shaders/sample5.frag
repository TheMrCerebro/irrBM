//===============================================
// another synthwave sunset thing
// Creado por stduhpf en 2020-04-16
// https://www.shadertoy.com/view/tsScRK
//===============================================

uniform float iTime;
uniform vec2 iResolution;

float jTime;

float pow512(float a)
{
	for(int i=0; i<8; i++)
		a *= a;
    return a * a;
}

float pow1d5(float a)
{
    return a * sqrt(a);
}

float hash21(vec2 co)
{
    return fract(sin(dot(co.xy,vec2(1.9898,7.233)))*45758.5433);
}

float hash(vec2 uv)
{
    float a = smoothstep(1.0, 8.0, abs(uv.x));
    float w = a>0.0?(1.0-0.4*pow512(0.51+0.49*sin((0.02*(uv.y+0.5*uv.x)-jTime)*2.0))):0.0;
    return a > 0.0 ? a * pow1d5(hash21(uv)) * w : 0.0;
}

vec2 trinoise(vec2 uv)
{
    const float sq = sqrt(3.0/2.0);
    uv.x *= sq;
    uv.y -= 0.5*uv.x;
    vec2 d = fract(uv);
    uv -= d;
    if(dot(d, vec2(1))<1.0)
	{
        float n1 = hash(uv);
        float n2 = hash(uv+vec2(1,0));
        float n3 = hash(uv+vec2(0,1));
        float nmid = mix(n2,n3,d.y);
        float ng = mix(n1,n3,d.y);
        float dx = d.x/(1.0-d.y);
        return vec2(mix(ng,nmid,dx),min(min((1.-dx)*(1.-d.y),d.x),d.y));
	}
	else
	{
    	float n2 = hash(uv+vec2(1,0));
        float n3 = hash(uv+vec2(0,1));
        float n4 = hash(uv+1.);
        float nmid = mix(n2,n3,d.y);
        float nd = mix(n2,n4,d.y);
        float dx = (1.0-d.x)/(d.y);
        return vec2(mix(nd,nmid,dx),min(min((1.0-dx)*d.y,1.0-d.x),1.0-d.y));
	}
    return vec2(0);
}


vec2 map(vec3 p)
{
    vec2 n = trinoise(p.xz);
    return vec2(p.y-2.*n.x,n.y);
}

vec3 grad(vec3 p)
{
    const vec2 e = vec2(.005,0);
    float a =map(p).x;
    return vec3(map(p+e.xyy).x-a
                ,map(p+e.yxy).x-a
                ,map(p+e.yyx).x-a)/e.x;

}

vec2 intersect(vec3 ro,vec3 rd)
{
    float d =0.0,h=0.0;
    for(int i = 0;i<500;i++)
	{
        vec3 p = ro+d*rd;
        vec2 s = map(p);
        h = s.x;
        d+= h*0.5;
        if(abs(h)<0.003*d)
            return vec2(d,s.y);
        if(d>150.0|| p.y>2.0)
			break;
    }
    
    return vec2(-1);
}


vec3 sun(vec3 rd,vec3 ld,vec3 base)
{
	float sun = smoothstep(0.21,0.2,distance(rd,ld));
    if(sun>0.0)
	{
        float yd = (rd.y-ld.y);
        float a =sin(3.1*exp(-(yd)*14.0)); 
        sun*=smoothstep(-0.8,0.0,a);
        base = mix(base,vec3(1.0,0.8,0.4)*0.75,sun);
    }
    return base;
}

vec3 gsky(vec3 rd,vec3 ld,bool mask)
{
    float haze = exp2(-5.*(abs(rd.y)-.2*dot(rd,ld)));
    float st = mask?pow512(hash21((rd.xy+vec2(300.1,100)*rd.z)*10.))*(1.-min(haze,1.)):0.;
    vec3 col=clamp(mix(vec3(.4,.1,.7),vec3(.7,.1,.4),haze)+st,0.,1.);
    return mask?sun(rd,ld,col):col;
   
}

void main()
{
    vec2 uv = (2.0*gl_FragCoord-iResolution.xy)/iResolution.x;
    
    float dt = fract(hash21(1.0 * gl_FragCoord)+iTime);
    jTime = mod(iTime-dt*0.001*.25,4000.);
    vec3 ro = vec3(0.,1,(-2000.+jTime)*10.);
        
    vec3 rd = normalize(vec3(uv,.75));//vec3(uv,sqrt(1.-dot(uv,uv)));
    
    vec2 i = intersect(ro,rd);
    float d = i.x;
    
    vec3 ld = normalize(vec3(0,.125+.05*sin(.1*jTime),1));

    float fog = d>0.?exp2(-d*.14):0.;
    vec3 sky = gsky(rd,ld,d<0.);
    
    vec3 p = ro+d*rd;
    vec3 n = normalize(grad(p));
    
    float diff = dot(n,ld)+.1*n.y;
    vec3 col = vec3(0.1,0.11,0.18)*diff;
    
    vec3 rfd = reflect(rd,n); 
    vec3 rfcol = gsky(rfd,ld,true);
    
    col = mix(col,rfcol,0.05+0.95*pow(max(1.0+dot(rd,n),0.0), 5.0));
    col = mix(col,vec3(0.8, 0.1, 0.92),smoothstep(0.05, 0.0, i.y));
    col = mix(sky,col,fog);

    //no gamma for that old cg look
    if(d<0.0)
        d = 1e6;

    d = min(d,10.0);

    gl_FragColor = vec4(clamp(col, 0.0, 1.0), d < 0.0? 0.0 : 0.1+exp2(-d));
}