/* modified by michael vorberg for 3deligt to fusion plugin*/
/* reichofevil.wordpress.com*/

color
LocIllumWardAnisotropic (normal N; vector V;
                        vector xdir; float xroughness, yroughness;)

{
    float sqr (float x) { return x*x; }
    
    float cos_theta_r = clamp (N.V, 0.0001, 1);
    vector X = xdir / xroughness;
    vector Y = (N ^ xdir) / yroughness;
    
    color C = 0;
    extern point P;
    illuminance (P, N, PI/2) {
    /* Must declare beacause extern L&Cl because we're in a function */
    extern vector L; extern color Cl;
    float nonspec = 0;
    lightsource ("__nonspecular", nonspec);
    if (nonspec < 1) {
        vector LN = normalize (L);
        float cos_theta_i = LN . N;
        if (cos_theta_i > 0.0) {
            vector H = normalize (V + LN);
            float rho = exp (-2 * (sqr(X.H) + sqr(Y.H)) / (1 + H.N)) / sqrt (cos_theta_i * cos_theta_r);
            C += Cl * ((1-nonspec) * cos_theta_i * rho);
        }
      }
    }
	return C;
}


surface
RoEWard ( float Ka = 1, Kd = 0.1, Ks = .9;
		float uroughness = 0.35, vroughness = 0.2; 
		color basecolor = color (.5, .5, .5);
		float DoAO = 0;
		float AOsamples = 32;
		float AODist = 5;
		color specularcolor = 1;
)
{
    normal Nf = faceforward (normalize(N), I);
    vector xdir = normalize (dPdu);
	vector V = normalize(-I);
	
	float occ = 0;
	if (DoAO>0){
			occ = occlusion(P,Nf,AOsamples,"maxdist", AODist);
		}
		else {
			occ = 0;
		}
		
	color spec = LocIllumWardAnisotropic(N, V, xdir, uroughness, vroughness); 
	
	Oi = Os;
	Ci = (1-occ)* (Cs * (Ka * ambient() + Kd * diffuse(Nf)) + specularcolor * Ks * spec);
	Oi = Os;  Ci *= Oi;
}

