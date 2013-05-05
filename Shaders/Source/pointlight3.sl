/*    Raytraced point light source shader
 *
 *     Pixie is:
 *     (c) Copyright 1999-2003 Okan Arikan. All rights reserved.
 *  Softness added by Atom.
 *  adopted by m.vorberg for reichofevil.wordpress.com 
 */

light pointlight3 (  float intensity = 1;
                  color lightcolor = 1;
                  float blur = 0.1;
                  float samples = 50;
                  point from = point "shader" (0,0,0); 
				  float decay = 2.0;
				  string __category = "" ;
				  float shadow_on = 1.0;
				  )
	{
	illuminate (from) {
        float distance_squared = L.L;
		float distance = sqrt( distance_squared );
				
		color vis;
        vis =  shadow_on*(1 - shadow("raytrace",Ps,"blur",blur,"samples",samples));
		
		if( decay == 2.0 )
		{
			Cl = vis*intensity * lightcolor / distance_squared;
		}
		else if( decay == 1.0 )
		{
			Cl = vis*intensity * lightcolor / distance;
		}
		else if( decay == 0.0 )
		{
			Cl = vis*intensity * lightcolor;
		}
		else 
		{
			Cl = vis*intensity * lightcolor / pow(distance_squared, 0.5 * decay ); 
		}
    }
}