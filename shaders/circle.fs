#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform int state; 

void main() {
    float dist = length(TexCoords - vec2(0.5, 0.5));
    float alpha = smoothstep(0.5, 0.49, dist);
    if (dist > 0.5) discard;
    
    vec3 color;
    if (state == 1)      color = vec3(1.0, 0.85, 0.0); // Amarelo Neon (Fila)
    else if (state == 2) color = vec3(0.0, 0.5, 1.0);  // Azul Royal (Visitado)
    else if (state == 3) color = vec3(1.0, 0.0, 0.5);  // Rosa Choque (Caminho Dijkstra)
    else if (state == 4) color = vec3(1.0, 0.4, 0.0);  // Laranja Elétrico (Start/End)
    else                 color = vec3(1.0, 1.0, 1.0);  // Branco Puro (Normal)

    // Contorno dinâmico: caminho rosa tem contorno rosa escuro, outros têm contorno preto
    if (dist > 0.44) {
        if (state == 3) FragColor = vec4(0.6, 0.0, 0.3, alpha);
        else            FragColor = vec4(0.0, 0.0, 0.0, alpha);
    } else {
        FragColor = vec4(color, alpha);
    }
}
