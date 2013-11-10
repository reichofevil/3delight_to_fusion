/******************************************************************************/
/*                                                                            */
/*    Copyright (c)The 3Delight Developers. 2013                              */
/*    All Rights Reserved.                                                    */
/*                                                                            */
/******************************************************************************/

/******************************************************************************
 * = LIBRARY
 *     3delight for Maya
 * = AUTHOR(S)
 *     Victor Yudin
 * = VERSION
 *     $Revision$
 * = DATE RELEASED
 *     $Date$
 * = RCSID
 *     $Id$
 ******************************************************************************/

/*
begin inputs
	float coating_on
	color coating_color
	color coating_transmittance
	float coating_thickness
	float coating_roughness
	float coating_samples
	float coating_ior
	float coating_has_reflection
	float coating_has_specular
	float coating_has_environment

	color color
	float diffuse_roughness
	color incandescence

	color reflect_color
	float reflect_roughness
	float reflect_samples
	float reflect_ior
	float reflect_anisotropy
	color reflect_anisotropy_direction
	init=point(ss,tt,0) point reflect_anisotropy_space
	float reflect_has_reflection
	float reflect_has_specular
	float reflect_has_environment

	float sss_on
	color sss_scattering
	float sss_scattering_scale
	color sss_transmittance
	float sss_transmittance_scale
	float sss_ior
	float sss_scale
	string sss_group

	color refract_color
	float refract_roughness
	float refract_samples
	float refract_ior

	color transparency

	float bump_layer
	normal normalCamera
end inputs

begin outputs
	color outColor
	color outTransparency
end outputs

begin shader_extra_parameters stangent
	varying vector stangent = 0; // for anisotropy of 3Delight Material
end shader_extra_parameters
*/

/*
 * 3Delight Material
 *
 * NOTES
 * - Follows closely the following paper:
 *
 * [1] "GPU-Based Importance Sampling"
 * Mark Colbert, Jaroslav Kivánek
 * http://http.developer.nvidia.com/GPUGems3/gpugems3_ch20.html
 *
 * [2] "Physical based rendering: From theory to implementation
 * Matt Pharr, Greg Humphreys
 *
 * [3] "Exploring the Potential of Layered BRDF Models"
 * Andrea Weidlich, Alexander Wilkie
 *
 * [4] "An Anisotropic Phong BRDF Model"
 * Michael Ashikhmin, Peter Shirley
 *
 * [A] "Microfacet Models for Refraction through Rough Surfaces"
 * Bruce Walter1, Stephen R. Marschner, Hongsong Li, Kenneth E. Torrance
 */

#ifndef __advancedMaterial_h
#define __advancedMaterial_h

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
	uniform string envmap = "";
	uniform string envspace = "";
	if( i_has_environment != 0 )
	{
		getGiEnvironmentMapParameters( envmap, envspace );
	}

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

		output color o_outColor;
		output color o_outTransparency;)
{
	extern point P;
	extern vector I;

	/* From Maya header */
	string __raytype = "diffuse";
	float __is_shadow_ray = 0;
	float __is_irradiance_ray = 0;
	float __reflects = 0;
	float __refracts = 0;
	vector stangent = 0;


	uniform float __is_subsurface_ray =
		(__raytype == "subsurface") ? 1:0;

	/* Useful variables */
	uniform float ray_depth;
	rayinfo( "depth", ray_depth );

	normal Nn_coating = i_normal;
	normal Nn_base = i_normal;

	/* Bump */
	if( i_bump_layer != 2)
	{
		Nn_coating = i_bump_normal;
	}

	if( i_bump_layer != 1)
	{
		Nn_base = i_bump_normal;
	}

	Nn_coating = normalize(Nn_coating);
	Nn_base = normalize(Nn_base);

	normal Nf_coating = ShadingNormal(Nn_coating);
	normal Nf_base = ShadingNormal(Nn_base);

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
	 *         \θ1
	 *          \ |        IOR = n1
	 *           \|
	 * -------------------------
	 *            |\θ3
	 *            | \ |    IOR = n2
	 *             θ2\|
	 * -------------------------
	 *                |\
	 *                | \  IOR = n3
	 *                 θ4\
	 *
	 * By Snell's Law: sinθ1/sinθ2=n1/n2; and sinθ3/sinθ4=n2/n3
	 * θ2=θ3, therefore sinθ1/sinθ4=n1/n3, exactly like middle layer
	 * doesn't exist. But light is still absorbed by the middle layer.
	 */

	if( refract_color != 0 &&
		__refracts != 0 &&
		__is_subsurface_ray != 1 )
	{
		refract = DL_ADV_GetReflectionRefractionLayer(
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
		if( Cl != 0 )
		{
			float nonspecular = 0;
			lightsource( "__nonspecular", nonspecular );
			float nondiffuse = 0;
			lightsource( "__nondiffuse", nondiffuse );
			string category = "";
			lightsource( "__category", category );

			vector Ln = normalize(L);

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
				if( i_sss_on != 1 || __is_subsurface_ray == 1 )
				{
					/* Lighting using Oren-Nayar diffuse model */
					diffuse +=
						Cl * coating_light_absorbed * PI *
						bsdf(
							Ln, Nf_base,
							"wo", V,
							"bsdf", "oren-nayar",
							"roughness", i_diffuse_roughness );
				}
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

			/* Maya's Global Illumination */
			indirect_diffuse = getGlobalIlluminationComponents(
				Nf_base, indirect_diffuse_weight,
				gi_visibility, gi_env_diffuse, gi_color_bleeding );

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

			diffuse = 0;
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


	if( __is_shadow_ray == 1 ||
		__is_subsurface_ray == 1 ||
		__is_irradiance_ray == 1 )
	{
		o_outTransparency =
			1 - (1 - refract_color * coating_absorbed) * opacity;

		/* Fog color in shadow */
		if( o_outTransparency != 0 &&
			i_refract_fog_color != 1 &&
			is_backside_base == 0 )
		{
			float distance = 0;
			trace( P, I, distance );
			color fog =
				distance *
				(1 - i_refract_fog_color) *
				i_refract_fog_strength;
			o_outTransparency *=
				color( exp(-fog[0]), exp(-fog[1]), exp(-fog[2]) );
		}

	}
	else
	{
		o_outTransparency = 1 - opacity;
	}
}

/****************************************************/
/**************** 3Delight Material *****************/
/****************************************************/

void maya_3DelightMaterial(
		/* Coating parameters */
		float i_coating_on;
		color i_coating_color;
		color i_coating_transmittance;
		float i_coating_thickness;
		float i_coating_roughness;
		float i_coating_samples;
		float i_coating_ior;
		float i_coating_has_reflection;
		float i_coating_has_specular;
		float i_coating_has_environment;

		/* Diffuse parameters */
		color i_diffuse_color;
		float i_diffuse_roughness;
		color i_incandescence;

		/* Reflection parameters */
		color i_reflect_color;
		float i_reflect_roughness;
		float i_reflect_samples;
		float i_reflect_ior;
		float i_reflect_anisotropy;
		color i_reflect_anisotropy_direction;
		point i_reflect_anisotropy_space;
		float i_reflect_has_reflection;
		float i_reflect_has_specular;
		float i_reflect_has_environment;

		/* Subsurface scattering parameters */
		float i_sss_on;
		color i_sss_scattering;
		float i_sss_scattering_scale;
		color i_sss_transmittance;
		float i_sss_transmittance_scale;
		float i_sss_ior;
		float i_sss_scale;
		string i_sss_group;

		color i_refract_color;
		float i_refract_roughness;
		float i_refract_samples;
		float i_refract_ior;

		color i_transparency;

		float i_bump_layer;
		normal i_normalCamera;

		output color o_outColor;
		output color o_outTransparency;)
{
	extern normal N;

	_3DelightMaterial(
		/* Diffuse parameters */
		i_diffuse_color,
		i_diffuse_roughness,
		i_incandescence,

		/* Subsurface scattering parameters */
		i_sss_on,
		i_sss_group,
		i_sss_scattering,
		i_sss_scattering_scale,
		i_sss_transmittance,
		i_sss_transmittance_scale,
		i_sss_ior,
		i_sss_scale,

		/* Specular parameters */
		/* Reflection parameters */
		i_reflect_color,
		i_reflect_roughness,
		i_reflect_ior,
		i_reflect_anisotropy,
		i_reflect_anisotropy_space,
		i_reflect_anisotropy_direction,
		i_reflect_samples,
		i_reflect_has_reflection,
		i_reflect_has_specular,
		i_reflect_has_environment,

		/* Refraction parameters */
		i_refract_color,
		i_refract_roughness,
		1, 1,
		i_refract_ior,
		i_refract_samples,
		1,
		1,

		i_transparency,

		i_coating_on,
		i_coating_color,
		i_coating_roughness,
		i_coating_ior,
		i_coating_samples,
		i_coating_has_reflection,
		i_coating_has_specular,
		i_coating_has_environment,
		i_coating_transmittance,
		i_coating_thickness,

		i_bump_layer,
		N,
		i_normalCamera,

		o_outColor,
		o_outTransparency);
}

void _3DFS_3DelightMaterial(
		/* Coating parameters */
		float i_coating_on;
		color i_coating_color;
		color i_coating_transmittance;
		float i_coating_thickness;
		float i_coating_roughness;
		float i_coating_samples;
		float i_coating_ior;
		float i_coating_has_reflection;
		float i_coating_has_specular;
		float i_coating_has_environment;

		/* Diffuse parameters */
		color i_diffuse_color;
		float i_diffuse_roughness;
		color i_incandescence;

		/* Reflection parameters */
		color i_reflect_color;
		float i_reflect_roughness;
		float i_reflect_samples;
		float i_reflect_ior;
		float i_reflect_anisotropy;
		color i_reflect_anisotropy_direction;
		point i_reflect_anisotropy_space;
		float i_reflect_has_reflection;
		float i_reflect_has_specular;
		float i_reflect_has_environment;

		/* Subsurface scattering parameters */
		float i_sss_on;
		color i_sss_scattering;
		float i_sss_scattering_scale;
		color i_sss_transmittance;
		float i_sss_transmittance_scale;
		float i_sss_ior;
		float i_sss_scale;
		string i_sss_group;

		color i_refract_color;
		float i_refract_roughness;
		float i_refract_samples;
		float i_refract_ior;

		color i_transparency;

		float i_bump_layer;

		output color o_outColor; )
{
	extern normal N;
	extern normal N_nobump;
	extern color Oi;

	color transparency;

	_3DelightMaterial(
		/* Diffuse parameters */
		i_diffuse_color,
		i_diffuse_roughness,
		i_incandescence,

		/* Subsurface scattering parameters */
		i_sss_on,
		i_sss_group,
		i_sss_scattering,
		i_sss_scattering_scale,
		i_sss_transmittance,
		i_sss_transmittance_scale,
		i_sss_ior,
		i_sss_scale,

		/* Specular parameters */
		/* Reflection parameters */
		i_reflect_color,
		i_reflect_roughness,
		i_reflect_ior,
		i_reflect_anisotropy,
		i_reflect_anisotropy_space,
		i_reflect_anisotropy_direction,
		i_reflect_samples,
		i_reflect_has_reflection,
		i_reflect_has_specular,
		i_reflect_has_environment,

		/* Refraction parameters */
		i_refract_color,
		i_refract_roughness,
		1, 1,
		i_refract_ior,
		i_refract_samples,
		1,
		1,

		i_transparency,

		i_coating_on,
		i_coating_color,
		i_coating_roughness,
		i_coating_ior,
		i_coating_samples,
		i_coating_has_reflection,
		i_coating_has_specular,
		i_coating_has_environment,
		i_coating_transmittance,
		i_coating_thickness,

		i_bump_layer,
		N_nobump,
		N,

		o_outColor,
		transparency);

	Oi = 1 - transparency;
}

#if 0
/* 3dfmax */
void _3Delight_Material(
			bool i_coating_on;
			color i_coating_color;
			RGBA_INPUT_NODE( i_coating_color_map );
			color i_coating_transmittance;
			RGBA_INPUT_NODE( i_coating_transmittance_map );
			float i_coating_thickness;
			RGBA_INPUT_NODE( i_coating_thickness_map );
			float i_coating_roughness;
			RGBA_INPUT_NODE( i_coating_roughness_map );
			float i_coating_samples;
			float i_coating_ior;
			RGBA_INPUT_NODE( i_coating_ior_map );
			bool i_coating_has_reflection;
			bool i_coating_has_specular;
			bool i_coating_has_environment;
			color i_diffuse_color;
			RGBA_INPUT_NODE( i_diffuse_color_map );
			float i_diffuse_roughness;
			RGBA_INPUT_NODE( i_diffuse_roughness_map );
			color i_reflect_color;
			RGBA_INPUT_NODE( i_reflect_color_map );
			float i_reflect_roughness;
			RGBA_INPUT_NODE( i_reflect_roughness_map );
			float i_reflect_samples;
			float i_reflect_ior;
			RGBA_INPUT_NODE( i_reflect_ior_map );
			float i_reflect_anisotropy;
			RGBA_INPUT_NODE( i_reflect_anisotropy_map );
			color i_reflect_anisotropy_direction;
			RGBA_INPUT_NODE( i_reflect_anisotropy_direction_map );
			bool i_reflect_has_reflection;
			bool i_reflect_has_specular;
			bool i_reflect_has_environment;
			bool i_sss_on;
			color i_sss_sctr;
			RGBA_INPUT_NODE( i_sss_sctr_map );
			float i_sss_sctr_scale;
			RGBA_INPUT_NODE( i_sss_sctr_scale_map );
			color i_sss_trnsmt;
			RGBA_INPUT_NODE( i_sss_trnsmt_map );
			float i_sss_trnsmt_scale;
			RGBA_INPUT_NODE( i_sss_trnsmt_scale_map );
			float i_sss_scale;
			RGBA_INPUT_NODE( i_sss_scale_map );
			float i_sss_ior;
			RGBA_INPUT_NODE( i_sss_ior_map );
			string i_sss_group;

			bool i_displacement_on;
			float i_displacement_space;
			float i_displacement_strength;
			RGBA_INPUT_NODE( i_displacement_strength_map );

			color i_refract_color;
			RGBA_INPUT_NODE( i_refract_color_map );
			float i_refract_roughness;
			RGBA_INPUT_NODE( i_refract_roughness_map );
			float i_refract_samples;
			float i_refract_ior;
			RGBA_INPUT_NODE( i_refract_ior_map );

			color i_transparency_color;
			RGBA_INPUT_NODE( i_transparency_color_map );

			float i_bump_layer;
			float i_bump;
			RGBA_INPUT_NODE( i_bump_map );

			RGBA_DECLARE_OUTPUT( o_output ); )
{
	extern normal N;
	extern color Oi;
	extern float s;
	extern float t;

	/* Displacement */
	if( i_displacement_on == true &&
		i_displacement_strength_map_Connected == true )
	{
		float displacement_level = RGB_CIE_LUMA( i_displacement_strength_map );
		displacement_level *= i_displacement_strength;

		string space = i_displacement_space==0 ? "object":"world";

		DISPLACEMENT( displacement_level, space );
	}

	/* Bump */
	normal N_bump = N;
	if( i_bump_map_Connected == true && i_bump != 0 )
	{
		float bump_level = RGB_CIE_LUMA(i_bump_map) * i_bump * 0.01;
		BUMP( N_bump, bump_level );
	}

	/* Get the parameters from maps */
	color coating_color = i_coating_color;
	if( i_coating_color_map_Connected == true )
	{
		coating_color = i_coating_color_map;
	}

	color coating_transmittance = i_coating_transmittance;
	if( i_coating_transmittance_map_Connected == true )
	{
		coating_transmittance = i_coating_transmittance_map;
	}

	float coating_thickness = i_coating_thickness;
	if( i_coating_thickness_map_Connected == true )
	{
		coating_thickness = RGB_CIE_LUMA( i_coating_thickness_map );
	}

	float coating_roughness = i_coating_roughness;
	if( i_coating_roughness_map_Connected == true )
	{
		coating_roughness = RGB_CIE_LUMA( i_coating_roughness_map );
	}

	float coating_ior = i_coating_ior;
	if( i_coating_ior_map_Connected == true )
	{
		coating_ior = RGB_CIE_LUMA( i_coating_ior_map );
	}

	color diffuse_color = i_diffuse_color;
	if( i_diffuse_color_map_Connected == true )
	{
		diffuse_color = i_diffuse_color_map;
	}

	float diffuse_roughness = i_diffuse_roughness;
	if( i_diffuse_roughness_map_Connected == true )
	{
		diffuse_roughness = RGB_CIE_LUMA( i_diffuse_roughness_map );
	}

	color reflect_color = i_reflect_color;
	if( i_reflect_color_map_Connected == true )
	{
		reflect_color = i_reflect_color_map;
	}

	float reflect_roughness = i_reflect_roughness;
	if( i_reflect_roughness_map_Connected == true )
	{
		reflect_roughness = RGB_CIE_LUMA( i_reflect_roughness_map );
	}

	float reflect_ior = i_reflect_ior;
	if( i_reflect_ior_map_Connected == true )
	{
		reflect_ior = RGB_CIE_LUMA( i_reflect_ior_map );
	}

	float reflect_anisotropy = i_reflect_anisotropy;
	if( i_reflect_anisotropy_map_Connected == true )
	{
		reflect_anisotropy = RGB_CIE_LUMA( i_reflect_anisotropy_map );
	}

	color reflect_anisotropy_direction = i_reflect_anisotropy_direction;
	if( i_reflect_anisotropy_direction_map_Connected == true )
	{
		reflect_anisotropy_direction = i_reflect_anisotropy_direction_map;
	}

	color sss_sctr = i_sss_sctr;
	if( i_sss_sctr_map_Connected == true )
	{
		sss_sctr = i_sss_sctr_map;
	}

	float sss_sctr_scale = i_sss_sctr_scale;
	if( i_sss_sctr_scale_map_Connected == true )
	{
		sss_sctr_scale = RGB_CIE_LUMA( i_sss_sctr_scale_map );
	}

	color sss_trnsmt = i_sss_trnsmt;
	if( i_sss_trnsmt_map_Connected == true )
	{
		sss_trnsmt = i_sss_trnsmt_map;
	}

	float sss_trnsmt_scale = i_sss_trnsmt_scale;
	if( i_sss_trnsmt_scale_map_Connected == true )
	{
		sss_trnsmt_scale = RGB_CIE_LUMA( i_sss_trnsmt_scale_map );
	}

	float sss_scale = i_sss_scale;
	if( i_sss_scale_map_Connected == true )
	{
		sss_scale = RGB_CIE_LUMA( i_sss_scale_map );
	}

	float sss_ior = i_sss_ior;
	if( i_sss_ior_map_Connected == true )
	{
		sss_ior = RGB_CIE_LUMA( i_sss_ior_map );
	}

	color refract_color = i_refract_color;
	if( i_refract_color_map_Connected == true )
	{
		refract_color = i_refract_color_map;
	}

	float refract_roughness = i_refract_roughness;
	if( i_refract_roughness_map_Connected == true )
	{
		refract_roughness = RGB_CIE_LUMA( i_refract_roughness_map );
	}

	float refract_ior = i_refract_ior;
	if( i_refract_ior_map_Connected == true )
	{
		refract_ior = RGB_CIE_LUMA( i_refract_ior_map );
	}

	color transparency_color = i_transparency_color;
	if( i_transparency_color_map_Connected == true )
	{
		transparency_color = i_transparency_color_map;
	}

	color transparency;

	_3DelightMaterial(
		/* Diffuse parameters */
		diffuse_color,
		diffuse_roughness,
		incandescence,

		/* Subsurface scattering parameters */
		i_sss_on,
		i_sss_group,
		sss_sctr,
		sss_sctr_scale,
		sss_trnsmt,
		sss_trnsmt_scale,
		sss_ior,
		sss_scale,

		/* Specular parameters */
		/* Reflection parameters */
		reflect_color,
		reflect_roughness,
		reflect_ior,
		reflect_anisotropy,
		point(s,t,0),
		reflect_anisotropy_direction,
		i_reflect_samples,
		i_reflect_has_reflection,
		i_reflect_has_specular,
		i_reflect_has_environment,

		/* Refraction parameters */
		refract_color,
		refract_roughness,
		1, 1,
		refract_ior,
		i_refract_samples,
		1,
		1,

		/* Transparency */
		transparency_color,

		i_coating_on,
		coating_color,
		coating_roughness,
		coating_ior,
		i_coating_samples,
		i_coating_has_reflection,
		i_coating_has_specular,
		i_coating_has_environment,
		coating_transmittance,
		coating_thickness,

		i_bump_layer,
		N,
		N_bump,

		o_output,
		transparency);

	Oi = 1 - transparency;
}
#endif

#endif /* __advancedMaterial_h */


