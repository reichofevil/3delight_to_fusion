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
light roe_slideprojector_2 (
           float intensity = 1;
		   float aspect = 1.777777;
		float fov = 55;
		string fit_type = "Width";
	   string slidename = "";
	   output float focal = 55;		/* in RAT use [mattr cameraShapeX.focalLength $f] */
	   //float angle = 0.6;
	   float h_apert = 1.260;	/*1.260 [mattr cameraShapeX.horizontalFilmAperture $f] */
	 output float v_apert = 1.260/aspect;	/* .945 [mattr cameraShapeX.verticalFilmAperture $f] */
	 float decay = 2.0;
	 float beamdistribution = 2;
	 point to = point "shader" (0,0,-1);
	 color lightcolor = 1;
	uniform string shadowmap = "";
	uniform float blur = 0;
)
{
	if (fit_type == "Height"){
		focal = (v_apert*25.4)/(2*tan(radians(fov/2)));
	}
	else if (fit_type == "Width"){
		focal = (h_apert*25.4)/(2*tan(radians(fov/2)));
	}
	else if (fit_type == "Stretch"){
		v_apert *= aspect;
		focal = (h_apert*25.4)/(2*tan(radians(fov/2)));
	}
	else{
		focal = (h_apert*25.4)/(2*tan(radians(fov/2)));
	}
	
	point from = point "shader" (0,0,0);
	vector axis = normalize(vector "shader" (0,0,-1));
	float h_tan = (h_apert / 2) / (focal / 25.4);		/* horizonal camera half-cone tangent */
	float v_tan = (v_apert / 2) / (focal / 25.4);		/* vertical one */
	float angle = atan(sqrt(h_tan*h_tan + v_tan*v_tan));	/* solid angle, for illuminate() only */ 
	
	
	point Pslide = transform("shader", Ps);
	color lcol = color(1);
	if (slidename != "") {
		float zslide = zcomp(Pslide);
		float xslide = 0.5 - 0.5 * xcomp(Pslide) / h_tan / zslide;
		float yslide = 0.5 - 0.5 * ycomp(Pslide) / v_tan / zslide;
		lcol = color texture (slidename, xslide, 1-yslide);
	}
	uniform vector A = (to - from) / length(to - from);
	illuminate (from, axis, angle) {
	
		color shad = shadow( shadowmap, Ps, "blur", blur );
		float distance_squared = L.L;
		float distance = sqrt( distance_squared );

		float cosangle = L.A / distance;

		float atten = pow(cosangle, beamdistribution);
		
		if( decay == 0.0 )
			/* do nothing */ ;
		else if( decay == 2.0 )
			atten /= distance_squared;
		else if( decay == 1.0 )
			atten /= distance;
		else
			atten /= pow(distance, decay);

		atten *=
			smoothstep( cos(fov),
				cos(fov), cosangle );
		Cl =atten *intensity * (1-shad) *lcol;
	}
}
