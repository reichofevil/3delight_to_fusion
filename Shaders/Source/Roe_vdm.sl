displacement
Roe_vdm(
    float Km = 1;
    string texturename = "";
    varying vector stangent = 0;
    float use_stangent = 0 )
{
    float dispu = 0.0;
    float dispv = 0.0;
    float dispN = 0.0;

    vector ndPdu = du*normalize(dPdu);
    vector ndPdv = dv*normalize(dPdv);
    vector diffs = normalize(Du(s)*ndPdu+ Dv(s)*ndPdv);
    vector difft = normalize(Du(t)*ndPdu+ Dv(t)*ndPdv);

    if( use_stangent != 0 )
    {
        vector udir, vdir;
        vdir = stangent ^ N;
        udir = N ^ vdir;
        vector uorient = Du(s) * dPdu + Dv(s) * dPdv;
        if( udir.uorient < 0 )
        {
            udir = -udir;
        }
        diffs = normalize(udir);
        difft = normalize(vdir);
    }

    if( texturename != "" )
    {
        dispu = float texture( texturename[0], s, t );
        dispv = float texture( texturename[1], s, t );
        dispN = float texture( texturename[2], s, t );

        P += Km*(dispN*normalize(N) + dispu*diffs + dispv*difft);
        N = calculatenormal(P);
    }
}