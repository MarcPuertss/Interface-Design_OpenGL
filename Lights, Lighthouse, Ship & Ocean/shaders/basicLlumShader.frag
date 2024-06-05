#version 330 core

in vec3 fcolor;
in vec3 ambient;
in vec3 diffuse;
in vec3 specular;
in float shininess;

out vec4 FragColor;

in vec4 vertexSCO;
in vec3 vertexSCA;
in vec3 normalSCO;
in vec3 LSCO;
in vec3 F1PosLSCO;
in vec3 F2PosLSCO;

uniform vec3 FColor;
uniform vec3 llumAmbient;
uniform vec3 SolPos;
uniform vec3 solColor;
uniform mat3 normalMatrix;
uniform vec3 DirF1;
uniform vec3 DirF2;
uniform mat4 view;

uniform vec3 posFocusTorxes[6];
uniform vec3 torchColors[6];

uniform bool isSea;
uniform float time;

//************************************************************************************************************

vec3 Ambient(vec3 llumAmbient) {
    return llumAmbient * ambient;
}

vec3 Difus(vec3 NormSCO, vec3 L, vec3 colFocus) {
    vec3 colRes = vec3(0);
    if (dot(L, NormSCO) > 0)
        colRes = colFocus * diffuse * dot(L, NormSCO);
    return colRes;
}

vec3 Especular(vec3 NormSCO, vec3 L, vec4 vertSCO, vec3 colFocus) {
    vec3 colRes = vec3(0);
    if (dot(NormSCO, L) < 0 || shininess == 0)
        return colRes;

    vec3 R = reflect(-L, NormSCO);
    vec3 V = normalize(-vertSCO.xyz);

    if (dot(R, V) < 0)
        return colRes;

    float shine = pow(max(0.0, dot(R, V)), shininess);
    return specular * colFocus * shine;
}

vec3 WaveNormal(vec2 waveDirNN, float waveLength, vec3 vertex_world, float time){
    float steepness = .5;
    vec2 waveDir = normalize(waveDirNN);
    float wl = 2;
    float f = (2 * 3.14159 / waveLength) * (dot(waveDir, vertex_world.xz) - 0.25 * time);

    vec3 B = vec3(
        1 - (steepness * waveDir.x * waveDir.x * cos(f)),
        waveDir.x * sin(f),
        -steepness * waveDir.x * waveDir.y * cos(f)
    );
    vec3 T = vec3(
        -steepness * waveDir.x * waveDir.y * cos(f),
        waveDir.y * sin(f),
        1 - steepness * waveDir.y * waveDir.y * cos(f)
    );

    return normalize(cross(normalize(T), normalize(B)));
}
//************************************************************************************************************

void main() {
    vec3 norm = normalSCO;

    if (isSea && norm.y > 0.0) {
        vec3 waveNorm1 = WaveNormal(vec2(1.2, 0.4), 2.0, vertexSCA, time);
        vec3 waveNorm2 = WaveNormal(vec2(0.5, 0.5), 3.0, vertexSCA, time);
        vec3 waveNorm3 = WaveNormal(vec2(0.7, 0.2), 1.5, vertexSCA, time);
        waveNorm1 = normalize(mat3(view) * waveNorm1);
        waveNorm2 = normalize(mat3(view) * waveNorm2);
        waveNorm3 = normalize(mat3(view) * waveNorm3);
        norm = normalize(waveNorm1 + waveNorm2 + waveNorm3);
    }

    vec3 ambientLight = Ambient(llumAmbient);


    vec3 difusLight = Difus(normalize(norm), normalize(LSCO), solColor);
    vec3 especularLight = Especular(normalize(norm), normalize(LSCO), vertexSCO, solColor);


    vec3 difusLightF1 = Difus(normalize(norm), normalize(F1PosLSCO), FColor);
    vec3 especularLightF1 = Especular(normalize(norm), normalize(F1PosLSCO), vertexSCO, FColor);
    vec3 DirecF1 = normalize(DirF1);
    float angleCosine1 = max(dot(normalize(-DirecF1), normalize(F1PosLSCO)), 0.0);
    float At1 = pow(angleCosine1, 4.0);


    vec3 difusLightF2 = Difus(normalize(norm), normalize(F2PosLSCO), FColor);
    vec3 especularLightF2 = Especular(normalize(norm), normalize(F2PosLSCO), vertexSCO, FColor);
    vec3 DirecF2 = normalize(DirF2);
    float angleCosine2 = max(dot(normalize(-DirecF2), normalize(F2PosLSCO)), 0.0);
    float At2 = pow(angleCosine2, 4.0);

    vec3 modelPhong = ambientLight + (difusLight + especularLight) +
                      (At1 * (difusLightF1 + especularLightF1)) +
                      (At2 * (difusLightF2 + especularLightF2));


    for (int i = 0; i < 6; ++i) {
        vec3 torchPos = posFocusTorxes[i];
        vec3 L = normalize(torchPos - vertexSCA);
        float distance = length(torchPos - vertexSCA);

        vec3 currentTorchColor = torchColors[i];

        if (distance < 3.0) {
            modelPhong += Difus(normalize(norm), L, currentTorchColor) + Especular(normalize(norm), L, vertexSCO, currentTorchColor);
        } else {
            float At = exp(-(distance - 3.0));
            modelPhong += At * (Difus(normalize(norm), L, currentTorchColor) + Especular(normalize(norm), L, vertexSCO, currentTorchColor));
        }
    }

    FragColor = vec4(modelPhong, 1.0);
}
