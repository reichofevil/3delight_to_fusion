color DL_ADV_GetSpecularReflector(
		vector i_L;
		float i_roughness;
		float i_anisotropy;
		vector i_anisotropy_stangent;
		vector i_anisotropy_direction;
		normal i_normal;
		float i_model )
{
	extern vector I;

	vector In = normalize(I);
	vector V = -In;
	vector H = normalize( i_L - In );

	/* TODO: Only Blinn is supported */
	if( i_anisotropy == 0 ||
		i_anisotropy_stangent == 0 ||
		i_anisotropy_direction == 0 )
	{
		return PI * bsdf(
				i_L, i_normal,
				"distribution", "blinn",
				"wo", V,
				"roughness", i_roughness );
	}
	else
	{
		float anisotropy = 1/(1-0.75*abs(i_anisotropy));
		if( i_anisotropy < 0 )
		{
			anisotropy = 1/anisotropy;
		}

		/* Create a tangent space coordinate system */
		vector anisotropy_udir =
			DL_ADV_TransformToTangentSpace(
					i_anisotropy_direction,
					i_anisotropy_stangent,
					i_normal );

		return PI * bsdf(
				i_L, i_normal,
				"distribution", "ashikhmin-shirley",
				"wo", V,
				"roughness", i_roughness * anisotropy,
				"roughnessv", i_roughness / anisotropy,
				"udir", anisotropy_udir);
	}
}

// Compute the CIE luminance (Rec. 709) of a given color.
float luminance(color c)
{
	return
	      comp(c, 0) * 0.212671
	    + comp(c, 1) * 0.715160
	    + comp(c, 2) * 0.072169;
}

color sRGBToLinear( color inCol )
	{
	  float _r,_g,_b = 0;
	  if( comp(inCol,0) < 0.04045 ) { _r = comp(inCol,0)/12.92; }else{ _r = pow( (comp(inCol,0)+0.055)/1.055, 2.4 ); }
	  if( comp(inCol,1) < 0.04045 ) { _g = comp(inCol,1)/12.92; }else{ _g = pow( (comp(inCol,1)+0.055)/1.055, 2.4 ); }
	  if( comp(inCol,2) < 0.04045 ) { _b = comp(inCol,2)/12.92; }else{ _b = pow( (comp(inCol,2)+0.055)/1.055, 2.4 ); }
			  
	  return color(_r,_g,_b);
	}

//orenNayer diffuse, from Advanced RenderMan (L.Glitz)
color diffOrenNayar (normal Nn;  vector _V;  float roughness;)
{
	vector Vn = normalize (_V);
	/* Surface roughness coefficients for Oren/Nayar's formula */
	float sigma2 = roughness * roughness;
	float A = 1 - 0.5 * sigma2 / (sigma2 + 0.33);
	float B = 0.45 * sigma2 / (sigma2 + 0.09);
	/* Useful precomputed quantities */
	float  theta_r = acos (Vn . Nn);        /* Angle between V and N */
	vector V_perp_N = normalize(Vn-Nn*(Vn.Nn)); /* Part of V perpendicular to N */

	/* Accumulate incoming radiance from lights in C */
	color  C = 0;
	extern point P;
	illuminance (P, Nn, PI/2) {
	/* Must declare extern L & Cl because we're in a function */
	extern vector L;  extern color Cl;
	float nondiff = 0;
	//lightsource ("__nondiffuse", nondiff);
	//if (nondiff < 1) {
		vector LN = normalize(L);
		float cos_theta_i = LN . Nn;
		float cos_phi_diff = V_perp_N . normalize(LN - Nn*cos_theta_i);
		float theta_i = acos (cos_theta_i);
		float alpha = max (theta_i, theta_r);
		float beta = min (theta_i, theta_r);
		C += (1-nondiff) * Cl * cos_theta_i * 
		(A + B * max(0,cos_phi_diff) * sin(alpha) * tan(beta));
	//}
	}
	return C;
}
// Cook-Torrance specular, from Odforce.com
color specCookTorrance(normal Nn; vector V; float eta, roughness;output float __Kr,__Kt)
{
	extern point P;
	color Ct = 0;
	float m2 = roughness*roughness;
	//First normalize every vector
	vector Vn = normalize(V);
	extern vector I;
	float Kr, Kt;
	fresnel(normalize(I), Nn, eta, Kr, Kt);
	//output fresnel
	__Kr = Kr;
	__Kt = Kt;
	illuminance ( P, Nn, PI/2 ) {
		extern vector L;
		vector Ln = normalize (L);
		vector Hn = normalize (Vn + Ln);
		//Cook-Torrance parmeters
		float t = Hn.Nn;
		float t2 = t*t; //t squared
		float v = Vn.Nn;
		float vp= Ln.Nn;
		float u = Hn.Vn;

		//Now BRDF
		float D = 0.5/(m2*t2*t2) * exp( (t2-1)/(m2*t2) );
		float G = min(1 , 2* min(t*v/u, t*vp/u) );
		
		
		Ct += Cl*(Kr*D*G/(vp*v)) * Nn.Ln;
		
	}
	
	return Ct;
}

class RoEShader3
(
	color diffColor = 1;
	string diffTex = "";
	float diffRoughness = .1;
	
	color specColor = 1;
	string specColorTex = "";
	string specRoughTex = "";
	string specExpoTex = "";
	float specRoughness = diffRoughness;
	float fresnel_IOR = .8;
	
	color reflColor = 1;
	string _reflMap = "";
	string reflColorTex = "";
	string reflRoughTex = "";
	string reflIntTex = "";
	float reflRoughness = specRoughness;
	float reflSamples = 1;
	float reflMetal = 0; // 1 = is metal
	float maxdist = 20;
	
	color _transm = 1;
	
	float DoGI = 0;
	float Kind = 1;
	
	float refr_ior = 1.3;
	float Krefr = 0;
	
	float Ks = 1;
	float Kr = 1;
	
	float DoAO = 0;
	float AOsamples = 32;
	float AODist = 5;
	
	float DoPho = 0;
	float Tin = 0;

	
)


{
	public void surface(output color Ci, Oi)
	{
		
		//get diffuse/spec textures
		color _diffTex = 1;
		if (diffTex != "")
		{
			_diffTex = color texture (diffTex);
		}
		else
		{
			_diffTex = diffColor;
		}
		//get diffuse/spec textures
		color _specTex = 1;
		if (specColorTex != "")
		{
			_specTex = color texture (specColorTex);
		}
		else
		{
			_specTex = specColor;
		}
		//get diffuse/spec textures
		color _specRough = 1;
		if (specRoughTex != "")
		{
			_specRough = (1 - color texture (specRoughTex)) * specRoughness ;
		}
		else
		{
			_specRough = specRoughness;
		}
		
		color _specInt = 1;
		if (specExpoTex != "")
		{
			_specInt = (color texture (specExpoTex)) * Ks ;
		}
		else
		{
			_specInt = Ks;
		}
		color _reflTex = 1;
		if (reflColorTex != "")
		{
			_reflTex = color texture (reflColorTex);
		}
		else
		{
			_reflTex = reflColor;
		}
		color _refRoughTex = 1;
		if (reflRoughTex != "")
		{
			_refRoughTex = ( color texture(reflRoughTex)) * reflRoughness ;
		}
		else
		{
			_refRoughTex = reflRoughness;
		}
		color _refIntTex = 1;
		if (reflIntTex != "")
		{
			_refIntTex = ( color texture(reflIntTex)) * Kr ;
		}
		else
		{
			_refIntTex = Kr;
		}
		
		normal Nn = normalize(N);
		vector In = normalize(I);
		vector  nf = faceforward(Nn, In);
		normal	Nf = faceforward(normalize(N), I );
								
		vector V = -I;
		vector R = reflect(normalize(I), Nn);
				
		color _refl = 0;
		color _diff = 0;
		color _spec = 0;
		float _occl = 1;
		color _sss = 0;
		color _indirect = 0;
		color _refr = 0;
		
		color refractcolor = 1;
		
		float KrR = 1;
		float _Ks = 1;
		float Kt = 1;
		
		//physical plausible??
		float Rd, Rs;
		float T = Tin;
		Rd = clamp(diffRoughness, 0, 1);
		Rs = clamp (1- diffRoughness, 0, 1);
		float Corr = 1.0/(Rd+Rs+T);
		Corr = min(1.0, Corr);
		Rd *= Corr;
		Rs *= Corr;
		T *= Corr;
		Kt = T;
		KrR = Rs;
		
			
		illuminance ( P, Nn, PI/2 ) {
		vector Ln = normalize(L);
		_diff +=
				Cl * PI *
				bsdf(
					Ln, Nf,
					"wo", V,
					"bsdf", "oren-nayar",
					"roughness", diffRoughness );
		

		}
		
		
		
		color _transm = 0;
		
		Oi = Os;
				
		Ci = _diff*_diffTex;
	}

}