surface
abs_glass(

float refrSamps = 20;
float refr_ior = 1.5;
float extinction = 5;
output varying float dist = 5;
float radius =180; 
output color extColor = color(1,0,0);
float maxdist = 20;
float Kr = .5,Ks = .5,roughness	= .1;

)

{
point PP;
normal Nf;
vector V,Nn;
color Ct, Ot;
color hitCi = color(0,0,0);

color _refr = 0;
color _refl = 0;

V = normalize(I);
Nf = faceforward (normalize(N),V);
Nn = normalize(N);

float eta = (V . Nn < 0.0) ? 1.0 / refr_ior : refr_ior;
vector refrVector = refract(V,Nf,eta);
vector R = reflect(normalize(I), Nn);

_refr = trace(P, refrVector);

gather("illuminance", P, refrVector, radius*PI/180, refrSamps, "surface:Ci", hitCi, "ray:length", dist) 
{
	if(-V.Nn > 0) { 
		//Clamp the extinction color to prevent surfaces from being
		//100% color filters or absorb 100% of light
		extColor[0] = max(0.05, min(0.95, extColor[0]));
		extColor[1] = max(0.05, min(0.95, extColor[1]));
		extColor[2] = max(0.05, min(0.95, extColor[2]));
		   
		Ci[0] += exp(-extinction * dist * (1-extColor[0])) * hitCi[0];
		Ci[1] += exp(-extinction * dist * (1-extColor[1])) * hitCi[1];
		Ci[2] += exp(-extinction * dist * (1-extColor[2])) * hitCi[2];	
	} else {
		Ci += hitCi;
	}
}
_refl = trace(P, R, 
						"maxdist", maxdist, 
						"samplecone",  radius*PI/180, 
						"samples", refrSamps,
						"transmission", extColor);
						
Ci = (Ci/refrSamps)+_refl*Kr+ Ks * specular(Nf, normalize( -I ), roughness);
##Ci = _refr+_refl*Kr+ Ks * specular(Nf, normalize( -I ), roughness);
}
	
