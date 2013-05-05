/******************************************************************************/
/*                                                                            */
/*    Copyright (c)The 3Delight Team.                                         */
/*    All Rights Reserved.                                                    */
/*                                                                            */
/******************************************************************************/

displacement
Roe_bumpy(
	float Km = 1;
	string texturename = ""; )
{
	if( texturename != "" )
	{
		float amp = Km * float texture( texturename, s, t );

		P += amp * normalize( Ng );
		N = calculatenormal( P );
	}
}
