#version 330 core
out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;

uniform vec3 lightPos; // position of the light (central body)
uniform vec3 lightColor; // color of the light (ambient light color)
uniform vec3 viewPos; // camera position

void main() {
    // Ambient lighting
    float ambientStrength = 300.0; // You can adjust this value
    vec3 ambient = ambientStrength * lightColor;

    FragColor = vec4(ambient, 1.0); // Applying ambient lighting to the fragment color
}
