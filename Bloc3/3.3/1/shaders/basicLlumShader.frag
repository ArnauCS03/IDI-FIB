#version 330 core

in vec3 fcolor;
out vec4 FragColor;


in vec4 vertexSCO;
in vec3 normalSCO;


in vec3 matambFS;
in vec3 matdifFS;
in vec3 matspecFS;
in float matshinFS;

in vec4 posFocusEscenaFS;  // ja en SCO

uniform vec4 posFocusCamera;
uniform vec3 colorFocusFS;
uniform vec3 llumAmbient;


vec3 Ambient() {
    return llumAmbient * matambFS;
}

vec3 Difus (vec3 NormSCO, vec3 L, vec3 colFocus) 
{
    // Tant sols retorna el terme difús
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats
    vec3 colRes = vec3(0);
    // Càlcul component difusa, si n'hi ha
    if (dot (L, NormSCO) > 0)
      colRes = colFocus * matdifFS * dot (L, NormSCO);
    return (colRes);
}

vec3 Especular (vec3 NormSCO, vec3 L, vec4 vertSCO, vec3 colFocus) 
{
    // Tant sols retorna el terme especular!
    // Els vectors rebuts com a paràmetres (NormSCO i L) estan normalitzats
    vec3 colRes = vec3 (0);
    // Si la llum ve de darrera o el material és mate no fem res
    if ((dot(NormSCO,L) < 0) || (matshinFS == 0))
      return colRes;  // no hi ha component especular

    // Calculem R i V
    vec3 R = reflect(-L, NormSCO); // equival a: 2.0*dot(NormSCO,L)*NormSCO - L;
    vec3 V = normalize(-vertSCO.xyz); // perquè la càmera està a (0,0,0) en SCO

    if (dot(R, V) < 0)
      return colRes;  // no hi ha component especular
    
    // Calculem i retornem la component especular
    float shine = pow(max(0.0, dot(R, V)), matshinFS);
    return (matspecFS * colFocus * shine); 
}


void main() {

  // Normalitzar la L Focus Escena
    vec3 LSCOEscena = normalize(posFocusEscenaFS.xyz - vertexSCO.xyz);

  // Normalitzar la L Focus Camera
    vec3 LSCOCamera = normalize(posFocusCamera.xyz - vertexSCO.xyz);


  // Tornar a normalitzar la Normal  
    vec3 normalSCOFS = normalize(normalSCO);   


  // Focus Escena
	  vec3 fcolor = Ambient() + 
             Difus(normalSCOFS, LSCOEscena, colorFocusFS) +
             Especular(normalSCOFS, LSCOEscena, vertexSCO, colorFocusFS);

  // Focus Camera
    fcolor += Difus(normalSCOFS, LSCOCamera, colorFocusFS) +
            Especular(normalSCOFS, LSCOCamera, vertexSCO, colorFocusFS);


	FragColor = vec4(fcolor,1);	
}
