/******************************************************************************/
/*                                                                            */
/*    Copyright (c)The 3Delight Team.                                         */
/*    All Rights Reserved.                                                    */
/*                                                                            */
/******************************************************************************/

light
pointlight2( 
	float intensity = 1;
	color lightcolor = 1;
	point from = point "shader" (0,0,0);
	float decay = 2.0;
	string __category = "" )
{
	illuminate( from )
	{
		float distance_squared = L.L;
		float distance = sqrt( distance_squared );

		if( decay == 2.0 )
		{
			Cl = intensity * lightcolor / distance_squared;
		}
		else if( decay == 1.0 )
		{
			Cl = intensity * lightcolor / distance;
		}
		else if( decay == 0.0 )
		{
			Cl = intensity * lightcolor;
		}
		else 
		{
			Cl = intensity * lightcolor / pow(distance_squared, 0.5 * decay ); 
		}
	}
}
