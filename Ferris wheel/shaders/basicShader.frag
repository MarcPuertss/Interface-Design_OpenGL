#version 330 core

out vec4 FragColor;
in vec3 nuvolPos;
in vec4 vertex_color;

uniform bool Base;
uniform bool Nuvol;
uniform vec3 nuvolPoints[5];

void main() {
    //Comprovem si estem tractant amb l'uniform la figura base o núvol, sinó, es pinta normal.
    //(Base)Amb móduls "descartem" algunes parts per formar la reixa
    //(Núvol)Amb un bucle capturem un punt proper a 0.08 i la resta es "descarta"
    if(Base){
        if(int(gl_FragCoord.x)%10 <=4) FragColor = vertex_color;
        else if(int(gl_FragCoord.y)%10 <= 4) FragColor = vertex_color;
        else discard;
    }
    else if (Nuvol) {
        bool parteNuvol = false;
        for (int i = 0; i < 5; ++i) {
          if (length(nuvolPos - nuvolPoints[i]) < 0.08) {
            parteNuvol = true;
            break;
          }
        }
        if (!parteNuvol) discard;
             FragColor = vertex_color;
    }
    else{
       FragColor = vertex_color;
    }
}

