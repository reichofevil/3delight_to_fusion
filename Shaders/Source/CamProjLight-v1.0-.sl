/*
	This lightsource will create accurate camera projection lighting.
	In many cases it's easier to use light vs surface shader to do this kind of job.
	The shader will work well if we render either from a projecting camera or not.

	If you are using MTOR, you can't attach a shader to camera directly.
	You have to parent Maya spotlight(s) to you camera(s), reset its xforms and attach this shader
	to it instead. Maya spot's attributes like cone angle, etc will be ignored.
	To match camera fov angles you should provide the following camera attributes: 
	focal length, horizonal and vertical aperture sizes.
*/
light camProj (
           float intensity = 1;
	   string slidename = "";
	   float focal = 55;		/* in RAT use [mattr cameraShapeX.focalLength $f] */
	   float angle = 0.6;
	   float h_apert = 1.260;	/* [mattr cameraShapeX.horizontalFilmAperture $f] */
	   float v_apert = 0.945;	/* [mattr cameraShapeX.verticalFilmAperture $f] */
)
{
	point from = point "shader" (0,0,0);
	vector axis = normalize(vector "shader" (0,0,-1));
	float h_tan = (h_apert / 2) / (focal / 25.4);		/* horizonal camera half-cone tangent */
	float v_tan = (v_apert / 2) / (focal / 25.4);		/* vertical one */
	//float angle = atan(sqrt(h_tan*h_tan + v_tan*v_tan));	/* solid angle, for illuminate() only */ 
	
	
	point Pslide = transform("shader", Ps);
	color lcol = color(1);
	if (slidename != "") {
		float zslide = zcomp(Pslide);
		float xslide = 0.5 - 0.5 * xcomp(Pslide) / h_tan / zslide;
		float yslide = 0.5 - 0.5 * ycomp(Pslide) / v_tan / zslide;
		lcol = color texture (slidename, xslide, yslide);
	}

	illuminate (from, axis, angle) {
		Cl = intensity * lcol;
	}
}
