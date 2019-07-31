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

	return std::make_shared<SceneData>();
}