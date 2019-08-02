#ifndef SCENE_IO_H
#define SCENE_IO_H

#include <memory>
#include <string>
#include <fstream>
#include <streambuf>

#include "peglib.h"

#include "hitable/hitable.h"
#include "material/material.h"
#include "math/vec3.h"
#include "texture/texture.h"
#include "tracer/tracer.h"
#include "util/string.h"

namespace rt {
	struct SceneData {
		std::shared_ptr<ITracer> tracer;
		std::shared_ptr<ICamera> camera;
		std::map<std::string, std::shared_ptr<IMaterial>> materials;
		std::map<std::string, std::shared_ptr<IHitable>> objects;
		std::shared_ptr<IHitable> world;
	};

	struct TracerObj { 
		std::string type; 
		int width, height, samples, depth; 
	};
	enum TracerType {
		RAYCASTER,
		RAYTRACER,
		DEBUGTRACER
	};
	enum TracerAttribute {
		TRACER_TYPE,
		TRACER_RESOLUTION,
		TRACER_SAMPLES,
		TRACER_DEPTH
	};
	enum CameraType {
		SIMPLE_CAMERA,
		DOF_CAMERA
	};
	enum CameraAttribute {
		CAMERA_TYPE,
		CAMERA_POS,
		CAMERA_LOOKAT,
		CAMERA_UP,
		CAMERA_FOV,
		CAMERA_APERTURE
	};
	enum MaterialType {
		MATERIAL_NORMAL,
		MATERIAL_LAMBERTIAN,
		MATERIAL_METAL,
		MATERIAL_DIELECTRIC,
		MATERIAL_DIFFUSE_LIGHT,
		MATERIAL_ISOTROPIC
	};
	enum MaterialAttribute {
		MATERIAL_TYPE,
		MATERIAL_NAME,
		MATERIAL_COLOR,
		MATERIAL_TEX_PATH,
		MATERIAL_TEX_INTERPOLATION,
		MATERIAL_TEX_WRAP,
		MATERIAL_REFRACTION_COEFF
	};

	struct CameraObj { 
		std::string type; 
		vec3 pos, lookat, up; 
		double fov;
		std::map<std::string, std::string> options;
	};

	struct MaterialObj {
		std::string type;
		std::string name;
		vec3        color;
		std::map<std::string, std::string> options;
	};

	struct ObjectObj {
		std::string type;
		std::string name;
		vec3        pos;
		std::string material;
		std::map<std::string, std::string> options;
	};

	struct SceneElement {
		std::string name;
	};

	struct SceneObj {
		std::string type;
		std::map<std::string, SceneElement> elements;
	};

	std::shared_ptr<SceneData> read_scene(std::string scenepath);

	void create_materials(std::shared_ptr<SceneData>& scene, std::map<std::string, MaterialObj>& materialmap);
	void create_objects(std::shared_ptr<SceneData>& scene, std::map<std::string, ObjectObj>& objectmap);
	void create_scene(std::shared_ptr<SceneData>& scene, std::string scenetype, std::map<std::string, SceneElement>& elementmap);

	bool has_option(std::map<std::string, std::string>& options, std::string findoption);
	
	template <typename T, typename S>
	inline bool map_contains(std::map<T, S>& m, T elem) {
		return m.find(elem) != m.end();
	}
	template <typename T, typename S, typename R>
	inline R map_get(std::map<T, S>& m, T el, R default) {
		if (map_contains(m, el)) {
			return m.at(el).get<R>();
		}
		return default;
	}
	template <typename T, typename S>
	inline void map_fill(std::map<T, S>& m, const peg::SemanticValues& sv) {
		for (size_t i = 0; i < sv.size(); ++i) {
			auto& val = sv[i].get<std::pair<T, S>>();
			m.insert(val);
		}
	}

	std::shared_ptr<ITexture> create_texture(MaterialObj& materialobj);
	std::shared_ptr<IMaterial> grab_material(std::shared_ptr<SceneData>& scene, std::string materialid);
	vec3 parse_vector(std::string text);
}

#endif//SCENE_IO_H