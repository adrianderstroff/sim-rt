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
#include "util/path.h"
#include "util/string.h"

namespace rt {
	struct SceneData {
		std::shared_ptr<ITracer> tracer;
		std::shared_ptr<ICamera> camera;
		std::map<std::string, std::shared_ptr<IMaterial>> materials;
		std::map<std::string, std::shared_ptr<IHitable>> objects;
		std::shared_ptr<IOrganization> organization;
		std::shared_ptr<IHitable> world;
		bool success;
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
		MATERIAL_ISOTROPIC,
		MATERIAL_BRDF
	};
	enum MaterialAttribute {
		MATERIAL_TYPE,
		MATERIAL_NAME,
		MATERIAL_COLOR,
		MATERIAL_CUBEMAP,
		MATERIAL_TEX_PATH,
		MATERIAL_TEX_INTERPOLATION,
		MATERIAL_TEX_WRAP,
		MATERIAL_REFRACTION_COEFF,
		MATERIAL_METALNESS,
		MATERIAL_ROUGHNESS,
		MATERIAL_DIFFUSE_COEFF
	};
	enum ObjectType {
		OBJECT_CUBE,
		OBJECT_CYLINDER,
		OBJECT_MESH,
		OBJECT_RECTANGLE,
		OBJECT_SPHERE
	};
	enum ObjectAttribute {
		OBJECT_TYPE,
		OBJECT_NAME,
		OBJECT_POS,
		OBJECT_POS2,
		OBJECT_MATERIAL,
		OBJECT_XAXIS,
		OBJECT_YAXIS,
		OBJECT_RADIUS,
		OBJECT_WIDTH,
		OBJECT_HEIGHT,
		OBJECT_DEPTH,
		OBJECT_INVERT,
		OBJECT_MESH_PATH,
		OBJECT_MESH_NORMALIZE,
		OBJECT_MESH_SMOOTH
	};
	enum SceneType {
		SCENE_BVH,
		SCENE_LIST
	};
	enum SceneAttribute {
		SCENE_TYPE
	};
	enum ElementAttribute {
		ELEMENT_OBJECT,
		ELEMENT_TRANSFORM
	};
	enum TransformAttribute {
		TRANSFORM_TRANSLATE,
		TRANSFORM_ROTATE
	};

	struct SceneElement {
		std::string name;
	};

	struct SceneObj {
		std::string type;
		std::map<std::string, SceneElement> elements;
	};

	std::shared_ptr<SceneData> read_scene(std::string scenepath);
	
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
	template <typename T, typename S>
	inline void vector_fill(std::vector<std::pair<T,S>> & v, const peg::SemanticValues& sv) {
		for (size_t i = 0; i < sv.size(); ++i) {
			auto val = sv[i].get<std::pair<T, S>>();
			v.push_back(val);
		}
	}
}

#endif//SCENE_IO_H