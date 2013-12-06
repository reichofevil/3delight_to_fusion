#include <global_illumination.h>
#include <shading_utils.h>
#include <utils.h>

color normalize_color(color c)
{
	float multiplier =
		max(max(c[0], c[1]), c[2]);

	if( multiplier>0 )
	{
		return c/multiplier;
	}

	return 0;
}

vector DL_ADV_TransformToTangentSpace(vector i_v; vector i_stangent; normal i_normal)
{
	extern point P;

	/* Create a tangent space coordinate system */
	vector anisotropy_coordsys_u = i_stangent;
	vector anisotropy_coordsys_v = i_stangent ^ i_normal;

	vector anisotropy_udir =
		i_v[0] * anisotropy_coordsys_u +
		i_v[1] * anisotropy_coordsys_v +
		i_v[2] * i_normal;

	return normalize( anisotropy_udir );
}

/*
 * Returns lighting using specified specular model
 */
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

/*
 * Absorption term
 *
 * REFERENCE
 * Equations 3.2, 3.4, 3.5 and
 * Section "3.3.2 Absorption" of
 * "Exploring the Potential of Layered BRDF Models"
 * Andrea Weidlich, Alexander Wilkie
 */
color DL_ADV_GetAbsorption(vector Nn; vector V; vector Ln; color alpha; float d)
{
	float vdotn = clamp(V.Nn, EPSILON, 1.0);
	float ldotn = clamp(Ln.Nn, EPSILON, 1.0);
	
	/* The length of the path by the thickness of the layer.
	   Equation 3.5 */
	float l = d*(1.0/vdotn + 1.0/ldotn);
	
	float r = exp( -alpha[0]*l );
	float g = exp( -alpha[1]*l );
	float b = exp( -alpha[2]*l );
	
	return color(r,g,b);
}

/*
 * Simulates the physical process of light propagation inside a layered material
 *
 * REFERENCE
 * Equation 3.1 of [3]
 */
color DL_ADV_GetLayerScale(
		vector i_L;
		vector i_V;
		vector i_N;
		float i_coating_ior;
		color i_coating_absorbtion;
		float i_coating_thickness;)
{
	vector H = normalize( i_L + i_V );

	/* Absorbtion term */
	color absorpt_term =
		DL_ADV_GetAbsorption(
				i_N,
				i_L,
				i_V,
				i_coating_absorbtion,
				i_coating_thickness);

	/* The original reference is Equation 3.1 of [3] but:
	 * 1) Internal Reflection Term produces a lot of artifacts on high IOR.
	 *    There is a note in original paper about this: "However, for a high
	 *    IOR, the effect becomes a little bit more prominent, albeit still
	 *    in a rather subtle fashion." See Chapter 3.3.3 of [3].
	 * 2) The Fresnel term is already included in each layer. */
#if 0
	/* Internal Reflection Term */
	float intrefl_term =
		internalReflectionTerm(
			i_N, i_V, i_L, H, i_coating_ior);

	/* Fresnel term */
	float fr, ft;
	fresnel(i_V, H, 1/i_coating_ior, fr, ft);
#endif

	return absorpt_term;
}

/*
 * Traces reflection or refration using specified specular model
 */
color DL_ADV_GetReflectionRefractionLayer(
		string al_envmap;
		float i_is_refration;
		float i_roughness;
		float i_ior;
		color i_fog_color;
		float i_fog_strength;
		float i_anisotropy;
		vector i_anisotropy_stangent;
		vector i_anisotropy_direction;
		normal i_normal;
		float i_samples;
		float i_model;
		float i_has_reflection;
		float i_has_environment;
		color i_weight;
		output color o_reflection_object;
		output color o_reflection_environment;)
{
	extern point P;
	extern vector I;
	float __tmp_buf;
	

	vector In = normalize(I);
	vector V = -In;
	normal Nn = i_normal;
	normal Nf = ShadingNormal(Nn);

	float is_backside = Nn.In < 0 ? 0:1;
	float is_refraction = i_is_refration;

	float ior = i_ior;
	if(is_backside == 0)
	{
		/* Front side */
		ior = 1/ior;
	}

	/* Get environment map parameters */
	uniform string envmap = al_envmap;
	uniform string envspace = "world";
	/*if( i_has_environment != 0 )
	{
		getGiEnvironmentMapParameters(envmap, envspace );
	}*/

	color result = 0;
	color environmentcontribution = 0;
	float distance = 0;

	vector trace_dir;
	string subset;

	if( is_refraction == 1 )
	{
		/* Refraction */
		trace_dir = refract( In, Nf, ior);
		if( length( trace_dir ) < EPSILON )
		{
			/* There is no refraction. It means an angle of incidence
			 * less than Brewster's angle, and the light is perfectly
			 * reflected. */
			trace_dir = reflect( In, Nf );

			is_refraction = 0;
		}

		subset = "-_3dfm_not_visible_in_refractions";
	}
	else
	{
		/* Reflection */
		trace_dir = reflect( In, Nf );

		subset = "-_3dfm_not_visible_in_reflections";
	}

	if( i_roughness == 0 )
	{
		if( i_has_reflection != 0 )
		{
			result =
				trace(
					P, trace_dir, distance,
					"samples", i_samples,
					"weight", i_weight,
					"environmentmap", envmap,
					"environmentspace", envspace,
					"environmentcontribution", environmentcontribution,
					"subset", subset);
		}
		else if( envmap != "" )
		{
			result =
				environment (envmap, vtransform(envspace, trace_dir));
			environmentcontribution = 1;
		}
	}
	/* TODO: Only Blinn is supported */
	else
	{
		vector W_o;
		normal __N;
		if( is_refraction == 1 )
		{
			/* Refraction */
			/* There is no blinn distribution for refraction
			 * in standard trace() */
			W_o = reflect(-trace_dir, -Nf);
			__N = -Nf;
		}
		else
		{
			/* Reflection */
			W_o = V;
			__N = Nf;
		}

		if( i_has_reflection == 0 )
		{
			/* Trace environment only. Temporary solution: use
			 * trace() with "subset" "__tmp__shit__" */
			subset = "__tmp__shit__";
		}

		if( i_anisotropy == 0 ||
			i_anisotropy_stangent == 0 ||
			i_anisotropy_direction == 0 )
		{
			result =
				trace(
					P, __N, distance,
					"samples", i_samples,
					"distribution", "blinn",
					"wo", W_o,
					"roughness", i_roughness,
					"weight", i_weight,
					"environmentmap", envmap,
					"environmentspace", envspace,
					"environmentcontribution", environmentcontribution,
					"subset", subset);
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
						__N );

			result =
				trace(
					P, __N, distance,
					"samples", i_samples,
					"distribution", "ashikhmin-shirley",
					"wo", W_o,
					"roughness", i_roughness * anisotropy,
					"roughnessv", i_roughness / anisotropy,
					"udir", anisotropy_udir,
					"weight", i_weight,
					"environmentmap", envmap,
					"environmentspace", envspace,
					"environmentcontribution", environmentcontribution,
					"subset", subset);
		}
	}

	if( i_fog_color != 1 && trace_dir.i_normal < 0 )
	{
		/* Fog should be if trace directed to inside the model. */
		color fog = distance * (1 - i_fog_color) * i_fog_strength;
		result *= color( exp(-fog[0]), exp(-fog[1]), exp(-fog[2]) );
	}

	o_reflection_object = result-environmentcontribution;
	o_reflection_environment = environmentcontribution;
	return result;
}

/****************************************************/
/******************** THE SHADER ********************/
/****************************************************/

void _3DelightMaterial(
		/* Diffuse parameters */
		color i_diffuse_color;
		float i_diffuse_roughness;
		color i_incandescence;

		/* Subsurface scattering parameters */
		float i_sss_on;
		string i_sss_group;
		color i_sss_scattering;
		float i_sss_scattering_scale;
		color i_sss_transmittance;
		float i_sss_transmittance_scale;
		float i_sss_ior;
		float i_sss_scale;

		/* Specular parameters */
		/* Reflection parameters */
		color i_reflect_color;
		float i_reflect_roughness;
		float i_reflect_ior;
		float i_reflect_anisotropy;
		point i_reflect_anisotropy_space;
		color i_reflect_anisotropy_direction;
		float i_reflect_samples;
		float i_reflect_has_reflection;
		float i_reflect_has_specular;
		float i_reflect_has_environment;

		/* Refraction parameters */
		color i_refract_color;
		float i_refract_roughness;
		color i_refract_fog_color;
		float i_refract_fog_strength;
		float i_refract_ior;
		float i_refract_samples;
		float i_refract_has_reflection;
		float i_refract_has_environment;

		/* Transparency parameters */
		color i_transparency;

		float i_coating_on;
		color i_coating_color;
		float i_coating_roughness;
		float i_coating_ior;
		float i_coating_samples;
		float i_coating_has_reflection;
		float i_coating_has_specular;
		float i_coating_has_environment;
		color i_coating_transmittance;
		float i_coating_thickness;

		float i_bump_layer;
		normal i_normal;
		normal i_bump_normal;
		string l_envmap;
		float i_do_gi;
		float i_gi_samples;

		output color o_outColor;
		output color o_outTransparency;)
{
	extern point P;
	extern vector I;
	extern normal N;

	/* From Maya header */
	string __raytype = "something";
	float __refracts = 0;
	float __reflects = 0;
	
	if( i_reflect_color == 0 )
		{
			__reflects = 0;
		}
	else
		{
			__reflects = 1;
		}

	if( i_refract_color == 0 )
		{
			__refracts = 0;
		}
	else
		{
			__refracts = 1;
		}

	/*if (i_transparency == 0) {__refracts = 0;}*/
	vector stangent = 0;

	rayinfo( "type", __raytype );
	uniform float __is_subsurface_ray =
		(__raytype == "subsurface") ? 1:0;
		
	uniform float __is_shadow_ray =
		(__raytype == "shadow") ? 1:0;

	uniform float __is_irradiance_ray =
		(__raytype == "irradiance") ? 1:0;
		
	/* Useful variables */
	uniform float ray_depth;
	rayinfo( "depth", ray_depth );

	normal Nn_coating = N;
	normal Nn_base = N;
	
	/* Bump */
	/*
	if( i_bump_layer != 2)
	{
		Nn_coating = i_bump_normal;
	}

	if( i_bump_layer != 1)
	{
		Nn_base = i_bump_normal;
	}*/

	Nn_coating = normalize(Nn_coating);
	Nn_base = normalize(Nn_base);

	normal Nf_coating = ShadingNormal(Nn_coating);
	normal Nf_base = ShadingNormal(Nn_base);
	/*normal	Nf_base = faceforward(normalize(N), I );*/

	/* If there is no refraction, every side should look like front side */
	if( i_refract_color == 0 )
	{
		Nn_coating = Nf_coating;
		Nn_base = Nf_base;
	}

	vector In = normalize(I);
	vector V = -In;

	float is_backside_coating = Nn_coating.In < 0 ? 0:1;
	float is_backside_base = Nn_base.In < 0 ? 0:1;

	float __tmp_buf;
	float i_reflect_model = 0;
	float i_refract_model = 0;
	float i_coating_model = 0;

	vector anisotropy_stangent = 0;
	if( stangent != 0 )
	{
		/* Get tangets from maya */
		anisotropy_stangent = normalize(stangent);
	}
	else
	{
		anisotropy_stangent =
			normalize(
				Du(i_reflect_anisotropy_space[0]) * Du(P) +
				Dv(i_reflect_anisotropy_space[1]) * Dv(P) );
	}

	if( anisotropy_stangent != 0 )
	{
		/* Bend tangent according to the shading normal */
		anisotropy_stangent = Nf_base ^ anisotropy_stangent ^ Nf_base;
	}

	color opacity = 1 - i_transparency;

	/* Shader components */
	color diffuse_color = i_diffuse_color * opacity;
	color diffuse = 0;
	color indirect_diffuse = 0;
	color sub_surface = 0;
	color incandescence = i_incandescence;

	color coating_color = 0;
	color coating = 0;
	color coating_reflect_object = 0;
	color coating_reflect_environment = 0;
	color coating_specular = 0;

	color reflect_color = i_reflect_color * opacity;
	color reflect = 0;
	color reflect_object = 0;
	color reflect_environment = 0;
	color reflect_specular = 0;

	color refract_color = i_refract_color * opacity;
	color refract = 0;
	color refract_object = 0;
	color refract_environment = 0;

	/* Coating layer */
	float coating_ior = 1;
	color coating_absorbed = 1;
	/* The absorption coefficient specifies which amount of light
	 * is _absorbed_ for which wavelength, i.e. which colours vanish.
	 * This means that we have to use the inverse transmittance */
	color coating_absorbtion = 1 - i_coating_transmittance;

	if( i_coating_on == 1 )
	{
		float coating_samples = i_coating_samples;
		if(ray_depth > 0)
		{
			coating_samples = 1;
		}

		coating_ior = i_coating_ior;
#if 0
		if(is_backside_coating == 0)
		{
			coating_ior = 1/coating_ior;
		}
#endif

		coating_color = i_coating_color * opacity;

		float coating_fresnel;
		fresnel(In, Nf_coating, 1/coating_ior, coating_fresnel, __tmp_buf);
		coating_color *= coating_fresnel;

		if( is_backside_coating == 1 )
		{
			/* Don't reflect on back side */
			coating_color = 0;
		}

		if( coating_color != 0 &&
			__reflects != 0 &&
			__is_subsurface_ray != 1 )
		{
			coating = DL_ADV_GetReflectionRefractionLayer(
					l_envmap,
					0,
					i_coating_roughness,
					coating_ior,
					1, 1,
					0, 0, 0,
					Nn_coating,
					coating_samples,
					i_coating_model,
					i_coating_has_reflection,
					i_coating_has_environment,
					coating_color,
					coating_reflect_object,
					coating_reflect_environment);
		}

		coating_absorbed *=
			DL_ADV_GetLayerScale(
					V, V,
					Nf_coating,
					coating_ior,
					coating_absorbtion,
					i_coating_thickness);
	}

	/* Reflection layer */
	float reflect_samples = i_reflect_samples;
	if(ray_depth > 0)
	{
		reflect_samples = 1;
	}

	float reflect_ior = i_reflect_ior;
#if 0
	if(is_backside_base == 0)
	{
		reflect_ior = 1/reflect_ior;
	}
#endif
	/* Layer above current layes is not air.
	 * So we have to modify current OR. */
	reflect_ior = reflect_ior/coating_ior;

	reflect_color *= 1 - CIEluminance(coating_color);

	float reflect_fresnel;
	fresnel(In, Nn_base, 1/reflect_ior, reflect_fresnel, __tmp_buf);
	reflect_color *= reflect_fresnel;

	if( is_backside_base == 1 )
	{
		/* Don't reflect on back side. Reflection from back side is already
		 * part of refraction. */
		reflect_color = 0;
	}

	if( reflect_color != 0 &&
		__reflects != 0 &&
		__is_subsurface_ray != 1 )
	{
		reflect = DL_ADV_GetReflectionRefractionLayer(
				l_envmap,
				0,
				i_reflect_roughness,
				reflect_ior,
				i_refract_fog_color,
				i_refract_fog_strength,
				i_reflect_anisotropy,
				anisotropy_stangent,
				vector(color(i_reflect_anisotropy_direction-0.5)),
				Nn_base,
				reflect_samples,
				i_reflect_model,
				i_reflect_has_reflection,
				i_reflect_has_environment,
				reflect_color,
				reflect_object,
				reflect_environment);
	}

	/* Refraction */
	refract_color *=
		1 -
		CIEluminance(coating_color) -
		CIEluminance(reflect_color);

	float refract_samples = i_refract_samples;
	if(ray_depth > 0)
	{
		refract_samples = 1;
	}

	/* Layer above current layes is not air. But in refraction case we
	 * don't have to modify current IOR because a layer between two layes
	 * doesn't affect to refraction of light:
	 *
	 *  Light *
	 *         \?1
	 *          \ |        IOR = n1
	 *           \|
	 * -------------------------
	 *            |\?3
	 *            | \ |    IOR = n2
	 *             ?2\|
	 * -------------------------
	 *                |\
	 *                | \  IOR = n3
	 *                 ?4\
	 *
	 * By Snell's Law: sin?1/sin?2=n1/n2; and sin?3/sin?4=n2/n3
	 * ?2=?3, therefore sin?1/sin?4=n1/n3, exactly like middle layer
	 * doesn't exist. But light is still absorbed by the middle layer.
	 */

	if( refract_color != 0 &&
		__refracts != 0 &&
		__is_subsurface_ray != 1 )
	{
		refract = DL_ADV_GetReflectionRefractionLayer(
				l_envmap,
				1,
				i_refract_roughness,
				i_refract_ior,
				i_refract_fog_color,
				i_refract_fog_strength,
				0, 0, 0,
				Nn_base,
				refract_samples,
				i_refract_model,
				i_refract_has_reflection,
				i_refract_has_environment,
				refract_color,
				refract_object,
				refract_environment);
	}

	/* Diffuse and Reflection specular */
	diffuse_color *=
		1 -
		CIEluminance(coating_color) -
		CIEluminance(reflect_color) -
		CIEluminance(refract_color);

	illuminance( "-__3dfm_gi_light", P, Nf_base, PI/2 )
	{
		vector Ln = normalize(L);
		if( Cl != 0 )
		{
			float nonspecular = 0;
			lightsource( "__nonspecular", nonspecular );
			float nondiffuse = 0;
			lightsource( "__nondiffuse", nondiffuse );
			string category = "";
			lightsource( "__category", category );

			

			/* Coating specular */
			color coating_light_absorbed = 1;
			if( i_coating_on == 1 )
			{
				coating_light_absorbed =
					DL_ADV_GetLayerScale(
							Ln,
							V,
							Nf_coating,
							coating_ior,
							coating_absorbtion,
							i_coating_thickness);

				if( nonspecular < 1 &&
					i_coating_has_specular != 0 &&
					/* TODO: File an issue about following line */
					category != "__3dfm_gi_light" &&
					coating_color != 0 &&
					i_coating_roughness > 0 &&
					__is_shadow_ray != 1 &&
					__is_subsurface_ray != 1 &&
					__is_irradiance_ray != 1 )
				{
					coating_specular +=
						Cl *
						DL_ADV_GetSpecularReflector(
								Ln, i_coating_roughness,
								0, 0, 0,
								Nf_coating,
								i_coating_model );
				}
			}

			/* Reflection specular */
			if( nonspecular < 1 &&
				i_reflect_has_specular != 0 &&
				category != "__3dfm_gi_light" &&
				reflect_color != 0 &&
				i_reflect_roughness > 0 &&
				__is_shadow_ray != 1 &&
				__is_subsurface_ray != 1 &&
				__is_irradiance_ray != 1 )
			{
				reflect_specular +=
					Cl * coating_light_absorbed *
					DL_ADV_GetSpecularReflector(
							Ln, i_reflect_roughness,
							i_reflect_anisotropy,
							anisotropy_stangent,
							vector(i_reflect_anisotropy_direction-0.5),
							Nf_base,
							i_reflect_model );
			}

			/* Diffuse */
			if( diffuse_color != 0 && nondiffuse < 1 )
			{
				/*if( i_sss_on != 1 || __is_subsurface_ray == 1 )
				{
					 Lighting using Oren-Nayar diffuse model */
					diffuse +=
						Cl * coating_light_absorbed * PI *
						bsdf(
							Ln, Nf_base,
							"wo", V,
							"bsdf", "oren-nayar",
							"roughness", i_diffuse_roughness );
				/*}*/
			}
		}
	}

	color gi_visibility = 1;
	color gi_env_diffuse = 0;
	color gi_color_bleeding = 0;

	if( diffuse_color != 0 )
	{
			
		if( i_sss_on != 1 || __is_subsurface_ray == 1 )
		{
			color indirect_diffuse_weight =
				coating_absorbed*diffuse_color;
					color IBL = 0;
			
			color out_visibility = 0;
			color out_environment_diffuse = 0;
			color out_color_bleeding = 0;

			/* Maya's Global Illumination */
			/*indirect_diffuse = getGlobalIlluminationComponents(
				Nf_base, indirect_diffuse_weight,
				gi_visibility, gi_env_diffuse, gi_color_bleeding );*/
			if (i_do_gi == 1)
			{			
				indirect_diffuse = trace(
					P, normalize(N),
					"raytype", "diffuse",
					"samples", i_gi_samples,
					"bsdf", "oren-nayar",
					"transmission", out_visibility,
					"weight", indirect_diffuse_weight,
					"wo", V,
					"roughness", i_diffuse_roughness,
					"environment:map", l_envmap,
					"environmentcontribution", out_environment_diffuse );
					
				out_color_bleeding = indirect_diffuse - out_environment_diffuse;
				indirect_diffuse = out_environment_diffuse + out_color_bleeding;
			}	

		}
		else
		{
			color absorption = i_sss_transmittance;

			/* Absorption should be in [0,1] range, otherwise inverting will
			 * make a negative value. */
			if( max( max( absorption[0], absorption[1] ), absorption[2] ) > 1 )
			{
				absorption = normalize_color( absorption );
			}

			absorption = normalize_color(1-absorption);
			absorption *= i_sss_transmittance_scale;

			/* Add a liitle value because a component of absorbtion
			 * should not be 0. */
			absorption += EPSILON;

			color scattering = normalize_color( i_sss_scattering );
			scattering *= i_sss_scattering_scale;
			
			color sss_skin_color = clamp(diffuse_color, 0.01, 0.99);
			
			/*sub_surface =
					subsurface(
					P, normalize(i_normal),
					"model", "grosjean",
					"diffusemeanfreepath", scattering,
					"albedo", sss_skin_color,
					"ior", i_sss_ior,
					"scale", i_sss_scale,
					"irradiance", diffuse * diffuse_color);
							
			sub_surface /= diffuse_color * coating_absorbed;

			diffuse = 0;*/
			sub_surface =
					subsurface(
							P, normalize(i_normal), /* do not pass bump normal */
							"model", "grosjean",
							"scattering", scattering,
							"absorption", absorption,
							"ior", i_sss_ior,
							"scale", i_sss_scale,
							"irradiance", diffuse * diffuse_color);
			sub_surface /= diffuse_color * coating_absorbed;
		}
		
		
		
	}

	
	
					

	o_outColor =
		coating * coating_color +
		coating_specular * coating_color +
		reflect * reflect_color * coating_absorbed +
		reflect_specular * reflect_color +
		refract * refract_color * coating_absorbed +
		diffuse * diffuse_color +
		indirect_diffuse * diffuse_color * coating_absorbed +
		incandescence * coating_absorbed +
		sub_surface * diffuse_color * coating_absorbed;
		

		o_outTransparency = 1 - opacity;
}

surface RoEShader3
(
		string diffTex = "";
		string reflColorTex = "";
		string reflRoughTex = "";
		string i_inca_tex = "";
		
		float ia_coating_on = 1 ;
		color ia_coating_color = 1;
		color ia_coating_transmittance = 1;
		float ia_coating_thickness = 0.05;
		float ia_coating_roughness = 0;
		float ia_coating_samples = 8;
		float ia_coating_ior = 1.333;
		float ia_coating_has_reflection = 1;
		float ia_coating_has_specular = 1;
		float ia_coating_has_environment = 1;

		/* Diffuse parameters */
		color ia_diffuse_color = .8;
		float ia_diffuse_roughness = 0.3;
		color ia_incandescence = 0;

		/* Reflection parameters */
		color ia_reflect_color = 1;
		float ia_reflect_roughness = 0.1;
		float ia_reflect_samples = 8;
		float ia_reflect_ior = 1.333;
		float ia_reflect_anisotropy = 0;
		color ia_reflect_anisotropy_direction = 0;
		string i_aniso_tex ="";
		point ia_reflect_anisotropy_space = 0;
		float ia_reflect_has_reflection = 1;
		float ia_reflect_has_specular = 1;
		float ia_reflect_has_environment = 1;

		/* Subsurface scattering parameters */
		float ia_sss_on = 0;
		color ia_sss_scattering = 0;
		float ia_sss_scattering_scale = 1;
		color ia_sss_transmittance = 1;
		float ia_sss_transmittance_scale = 1;
		float ia_sss_ior = 1.33;
		float ia_sss_scale = 1;
		string ia_sss_group = "SSS";

		color ia_refract_color = 0;
		float ia_refract_roughness = 0;
		float ia_refract_samples = 4;
		float ia_refract_ior = 1.3333;
		color ia_refract_fog_color = 1;
		float ia_refract_fog_strength = 0;

		color ia_transparency = 0;
		
		float ia_do_gi = 0;
		float ia_gi_samples = 64;

		float ia_bump_layer = 0;
		normal ia_normalCamera = 0;
		
		uniform string ia_envmap = "";

		output varying color oa_outColor = 0;
		output varying color oa_outTransparency = 0;	
		
)


{
	{
		
		color _diff = 1;
		color _transp = 0;
		
		
		color _diffTex = 1;
		if (diffTex != "")
		{
			_diffTex = color texture (diffTex);
		}
		else
		{
			_diffTex = ia_diffuse_color;
		}
		
		color _reflTex = 1;
		if (reflColorTex != "")
		{
			_reflTex = color texture (reflColorTex)*ia_reflect_color;
		}
		else
		{
			_reflTex = ia_reflect_color;
		}
		float _refRoughTex = 1;
		if (reflRoughTex != "")
		{
			_refRoughTex = comp((1- color texture(reflRoughTex)) * ia_reflect_roughness,0) ;
		}
		else
		{
			_refRoughTex = ia_reflect_roughness;
		}
		
		color _aniso_direction = 1;
		if (i_aniso_tex != "")
		{
			_aniso_direction = color texture (i_aniso_tex) ;
		}
		else
		{
			_aniso_direction = ia_reflect_anisotropy_direction;
		}
		color _inca_in = 0;
		if (i_inca_tex != "")
		{
			_inca_in = color texture (i_inca_tex) ;
		}
		else
		{
			_inca_in = ia_incandescence;
		}

		
		
		_3DelightMaterial(
		/* Diffuse parameters */
		_diffTex,
		/*ia_diffuse_color,*/
		ia_diffuse_roughness,
		_inca_in,

		/* Subsurface scattering parameters */
		ia_sss_on,
		ia_sss_group,
		ia_sss_scattering,
		ia_sss_scattering_scale,
		ia_sss_transmittance,
		ia_sss_transmittance_scale,
		ia_sss_ior,
		ia_sss_scale,

		/* Specular parameters */
		/* Reflection parameters */
		_reflTex,
		_refRoughTex,
		ia_reflect_ior,
		ia_reflect_anisotropy,
		/*ia_reflect_anisotropy_space,*/
		point(s,t,0),
		_aniso_direction,
		ia_reflect_samples,
		ia_reflect_has_reflection,
		ia_reflect_has_specular,
		ia_reflect_has_environment,
		
		/* Refraction parameters */
		ia_refract_color,
		ia_refract_roughness,
		ia_refract_fog_color,
		ia_refract_fog_strength,
		ia_refract_ior,
		ia_refract_samples,
		1,
		1,
		

		ia_transparency,

		ia_coating_on,
		ia_coating_color,
		ia_coating_roughness,
		ia_coating_ior,
		ia_coating_samples,
		ia_coating_has_reflection,
		ia_coating_has_specular,
		ia_coating_has_environment,
		ia_coating_transmittance,
		ia_coating_thickness,

		ia_bump_layer,
		N,
		N,
		ia_envmap,
		ia_do_gi,
		ia_gi_samples,

		oa_outColor,
		oa_outTransparency
		);
		/*_diff = i_diffuse_color;*/
		
		Ci = oa_outColor;
		Oi = 1 - oa_outTransparency;
	}

}