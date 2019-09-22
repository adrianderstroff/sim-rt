#include "sceneio.h"

std::shared_ptr<rt::SceneData> rt::read_scene(std::string scenepath) {
	// file to string
	std::ifstream t(scenepath); std::string text;
	t.seekg(0, std::ios::end); text.reserve(t.tellg()); t.seekg(0, std::ios::beg);
	text.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	// define grammar
	auto grammar = R"(
        Start       <- (_ Statement _)*
		Statement   <- Tracer / Camera / Materials / Objects / Scene

		# tracer statements
		Tracer        <- 'TRACER' (_ TracerAttrib)*
		TracerAttrib  <- TracerType / TracerRes / TracerSamples / TracerDepth
		TracerType    <- 'TYPE' _ Word
		TracerRes     <- 'RESOLUTION' _ Number _ Number
		TracerSamples <- 'SAMPLES' _ Number
		TracerDepth   <- 'DEPTH' _ Number

		# camera statements
		Camera         <- 'CAMERA' (_ CameraAttrib)* 
		CameraAttrib   <- CameraType / CameraPos / CameraLookAt / CameraUp / CameraFOV
		CameraType     <- 'TYPE' _ Word
		CameraPos      <- 'POS' _ Vector
		CameraLookAt   <- 'LOOKAT' _ Vector
		CameraUp       <- 'UP' _ Vector
		CameraFOV	   <- 'FOV' _ Double
		CameraAperture <- 'APERTURE' _ Double

		# materials statement
		Materials            <- 'MATERIALS' (_ Material)*
		Material             <- 'MATERIAL' (_ MaterialAttrib)*
		MaterialAttrib       <- MaterialName / MaterialType / MaterialColor / MaterialCubeMap / MaterialTexPath / MaterialGlassCoeff / MaterialTexInterp / MaterialTexWrap / MaterialMetalness / MaterialRoughness / MaterialDiffuseCoeff
		MaterialName         <- 'NAME' _ Word
		MaterialType         <- 'TYPE' _ Word
		MaterialColor        <- 'COLOR' _ Vector
		MaterialCubeMap      <- 'CUBEMAP' (_ MaterialTexPath)*
		MaterialTexPath      <- 'PATH' _ Path
		MaterialTexInterp    <- 'ITPLT' _ Word
		MaterialTexWrap      <- 'WRAP' _ Word Word?
		MaterialGlassCoeff   <- 'COEFF' _ Double
		MaterialMetalness    <- 'METALNESS' _ Double
		MaterialRoughness    <- 'ROUGHNESS' _ Double
		MaterialDiffuseCoeff <- 'KD' _ Double

		# objects statement
		Objects             <- 'OBJECTS' (_ Object)*
		Object              <- 'OBJECT' (_ ObjectAttrib)*
		ObjectAttrib        <- ObjectName / ObjectType / ObjectPos / ObjectPos2 / ObjectMaterial / ObjectXAxis / ObjectYAxis / ObjectRadius / ObjectWidth / ObjectHeight / ObjectDepth / ObjectMeshPath / ObjectInvert / ObjectMeshNormalize / ObjectMeshSmooth
		ObjectName          <- 'NAME' _ Word
		ObjectType          <- 'TYPE' _ Word
		ObjectPos           <- 'POS' _ Vector
		ObjectPos2          <- 'POS2' _ Vector
		ObjectMaterial      <- 'MATERIAL' _ Word
		ObjectXAxis         <- 'XAXIS' _ Vector
		ObjectYAxis         <- 'YAXIS' _ Vector
		ObjectRadius        <- 'R' _ Double
		ObjectWidth         <- 'WIDTH' _ Double
		ObjectHeight        <- 'HEIGHT' _ Double
		ObjectDepth         <- 'DEPTH' _ Double
		ObjectMeshPath      <- 'PATH' _ Path
		ObjectInvert        <- 'INVERT' _ Bool
		ObjectMeshNormalize <- 'NORMALIZE' _ Bool
		ObjectMeshSmooth    <- 'SMOOTH' _ Bool

		# scene statement
		Scene              <- 'SCENE' (_ SceneAttrib)*
		SceneAttrib        <- SceneType / Elements
		SceneType          <- 'TYPE' _ Word
		Elements           <- 'ELEMENTS' (_ Element)*
		Element            <- 'ELEMENT' (_ ElementAttrib)*
		ElementAttrib      <- ElementObject / ElementTransform
		ElementObject      <- 'OBJECT' _ Word
		ElementTransform   <- 'TRANSFORM' (_ TransformAction)*
		# transform statement
		TransformAction    <- TransformTranslate / TransformRotate
		TransformTranslate <- 'TRANSLATE' _ Vector
		TransformRotate    <- 'ROTATE' _ Vector _ Double

		# general statements
		Word        <- [a-z][a-z0-9]*
		Path        <- [a-zA-Z0-9./]+
		Bool        <- 'true' / 'false'
		Number      <- [0-9]+
		Double      <- '-'? [0-9]+ ('.' [0-9]+)?
		Vector      <- '(' _ Double _ ',' _ Double _ ',' _ Double _ ')'
        Comment     <- '#' .+ EOL
		EOL         <- [\r\n]
		~_			<- [ \t\r\n]*
    )";



	// setup parser
	peg::parser parser;
	parser.log = [](size_t line, size_t col, const std::string& msg) {
		std::cerr << line << ":" << col << ": " << msg << "\n";
	};
	auto ok = parser.load_grammar(grammar);
	assert(ok);


	// setup scene
	std::shared_ptr<SceneData> scene = std::make_shared<SceneData>();


	/**
	 * define general parser rules
	 */
	parser["Word"] = [](const peg::SemanticValues& sv) {
		return sv.token();
	};
	parser["Path"] = [&](const peg::SemanticValues& sv) {
		std::string path = sv.token();
		path = fix_path(scenepath, path);
		return path;
	};
	parser["Number"] = [](const peg::SemanticValues& sv) {
		return stoi(sv.token());
	};
	parser["Double"] = [](const peg::SemanticValues& sv) {
		return stod(sv.token());
	};
	parser["Vector"] = [](const peg::SemanticValues& sv) {
		double x = sv[0].get<double>();
		double y = sv[1].get<double>();
		double z = sv[2].get<double>();
		return vec3(x, y, z);
	};
	parser["Bool"] = [](const peg::SemanticValues& sv) {
		std::string val = sv.token();

		bool res = false;
		if (val == "true") res = true;

		return res;
	};

	/**
	 * building the tracer object
	 */
	parser["Tracer"] = [&](const peg::SemanticValues& sv) {
		// collect attributes
		std::map<TracerAttribute, peg::any> attributemap;
		map_fill(attributemap, sv);

		// grab common tracer attributes
		TracerType type       = map_get(attributemap, TRACER_TYPE,       RAYCASTER          );
		auto& [width, height] = map_get(attributemap, TRACER_RESOLUTION, std::pair(640, 460));
		int samples           = map_get(attributemap, TRACER_SAMPLES,    100                );
		int depth             = map_get(attributemap, TRACER_DEPTH,      100                );

		// create the appropriate tracer
		switch (type) {
		case TracerType::RAYCASTER:
			scene->tracer = std::make_shared<Raycaster>(width, height, samples, depth);
			break;
		case TracerType::RAYTRACER:
			scene->tracer = std::make_shared<Raytracer>(width, height, samples, depth);
			break;
		case TracerType::DEBUGTRACER:
			scene->tracer = std::make_shared<Debugtracer>(width, height, samples, depth);
			break;
		default: // raycaster
			scene->tracer = std::make_shared<Raycaster>(width, height, samples, depth);
			break;
		}
	};
	parser["TracerType"] = [](const peg::SemanticValues& sv) {
		// grab value
		std::string val = sv[0].get<std::string>();
		
		// determine tracer type
		TracerType type = TracerType::RAYCASTER;
		if     (val == "raycaster"  ) type = TracerType::RAYCASTER;
		else if(val == "raytracer"  ) type = TracerType::RAYTRACER;
		else if(val == "debugtracer") type = TracerType::DEBUGTRACER;

		return std::pair(TRACER_TYPE, peg::any(type));
	};
	parser["TracerRes"] = [](const peg::SemanticValues& sv) {
		// grab value
		int width  = sv[0].get<int>();
		int height = sv[1].get<int>();
		std::pair resolution(width, height);

		return std::pair(TRACER_RESOLUTION, peg::any(resolution));
	};
	parser["TracerSamples"] = [](const peg::SemanticValues& sv) {
		// grab value
		int samples = sv[0].get<int>();

		return std::pair(TRACER_SAMPLES, peg::any(samples));
	};
	parser["TracerDepth"] = [](const peg::SemanticValues& sv) {
		// grab value
		int depth = sv[0].get<int>();

		return std::pair(TRACER_DEPTH, peg::any(depth));
	};

	/**
	 * building the camera object
	 */
	parser["Camera"] = [&](const peg::SemanticValues& sv) {
		// collect attributes
		std::map<CameraAttribute, peg::any> attributemap;
		map_fill(attributemap, sv);

		// grab common tracer attributes
		CameraType type = map_get(attributemap, CAMERA_TYPE,     SIMPLE_CAMERA);
		vec3 pos        = map_get(attributemap, CAMERA_POS,      vec3(0, 0, 1));
		vec3 lookat     = map_get(attributemap, CAMERA_LOOKAT,   vec3(0, 0, 0));
		vec3 up         = map_get(attributemap, CAMERA_UP,       vec3(0, 1, 0));
		double fov      = map_get(attributemap, CAMERA_FOV,      45.0         );
		double aperture = map_get(attributemap, CAMERA_APERTURE, 1.0          );
		double aspect   = scene->tracer->aspect();

		// create the appropriate tracer
		switch (type) {
		case CameraType::SIMPLE_CAMERA:
			scene->camera = std::make_shared<SimpleCamera>(pos, lookat, up, fov, aspect);
			break;
		case CameraType::DOF_CAMERA:
			scene->camera = std::make_shared<DOFCamera>(pos, lookat, up, fov, aspect, aperture);
			break;
		default: // simple camera
			scene->camera = std::make_shared<SimpleCamera>(pos, lookat, up, fov, aspect);
			break;
		}
	};
	parser["CameraType"] = [](const peg::SemanticValues& sv) {
		// grab value
		std::string val = sv[0].get<std::string>();

		// determine camera type
		CameraType type = SIMPLE_CAMERA;
		if      (val == "simple") type = SIMPLE_CAMERA;
		else if (val == "dof"   ) type = DOF_CAMERA;

		return std::make_pair(CAMERA_TYPE, peg::any(type));
	};
	parser["CameraPos"] = [](const peg::SemanticValues& sv) {
		// grab value
		vec3 pos = sv[0].get<vec3>();

		return std::make_pair(CAMERA_POS, peg::any(pos));
	};
	parser["CameraLookAt"] = [](const peg::SemanticValues& sv) {
		// grab value
		vec3 lookat = sv[0].get<vec3>();

		return std::make_pair(CAMERA_LOOKAT, peg::any(lookat));
	};
	parser["CameraUp"] = [](const peg::SemanticValues& sv) {
		// grab value
		vec3 up = sv[0].get<vec3>();

		return std::make_pair(CAMERA_UP, peg::any(up));
	};
	parser["CameraFOV"] = [](const peg::SemanticValues& sv) {
		// grab value
		double fov = sv[0].get<double>();

		return std::make_pair(CAMERA_POS, peg::any(fov));
	};
	parser["CameraAperture"] = [](const peg::SemanticValues& sv) {
		// grab value
		double aperture = sv[0].get<double>();

		return std::make_pair(CAMERA_APERTURE, peg::any(aperture));
	};

	/**
	 * building the material list
	 */
	parser["Material"] = [&](const peg::SemanticValues& sv) {
		// collect attributes
		std::map<MaterialAttribute, peg::any> attributemap;
		map_fill(attributemap, sv);

		// grab common tracer attributes
		MaterialType type = map_get(attributemap, MATERIAL_TYPE, MATERIAL_NORMAL);
		std::string name = map_get(attributemap, MATERIAL_NAME, "unnamed" + std::to_string(scene->materials.size()));
		vec3 color = map_get(attributemap, MATERIAL_COLOR, vec3(1));
		std::string texpath = map_get(attributemap, MATERIAL_TEX_PATH, std::string(""));
		auto cubemappaths = map_get(attributemap, MATERIAL_CUBEMAP, std::vector<std::pair<MaterialAttribute, peg::any>>());
		std::shared_ptr<ITexture> tex = std::make_shared<ConstantTexture>(color);
		if (!texpath.empty()) {
			auto& [image, status] = read_image(texpath);
			if (status) {
				std::shared_ptr<ImageTexture> imagetex = std::make_shared<ImageTexture>(image);
				ImageTexture::Interpolation interpolation = map_get(attributemap, MATERIAL_TEX_INTERPOLATION, ImageTexture::BILINEAR);
				auto& [wrapx, wrapy] = map_get(attributemap, MATERIAL_TEX_WRAP, std::pair(ImageTexture::CLAMP, ImageTexture::CLAMP));
				imagetex->set_interpolation_method(interpolation);
				imagetex->set_wrap_method(wrapx, wrapy);
				tex = imagetex;
			}
		}
		else if (cubemappaths.size() >= 6) {
			std::vector<Image> images;
			bool allstatus = true;
			for (size_t i = 0; i < 6; ++i) {
				std::string path = cubemappaths.at(i).second.get<std::string>();
				auto& [image, status] = read_image(path);
				allstatus = allstatus | status;
				if (status) images.push_back(image);
			}
			if(allstatus) tex = std::make_shared<CubeMap>(images.at(0), images.at(1), images.at(2), images.at(3), images.at(4), images.at(5));
		}

		// create the appropriate tracer
		switch(type) {
		case MaterialType::MATERIAL_NORMAL:
			scene->materials.insert(std::make_pair(name, std::make_shared<NormalMaterial>()));
			break;
		case MaterialType::MATERIAL_LAMBERTIAN:
			scene->materials.insert(std::make_pair(name, std::make_shared<Lambertian>(tex)));
			break;
		case MaterialType::MATERIAL_METAL:
			scene->materials.insert(std::make_pair(name, std::make_shared<Metal>(tex)));
			break;
		case MaterialType::MATERIAL_DIELECTRIC:
			{
				double coeff = map_get(attributemap, MATERIAL_REFRACTION_COEFF, 1.0);
				scene->materials.insert(std::make_pair(name, std::make_shared<Dielectric>(coeff, tex)));
			}
			break;
		case MaterialType::MATERIAL_DIFFUSE_LIGHT:
			scene->materials.insert(std::make_pair(name, std::make_shared<DiffuseLight>(tex)));
			break;
		case MaterialType::MATERIAL_ISOTROPIC:
			scene->materials.insert(std::make_pair(name, std::make_shared<Isotropic>(tex)));
			break;
		case MaterialType::MATERIAL_BRDF:
			{
				double metalness = map_get(attributemap, MATERIAL_METALNESS, 0.0);
				double roughness = map_get(attributemap, MATERIAL_ROUGHNESS, 0.0);
				double kd = map_get(attributemap, MATERIAL_DIFFUSE_COEFF, 0.5);
				scene->materials.insert(std::make_pair(name, std::make_shared<BRDF>(tex, metalness, kd, roughness)));
			}
			break;
		default: // normal material
			scene->materials.insert(std::make_pair(name, std::make_shared<NormalMaterial>()));
			break;
		}
	};
	parser["MaterialName"] = [](const peg::SemanticValues& sv) {
		// grab value
		std::string name = sv[0].get<std::string>();

		return std::make_pair(MATERIAL_NAME, peg::any(name));
	};
	parser["MaterialType"] = [](const peg::SemanticValues& sv) {
		std::string val = sv[0].get<std::string>();

		// determine camera type
		MaterialType type = MATERIAL_NORMAL;
		if      (val == "normal"      ) type = MATERIAL_NORMAL;
		else if (val == "lambertian"  ) type = MATERIAL_LAMBERTIAN;
		else if (val == "brdf"        ) type = MATERIAL_BRDF;
		else if (val == "metal"       ) type = MATERIAL_METAL;
		else if (val == "dielectric"  ) type = MATERIAL_DIELECTRIC;
		else if (val == "diffuselight") type = MATERIAL_DIFFUSE_LIGHT;
		else if (val == "isotropic"   ) type = MATERIAL_ISOTROPIC;

		return std::make_pair(MATERIAL_TYPE, peg::any(type));
	};
	parser["MaterialColor"] = [](const peg::SemanticValues& sv) {
		// grab value
		vec3 color = sv[0].get<vec3>();

		return std::make_pair(MATERIAL_COLOR, peg::any(color));
	};
	parser["MaterialCubeMap"] = [](const peg::SemanticValues& sv) {
		// grab all values in order
		std::vector<std::pair<MaterialAttribute, peg::any>> cubemappaths;
		vector_fill(cubemappaths, sv);

		return std::make_pair(MATERIAL_CUBEMAP, peg::any(cubemappaths));
	};
	parser["MaterialTexPath"] = [](const peg::SemanticValues& sv) {
		// grab value
		std::string path = sv[0].get<std::string>();
		
		return std::make_pair(MATERIAL_TEX_PATH, peg::any(path));
	};
	parser["MaterialTexInterp"] = [](const peg::SemanticValues& sv) {
		// grab value
		std::string val = sv[0].get<std::string>();

		// determine interpolation type
		ImageTexture::Interpolation interpolation = ImageTexture::BILINEAR;
		if      (val == "bilinear") interpolation = ImageTexture::BILINEAR;
		else if (val == "nearest" ) interpolation = ImageTexture::NEAREST_NEIGHBOR;

		return std::make_pair(MATERIAL_TEX_INTERPOLATION, peg::any(interpolation));
	};
	parser["MaterialTexWrap"] = [](const peg::SemanticValues& sv) {
		// grab value
		std::string wrapx = sv[0].get<std::string>();
		std::string wrapy = (sv.size() > 1) ? sv[1].get<std::string>() : wrapx;

		// determine wrap type
		ImageTexture::Wrap itpltx = ImageTexture::CLAMP;
		if      (wrapx == "clamp" ) itpltx = ImageTexture::CLAMP;
		else if (wrapx == "repeat") itpltx = ImageTexture::REPEAT;
		ImageTexture::Wrap itplty = ImageTexture::CLAMP;
		if      (wrapx == "clamp" ) itplty = ImageTexture::CLAMP;
		else if (wrapx == "repeat") itplty = ImageTexture::REPEAT;

		std::pair wrap(itpltx, itplty);
		return std::make_pair(MATERIAL_TEX_WRAP, peg::any(wrap));
	};
	parser["MaterialGlassCoeff"] = [](const peg::SemanticValues& sv) {
		// grab value
		double coeff = sv[0].get<double>();
		
		return std::make_pair(MATERIAL_REFRACTION_COEFF, peg::any(coeff));
	};
	parser["MaterialMetalness"] = [](const peg::SemanticValues& sv) {
		// grab value
		double metalness = sv[0].get<double>();

		return std::make_pair(MATERIAL_METALNESS, peg::any(metalness));
	};
	parser["MaterialRoughness"] = [](const peg::SemanticValues& sv) {
		// grab value
		double roughness = sv[0].get<double>();

		return std::make_pair(MATERIAL_ROUGHNESS, peg::any(roughness));
	};
	parser["MaterialDiffuseCoeff"] = [](const peg::SemanticValues& sv) {
		// grab value
		double kd = sv[0].get<double>();

		return std::make_pair(MATERIAL_DIFFUSE_COEFF, peg::any(kd));
	};

	/**
	 * building the object list
	 */
	parser["Object"] = [&](const peg::SemanticValues& sv) {
		// collect attributes
		std::map<ObjectAttribute, peg::any> attributemap;
		map_fill(attributemap, sv);

		// grab common object attributes
		ObjectType type = map_get(attributemap, OBJECT_TYPE, OBJECT_SPHERE);
		std::string name = map_get(attributemap, OBJECT_NAME, "unnamed" + std::to_string(scene->objects.size()));
		vec3 pos = map_get(attributemap, OBJECT_POS, vec3(0));
		std::string materialid = map_get(attributemap, OBJECT_MATERIAL, std::string(""));
		std::shared_ptr<IMaterial> material = std::make_shared<NormalMaterial>();
		if (map_contains(scene->materials, materialid)) {
			material = scene->materials.at(materialid);
		}

		// create the appropriate tracer
		switch (type) {
		case ObjectType::OBJECT_CUBE:
			{
				double width  = map_get(attributemap, OBJECT_WIDTH,  1.0);
				double height = map_get(attributemap, OBJECT_HEIGHT, 1.0);
				double depth  = map_get(attributemap, OBJECT_DEPTH,  1.0);
				bool   invert = map_get(attributemap, OBJECT_INVERT, false);
				std::shared_ptr<IHitable> cube = std::make_shared<Cube>(pos, width, height, depth, material, invert);
				scene->objects.insert(std::make_pair(name, cube));
			}
			break;
		case ObjectType::OBJECT_CYLINDER:
			{
				vec3 pos2 = map_get(attributemap, OBJECT_POS2, vec3(1));
				double radius = map_get(attributemap, OBJECT_RADIUS, 1.0);
				std::shared_ptr<IHitable> cylinder = std::make_shared<Cylinder>(pos, pos2, radius, material);
				scene->objects.insert(std::make_pair(name, cylinder));
			}
			break;
		case ObjectType::OBJECT_MESH:
			{
				std::string path = map_get(attributemap, OBJECT_MESH_PATH, std::string(""));
				bool flip = map_get(attributemap, OBJECT_INVERT, false);
				bool normalize = map_get(attributemap, OBJECT_MESH_NORMALIZE, false);
				bool smooth = map_get(attributemap, OBJECT_MESH_SMOOTH, false);
				std::shared_ptr<IHitable> mesh = load_mesh(path, material, flip, normalize, smooth);
				scene->objects.insert(std::make_pair(name, mesh));
			}
			break;
		case ObjectType::OBJECT_RECTANGLE:
			{
				vec3 xaxis = map_get(attributemap, OBJECT_XAXIS, vec3(1, 0, 0));
				vec3 yaxis = map_get(attributemap, OBJECT_YAXIS, vec3(0, 1, 0));
				std::shared_ptr<IHitable> rectangle = std::make_shared<Rectangle>(pos, xaxis, yaxis, material);
				scene->objects.insert(std::make_pair(name, rectangle));
			}
		break;
		case ObjectType::OBJECT_SPHERE:
			{
				double radius = map_get(attributemap, OBJECT_RADIUS, 1.0);
				std::shared_ptr<IHitable> sphere = std::make_shared<Sphere>(pos, radius, material);
				scene->objects.insert(std::make_pair(name, sphere));
			}
			break;
		default: // sphere
			{
				double radius = map_get(attributemap, OBJECT_RADIUS, 1.0);
				std::shared_ptr<IHitable> sphere = std::make_shared<Sphere>(pos, radius, material);
				scene->objects.insert(std::make_pair(name, sphere));
			}
			break;
		}
	};
	parser["ObjectName"] = [](const peg::SemanticValues& sv) {
		// grab value
		std::string name = sv[0].get<std::string>();

		return std::make_pair(OBJECT_NAME, peg::any(name));
	};
	parser["ObjectType"] = [](const peg::SemanticValues& sv) {
		std::string val = sv[0].get<std::string>();

		// determine camera type
		ObjectType type = OBJECT_SPHERE;
		if      (val == "cube"     ) type = OBJECT_CUBE;
		else if (val == "cylinder" ) type = OBJECT_CYLINDER;
		else if (val == "mesh"     ) type = OBJECT_MESH;
		else if (val == "rectangle") type = OBJECT_RECTANGLE;
		else if (val == "sphere"   ) type = OBJECT_SPHERE;

		return std::make_pair(OBJECT_TYPE, peg::any(type));
	};
	parser["ObjectPos"] = [](const peg::SemanticValues& sv) {
		// grab value
		vec3 pos = sv[0].get<vec3>();

		return std::make_pair(OBJECT_POS, peg::any(pos));
	};
	parser["ObjectPos2"] = [](const peg::SemanticValues& sv) {
		// grab value
		vec3 pos = sv[0].get<vec3>();

		return std::make_pair(OBJECT_POS2, peg::any(pos));
	};
	parser["ObjectMaterial"] = [](const peg::SemanticValues& sv) {
		// grab value
		std::string material = sv[0].get<std::string>();

		return std::make_pair(OBJECT_MATERIAL, peg::any(material));
	};
	parser["ObjectXAxis"] = [&](const peg::SemanticValues& sv) {
		// grab value
		vec3 xaxis = sv[0].get<vec3>();
		
		return std::make_pair(OBJECT_XAXIS, peg::any(xaxis));
	};
	parser["ObjectYAxis"] = [](const peg::SemanticValues& sv) {
		// grab value
		vec3 yaxis = sv[0].get<vec3>();
		
		return std::make_pair(OBJECT_YAXIS, peg::any(yaxis));
	};
	parser["ObjectRadius"] = [](const peg::SemanticValues& sv) {
		// grab value
		double radius = sv[0].get<double>();
		
		return std::make_pair(OBJECT_RADIUS, peg::any(radius));
	};
	parser["ObjectWidth"] = [](const peg::SemanticValues& sv) {
		// grab value
		double width = sv[0].get<double>();

		return std::make_pair(OBJECT_WIDTH, peg::any(width));
	};
	parser["ObjectHeight"] = [](const peg::SemanticValues& sv) {
		// grab value
		double height = sv[0].get<double>();

		return std::make_pair(OBJECT_HEIGHT, peg::any(height));
	};
	parser["ObjectDepth"] = [](const peg::SemanticValues& sv) {
		// grab value
		double depth = sv[0].get<double>();

		return std::make_pair(OBJECT_DEPTH, peg::any(depth));
	};
	parser["ObjectMeshPath"] = [](const peg::SemanticValues& sv) {
		// grab value
		std::string path = sv[0].get<std::string>();

		return std::make_pair(OBJECT_MESH_PATH, peg::any(path));
	};
	parser["ObjectInvert"] = [](const peg::SemanticValues& sv) {
		// grab value
		bool invert = sv[0].get<bool>();

		return std::make_pair(OBJECT_INVERT, peg::any(invert));
	};
	parser["ObjectMeshNormalize"] = [](const peg::SemanticValues& sv) {
		// grab value
		bool normalize = sv[0].get<bool>();

		return std::make_pair(OBJECT_MESH_NORMALIZE, peg::any(normalize));
	};
	parser["ObjectMeshSmooth"] = [](const peg::SemanticValues& sv) {
		// grab value
		bool smooth = sv[0].get<bool>();

		return std::make_pair(OBJECT_MESH_SMOOTH, peg::any(smooth));
	};

	/**
	 * building the scene
	 */
	parser["Scene"] = [&](const peg::SemanticValues& sv) {
		// build scene
		scene->organization->build();

		// add hitable and camera to the tracer
		scene->tracer->setHitable(scene->organization);
		scene->tracer->setCamera(scene->camera);
	};
	parser["Element"] = [&](const peg::SemanticValues& sv) {
		// collect attributes
		std::map<ElementAttribute, peg::any> attributemap;
		map_fill(attributemap, sv);

		// grab common object attributes
		std::string objectid = map_get(attributemap, ELEMENT_OBJECT, std::string("unnamed"));
		if (map_contains(scene->objects, objectid)) {
			std::shared_ptr<IHitable> obj = scene->objects.at(objectid);

			// apply transformations to object
			std::shared_ptr<IHitable> cur = obj;
			auto transformations = map_get(attributemap, ELEMENT_TRANSFORM, std::vector<std::pair<TransformAttribute, peg::any>>());
			for (size_t i = 0; i < transformations.size(); ++i) {
				auto transformation = transformations.at(i);
				if (transformation.first == TRANSFORM_TRANSLATE) {
					vec3 offset = transformation.second.get<vec3>();
					auto translation = std::make_shared<Translation>(cur, offset);
					cur = translation;
				}
				if (transformation.first == TRANSFORM_ROTATE) {
					auto axisangle = transformation.second.get<std::pair<vec3, double>>();
					vec3 axis = axisangle.first;
					double angle = axisangle.second;
					auto rotation = std::make_shared<Rotation>(cur, axis, angle);
					cur = rotation;
				}
			}

			if (scene->organization != nullptr) {
				scene->organization->insert(cur);
			}
		}

	};
	parser["ElementObject"] = [](const peg::SemanticValues& sv) {
		// grab value
		std::string objectid = sv[0].get<std::string>();

		return std::make_pair(ELEMENT_OBJECT, peg::any(objectid));
	};
	parser["SceneType"] = [&](const peg::SemanticValues& sv) {
		// grab value
		std::string val = sv[0].get<std::string>();

		// determine scene typé
		if     (val == "bvh" ) scene->organization = std::make_shared<BVH>();
		else if(val == "list") scene->organization = std::make_shared<HitableList>();
	};
	parser["ElementTransform"] = [](const peg::SemanticValues& sv) {
		// grab all values in order
		std::vector<std::pair<TransformAttribute, peg::any>> transformations;
		vector_fill(transformations, sv);

		return std::make_pair(ELEMENT_TRANSFORM, peg::any(transformations));
	};
	parser["TransformTranslate"] = [](const peg::SemanticValues& sv) {
		// grab value
		vec3 translate = sv[0].get<vec3>();

		return std::make_pair(TRANSFORM_TRANSLATE, peg::any(translate));
	};
	parser["TransformRotate"] = [](const peg::SemanticValues& sv) {
		// grab value
		vec3 axis = sv[0].get<vec3>();
		double angle = sv[1].get<double>();

		auto axisangle = std::make_pair(axis, angle);
		return std::make_pair(TRANSFORM_ROTATE, peg::any(axisangle));
	};



	// parse file
	parser.parse(text.c_str());

	return scene;
}