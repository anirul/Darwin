#version 330 core

in vec2 vert_texcoord;

layout(location = 0) out vec4 frag_color;

uniform float time_s;
uniform vec2 resolution;
uniform vec3 camera_pos;
uniform vec3 camera_target;
uniform vec3 camera_up;
uniform vec3 light_dir;
uniform int sphere_size;
uniform vec4 sphere_pos[384];
uniform vec4 sphere_col[384];

// Ray marching algorithm limits.
const int max_steps = 200;
const float min_dist = 0.01;
const float max_dist = 100.;

// light constants.
const float ambiant_treshold = 0.15;
const float material_shininess = 32.0;
const vec3 spec_col = vec3(1.0);

// Hit structure.
struct Hit {
	vec3 normal;
	float dist;
	vec4 color;
};

struct RayCamera {
	vec3 position;
	vec3 forward;
	vec3 right;
	vec3 up;
};

// Classic Perlin noise, 3D version
vec4 permute(vec4 x) {
	return mod(((x*34.0)+1.0)*x, 289.0);
}

vec4 taylorInvSqrt(vec4 r) {
	return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v) { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

  // First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

  // Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //  x0 = x0 - 0. + 0.0 * C 
  vec3 x1 = x0 - i1 + 1.0 * C.xxx;
  vec3 x2 = x0 - i2 + 2.0 * C.xxx;
  vec3 x3 = x0 - 1. + 3.0 * C.xxx;

  // Permutations
  i = mod(i, 289.0 ); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

  // Gradients
  // ( N*N points uniformly over a square, mapped onto an octahedron.)
  float n_ = 1.0/7.0; // N=7
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z *ns.z);  //  mod(p,N*N)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

  //Normalise gradients
  vec4 norm = 
	taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

  // Mix final noise value
  vec4 m = 
	max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2, x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
}

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
	if (sphere_col[smallest_id].w > 1.0) {
		hit.color = 
			vec4(vec3(snoise(hit.normal)), 1.0) * 
			vec4(vec3(sphere_col[smallest_id]), 1.0);
	} else {
		hit.color = sphere_col[smallest_id];
	}
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

vec3 DirectionalLightDirection() {
	float time_slow = time_s * 0.1;
	vec3 normal_light_dir = normalize(light_dir);
	return normal_light_dir;
    return normalize(vec3(
		0.0, 
		cos(time_slow), 
		sin(time_slow)));
}

// Get the light normal and the light value for a directional light.
vec4 LightNormalValue(vec3 position, vec3 normal)
{
    vec3 light_direction = DirectionalLightDirection();
    float light_value = dot(normal, light_direction);
    return vec4(light_direction, light_value);
}

// Function to compute the specular component.
float SpecularLight(vec3 light_dir, vec3 view_dir, vec3 normal, float shininess) {
	// Reflect the light around the normal.
    vec3 reflect_dir = reflect(-light_dir, normal); 
    float spec_angle = max(dot(reflect_dir, view_dir), 0.0);
	// shininess controls the specular highlight size.
    return pow(spec_angle, shininess); 
}

// Calculate the Shadow and light for a directional light.
float LightAndShadow(vec3 position, vec3 normal)
{
    vec3 light_direction = DirectionalLightDirection();
    vec4 light_normal_value = LightNormalValue(position, normal);
    // Based on the constant light direction.
    float dist_light = 
        RayMarching(
			position + normal * min_dist * 2, light_normal_value.xyz).dist;
    // Shadow attenuation can be adjusted as needed.
	if (dist_light < max_dist) {
        return ambiant_treshold; // Dimming factor for shadowed areas.
	}
	return clamp(light_normal_value.w, ambiant_treshold, 1.0);
}

RayCamera GetRayCamera()
{
    RayCamera camera;
    camera.position = camera_pos;
    camera.forward = normalize(camera_target - camera_pos);
	camera.right = normalize(cross(camera_up, camera.forward));
    camera.up = normalize(cross(camera.forward, camera.right));
    return camera;
}

void main()
{
	// Camera aperture computation.
	vec2 uv = vert_texcoord - vec2(0.5);
	uv.x *= resolution.x / resolution.y;

	// Camera computation.
	RayCamera camera = GetRayCamera();
	vec3 ray_direction = 
		normalize(uv.x * camera.right + uv.y * camera.up + camera.forward);

	// Ray marching algorithm.
	Hit result = RayMarching(camera.position, ray_direction);
	vec3 position = camera.position + ray_direction * result.dist;
	float spec = 
		SpecularLight(
			DirectionalLightDirection(), 
			-ray_direction, 
			result.normal, 
			material_shininess);

	// Light and shadow computation.
	float light_shadow = LightAndShadow(position, result.normal);
	vec3 diffuse_col = vec3(light_shadow * result.color);
	vec3 specular_col = vec3(0.0);
	if ((light_shadow > ambiant_treshold) && 
		(diffuse_col != vec3(0, 0, 0))) 
	{
		specular_col = vec3(spec * spec_col);
    }

	// Final color.
	frag_color = vec4(diffuse_col + specular_col, 1.0);
}
