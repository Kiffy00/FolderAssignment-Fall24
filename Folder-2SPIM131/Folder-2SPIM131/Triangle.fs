#version 330 core
out vec4 FragColor;

in vec3 ourColor;
uniform vec3 u_color;

void main() {
    // Combine uniform color and vertex color:
    vec3 finalColor = ourColor * u_color;

    FragColor = vec4(finalColor, 1.0);
}
