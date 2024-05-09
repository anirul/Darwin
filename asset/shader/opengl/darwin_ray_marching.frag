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
const float min_dist = 0.001;
const float max_dist = 100.0;

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

vec3 planetColor(vec3 pos_in) {
  vec3 pos = pos_in + vec3(1500., 1000., 510.);
  return vec3(mod(pos.x/20., 1.),
              mod(pos.y/30., 1.),
              sin(pos.z/20.));
}

uniform float atmosphere_scale = 20.0; // Scale of the atmosphere

// Simplified Rayleigh and Mie coefficients
const vec3 rayleigh_coefficient = vec3(0.005, 0.013, 0.033);
const float mie_coefficient = 0.0035;
const float rayleigh_scale_height = 8.4;
const float mie_scale_height = 1.25;

vec3 calculateAtmosphereEffect(vec3 direction, vec3 light_direction) {
    float cos_angle = dot(direction, light_direction);
    // Rayleigh phase function
    float rayleigh_phase = 0.75 * (1.0 + cos_angle * cos_angle);
    // Approximate Mie phase function (Henyey-Greenstein)
    float g = 0.26; // Asymmetry parameter for the Mie scattering
    float mie_phase = 
		1.5 * ((1.0 - g * g) / (2.0 + g * g)) * (1.0 + cos_angle * cos_angle) / 
		pow(1.0 + g * g - 2.0 * g * cos_angle, 1.5);
    
    // Combine effects with atmospheric scale
    vec3 rayleigh_scattering = rayleigh_coefficient * rayleigh_phase;
    float mie_scattering = mie_coefficient * mie_phase;
    return rayleigh_scattering + vec3(mie_scattering);
}

// Get the new distance and the normal to the surface.
// (if the distance is < min_dist in w).
Hit RayTracing(vec3 ray_origin, vec3 u)
{
  int min_i = -1;
  float min_d = max_dist;
  for (int i = 0; i < sphere_size; ++i) {
    vec3 oc = ray_origin - sphere_pos[i].xyz;
    float uoc = dot(u, oc);
    float delta = uoc * uoc - (length(oc) * length(oc) - sphere_pos[i].w * sphere_pos[i].w);
    if (delta >= 0) {
      float d1 = -uoc - sqrt(delta);
      if (d1 > 0 && d1 < min_d) {
        min_d = d1;
        min_i = i;
      }
      float d2 = -uoc + sqrt(delta);
      if (d2 > 0 && d2 < min_d) {
        min_d = d2;
        min_i = i;
      }
    }
  }
  if (min_i == -1) {
    Hit result;
    result.normal = vec3(0, 1, 0);
    result.dist = max_dist+1;
    result.color = vec4(0);
    return result;
  }
  vec3 position = ray_origin + min_d * u;
  Hit hit;
  hit.dist = min_d;
	hit.normal = normalize(position - sphere_pos[min_i].xyz);
	if (sphere_pos[min_i].w > 20.0) {
		// Planet texture.
        hit.color = 
			vec4(
                planetColor(
                    position * 
                    snoise(position * (0.2 + sin(time_s/1000.) * 0.1))), 1.0) * 
                vec4(0.5) + vec4(0.5);
    } else if (sphere_col[min_i].w > 1.0) {
		// Character texture.
		hit.color = 
			vec4(vec3(snoise(hit.normal + position * 0.2)), 1.0) * 
			vec4(vec3(sphere_col[min_i]), 1.0);
	} else {
		hit.color = sphere_col[min_i];
	}
  return hit;
}

// Get the light normal and the light value for a directional light.
vec4 LightNormalValue(vec3 position, vec3 normal)
{
    vec3 light_direction = normalize(light_dir);
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
    vec3 to_light = normalize(light_dir);
    // Based on the constant light direction.
    float dist_light = 
        RayTracing(
			position + normal * min_dist * 2, to_light).dist;
    // Shadow attenuation can be adjusted as needed.
	if (dist_light < max_dist) {
        return ambiant_treshold; // Dimming factor for shadowed areas.
	}
	return clamp(dot(normal, to_light), ambiant_treshold, 1.0);
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
	Hit result = RayTracing(camera.position, ray_direction);
	vec3 position = camera.position + ray_direction * result.dist;
	float spec = 
		SpecularLight(
			normalize(light_dir), 
			-ray_direction, 
			result.normal, 
			material_shininess);

    if (result.dist >= max_dist) {
		// Assuming fragPos is available
		vec3 atmosphere_color = 
			calculateAtmosphereEffect(ray_direction, light_dir);
        frag_color = vec4(atmosphere_color * 32.0 , 1.0);
    } else {
		// Light and shadow computation.
		float light_shadow = LightAndShadow(position, result.normal);
    vec3 color_light_shadow = vec3(light_shadow);

    // Light up food and player.
    for (int i = 0; i < sphere_size; ++i) {
      if (sphere_pos[i].w <= 10) {
        float dist = length(position - vec3(sphere_pos[i]));
        color_light_shadow += max(5-dist, 0.)/6. * vec3(sphere_col[i]);
      }
    }

		vec3 diffuse_col = color_light_shadow * vec3(result.color);
		vec3 specular_col = vec3(0.0);
		if ((light_shadow > ambiant_treshold) && 
			(diffuse_col != vec3(0, 0, 0))) 
		{
			specular_col = vec3(spec * spec_col);
		}
		// Final color.
		frag_color = vec4(diffuse_col + specular_col, 1.0);
    }
}
