/******************************************************************************/
/*                                                                            */
/*    Copyright (c)The 3Delight Team.                                         */
/*    All Rights Reserved.                                                    */
/*                                                                            */
/******************************************************************************/

/*
	A light source for "final gathering". Has to be used with photon mapping
    to achieve its purpose.
	
	PARAMETERS
	intensity : intensity multiplier
	samples   : number of samples to use when calling occlusion().
	finalgather
	          : specifie whetever final gatherin is to be used or not. Enabling
	            final gathering means tracing "samples" ray into the scene.
	maxdist   : maximum distance for intersection testing

	---
	Aghiles Kheffache.
	Tue Oct 31 19:49:04 EST 2006
	
	edited by m.vorberg 11/2012 for reichofevil.wordpress.com
*/
light roe_indirectlight(
	float intensity = 1.0;
	float finalgather = 0.0;
	float samples = 64; 
	float maxdist = 1e36;
	float __nonspecular = 1; )
{
	normal shading_normal = normalize( faceforward(Ns, I) );

	illuminate( Ps + shading_normal ) /* shade all surface points */
	{
		Cl = 0;

		if( intensity > 0.0 )
		{
			if( finalgather == 1 )
			{
				Cl = intensity * 
					indirectdiffuse(
						Ps, shading_normal, samples, "maxdist", maxdist );
			}
			else
			{
				string globalmap = "global";
				attribute( "photon:globalmap", globalmap );

				Cl = photonmap(
					globalmap, Ps, shading_normal, "estimator", samples );
			}
		}
	}
}
