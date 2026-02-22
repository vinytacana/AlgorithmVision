#version 330 core
out vec4 FragColor;

uniform int state; 

void main() {
    if (state == 1)      FragColor = vec4(1.0, 0.6, 0.0, 1.0); // Laranja Vibrante (Comp)
    else if (state == 2) FragColor = vec4(0.9, 0.0, 0.0, 1.0); // Vermelho Escuro (Write)
    else if (state == 3) FragColor = vec4(0.5, 0.0, 0.8, 1.0); // Roxo Profundo (Pivot)
    else if (state == 4) FragColor = vec4(0.0, 0.4, 0.8, 1.0); // Azul Elétrico (Sub-array)
    else if (state == 5) FragColor = vec4(0.0, 0.7, 0.2, 1.0); // Verde Esmeralda (Done)
    else                 FragColor = vec4(0.15, 0.2, 0.35, 1.0); // Azul Royal Profundo (Normal)
}
