#version 330

// Uniform variables
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

// Attributes
in vec4 vertex;     // Vertex coordinates in model space
in vec4 color;      // Vertex color
in vec4 normal;     // Vertex normal in model space
in vec2 texCoord0;

// Varying variables
out vec4 ic;
out vec4 l1;  // Light vector for first light
out vec4 l2;  // Light vector for second light
out vec4 n;
out vec4 v;
out vec2 iTexCoord0;

void main(void) {
    vec4 lp1 = vec4(0, 6, 6, 1);   // Light position 1 (world space)
    vec4 lp2 = vec4(0, -10, -5, 1);   // Light position 2 (world space)

    vec4 eyeVertex = V * M * vertex;
    
    l1 = normalize(V * lp1 - eyeVertex);  // Light vector 1 in eye space
    l2 = normalize(V * lp2 - eyeVertex);  // Light vector 2 in eye space
    v = normalize(vec4(0, 0, 0, 1) - eyeVertex); // View vector
    n = normalize(V * M * normal);        // Normal in eye space

    ic = color;
    iTexCoord0 = texCoord0;

    gl_Position = P * eyeVertex;
}