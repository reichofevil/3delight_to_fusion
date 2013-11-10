/*# a simple shader showing off 3Delight's raytrace subsurface scattering features.
# Paolo Berto Durante. <Thu Nov 07 12:21:00 EST 2013>*/
surface simple_rtss( float Ks = .5;
                float Kd = .5;
                float roughness = 0.2;
                color diffuse_color = color(1, 1, 1);
                float sss_unit_multiplier = 1; // use powers of 10
                string diffuse_texture = "";
                float sss_radius = 5;
                color sss_tint = color(0.264, 0.939, 0.316);
                float sss_samples = 64 )
{
    normal Nn = normalize(N);
    normal Nf = faceforward(Nn, I);
    vector Vn = normalize(-I);

    uniform string raytype = "unknown";
    rayinfo( "type", raytype );
    
    color diffuseC = 0;
    color specularC = 0;
    color subsurfaceC = 0;
	
	float i_sss_scale = .5;
	float scattering = .5;
	color absorption = color(0.964, 0.239, 0.316);
	
	

    color Ct = ( diffuse_texture != "" ) ?
             diffuse_color * texture( diffuse_texture, "filter", "gaussian" ) : diffuse_color;

    illuminance ( P , Nn , PI / 2 )
    {
        vector Ln = normalize( L ) ;
    
        diffuseC += Cl * Ct * Kd * PI * bsdf( Ln , Nn ,
                                            "bsdf" , "oren-nayar" ,
                                            "wo" , Vn ,
                                            "roughness" , roughness );

        specularC += Cl * Ks * PI * bsdf( Ln , Nn ,
                                         "bsdf" , "blinn" ,
                                         "wo" , Vn ,
                                         "roughness" , roughness ,
                                         "eta" , 1 / 1.55 ) ;
    }
    
    
    if( raytype == "subsurface" || raytype == "diffuse" )
    {
            Ci = diffuseC;
    }
    else
    {
        color sss_albedo_color = clamp(Ct, 0.01, 0.99);

        /*subsurfaceC = subsurface(P, Nn,
                     "type", "raytrace",
                     "model", "grosjean",
                     "diffusemeanfreepath", sss_tint * Ct * sss_radius,
                     "albedo", sss_albedo_color,
                     "scale", sss_unit_multiplier,
                     "irradiance" , diffuseC ,
                     "ior" , 1.55, 
                     "samples", sss_samples);*/
					 
		subsurfaceC = 	 subsurface(
							"type", "raytrace",
							"model", "grosjean",
							"scattering", scattering,
							"absorption", absorption,
							"ior", 1.55,
							"scale", i_sss_scale,
							"irradiance", diffuseC);
    }

    Ci = subsurfaceC + diffuseC + specularC;
}