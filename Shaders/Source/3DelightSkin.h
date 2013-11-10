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
	color color
	float diffuse_roughness

	color reflect_color
	float reflect_roughness
	float reflect_samples
	float reflect_ior
	float reflect_has_reflection
	float reflect_has_specular
	float reflect_has_environment

	color sss_color
	float sss_strength
	float sss_ior
	float sss_scale
	string sss_group

	normal normalCamera
end inputs

begin outputs
	color outColor
	color outTransparency
end outputs
*/

#ifndef __3delightskin_h
#define __3delightskin_h

#include <3DelightMaterial.h>

void _3DelightSkin(
		/* Diffuse parameters */
		color i_diffuse_color;
		float i_diffuse_roughness;

		/* Subsurface scattering parameters */
		float i_sss_on;
		string i_sss_group;
		color i_sss_color;
		float i_sss_strength;
		float i_sss_ior;
		float i_sss_scale;

		/* Specular parameters */
		/* Reflection parameters */
		color i_reflect_color;
		float i_reflect_roughness;
		float i_reflect_ior;
		float i_reflect_samples;
		float i_reflect_has_reflection;
		float i_reflect_has_specular;
		float i_reflect_has_environment;

		/* Transparency parameters */
		color i_transparency;

		normal i_normal;

		output color o_outColor;
		output color o_outTransparency;)
{
	extern point P;
	extern vector I;

	/* From Maya header */
	extern string __raytype;
	extern float __is_shadow_ray;
	extern float __is_irradiance_ray;
	extern float __reflects;

	uniform float __is_subsurface_ray =
		(__raytype == "subsurface") ? 1:0;

	/* Useful variables */
	uniform float ray_depth;
	rayinfo( "depth", ray_depth );

	normal Nn = normalize(i_normal);

	normal Nf = ShadingNormal(Nn);

	vector In = normalize(I);
	vector V = -In;

	float is_backside_base = Nn.In < 0 ? 0:1;

	float __tmp_buf;
	float i_reflect_model = 0;

	color opacity = 1 - i_transparency;

	/* Shader components */
	color diffuse_color = i_diffuse_color * opacity;
	color diffuse = 0;
	color indirect_diffuse = 0;
	color sub_surface = 0;

	color reflect_color = i_reflect_color * opacity;
	color reflect = 0;
	color reflect_object = 0;
	color reflect_environment = 0;
	color reflect_specular = 0;

	/* Reflection layer */
	float reflect_samples = i_reflect_samples;
	if(ray_depth > 0)
	{
		reflect_samples = 1;
	}

	float reflect_ior = i_reflect_ior;
	if(is_backside_base == 0)
	{
		reflect_ior = 1/reflect_ior;
	}

	float reflect_fresnel;
	fresnel(In, Nf, reflect_ior, reflect_fresnel, __tmp_buf);
	reflect_color *= reflect_fresnel;

	if( is_backside_base == 1 )
	{
		/* Don't reflect on back side */
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
				1, 1,
				0, 0, 0,
				Nf,
				reflect_samples,
				i_reflect_model,
				i_reflect_has_reflection,
				i_reflect_has_environment,
				reflect_color,
				reflect_object,
				reflect_environment);
	}

	/* Diffuse and Reflection specular */
	diffuse_color *=
		1 -
		CIEluminance(reflect_color);

	illuminance( "-__3dfm_gi_light", P, Nf, PI/2 )
	{
		if( Cl != 0 )
		{
			float nonspecular = 0;
			lightsource( "__nonspecular", nonspecular );
			float nondiffuse = 0;
			lightsource( "__nondiffuse", nondiffuse );

			vector Ln = normalize(L);

			/* Reflection specular */
			if( nonspecular < 1 &&
				i_reflect_has_specular != 0 &&
				reflect_color != 0 &&
				i_reflect_roughness > 0 &&
				__is_shadow_ray != 1 &&
				__is_subsurface_ray != 1 &&
				__is_irradiance_ray != 1 )
			{
				reflect_specular +=
					Cl *
					DL_ADV_GetSpecularReflector(
							Ln, i_reflect_roughness,
							0, 0, 0,
							Nf,
							i_reflect_model );
			}

			/* Diffuse */
			if( diffuse_color != 0 && nondiffuse < 1 )
			{
				if( i_sss_on != 1 || __is_subsurface_ray == 1 )
				{
					/* Lighting using Oren-Nayar diffuse model */
					diffuse +=
						Cl * PI *
						bsdf(
							Ln, Nf,
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
			color indirect_diffuse_weight = diffuse_color;
#ifdef SHADER_TYPE_surface
			/* Maya's Global Illumination */
			indirect_diffuse = getGlobalIlluminationComponents(
				Nf, indirect_diffuse_weight,
				gi_visibility, gi_env_diffuse, gi_color_bleeding );
#else
			indirect_diffuse =
				getIndirectDiffuse( Nf, indirect_diffuse_weight );
#endif
		}
		else
		{
			color sss_skin_color = clamp(diffuse_color, 0.01, 0.99);

			sub_surface =
				subsurface(
						P, Nn,
						"model", "grosjean",
						"diffusemeanfreepath", i_sss_color * i_sss_strength,
						"albedo", sss_skin_color,
						"ior", i_sss_ior,
						"scale", i_sss_scale,
						"irradiance", diffuse * diffuse_color);
			sub_surface /= diffuse_color;

			diffuse = 0;
		}
	}

	o_outColor =
		reflect * reflect_color +
		reflect_specular * reflect_color +
		diffuse * diffuse_color +
		indirect_diffuse * diffuse_color +
		sub_surface * diffuse_color;

#ifdef SHADER_TYPE_surface
	/* 3DFM */
	if( isoutput( "aov_diffuse" ) )
	{
		outputchannel(
				"aov_diffuse",
				diffuse * diffuse_color );
	}

	if( isoutput( "aov_gi" ) )
	{
		outputchannel(
				"aov_gi",
				indirect_diffuse * diffuse_color );
	}
	if( isoutput( "aov_occlusion" )!=0 ||
		isoutput( "aov_env_diffuse" )!=0 ||
		isoutput( "aov_indirect" )!=0 )
	{
		if( isoutput( "aov_occlusion" ) )
		{
			outputchannel(
					"aov_occlusion",
					gi_visibility );
		}
		if( isoutput( "aov_env_diffuse" ) )
		{
			outputchannel(
					"aov_env_diffuse",
					gi_env_diffuse * diffuse_color );
		}
		if( isoutput( "aov_indirect" ) )
		{
			outputchannel(
					"aov_indirect",
					gi_color_bleeding * diffuse_color );
		}
	}

	if( isoutput( "aov_subsurface" ) )
	{
		outputchannel(
				"aov_subsurface",
				sub_surface * diffuse_color );
	}
	if( isoutput( "aov_specular" ) )
	{
		outputchannel(
				"aov_specular",
				reflect_specular * reflect_color );
	}

	if( isoutput( "aov_reflection" ) )
	{
		outputchannel(
				"aov_reflection",
				reflect * reflect_color );
	}
	if( isoutput( "aov_rt_reflection" ) )
	{
		outputchannel(
				"aov_rt_reflection",
				reflect_object * reflect_color );
	}
	if( isoutput( "aov_env_reflection" ) )
	{
		outputchannel(
				"aov_env_reflection",
				reflect_environment * reflect_color );
	}

	if( isoutput( "aov_camera_space_normal" ) )
	{
		outputchannel(
				"aov_camera_space_normal",
				Nf );
	}
	if( isoutput( "aov_world_space_normal" ) )
	{
		outputchannel(
				"aov_world_space_normal",
				ntransform("world", Nf) );
	}
	if( isoutput( "aov_motion_vector" ) )
	{
		outputchannel(
				"aov_motion_vector",
				motionVector() );
	}
	if( isoutput( "aov_facing_ratio" ) )
	{
		outputchannel(
				"aov_facing_ratio",
				Nf.V );
	}
#else
	float alpha = CIEluminance( opacity );

	RGBA_SET_OUTPUT(
			Diffuse,
			diffuse * diffuse_color,
			alpha );

	RGBA_SET_OUTPUT(
			Irradiance,
			indirect_diffuse * diffuse_color,
			alpha );

	RGBA_SET_OUTPUT(
			Subsurface,
			sub_surface * diffuse_color,
			alpha );

	RGBA_SET_OUTPUT(
			Specular,
			reflect_specular * reflect_color,
			alpha );

	RGBA_SET_OUTPUT(
			Reflection,
			reflect * reflect_color,
			alpha );
#endif

	o_outTransparency = 1 - opacity;
}

/****************************************************/
/******************* 3Delight Skin ******************/
/****************************************************/

void maya_3DelightSkin(
		/* Diffuse parameters */
		color i_diffuse_color;
		float i_diffuse_roughness;

		/* Reflection parameters */
		color i_reflect_color;
		float i_reflect_roughness;
		float i_reflect_samples;
		float i_reflect_ior;
		float i_reflect_has_reflection;
		float i_reflect_has_specular;
		float i_reflect_has_environment;

		/* Subsurface scattering parameters */
		color i_sss_color;
		float i_sss_strength;
		float i_sss_ior;
		float i_sss_scale;
		string i_sss_group;

		normal i_normalCamera;

		output color o_outColor;
		output color o_outTransparency;)
{
	_3DelightSkin(
		/* Diffuse parameters */
		i_diffuse_color,
		i_diffuse_roughness,

		/* Subsurface scattering parameters */
		1,
		i_sss_group,
		i_sss_color,
		i_sss_strength,
		i_sss_ior,
		i_sss_scale,

		/* Specular parameters */
		/* Reflection parameters */
		i_reflect_color,
		i_reflect_roughness,
		i_reflect_ior,
		i_reflect_samples,
		i_reflect_has_reflection,
		i_reflect_has_specular,
		i_reflect_has_environment,

		0,

		i_normalCamera,

		o_outColor,
		o_outTransparency);
}

void _3DFS_3DelightSkin(
		/* Diffuse parameters */
		color i_diffuse_color;
		float i_diffuse_roughness;

		/* Reflection parameters */
		color i_reflect_color;
		float i_reflect_roughness;
		float i_reflect_samples;
		float i_reflect_ior;
		float i_reflect_has_reflection;
		float i_reflect_has_specular;
		float i_reflect_has_environment;

		/* Subsurface scattering parameters */
		color i_sss_color;
		float i_sss_strength;
		float i_sss_ior;
		string i_sss_group;

		output color o_outColor;)
{
	extern normal N;
	extern color Oi;

	color transparency;

	_3DelightSkin(
			/* Diffuse parameters */
			i_diffuse_color,
			i_diffuse_roughness,

			/* Subsurface scattering parameters */
			1,
			i_sss_group,
			i_sss_color,
			i_sss_strength,
			i_sss_ior,
			1,

			/* Specular parameters */
			/* Reflection parameters */
			i_reflect_color,
			i_reflect_roughness,
			i_reflect_ior,
			i_reflect_samples,
			i_reflect_has_reflection,
			i_reflect_has_specular,
			i_reflect_has_environment,

			0,

			N,

			o_outColor,
			o_outTransparency);

	Oi = 1 - transparency;
}

#endif /* __advancedMaterial_h */

