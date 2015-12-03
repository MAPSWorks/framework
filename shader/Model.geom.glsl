#version 400

layout( triangles ) in;
layout (triangle_strip, max_vertices = 3) out;

// In
in vec3 Normal[];
in vec3 Position[];
in vec2 TexCoords[];
in vec4 ShadowCoord[];

// Out
out vec3 GPosition;
out vec3 GNormal;
out vec2 GTexCoords;
out vec4 GShadowCoord;
out vec3 GEdgeDistance;

// Uniform
uniform mat4 matViewport;

void main(){
    // Transform each vertex into viewport space
    vec3 p0 = vec3(matViewport * (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec3 p1 = vec3(matViewport * (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
    vec3 p2 = vec3(matViewport * (gl_in[2].gl_Position / gl_in[2].gl_Position.w));

    // Find the altitudes (ha, hb and hc)
    float a = length(p1 - p2);
    float b = length(p2 - p0);
    float c = length(p1 - p0);
    float alpha = acos( (b*b + c*c - a*a) / (2.0*b*c) );
    float beta  = acos( (a*a + c*c - b*b) / (2.0*a*c) );
    float ha = abs( c * sin(beta) );
    float hb = abs( c * sin(alpha) );
    float hc = abs( b * sin(alpha) );

    // Send the triangle along with the edge distances
    GEdgeDistance = vec3(ha, 0, 0);
    GNormal       = Normal[0];
    GPosition     = Position[0];
    gl_Position   = gl_in[0].gl_Position;
    GTexCoords    = TexCoords[0];
    GShadowCoord  = ShadowCoord[0];
    EmitVertex();

    GEdgeDistance = vec3(0, hb, 0);
    GNormal       = Normal[1];
    GPosition     = Position[1];
    gl_Position   = gl_in[1].gl_Position;
    GTexCoords    = TexCoords[1];
    GShadowCoord  = ShadowCoord[1];
    EmitVertex();

    GEdgeDistance = vec3(0, 0, hc);
    GNormal       = Normal[2];
    GPosition     = Position[2];
    gl_Position   = gl_in[2].gl_Position;
    GTexCoords    = TexCoords[2];
    GShadowCoord  = ShadowCoord[2];
    EmitVertex();

    EndPrimitive();
}
