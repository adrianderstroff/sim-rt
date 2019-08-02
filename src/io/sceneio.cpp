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
		CameraAperture <- 'Aperture' _ Double

		# materials statement
		Materials          <- 'MATERIALS' (_ Material)*
		Material           <- 'MATERIAL' (_ MaterialAttrib)*
		MaterialAttrib     <- MaterialName / MaterialType / MaterialColor / MaterialTexPath / MaterialGlassCoeff / MaterialTexInterp / MaterialTexWrap
		MaterialName       <- 'NAME' _ Word
		MaterialType       <- 'TYPE' _ Word
		MaterialColor      <- 'COLOR' _ Vector
		MaterialTexPath    <- 'PATH' _ Path
		MaterialTexInterp  <- 'ITPLT' _ Word
		MaterialTexWrap    <- 'WRAP' _ Word Word?
		MaterialGlassCoeff <- 'COEFF' _ Double

		# objects statement
		Objects        <- 'OBJECTS' (_ Object)*
		Object         <- 'OBJECT' (_ ObjectAttrib)*
		ObjectAttrib   <- ObjectName / ObjectType / ObjectPos / ObjectMaterial / ObjectXAxis / ObjectYAxis / ObjectRadius
		ObjectName     <- 'NAME' _ Word
		ObjectType     <- 'TYPE' _ Word
		ObjectPos      <- 'POS' _ Vector
		ObjectMaterial <- 'MATERIAL' _ Word
		ObjectXAxis    <- 'XAXIS' _ Vector
		ObjectYAxis    <- 'YAXIS' _ Vector
		ObjectRadius   <- 'R' _ Double

		# scene statement
		Scene              <- 'SCENE' (_ SceneAttrib)*
		SceneAttrib        <- SceneType / Elements
		SceneType          <- 'TYPE' _ Word
		Elements           <- 'ELEMENTS' (_ Element)*
		Element            <- 'ELEMENT' (_ ElementAttrib)*
		ElementAttrib      <- ElementObject / ElementTranslation / ElementRotation
		ElementObject      <- 'OBJECT' _ Word
		ElementTranslation <- 'TRANSLATE' Vector
		ElementRotation    <- 'ROTATE' Vector Double

		# general statements
		Word        <- [a-z][a-z0-9]*
		Path        <- [a-zA-Z0-9./]+
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
	parser["Path"] = [](const peg::SemanticValues& sv) {
		return sv.token();
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
		std::string texpath = map_get(attributemap, MATERIAL_TEX_PATH, "");
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
			double coeff = map_get(attributemap, MATERIAL_REFRACTION_COEFF, 1.0);

			scene->materials.insert(std::make_pair(name, std::make_shared<Dielectric>(coeff, tex)));
			break;
		case MaterialType::MATERIAL_DIFFUSE_LIGHT:
			scene->materials.insert(std::make_pair(name, std::make_shared<DiffuseLight>(tex)));
			break;
		case MaterialType::MATERIAL_ISOTROPIC:
			scene->materials.insert(std::make_pair(name, std::make_shared<Isotropic>(tex)));
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
		std::pair wrap(wrapx, wrapy);

		return std::make_pair(MATERIAL_TEX_WRAP, peg::any(wrap));
	};
	parser["MaterialGlassCoeff"] = [](const peg::SemanticValues& sv) {
		// grab value
		double coeff = sv[0].get<double>();
		
		return std::make_pair(MATERIAL_REFRACTION_COEFF, peg::any(coeff));
	};

	/**
	 * building the object list
	 */
	/*
	std::map<std::string, ObjectObj> objectmap;
	ObjectObj currentObject{ "sphere", "unnamed0", vec3(0), "", std::map<std::string,std::string>() };
	parser["Object"] = [&](const peg::SemanticValues& sv) {
		// insert current object
		objectmap.insert(std::make_pair(currentObject.name, currentObject));
		// reset object
		currentObject = ObjectObj{ "sphere", "unnamed" + std::to_string(objectmap.size()), vec3(0), "", std::map<std::string,std::string>() };
	};
	parser["ObjectName"] = [](const peg::SemanticValues& sv) {
		currentObject.name = sv[0].get<std::string>();
	};
	parser["ObjectType"] = [](const peg::SemanticValues& sv) {
		currentObject.type = sv[0].get<std::string>();
	};
	parser["ObjectPos"] = [](const peg::SemanticValues& sv) {
		currentObject.pos = sv[0].get<vec3>();
	};
	parser["ObjectMaterial"] = [](const peg::SemanticValues& sv) {
		currentObject.material = sv[0].get<std::string>();
	};
	parser["ObjectXAxis"] = [&](const peg::SemanticValues& sv) {
		vec3 xaxis = sv[0].get<vec3>();
		std::string x = std::to_string(xaxis.x);
		std::string y = std::to_string(xaxis.y);
		std::string z = std::to_string(xaxis.z);
		std::string axis = "(" + x + "," + y + "," + z + ")";
		currentMaterial.options.insert(std::make_pair("XAXIS", axis));
	};
	parser["ObjectYAxis"] = [](const peg::SemanticValues& sv) {
		vec3 yaxis = sv[0].get<vec3>();
		std::string x = std::to_string(yaxis.x);
		std::string y = std::to_string(yaxis.y);
		std::string z = std::to_string(yaxis.z);
		std::string axis = "(" + x + "," + y + "," + z + ")";
		currentMaterial.options.insert(std::make_pair("YAXIS", axis));
	};
	parser["ObjectRadius"] = [](const peg::SemanticValues& sv) {
		double r = sv[0].get<double>();
		currentMaterial.options.insert(std::make_pair("R", std::to_string(r)));
	};
	*/

	/**
	 * building the scene
	 */
	/*
	std::map<std::string, SceneElement> elementmap;
	SceneElement currentElement{ "unnamed0" };
	std::string scenetype = "bvh";
	parser["Element"] = [&](const peg::SemanticValues& sv) {
		// insert current object
		elementmap.insert(std::make_pair(currentElement.name, currentElement));
		// reset object
		currentElement = SceneElement{ "unnamed" + std::to_string(elementmap.size()) };
	};
	parser["ElementObject"] = [](const peg::SemanticValues& sv) {
		currentObject.name = sv[0].get<std::string>();
	};
	parser["SceneType"] = [](const peg::SemanticValues& sv) {
		scenetype = sv[0].get<std::string>();
	};
	*/



	// parse file
	parser.parse(text.c_str());

	return scene;
}

void rt::create_materials(std::shared_ptr<SceneData>& scene, std::map<std::string, MaterialObj>& materialmap) {
	// iterate over all materials
	for (auto& materialobj : materialmap) {
		if (materialobj.second.type == "normal") {
			std::shared_ptr<IMaterial> mat = std::make_shared<NormalMaterial>();
			scene->materials.insert(std::make_pair(materialobj.first, mat));
		}
		else if (materialobj.second.type == "lambertian") {
			std::shared_ptr<ITexture> color = create_texture(materialobj.second);
			std::shared_ptr<IMaterial> mat = std::make_shared<Lambertian>(color);
			scene->materials.insert(std::make_pair(materialobj.first, mat));
		}
		else if (materialobj.second.type == "metal") {
			std::shared_ptr<ITexture> color = create_texture(materialobj.second);
			std::shared_ptr<IMaterial> mat = std::make_shared<Metal>(color);
			scene->materials.insert(std::make_pair(materialobj.first, mat));
		}
		else if (materialobj.second.type == "dielectric") {
			// create texture
			std::shared_ptr<ITexture> color = create_texture(materialobj.second);

			// grab refraction coefficient
			double coeff = 1;
			if (has_option(materialobj.second.options, "COEFF")) {
				coeff = stod(materialobj.second.options.at("COEFF"));
			}

			// create dielectric material
			std::shared_ptr<IMaterial> mat = std::make_shared<Dielectric>(coeff, color);
			scene->materials.insert(std::make_pair(materialobj.first, mat));
		}
		else if (materialobj.second.type == "diffuselight") {
			std::shared_ptr<ITexture> color = create_texture(materialobj.second);
			std::shared_ptr<IMaterial> mat = std::make_shared<DiffuseLight>(color);
			scene->materials.insert(std::make_pair(materialobj.first, mat));
		}
		else if (materialobj.second.type == "isotropic") {
			std::shared_ptr<ITexture> color = create_texture(materialobj.second);
			std::shared_ptr<IMaterial> mat = std::make_shared<Isotropic>(color);
			scene->materials.insert(std::make_pair(materialobj.first, mat));
		}
		else {
			console::println("Material not recognized");
			console::println("Options are 'normal', 'lambertian', 'metal', 'dielectric', 'diffuselight', 'isotropic'");
			console::println("assume 'normal'");
			std::shared_ptr<IMaterial> mat = std::make_shared<NormalMaterial>();
			scene->materials.insert(std::make_pair(materialobj.first, mat));
		}
	}
}

void rt::create_objects(std::shared_ptr<SceneData>& scene, std::map<std::string, ObjectObj>& objectmap) {	
	// iterate over all objects
	for (auto& obj : objectmap) {

		// grab common object attributes
		vec3 pos = obj.second.pos;
		std::shared_ptr<rt::IMaterial> material = grab_material(scene, obj.second.material);

		if (obj.second.type == "rectangle") {
			// grab first axis
			vec3 xaxis(0);
			if (has_option(obj.second.options, "XAXIS")) {
				xaxis = parse_vector(obj.second.options.at("XAXIS"));
			}

			// grab second axis
			vec3 yaxis(0);
			if (has_option(obj.second.options, "YAXIS")) {
				yaxis = parse_vector(obj.second.options.at("YAXIS"));
			}

			// create rectangle
			std::shared_ptr<IHitable> rectangle = std::make_shared<Rectangle>(pos, xaxis, yaxis, material);
			scene->objects.insert(std::make_pair(obj.first, rectangle));
		}
		else if (obj.second.type == "sphere") {
			// grab radius
			double radius = 1;
			if (has_option(obj.second.options, "R")) {
				radius = stod(obj.second.options.at("R"));
			}

			// create sphere
			std::shared_ptr<IHitable> sphere = std::make_shared<Sphere>(pos, radius, material);
			scene->objects.insert(std::make_pair(obj.first, sphere));
		}
		else if (obj.second.type == "cube") {
			// grab width
			double width = 1;
			if (has_option(obj.second.options, "WIDTH")) {
				width = stod(obj.second.options.at("WIDTH"));
			}

			// grab height
			double height = 1;
			if (has_option(obj.second.options, "HEIGHT")) {
				height = stod(obj.second.options.at("HEIGHT"));
			}

			// grab depth
			double depth = 1;
			if (has_option(obj.second.options, "DEPTH")) {
				depth = stod(obj.second.options.at("DEPTH"));
			}

			// create cube
			std::shared_ptr<IHitable> cube = std::make_shared<Cube>(pos, width, height, depth, material);
			scene->objects.insert(std::make_pair(obj.first, cube));
		}
		else if (obj.second.type == "cylinder") {
			// grab p1
			vec3 p1(0);
			if (has_option(obj.second.options, "P1")) {
				p1 = parse_vector(obj.second.options.at("P1"));
			}

			// grab p2
			vec3 p2(1);
			if (has_option(obj.second.options, "P2")) {
				p2 = parse_vector(obj.second.options.at("P2"));
			}

			// grab radius
			double radius = 1;
			if (has_option(obj.second.options, "R")) {
				radius = stod(obj.second.options.at("R"));
			}

			// create cylinder
			std::shared_ptr<IHitable> cube = std::make_shared<Cylinder>(p1, p2, radius, material);
			scene->objects.insert(std::make_pair(obj.first, cube));
		}
		else if (obj.second.type == "mesh") {
			std::string filename = ".";
			if (has_option(obj.second.options, "PATH")) {
				filename = obj.second.options.at("PATH");
			}

			// create mesh
			std::shared_ptr<IHitable> mesh = load_mesh(filename, material);
			scene->objects.insert(std::make_pair(obj.first, mesh));
		}
	}
}

void rt::create_scene(std::shared_ptr<SceneData>& scene, std::string scenetype, std::map<std::string, SceneElement>& elementmap) {
	// create world
	std::shared_ptr<BVH> world = std::make_shared<BVH>();
	for (auto& element : elementmap) {
		if (scene->objects.find(element.second.name) != scene->objects.end()) {
			std::shared_ptr<IHitable> object = scene->objects.at(element.second.name);
			world->insert(object);
		}
	}
	world->build();

	// add world to scene and tracer
	scene->world = world;
	scene->tracer->setHitable(world);
}

bool rt::has_option(std::map<std::string, std::string>& options, std::string findoption) {
	return options.find(findoption) != options.end();
}

std::shared_ptr<rt::ITexture> rt::create_texture(MaterialObj& materialobj) {
	// grab the color
	vec3 color = materialobj.color;
	std::shared_ptr<ITexture> texture = std::make_shared<ConstantTexture>(color);

	// see if a texture path is specified
	if (has_option(materialobj.options, "PATH")) {
		auto [image, status] = read_image(materialobj.options.at("PATH"));
		if (status) {
			// create image texture
			auto imagetexture = std::make_shared<ImageTexture>(image);

			// check for interpolation method and update image texture
			if (has_option(materialobj.options, "ITPLT")) {
				std::string interpolationmethod = materialobj.options.at("ITPLT");
				if (interpolationmethod == "bilinear") {
					imagetexture->set_interpolation_method(ImageTexture::Interpolation::BILINEAR);
				}
				else if (interpolationmethod == "nearest") {
					imagetexture->set_interpolation_method(ImageTexture::Interpolation::NEAREST_NEIGHBOR);
				}
				else {
					console::println("Interpolation method not recognized");
					console::println("Options are 'bilinear', 'nearest'");
					console::println("assume 'bilinear'");
					imagetexture->set_interpolation_method(ImageTexture::Interpolation::BILINEAR);
				}
			}

			// check for texture wrap and update image texture
			if (has_option(materialobj.options, "WRAP")) {
				std::string wrap = materialobj.options.at("WRAP");
				std::vector<std::string> tokens = split(wrap, ' ');

				// set wrap in x
				ImageTexture::Wrap wrapx = ImageTexture::Wrap::CLAMP;
				if (tokens.at(0) == "repeat") {
					wrapx = ImageTexture::Wrap::REPEAT;
				}
				else if (tokens.at(0) == "clamp") {
					wrapx = ImageTexture::Wrap::CLAMP;
				}
				else {
					console::println("Texture wrap x not recognized");
					console::println("Options are 'clamp', 'repeat'");
					console::println("assume 'clamp'");
				}

				// set wrap in x
				ImageTexture::Wrap wrapy = ImageTexture::Wrap::CLAMP;
				if (tokens.at(1) == "repeat") {
					wrapy = ImageTexture::Wrap::REPEAT;
				}
				else if (tokens.at(1) == "clamp") {
					wrapy = ImageTexture::Wrap::CLAMP;
				}
				else {
					console::println("Texture wrap x not recognized");
					console::println("Options are 'clamp', 'repeat'");
					console::println("assume 'clamp'");
				}

				imagetexture->set_wrap_method(wrapx, wrapy);
			}

			// replace constant texture with image texture
			texture = imagetexture;
		}
	}

	return texture;
}

std::shared_ptr<rt::IMaterial> rt::grab_material(std::shared_ptr<SceneData>& scene, std::string materialid) {
	std::shared_ptr<IMaterial> material;
	
	// look up material in list of materials
	if (scene->materials.find(materialid) != scene->materials.end()) {
		material = scene->materials.at(materialid);
	}
	else {
		// fall back material
		material = std::make_shared<Lambertian>(new_color(1));
	}

	return material;
}

rt::vec3 rt::parse_vector(std::string text) {
	auto grammar = R"(
		Vector      <- '(' _ Double _ ',' _ Double _ ',' _ Double _ ')'
		Double      <- '-'? [0-9]+ ('.' [0-9]+)?
		~_			<- [ \t\r\n]*
    )";

	peg::parser parser;
	auto ok = parser.load_grammar(grammar);
	assert(ok);

	vec3 out(0);

	parser["Double"] = [](const peg::SemanticValues& sv) -> double {
		return stod(sv.token());
	};
	parser["Vector"] = [&](const peg::SemanticValues& sv) {
		double x = sv[0].get<double>();
		double y = sv[1].get<double>();
		double z = sv[2].get<double>();
		out = vec3(x, y, z);
	};

	parser.parse(text.c_str());

	return out;
}