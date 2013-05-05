/* a phong shader for my 3delight implementation in eyeon fusion*/
/* based on*/
/* Copyrighted Pixar 1989 */
/* From the RenderMan Companion p.374 */
/* Listing 16.29  Plastic surface shader using a texture map*/

/* 
 *  txtplastic(): version of plastic() shader using an optional texture map
 */

color SampleEnvironment(
    point P;  vector R;  float blur, raysamples;  string envname;)
{
   /* First trace a ray and then composite the environment on top if
      the transmission != 0 */
    color transmission;
    color ray = trace( P, R,
        "samples", raysamples,
        "samplecone", blur,
        "transmission", transmission );

    if( transmission == 0 || envname == "" )
        return ray;

    /* NOTE: should apply correct blur here */
    color env = environment(envname, vtransform("world",R) );
    return ray + transmission * env;
}

surface fu_3de_phong2( 
	float	Kd		= 1; //diffuse amount
	float   Ks		= 1; //specular amount
	float   Ka		=  1; //ambient amount
	float roughness	= .1; //equals specular exponent in fusion
	color 	specularcolor = 1; 
	string	diff_mapname = "";
	string	spec_mapname = "";
	string  spec_amount_mapname = "";
	string  spec_exponent_mapname = "";
	varying float roughness_map = 1;
	varying float Ks_map = 1;
	varying color specularcolor_map = 1;
	color spec = 1;
	float occ_samples = 16;
	float maxdist = 1000;
	float IOR = 1.3;
	float samples = 8;
	float blur = 0;
	string envname = "";
	float DoAO =  0;
	
	
	)
	

	
{
	normal	Nf = faceforward(normalize(N), I );
	float occ = 0;
	if (DoAO>0){
		occ = occlusion(P,Nf,occ_samples,"maxdist", maxdist);
		}
	else {
		occ = 0;
	}
	normal Nn = normalize(N);
    vector In = normalize(I);

	color specularcolor_map = 0;
	float Ks_map = 1;
	float roughness_map = 1;
	color spec = 1;
	color reflectColor = 0;
	float Kr, Kt;

	
	if( diff_mapname != "" ){
		Ci = color texture( diff_mapname );		/* Use s and t */
		}
	else{
		Ci = Cs;
		
	//if (Nn.In < 0) {
	//		fresnel(In,Nf,1/IOR,Kr,Kt);
	//		vector r = reflect(In,Nf);
	//		Ci += SampleEnvironment(P, r, blur, samples, envname);
	//		}
		}
	
		
	if( spec_mapname != "" )
		specularcolor_map = color texture( spec_mapname );		/* Use s and t */
	else specularcolor_map = specularcolor;
	
	
	//if( spec_amount_mapname != "" )
	//	Ks_map = float texture( spec_amount_mapname );		/* Use s and t */
	//else Ks_map = Ks;
	
	spec *= Ks * specularcolor;
	
	//if( spec_exponent_mapname != "" )
	//	roughness_map = float texture( spec_exponent_mapname );		/* Use s and t */
	//else roughness_map = 1;
	
	//vector R = reflect(normalize(I), Nn);
	
	
		
	Oi = Os ;
	//Ci = Cs*Oi;	
	//Ci = Oi * ( Ci + spec * specular(Nf, normalize( -I ), (roughness)));
	Ci = (1-occ)* Oi * ( Ci * (Ka*ambient() + Kd*diffuse(Nf)) + Ks*specularcolor * specular(Nf, normalize( -I ), (roughness)));
	//Ci = Os * (Ci * (diffuse(Nf)+ambient()) + specularcolor * specular(Nf, normalize( -I ), (roughness)));
}