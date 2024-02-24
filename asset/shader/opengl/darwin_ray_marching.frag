#version 330 core

in vec2 vert_texcoord;

layout(location = 0) out vec4 frag_color;

uniform float time_s;
uniform vec2 resolution;
uniform vec3 camera_pos			= vec3(0.0, 1.0, 0.0);
uniform vec3 camera_target      = vec3(0.0, 1.0, 6.0);
uniform vec3 camera_up			= vec3(0.0, 1.0, 0.0);
uniform int sphere_size;
uniform vec4 sphere_pos[384];
uniform vec4 sphere_col[384];

// Ray marching algorithm limits.
const int max_steps = 200;
const float min_dist = 0.01;
const float max_dist = 100.;

// Hit structure.
struct Hit {
	vec3 normal;
	float dist;
	vec4 color;
};

// Get a Hit from a surface.
Hit GetDistance(vec3 position)
{
	int smallest_id = -1;
	float smallest_dist = max_dist;
    for (int i = 0; i < sphere_size; ++i) {
		float dist_sphere = 
			length(position - sphere_pos[i].xyz) - sphere_pos[i].w;
		if (dist_sphere < smallest_dist) {
			smallest_id = i;
			smallest_dist = dist_sphere;
		}
	}
	if (smallest_id == -1) {
		Hit hit;
		hit.normal = vec3(0);
		hit.dist = max_dist;
		hit.color = vec4(0);
		return hit;
    }
	Hit hit;
	hit.normal = normalize(position - sphere_pos[smallest_id].xyz);
	hit.dist = smallest_dist;
	hit.color = sphere_col[smallest_id];
	return hit;
}

// Get the new distance and the normal to the surface.
// (if the distance is < min_dist in w).
Hit RayMarching(vec3 ray_origin, vec3 ray_direction)
{
	Hit result;
	float dist0 = 0;
	for (int i = 0; i < max_steps; ++i)
	{
		vec3 p = ray_origin + ray_direction * dist0;
		Hit hit = GetDistance(p);
		vec3 normal = hit.normal;
		dist0 += hit.dist;
		if (hit.dist < min_dist) {
			result.normal = normal;
			result.dist = dist0;
			result.color = hit.color;
			return result;
		}
		if (dist0 > max_dist) {
			break;
		}
	}
	result.normal = vec3(0, 1, 0);
	result.dist = dist0;
	result.color = vec4(0);
	return result;
}

// Get the light position at time.
vec3 LightPosition()
{
	vec3 light_position = vec3(0, 5, 6);
	light_position.xz += vec2(sin(time_s), cos(time_s) * 2);
	return light_position;
}

// Get the light normal and the light value.
vec4 LightNormalValue(vec3 position, vec3 normal)
{
	vec3 light_position = LightPosition();
	vec3 light_normal = normalize(light_position - position);
	float light_value = dot(normal, light_normal);
	return vec4(light_normal.xyz, light_value);
}

// Get light without shadow.
float LightOnly(vec3 position, vec3 normal)
{
	return LightNormalValue(position, normal).w;
}

// Calculate the Shadow and light.
float LightAndShadow(vec3 position, vec3 normal)
{
	vec3 light_position = LightPosition();
	vec4 light_normal_value = LightNormalValue(position, normal);
	float dist_light = 
		RayMarching(position + normal * min_dist * 2, light_normal_value.xyz).dist;
	if (dist_light < length(light_position - position)) 
		light_normal_value.w *= 0.1;
	return light_normal_value.w;
}

void main()
{
	// Camera computation.
	vec2 uv = vert_texcoord - vec2(0.5);
	uv.x *= resolution.x / resolution.y;

	vec3 ray_origin = camera_pos;
	vec3 forward = normalize(camera_target - camera_pos);
	vec3 camera_right = normalize(cross(camera_up, forward));
	// Recalculated to ensure orthogonality.
	vec3 ray_up = normalize(cross(forward, camera_right)); 
	vec3 ray_direction = normalize(uv.x * camera_right + uv.y * ray_up + forward);

	Hit result = RayMarching(ray_origin, ray_direction);
	vec3 position = ray_origin + ray_direction * result.dist;
	float light = LightOnly(position, result.normal);
	float light_shadow = LightAndShadow(position, result.normal);

	vec3 color = vec3(light_shadow * result.color);
	frag_color = vec4(color, 1.0);
}
