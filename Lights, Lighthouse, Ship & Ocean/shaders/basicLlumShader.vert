#version 330 core

in vec3 vertex;
in vec3 normal;
in vec3 matamb;
in vec3 matdiff;
in vec3 matspec;
in float matshin;

out vec3 ambient;
out vec3 diffuse;
out vec3 specular;
out float shininess;



uniform mat4 proj;
uniform mat4 view;
uniform mat4 TG;
uniform mat3 normalMatrix;
uniform vec3 SolPos;
uniform vec3 F1Pos;
uniform vec3 F2Pos;

out vec3 fcolor;

out vec4 vertexSCO;
out vec3 vertexSCA;

out vec3 normalSCO;
out vec3 LSCO;
out vec3 F1PosLSCO;
out vec3 F2PosLSCO;

void main() {
    gl_Position = proj * view * TG * vec4(vertex, 1.0);

    fcolor = matdiff;
    ambient = matamb;
    diffuse = matdiff;
    specular = matspec;
    shininess = matshin;

    
    vertexSCA = vec3(TG * vec4(vertex, 1.0));
    vertexSCO = view * TG * vec4(vertex, 1.0);
    mat3 normalMatrix = inverse(transpose(mat3(view*TG)));

    
    normalSCO = normalMatrix * normal;
    normalSCO = normalize(normalSCO);

    
    LSCO = (SolPos.xyz - vertexSCO.xyz);
    F1PosLSCO = F1Pos.xyz - vertexSCO.xyz;
    F2PosLSCO = F2Pos.xyz - vertexSCO.xyz;
}
