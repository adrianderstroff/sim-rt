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
		Camera        <- 'CAMERA' (_ CameraAttrib)* 
		CameraAttrib  <- CameraType / CameraPos / CameraLookAt / CameraUp / CameraFOV
		CameraType    <- 'TYPE' _ Word
		CameraPos     <- 'POS' _ Vector
		CameraLookAt  <- 'LOOKAT' _ Vector
		CameraUp      <- 'UP' _ Vector
		CameraFOV	  <- 'FOV' _ Double

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



	/**
	 * define general parser rules
	 */
	parser["Comment"] = [](const peg::SemanticValues& sv) {
		std::cout << "Comment: '" << sv.token()  << "'" << std::endl;
	};
	parser["EOL"] = [](const peg::SemanticValues& sv) {
		std::cout << "EOL: '" << sv.token() << "'" << std::endl;
	};
	parser["Word"] = [](const peg::SemanticValues& sv) -> std::string {
		return sv.token();
	};
	parser["Path"] = [](const peg::SemanticValues& sv) -> std::string {
		return sv.token();
	};
	parser["Number"] = [](const peg::SemanticValues& sv) -> int {
		return stoi(sv.token());
	};
	parser["Double"] = [](const peg::SemanticValues& sv) -> double {
		return stod(sv.token());
	};
	parser["Vector"] = [](const peg::SemanticValues& sv) -> vec3 {
		double x = sv[0].get<double>();
		double y = sv[1].get<double>();
		double z = sv[2].get<double>();
		return vec3(x, y, z);
	};

	/**
	 * building the tracer object
	 */
	TracerObj tracerobj{ "raycaster", 640, 480, 100, 100 };
	parser["TracerType"] = [&](const peg::SemanticValues& sv) {
		tracerobj.type = sv[0].get<std::string>();
	};
	parser["TracerRes"] = [&](const peg::SemanticValues& sv) {
		tracerobj.width  = sv[0].get<int>();
		tracerobj.height = sv[1].get<int>();
	};
	parser["TracerSamples"] = [&](const peg::SemanticValues& sv) {
		tracerobj.samples = sv[0].get<int>();
	};
	parser["TracerDepth"] = [&](const peg::SemanticValues& sv) {
		tracerobj.depth = sv[0].get<int>();
	};

	/**
	 * building the camera object
	 */
	CameraObj cameraobj{ "simple", vec3(0,0,1), vec3(0), vec3(0,1,0), 45.0 };
	parser["CameraType"] = [&](const peg::SemanticValues& sv) {
		cameraobj.type = sv[0].get<std::string>();
	};
	parser["CameraPos"] = [&](const peg::SemanticValues& sv) {
		cameraobj.pos = sv[0].get<vec3>();
	};
	parser["CameraLookAt"] = [&](const peg::SemanticValues& sv) {
		cameraobj.lookat = sv[0].get<vec3>();
	};
	parser["CameraUp"] = [&](const peg::SemanticValues& sv) {
		cameraobj.up = sv[0].get<vec3>();
	};
	parser["CameraFOV"] = [&](const peg::SemanticValues& sv) {
		cameraobj.fov = sv[0].get<double>();
	};

	/**
	 * building the material list
	 */
	std::map<std::string, MaterialObj> materialmap;
	MaterialObj currentMaterial{"normal", "unnamed0", vec3(1), std::map<std::string,std::string>()};
	parser["Material"] = [&](const peg::SemanticValues& sv) {
		// insert last material
		materialmap.insert(std::make_pair(currentMaterial.name, currentMaterial));
		// reset material
		currentMaterial = MaterialObj{ "normal", "unnamed"+std::to_string(materialmap.size()), vec3(1), std::map<std::string,std::string>() };
	};
	parser["MaterialName"] = [&](const peg::SemanticValues& sv) {
		currentMaterial.name = sv[0].get<std::string>();
	};
	parser["MaterialType"] = [&](const peg::SemanticValues& sv) {
		currentMaterial.type = sv[0].get<std::string>();
	};
	parser["MaterialColor"] = [&](const peg::SemanticValues& sv) {
		currentMaterial.color = sv[0].get<vec3>();
	};
	parser["MaterialTexPath"] = [&](const peg::SemanticValues& sv) {
		currentMaterial.options.insert(std::make_pair("PATH", sv[0].get<std::string>()));
	};
	parser["MaterialTexInterp"] = [&](const peg::SemanticValues& sv) {
		currentMaterial.options.insert(std::make_pair("ITPLT", sv[0].get<std::string>()));
	};
	parser["MaterialTexWrap"] = [&](const peg::SemanticValues& sv) {
		std::string wrap = (sv.size() > 1)
			? sv[0].get<std::string>() + " " + sv[1].get<std::string>()
			: sv[0].get<std::string>() + " " + sv[0].get<std::string>();
		currentMaterial.options.insert(std::make_pair("WRAP", wrap));
	};
	parser["MaterialGlassCoeff"] = [&](const peg::SemanticValues& sv) {
		double coeff = sv[0].get<double>();
		currentMaterial.options.insert(std::make_pair("COEFF", std::to_string(coeff)));
	};

	/**
	 * building the material list
	 */
	std::map<std::string, ObjectObj> objectmap;
	ObjectObj currentObject{ "sphere", "unnamed0", vec3(0), "", std::map<std::string,std::string>() };
	parser["Object"] = [&](const peg::SemanticValues& sv) {
		// insert current object
		objectmap.insert(std::make_pair(currentObject.name, currentObject));
		// reset object
		currentObject = ObjectObj{ "sphere", "unnamed" + std::to_string(objectmap.size()), vec3(0), "", std::map<std::string,std::string>() };
	};
	parser["ObjectName"] = [&](const peg::SemanticValues& sv) {
		currentObject.name = sv[0].get<std::string>();
	};
	parser["ObjectType"] = [&](const peg::SemanticValues& sv) {
		currentObject.type = sv[0].get<std::string>();
	};
	parser["ObjectPos"] = [&](const peg::SemanticValues& sv) {
		currentObject.pos = sv[0].get<vec3>();
	};
	parser["ObjectMaterial"] = [&](const peg::SemanticValues& sv) {
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
	parser["ObjectYAxis"] = [&](const peg::SemanticValues& sv) {
		vec3 yaxis = sv[0].get<vec3>();
		std::string x = std::to_string(yaxis.x);
		std::string y = std::to_string(yaxis.y);
		std::string z = std::to_string(yaxis.z);
		std::string axis = "(" + x + "," + y + "," + z + ")";
		currentMaterial.options.insert(std::make_pair("YAXIS", axis));
	};
	parser["ObjectRadius"] = [&](const peg::SemanticValues& sv) {
		double r = sv[0].get<double>();
		currentMaterial.options.insert(std::make_pair("R", std::to_string(r)));
	};

	/**
	 * building the element list
	 */
	std::map<std::string, SceneElement> elementmap;
	SceneElement currentElement{ "unnamed0" };
	std::string scenetype = "bvh";
	parser["Element"] = [&](const peg::SemanticValues& sv) {
		// insert current object
		elementmap.insert(std::make_pair(currentElement.name, currentElement));
		// reset object
		currentElement = SceneElement{ "unnamed" + std::to_string(elementmap.size()) };
	};
	parser["ElementObject"] = [&](const peg::SemanticValues& sv) {
		currentObject.name = sv[0].get<std::string>();
	};
	parser["SceneType"] = [&](const peg::SemanticValues& sv) {
		scenetype = sv[0].get<std::string>();
	};



	// parse file
	parser.parse(text.c_str());


	/**
	 * print stuff for debugging
	 */
	std::cout << "Tracer" << std::endl;
	std::cout << "    " << tracerobj.type << " " << tracerobj.width << ","
		<< tracerobj.height << "," << tracerobj.samples << "," << tracerobj.depth << std::endl;

	std::cout << "Camera" << std::endl;
	std::cout << "    " << cameraobj.type << " " << cameraobj.pos << ","
		<< cameraobj.lookat << "," << cameraobj.up << "," << cameraobj.fov << std::endl;

	for (auto m : materialmap) {
		auto mat = m.second;
		std::cout << "Material" << std::endl;
		std::cout << "    " << mat.name << " " << mat.type << " " << mat.color << std::endl;
		for (auto o : mat.options) {
			std::cout << "    " << o.first << " " << o.second << std::endl;
		}
	}

	for (auto o : objectmap) {
		auto obj = o.second;
		std::cout << "Object" << std::endl;
		std::cout << "    " << obj.name << " " << obj.type << " " << obj.pos << " " << obj.material << std::endl;
		for (auto opt : obj.options) {
			std::cout << "    " << opt.first << " " << opt.second << std::endl;
		}
	}

	std::cout << "Scene" << std::endl;
	std::cout << "    " << scenetype << std::endl;
	for (auto e : elementmap) {
		auto elem = e.second;
		std::cout << "Element" << std::endl;
		std::cout << "    " << elem.name << std::endl;
	}

	/**
	 * create objects based on the descriptions of the scene file
	 */
	std::shared_ptr<SceneData> scenedata = std::make_shared<SceneData>();
	create_tracer(scenedata, tracerobj);
	create_camera(scenedata, cameraobj);
	create_materials(scenedata, materialmap);
	create_objects(scenedata, objectmap);
	create_scene(scenedata, scenetype, elementmap);

	return scenedata;
}

void rt::create_tracer(std::shared_ptr<SceneData>& scene, TracerObj& tracerobj) {
	// extract common tracer attributes
	size_t width = tracerobj.width;
	size_t height = tracerobj.height;
	size_t samples = tracerobj.samples;
	size_t depth = tracerobj.depth;

	// create tracer
	if (tracerobj.type == "raycaster") {
		scene->tracer = std::make_shared<Raycaster>(width, height, samples, depth);
	}
	else if (tracerobj.type == "raytracer") {
		scene->tracer = std::make_shared<Raytracer>(width, height, samples, depth);
	}
	else if (tracerobj.type == "debugtracer") {
		scene->tracer = std::make_shared<Debugtracer>(width, height, samples, depth);
	}
	else {
		scene->tracer = std::make_shared<Raycaster>(width, height, samples, depth);
	}
}

void rt::create_camera(std::shared_ptr<SceneData>& scene, CameraObj& cameraobj) {
	// extract common camera attributes
	vec3 pos    = cameraobj.pos;
	vec3 lookat = cameraobj.lookat;
	vec3 up     = cameraobj.up;
	double fov  = cameraobj.fov;

	// create camera
	if (cameraobj.type == "simple") {
		scene->camera = std::make_shared<SimpleCamera>(pos, lookat, up, fov, scene->tracer->aspect());
	}
	else if (cameraobj.type == "dof") {
		// grab aperture if available
		double aperture = 1.0;
		if (has_option(cameraobj.options, "DOF"))
			aperture = stod(cameraobj.options.at("DOF"));

		scene->camera = std::make_shared<DOFCamera>(pos, lookat, up, fov, scene->tracer->aspect(), aperture);
	}

	// add camera to tracer
	scene->tracer->setCamera(scene->camera);
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