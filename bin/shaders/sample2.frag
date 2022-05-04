//===============================================
// synthetic aperture
// Creado por FabriceNeyret2 en 2014-07-13
// https://www.shadertoy.com/view/ldlSzX
//===============================================

uniform float iTime;
uniform vec2 iResolution;

#define N 23    	// number of sources

float rnd(float i)
{
	return mod(4000.*sin(23464.345*i+45.345),1.0);
}

float srnd(float i)
{
	return 2.0 * rnd(i) - 1.0;
}

void main( out vec4 fragColor, in vec2 fragCoord )
{
	vec2 uv = (2.0 * gl_FragCoord.xy - iResolution ) / iResolution.y;

	// --- calc sources contribs
	vec2 mouse = vec2(1.5*cos(.2345*iTime)-.7*sin(iTime),sin(.3214*iTime)+.5*cos(1.234*iTime))/1.5;

	float x = -0.75, xt = x;
	
	float Phi[N], D2[N];
	for (int i=0; i<N; i++)
	{
		vec2 P = 0.99 * vec2(sin(4.0 * xt), -cos(4.0 * xt));
		xt += (1.54 / float(N)) * (1.0 + 0.7 * srnd(float(i)));
		float dm = length(mouse-P), phim = dm, d = length(uv-P), phi = d - 0.1 * iTime;
		Phi[i] = (2.0 * 3.14159 / 0.04) * (phi - phim);  // stores wave attributes
		D2[i] = pow(d,1.0);

		if (d<0.01)
		{
			fragColor = vec4(0,0,1,0);
			return;
		}
	}
	
	// --- combines waves or energy
	
	float v = 0.0;
	for (int i=0; i<N; i++)
	{
		for (int j=0; j<N; j++) 
			v += cos(Phi[j]-Phi[i]) / (D2[i]*D2[j]);			
	}
		
	v = sqrt(v/2.0);
	v = v * 4.5 / float(N);

	fragColor = v * vec4(1, 0.5, 0.25, 1);
}