#version 120

// see the GLSL 1.2 specification:
// https://www.khronos.org/registry/OpenGL/specs/gl/GLSLangSpec.1.20.pdf

uniform bool is_reflection; // variable of the program
varying vec3 normal; // normal vector pass to the rasterizer and fragment shader

void main()
{
    normal = vec3(gl_Normal);// set normal and pass it to fragment shader

    // "gl_Vertex" is the *input* vertex coordinate of triangle.
    // "gl_Vertex" has type of "vec4", which is homogeneious coordinate
    float x0 = gl_Vertex.x/gl_Vertex.w;// x-coord
    float y0 = gl_Vertex.y/gl_Vertex.w;// y-coord
    float z0 = gl_Vertex.z/gl_Vertex.w;// z-coord
    if (is_reflection) {
        vec3 nrm = normalize(vec3(0.4, 0.0, 1.0)); // normal of the mirror
        vec3 org = vec3(-0.3, 0.0, -0.5); // point on the mirror
        // wite code to change the input position (x0,y0,z0).
        // the transformed position (x0, y0, z0) should be drawn as the mirror reflection.
        //
        // make sure the occlusion is correctly computed.
        // the mirror is behind the armadillo, so the reflected image should be behind the armadillo.
        // furthermore, make sure the occlusion is correctly computed for the reflected image.
        //****implementation start****
        // Calculate 3 items: 
        // 1. vector: vertex <-> mirror point
        // 2. distance: vertex <-> mirror plane
        // 3. reflected vertex location
        vec3 vertex_mirror = vec3(x0, y0, z0) - org;
        float distance_plane = dot(vertex_mirror, nrm);
        vec3 reflected_v = vec3(x0, y0, z0) - 2.0 * distance_plane * nrm;
        
        // vertex location update
        x0 = reflected_v.x;
        y0 = reflected_v.y;
        z0 = reflected_v.z;
        
        // adjust depth
        float mirror_depth = dot(org, nrm);
        float vertex_depth = dot(vec3(x0, y0, z0), nrm);
        float depth_difference = vertex_depth - mirror_depth;
        
        // offset for occlusion
        z0 -= depth_difference;
    }
    // do not edit below

    // "gl_Position" is the *output* vertex coordinate in the
    // "canonical view volume (i.e.. [-1,+1]^3)" pass to the rasterizer.
    gl_Position = vec4(x0, y0, -z0, 1);// opengl actually draw a pixel with *maximum* depth. so invert z
}
