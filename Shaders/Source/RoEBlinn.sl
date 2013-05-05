/******************************************************************************/
/*                                                                            */
/*    Copyright (c)The 3Delight Team.                                         */
/*    All Rights Reserved.                                                    */
/*                                                                            */
/******************************************************************************/

surface 
RoEBlinn(
	float Ks = .5;
	float Kd = .5;
	float Ka = 1;
	float roughness = .1;
	color specularcolor = 1;
	float DoAO = 0;
	float AOsamples = 32;
	float AODist = 5;
	)
{
	normal Nf =  faceforward( normalize(N), I );
	
	vector V = -I;
	vector Vn = normalize(V);
	vector Nn = normalize (N);
	float occ = 0;
	if (DoAO>0){
			occ = occlusion(P,Nf,AOsamples,"maxdist", AODist);
		}
		else {
			occ = 0;
		}
	
	Oi = Os;
	Ci = (1-occ)*( Cs * (Ka * ambient() + Kd * diffuse(Nf)) + 
		specularcolor * Ks * specularstd(Nn,Vn,roughness)  );

	Ci *= Oi;
}
