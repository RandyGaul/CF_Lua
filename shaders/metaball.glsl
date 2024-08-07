@module metaball

@ctype mat4 CF_Matrix4x4
@ctype vec4 CF_Color
@ctype vec2 CF_V2

@block shader_block
uniform sampler2D tex;

vec4 shader(vec4 color, vec2 pos, vec2 atlas_uv, vec2 screen_uv, vec4 params)
{
	float d = distance(screen_uv, vec2(1));
	return vec4(d,d,d, 1);
}
@end

@include include/draw.glsl
