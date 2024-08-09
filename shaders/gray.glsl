@module gray

@ctype mat4 CF_Matrix4x4
@ctype vec4 CF_Color
@ctype vec2 CF_V2

@block shader_block
uniform sampler2D tex;

vec4 shader(vec4 color, vec2 pos, vec2 atlas_uv, vec2 screen_uv, vec4 params)
{
	float alpha = texture(tex, screen_uv).a;
    if (alpha == 0.0) {
        discard;
    }
	return vec4(0.1, 0.1, 0.1, alpha*0.5);
}
@end

@include ../build_msvc_2022/_deps/cute-src/include/shaders/draw.glsl
