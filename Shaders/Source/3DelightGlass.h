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

	color refract_color
	float refract_roughness
	color refract_fog_color
	float refract_fog_strength
	float refract_samples
	float refract_ior

	color transparency

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

#ifndef __3delightglass_h
#define __3delightglass_h

#include "3DelightMaterial.h"

/****************************************************/
/***************** 3Delight Glass *******************/
/****************************************************/

void maya_3DelightGlass(
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

		color i_refract_color;
		float i_refract_roughness;
		color i_refract_fog_color;
		float i_refract_fog_strength;
		float i_refract_samples;
		float i_refract_ior;

		color i_transparency;

		normal i_normalCamera;

		output color o_outColor;
		output color o_outTransparency;)
{
	extern normal N;
	extern float ss;
	extern float tt;

	_3DelightMaterial(
		/* Diffuse parameters */
		i_diffuse_color,
		i_diffuse_roughness,
		color(0), // i_incandescence

		/* Subsurface scattering parameters */
		0, // i_sss_on
		"", // i_sss_group
		color( 0.73, 0.873, 1 ), // i_sss_scattering
		3, // i_sss_scattering_scale
		color( 0.79, 0.59, 0.29 ), // i_sss_transmittance
		0.01, // i_sss_transmittance_scale
		1.3, // i_sss_ior
		1.0, // i_sss_scale

		/* Specular parameters */
		/* Reflection parameters */
		i_reflect_color,
		i_reflect_roughness,
		i_reflect_ior,
		0.0, // i_reflect_anisotropy
		point(ss,tt,0), // i_reflect_anisotropy_space
		color(0.5, 1, 0.5), // i_reflect_anisotropy_direction
		i_reflect_samples,
		i_reflect_has_reflection,
		i_reflect_has_specular,
		i_reflect_has_environment,

		/* Refraction parameters */
		i_refract_color,
		i_refract_roughness,
		i_refract_fog_color,
		i_refract_fog_strength,
		i_refract_ior,
		i_refract_samples,
		1, // i_refract_has_reflection
		1, // i_refract_has_environment

		i_transparency,

		0, // i_coating_on
		color(1), // i_coating_color
		0.0, // i_coating_roughness
		1.3, // i_coating_ior
		1, // i_coating_samples
		0, // i_coating_has_reflection
		0, // i_coating_has_specular
		0, // i_coating_has_environment
		color( 1, 0.5, 0.1 ), // i_coating_transmittance
		0, // i_coating_thickness

		0, // i_bump_layer
		N,
		i_normalCamera,

		o_outColor,
		o_outTransparency);
}

void _3DFS_3DelightGlass(
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

		/* Refraction parameters */
		color i_refract_color;
		float i_refract_roughness;
		color i_refract_fog_color;
		float i_refract_fog_strength;
		float i_refract_samples;
		float i_refract_ior;

		color i_transparency;

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
		color(0), // i_incandescence

		/* Subsurface scattering parameters */
		0, // i_sss_on
		"", // i_sss_group
		color( 0.73, 0.873, 1 ), // i_sss_scattering
		3, // i_sss_scattering_scale
		color( 0.79, 0.59, 0.29 ), // i_sss_transmittance
		0.01, // i_sss_transmittance_scale
		1.3, // i_sss_ior
		1.0, // i_sss_scale

		/* Specular parameters */
		/* Reflection parameters */
		i_reflect_color,
		i_reflect_roughness,
		i_reflect_ior,
		0.0, // i_reflect_anisotropy
		point(0), // i_reflect_anisotropy_space
		color(0.5, 1, 0.5), // i_reflect_anisotropy_direction
		i_reflect_samples,
		i_reflect_has_reflection,
		i_reflect_has_specular,
		i_reflect_has_environment,

		/* Refraction parameters */
		i_refract_color,
		i_refract_roughness,
		i_refract_fog_color,
		i_refract_fog_strength,
		i_refract_ior,
		i_refract_samples,
		1, // i_refract_has_reflection
		1, // i_refract_has_environment

		i_transparency,

		0, // i_coating_on
		color(1), // i_coating_color
		0.0, // i_coating_roughness
		1.3, // i_coating_ior
		1, // i_coating_samples
		0, // i_coating_has_reflection
		0, // i_coating_has_specular
		0, // i_coating_has_environment
		color( 1, 0.5, 0.1 ), // i_coating_transmittance
		0, // i_coating_thickness

		0, // i_bump_layer
		N_nobump,
		N,

		o_outColor,
		transparency);

	Oi = 1 - transparency;
}

#endif /* __advancedMaterial_h */


