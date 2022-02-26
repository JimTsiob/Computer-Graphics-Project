#include "Renderer.h"
#include "GeometryNode.h"
#include "Tools.h"
#include "ShaderProgram.h"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "OBJLoader.h"
#include <algorithm>
#include <array>
#include <iostream>

// RENDERER
Renderer::Renderer()
{
	this->m_nodes = {};
	this->m_continous_time = 0.0;
}

Renderer::~Renderer()
{
	glDeleteTextures(1, &m_fbo_depth_texture);
	glDeleteTextures(1, &m_fbo_pos_texture);
	glDeleteTextures(1, &m_fbo_normal_texture);
	glDeleteTextures(1, &m_fbo_albedo_texture);
	glDeleteTextures(1, &m_fbo_mask_texture);

	glDeleteFramebuffers(1, &m_fbo);

	glDeleteVertexArrays(1, &m_vao_fbo);
	glDeleteBuffers(1, &m_vbo_fbo_vertices);
}

bool Renderer::Init(int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	this->m_screen_width = SCREEN_WIDTH;
	this->m_screen_height = SCREEN_HEIGHT;

	bool techniques_initialization = InitShaders();

	bool meshes_initialization = InitGeometricMeshes();
	bool light_initialization = InitLights();

	bool common_initialization = InitCommonItems();
	bool inter_buffers_initialization = InitIntermediateBuffers();

	//If there was any errors
	if (Tools::CheckGLError() != GL_NO_ERROR)
	{
		printf("Exiting with error at Renderer::Init\n");
		return false;
	}

	this->BuildWorld();
	this->InitCamera();
	this->InitCraft();


	//If everything initialized
	return techniques_initialization && meshes_initialization &&
		common_initialization && inter_buffers_initialization;
}

void Renderer::BuildWorld()
{
	/*GeometryNode& craft1 = *this->m_nodes[0];
	GeometryNode& craft2 = *this->m_nodes[1];

	craft1.model_matrix = glm::translate(glm::mat4(1.f), glm::vec3(-15.f, 0.f, 0.f));
	craft1.m_aabb.center = glm::vec3(craft1.model_matrix * glm::vec4(craft1.m_aabb.center, 1.f));

	craft2.model_matrix = glm::translate(glm::mat4(1.f), craft2.m_aabb.center) *
		glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f)) *
		glm::translate(glm::mat4(1.f), -craft2.m_aabb.center);

	craft2.m_aabb.center = glm::vec3(craft2.model_matrix * glm::vec4(craft2.m_aabb.center, 1.f));*/

	// Dhmiourgia kai topothethsh twn antikeimenwn mas
	GeometryNode& craft = *this->m_nodes[OBJECTS::CRAFT];
	//GeometryNode& collision_hull = *this->m_nodes[OBJECS::COLLISION_HULL];
	GeometryNode& ball = *this->m_nodes[OBJECTS::BALL];
	GeometryNode& ball2 = *this->m_nodes[OBJECTS::BALL2];
	GeometryNode& ball3 = *this->m_nodes[OBJECTS::BALL3];
	GeometryNode& ball4 = *this->m_nodes[OBJECTS::BALL4];
	GeometryNode& ball5 = *this->m_nodes[OBJECTS::BALL5];
	GeometryNode& terrain = *this->m_nodes[OBJECTS::TERRAIN];


	craft.model_matrix = glm::mat4(1.f);
	//collision_hull.model_matrix = glm::mat4(1.f);
	terrain.model_matrix = glm::mat4(1.f);
	ball.model_matrix = glm::mat4(1.f);
	ball2.model_matrix = glm::mat4(1.f);
	ball3.model_matrix = glm::mat4(1.f);
	ball4.model_matrix = glm::mat4(1.f);
	ball5.model_matrix = glm::mat4(1.f);
	

	craft.app_model_matrix =
		//glm::rotate(glm::mat4(1.f), glm::radians(-40.f), glm::vec3(0.f, 1.f, 0.f)) *
		glm::translate(glm::mat4(1.f), glm::vec3(-80.f, 60.f, 100.f)) *
		craft.model_matrix;

	ball.app_model_matrix =
		//glm::rotate(glm::mat4(1.f), glm::radians(-40.f), glm::vec3(0.f, 1.f, 0.f)) *
		glm::translate(glm::mat4(1.f), glm::vec3(-80.f, 60.f, 80.f)) *
		ball.model_matrix;

	ball2.app_model_matrix = 
		glm::translate(glm::mat4(1.f), glm::vec3(-100.f, 60.f, -60.f)) *
		ball2.model_matrix;

	ball3.app_model_matrix =
		glm::translate(glm::mat4(1.f), glm::vec3(-150.f, 80.f, -300.f)) *
		ball3.model_matrix;

	ball4.app_model_matrix =
		glm::translate(glm::mat4(1.f), glm::vec3(-20.f, 60.f, -300.f)) *
		ball4.model_matrix;

	ball5.app_model_matrix=
		glm::translate(glm::mat4(1.f), glm::vec3(110.f, 60.f, -100.f)) *
		ball4.model_matrix;

	//this->m_world_matrix = glm::mat4(1.f);
	this->m_world_matrix = glm::scale(glm::mat4(1.f), glm::vec3(0.05, 0.05, 0.05));
}

void Renderer::InitCamera()
{
	/*this->m_camera_position = glm::vec3(-1.5, 2, 25);
	this->m_camera_target_position = glm::vec3(-1.5, 0, 0);
	this->m_camera_up_vector = glm::vec3(0, 1, 0);*/

	// proper settings for 0.05 scaled world
	this->m_camera_position = glm::vec3(-4.02803, 3.57532 ,7.33156);
	this->m_camera_target_position = glm::vec3(-4.13026, 1.11377, -11.6006);
	this->m_camera_up_vector = glm::vec3(0, 1, 0);

	
	// testing for the light
	/*this->m_camera_position = glm::vec3(2.9125, 25.1817, -10.2743);
	this->m_camera_target_position = glm::vec3(3.68301, 7.2591, -1.70313);
	this->m_camera_up_vector = glm::vec3(0, 1, 0);*/

	this->m_view_matrix = glm::lookAt(
		this->m_camera_position,
		this->m_camera_target_position,
		m_camera_up_vector);

	this->m_projection_matrix = glm::perspective(
		glm::radians(45.f),
		this->m_screen_width / (float)this->m_screen_height,
		0.1f, 100.f);
}

void Renderer::InitCraft() {

	this->craft_position = glm::vec3(-80.f, 60.f, 100.f);
	this->dfacing = glm::vec3(0.f, 0.f, -1.f); // allazoume ayto gia na kineitai eytheia
	this->dup = glm::vec3(0.f, 1.f, 0.f);
	this->dright = glm::vec3(0.f, 0.f, 0.f);

}

bool Renderer::InitLights()
{	
	// poly kala settings gia Cook Torrance model (kati paizei me ta shadows though)
	// don't forget to comment out deferred pass lines 247,255,259 each time for the Blinn Phong experiment
	/*this->m_light.SetColor(glm::vec3(8000.f));
	this->m_light.SetPosition(glm::vec3(25.7788, 23.1089, 20.1286));
	this->m_light.SetTarget(glm::vec3(14.4515 ,12.898 ,7.78002));
	this->m_light.SetConeSize(4096, 4096);
	this->m_light.CastShadow(true);*/

	// pos : 
	// tar pos : 

	// kala settings gia Blinn Phong model
	this->m_light.SetColor(glm::vec3(4000.f));
	this->m_light.SetPosition(glm::vec3(2.9125, 25.1817, -10.2743));
	this->m_light.SetTarget(glm::vec3(3.68301 ,8.2591 ,-1.70313));
	this->m_light.SetConeSize(4096,4096);
	this->m_light.CastShadow(true);

	return true;
}

bool Renderer::InitShaders()
{
	std::string vertex_shader_path = "Assets/Shaders/geometry pass.vert";
	std::string geometry_shader_path = "Assets/Shaders/geometry pass.geom";
	std::string fragment_shader_path = "Assets/Shaders/geometry pass.frag";

	m_geometry_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_geometry_program.LoadGeometryShaderFromFile(geometry_shader_path.c_str());
	m_geometry_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_geometry_program.CreateProgram();

	vertex_shader_path = "Assets/Shaders/deferred pass.vert";
	fragment_shader_path = "Assets/Shaders/deferred pass.frag";

	m_deferred_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_deferred_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_deferred_program.CreateProgram();


	vertex_shader_path = "Assets/Shaders/post_process.vert";
	fragment_shader_path = "Assets/Shaders/post_process.frag";

	m_post_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_post_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_post_program.CreateProgram();

	vertex_shader_path = "Assets/Shaders/shadow_map_rendering.vert";
	fragment_shader_path = "Assets/Shaders/shadow_map_rendering.frag";

	m_spot_light_shadow_map_program.LoadVertexShaderFromFile(vertex_shader_path.c_str());
	m_spot_light_shadow_map_program.LoadFragmentShaderFromFile(fragment_shader_path.c_str());
	m_spot_light_shadow_map_program.CreateProgram();

	return true;
}

bool Renderer::InitIntermediateBuffers()
{
	glGenTextures(1, &m_fbo_depth_texture);
	glGenTextures(1, &m_fbo_pos_texture);
	glGenTextures(1, &m_fbo_normal_texture);
	glGenTextures(1, &m_fbo_albedo_texture);
	glGenTextures(1, &m_fbo_mask_texture);
	glGenTextures(1, &m_fbo_texture);

	glGenFramebuffers(1, &m_fbo);

	return ResizeBuffers(m_screen_width, m_screen_height);
}

bool Renderer::ResizeBuffers(int width, int height)
{
	m_screen_width = width;
	m_screen_height = height;

	// texture
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, m_fbo_pos_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	
	glBindTexture(GL_TEXTURE_2D, m_fbo_normal_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, m_fbo_albedo_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, m_fbo_mask_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_screen_width, m_screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_screen_width, m_screen_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	// framebuffer to link to everything together
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_pos_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_fbo_normal_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_fbo_albedo_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_fbo_mask_texture, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fbo_depth_texture, 0);

	GLenum status = Tools::CheckFramebufferStatus(m_fbo);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool Renderer::InitCommonItems()
{
	glGenVertexArrays(1, &m_vao_fbo);
	glBindVertexArray(m_vao_fbo);

	GLfloat fbo_vertices[] = {
		-1, -1,
		1, -1,
		-1, 1,
		1, 1, };

	glGenBuffers(1, &m_vbo_fbo_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_fbo_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindVertexArray(0);
	return true;
}

bool Renderer::InitGeometricMeshes()
{
	/*std::array<const char*, 1> assets = {
		"Assets/game_assets/craft.obj", };*/


	std::vector<const char*> spaceAssets = {
		"Assets/SpaceshipGameStuff/craft.obj",
		"Assets/SpaceshipGameStuff/terrain.obj",
		"Assets/SpaceshipGameStuff/11715_ball_v3_L1.obj",
		"Assets/SpaceshipGameStuff/11715_ball_v3_L1.obj",
		"Assets/SpaceshipGameStuff/11715_ball_v3_L1.obj",
		"Assets/SpaceshipGameStuff/11715_ball_v3_L1.obj",
		"Assets/SpaceshipGameStuff/11715_ball_v3_L1.obj",
		//"Assets/SpaceshipGameStuff/collision_hull.obj",
	};

	/*std::vector<const char*> col_hull = {
		"Assets/SpaceshipGameStuff/collision_hull.obj",
	};*/

	bool initialized = true;
	OBJLoader loader;
	for (auto& asset : spaceAssets)
	{
		GeometricMesh* mesh = loader.load(asset);

		if (mesh != nullptr)
		{
			GeometryNode* node = new GeometryNode();
			node->Init(asset,mesh);
			this->m_nodes.push_back(node);
			delete mesh;
		}
		else
		{
			initialized = false;
		}
	}

	// collidable geometry code

	/*GeometricMesh* mesh = loader.load(spaceAssets[0]);

	if (mesh != nullptr)
	{
		CollidableNode* node = new CollidableNode();
		node->Init("col_craft",mesh);
		this->m_collidables_nodes.push_back(node);
		delete mesh;
	}*/

	/*GeometricMesh* mesh2 = loader.load(spaceAssets[1]);

	if (mesh2 != nullptr) {
		CollidableNode* node = new CollidableNode();
		node->Init("col_terrain", mesh2);
		this->m_collidables_nodes.push_back(node);
		delete mesh2;
	}*/

	return initialized;


	/*auto& asset = assets[0];
	GeometricMesh* mesh = loader.load(asset);

	if (mesh != nullptr)
	{
		GeometryNode* node1 = new GeometryNode();
		node1->Init("craft_1", mesh);

		GeometryNode* node2 = new GeometryNode();
		node2->Init("craft_2", mesh);

		this->m_nodes.push_back(node1);
		this->m_nodes.push_back(node2);

		delete mesh;
	}
	else
	{
		initialized = false;
	}
	*/
}

void Renderer::Update(float dt)
{
	this->UpdateGeometry(dt);
	this->UpdateCraft(dt);
	this->UpdateBall();
	this->RockCollision();
	this->UpdateCamera(dt);	
	m_continous_time += dt;
}

void Renderer::UpdateGeometry(float dt)
{
	/*GeometryNode& craft1 = *this->m_nodes[0];
	GeometryNode& craft2 = *this->m_nodes[1];
	craft1.app_model_matrix = craft1.model_matrix;
	craft2.app_model_matrix = craft2.model_matrix;*/
	GeometryNode& craft = *this->m_nodes[OBJECTS::CRAFT];
	//GeometryNode& collision_hull = *this->m_nodes[OBJECTS::COLLISION_HULL];
	GeometryNode& terrain = *this->m_nodes[OBJECTS::TERRAIN];
	/*GeometryNode& ball = *this->m_nodes[OBJECTS::BALL];
	GeometryNode& ball2 = *this->m_nodes[OBJECTS::BALL2];*/


	craft.app_model_matrix = craft.model_matrix;
	terrain.app_model_matrix = terrain.model_matrix;
	/*ball.app_model_matrix = ball.model_matrix;
	ball2.app_model_matrix = ball2.model_matrix;*/ // gia kapoio logo exafanizontai oi mpales
	//collision_hull.app_model_matrix = collision_hull.model_matrix;
}

void Renderer::UpdateCraft(float dt) {

	GeometryNode& craft = *this->m_nodes[OBJECTS::CRAFT];

	craft.app_model_matrix =
		glm::translate(glm::mat4(1.f), craft_position) *
		glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0.f, 1.f, 0.f)) *
		//glm::rotate(glm::mat4(1.f), glm::radians(-20.f), glm::vec3(1.f, 0.f, 0.f)) * 
		craft.model_matrix;


	if (dfacing.y > 0) { // an pame panw kane rotate to skafos pros ta panw
		craft.app_model_matrix =
			glm::translate(glm::mat4(1.f), craft_position) *
			glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0.f, 1.f, 0.f)) *
			glm::rotate(glm::mat4(1.f), glm::radians(20.f) , glm::vec3(1.f, 0.f, 0.f)) *
			craft.model_matrix;
	}

	if (turnHelper == 1) { // an pame dexia rotate to skafos pros ta dexia
		craft.app_model_matrix =
			glm::translate(glm::mat4(1.f), craft_position) *
			//glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0.f, 1.f, 0.f)) *
			glm::rotate(glm::mat4(1.f), glm::radians(-40.f), glm::vec3(0.f, 1.f, 1.f)) *
			craft.model_matrix;
		
	}

	if (dfacing.y < 0) { // an pame katw rotate pros ta katw
		craft.app_model_matrix =
			glm::translate(glm::mat4(1.f), craft_position) *
			glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0.f, 1.f, 0.f)) *
			glm::rotate(glm::mat4(1.f), glm::radians(-20.f), glm::vec3(1.f, 0.f, 0.f)) *
			craft.model_matrix;
	}

	if (turnHelper == 0) { // an pame aristera rotate pros ta aristera
		craft.app_model_matrix =
			glm::translate(glm::mat4(1.f), craft_position) *
			//glm::rotate(glm::mat4(1.f), glm::radians(angle) , glm::vec3(0.f, 1.f, 0.f)) * // change direction
			glm::rotate(glm::mat4(1.f), glm::radians(30.f) , glm::vec3(0.f, 1.f, 1.f)) *
			craft.model_matrix;
		
	}
	float speed = glm::pi<float>() * 10; // Starting speed
	//float speed = 0;
	//u = 0.f; // ayta allazw gia to pou kineitai to skafos
	//v = 0.f; // u,v variables used for horizontal and vertical movement of the spacecraft

	//std::cout << dfacing.z << std::endl;

	dfacing = glm::normalize(dfacing + u * dt * dright + v * dt * dup);

	dright = glm::normalize(glm::cross(dfacing, dup));

	dup = glm::cross(dright, dfacing);

	craft_position = craft_position + speed * dt * dfacing;

	//std::cout << dt << std::endl;

	//std::cout << Xhelper << std::endl;

	//std::cout << craft_position.x << " " << craft_position.y << " " << craft_position.z << std::endl;
}

void Renderer::UpdateBall() {

	GeometryNode& ball1 = *this->m_nodes[OBJECTS::BALL];
	GeometryNode& ball2 = *this->m_nodes[OBJECTS::BALL2];
	GeometryNode& ball3 = *this->m_nodes[OBJECTS::BALL3];
	GeometryNode& ball4 = *this->m_nodes[OBJECTS::BALL4];
	GeometryNode& ball5 = *this->m_nodes[OBJECTS::BALL5];

	// if the balls are hit they're sent "below ground" so that the player can't see them anymore.

	//std::cout << craft_position.x << " " << craft_position.y << " " << craft_position.z << std::endl;

	// collision with first ball

	if ((craft_position.x >= -94 && craft_position.x <= -70) && (craft_position.y <= 64 && craft_position.y >= 56) && (craft_position.z >= 58 && craft_position.z <= 75.f)) {
		ball1hit = true;
	}

	if (ball1hit == true) {
		ball1.app_model_matrix =
			glm::translate(glm::mat4(1.f), glm::vec3(-80.f, -200.f, -60.f)) *
			ball1.model_matrix;
	}

	// collision with second ball
	// -97.3009 60 -53.6275
	// -113.353 60 -71.1641
	// -103.28 60 -54.0361
	// -93.1699 60 - 54.2169
	// -92.6193 60 -60.2581
	// -89.5742 60 -56.3666 peak aristerou fterou
	// -97.1796 60 -55.2003 center

	if ((craft_position.x <= -87 && craft_position.x >= -110) && (craft_position.y <= 64 && craft_position.y >= 55) && (craft_position.z <= -52 && craft_position.z >= -72)) {
		ball2hit = true;
	}	

	
	if (ball2hit == true) { 
		ball2.app_model_matrix =
			glm::translate(glm::mat4(1.f), glm::vec3(-100.f, -200.f, -60.f)) *
			ball2.model_matrix;
	}

	// collision with third ball
	// -147.445 79.2551 -293.792 center
	// -157.778 80.1738 -288.082 dexi ftero
	// -159.811 79.547 -296.873 peak dexiou fterou
	// -143.789 79.1822 -304.018 aristero ftero
	// -142.468 79.703 -296.131 peak aristerou fterou
	// -126.755 77.2029 -328.185

	if ((craft_position.x <= -125 && craft_position.x >= -161) && (craft_position.y >= 76 && craft_position.y <= 82) && (craft_position.z <= -287 && craft_position.z >= -305)) {
		ball3hit = true;
	}

	if (ball3hit == true) {
		ball3.app_model_matrix =
			glm::translate(glm::mat4(1.f), glm::vec3(-100.f, -200.f, -60.f)) *
			ball3.model_matrix;
	}

	// collision with fourth ball 
	// -20.5992 62.5992 -297.004 dexi ftero
	// -13.2395 60.6776 -290.22 aristero ftero
	// -25.8885 57.379 -301.176 dexi ftero (katw apo tunnel)

	if ((craft_position.x <= -13 && craft_position.x >= -26) && (craft_position.y >= 56 && craft_position.y <= 64) && (craft_position.z <= -289 && craft_position.z >= -302)) {
		ball4hit = true;
	}

	if (ball4hit == true) {
		ball4.app_model_matrix =
			glm::translate(glm::mat4(1.f), glm::vec3(-100.f, -200.f, -60.f)) *
			ball4.model_matrix;
	}

	// collision with fifth ball
	// 111.501 55.7348 -104.84 center
	// 104.547 60 - 101.84 dexi ftero
	// 110.003 57.6275 -90.134
	if ((craft_position.x >= 103 && craft_position.x <= 113) && (craft_position.y >= 51 && craft_position.y <= 62) && (craft_position.z <= -83 && craft_position.z >= -106)) {
		ball5hit = true;
	}

	if (ball5hit == true) {
		ball5.app_model_matrix =
			glm::translate(glm::mat4(1.f), glm::vec3(-100.f, -200.f, -60.f)) *
			ball5.model_matrix;
	}

}


void Renderer::RockCollision() {
	//				-------------------------------- left rock (2) from start------------- 
	// -109.114 60 -5.07149 dexi meros
	// -143.538 60 5.55803 aristero meros

	if ((craft_position.x >= -144 && craft_position.x <= -108) && (craft_position.z <= 6 && craft_position.z >= -6)) {
		hit = true;
	}

	//				---------------------------------- middle rock ------------------
	// left side
	// -116.83 60 -178.365 aristero meros
	// -41.4572 39.1444 -201.085 dexi meros

	if ((craft_position.x >= -117 && craft_position.x <= -40) && (craft_position.z >= -202 && craft_position.z <= -177)) {
		hit = true;
	}

	// upper side
	// 6.23826 102.021 -74.039 dexi meros
	// -71.1655 84.544 -155.722 aristero meros

	if ((craft_position.x >= -72 && craft_position.x <= 5) && (craft_position.y >= 83 && craft_position.y <= 104) && (craft_position.z <= -120 && craft_position.z >= -159)) {
		hit = true;
	}

	// right side
	// -20.9911 47.2392 -70.6158 aristero meros
	//  40.5556 60 -59.971  dexi meros
	// 6.43741 60 -133.195
	// 55.2335 60 -84.1836
	if ((craft_position.x >= -32 && craft_position.x <= 56) && (craft_position.z >= -134 && craft_position.z <= -58)) {
		hit = true;
	}

	//								---------------- left rock 1 --------------------------
	// -212.994 60 97.6668 center
	// -233.324 60 119.486 aristero meros
	// -215.915 60 79.4595 dexi meros

	if ((craft_position.x >= -234 && craft_position.x <= -211) && (craft_position.z >= 78 && craft_position.z <= 98)) {
		hit = true;
	}

	//								------------------- right rock from start ----------
	// -36.633 60 62.4383 center
	// -26.7966 60 99.3748 dexi meros
	// -9.97892 60 26.1638 aristero meros
	if ((craft_position.x >= -37 && craft_position.x <= -8) && (craft_position.z >= 25 && craft_position.z <= 100)) {
		hit = true;
	}

	//								---------------- rock next to ball 5 --------------------
	// 97.0068 60 -21.0039 center
	// 138.332 60 -61.1731 aristero meros
	// 114.996 60 6.07979 dexi meros

	if ((craft_position.x >= 96 && craft_position.x <= 139) && (craft_position.z <= 6 && craft_position.z >= -62)) {
		hit = true;
	}

	//								--------------- rock next to ball 4 ---------------------
	// 106.413 60.2723 -253.565 dexi meros
	// 81.8739 45.4704 -306.717 aristero meros

	if ((craft_position.x >= 80 && craft_position.x <= 108) && (craft_position.z <= -251 && craft_position.z >= -307)) {
		hit = true;
	}


	//								--------------- rock next to ball 3 ---------------------
	// -185.537 60 -285.929 dexi meros
	// -220.683 60 -277.087 aristero meros

	if ((craft_position.x <= -184 && craft_position.x >= -221) && (craft_position.z >= -286 && craft_position.z <= -276)) {
		hit = true;
	}

	//							   --------------- left rock 3 ---------------------
	// -208.822 60 -102.969 panw meros
	// -218.011 60 -167.551 katw meros

	if ((craft_position.x >= -219 && craft_position.x <= -207) && (craft_position.z >= -168 && craft_position.z <= -101)) {
		hit = true;
	}



}

void Renderer::UpdateAngle(bool left) {

	float e = 0.01f;
	if (left) {
		angle = angle + 27.f;
		/*m_camera_position.x += 0.5f;
		m_camera_target_position.x -= 1.5f;*/
		//m_camera_position.z -= 0.2f;
	}
	else {
		angle = angle + 333.f;
		/*m_camera_position.x -= 0.5f;
		m_camera_target_position.x += 1.5f;*/
		//m_camera_position.z -= 0.2f;
	}

	angle = fmod(angle, 360.f);
}

void Renderer::UpdateDirection(int direction){
	std::cout << "Angle : " << angle << std::endl;
	std::cout << "dfacing x : " << dfacing.x << std::endl;
	std::cout << "dfacing z : " << dfacing.z << std::endl;
	if (angle >= 0.f && angle < 90.f) {
		dfacing.x = dfacing.x - 0.3f * direction; // -1
		dfacing.z = dfacing.z + 0.3f * direction; // 0
	}
	else if (angle >= 90.f && angle < 180.f) {
		dfacing.x = dfacing.x + 0.3f * direction; // 0
		dfacing.z = dfacing.z + 0.3f * direction; // 1
	}else if (angle >= 180.f && angle < 270.f){
		dfacing.x = dfacing.x + 0.3f * direction; // 1
		dfacing.z = dfacing.z - 0.3f * direction; // 0
		//m_camera_position.z = m_camera_position.z + 3.5f * direction;
	}
	else if (angle >= 270.f && angle < 360.f){
		dfacing.x = dfacing.x - 0.3f * direction; // 0
		dfacing.z = dfacing.z - 0.3f * direction; // -1
		//m_camera_position.z = m_camera_position.z + 3.5f * direction;
	}
	// camera following spacecraft
	//m_camera_movement.y = direction;

	//m_camera_position.x = m_camera_position.x + 0.4f * direction;
	//m_camera_position.y = m_camera_position.y + 0.15f * direction;
	//m_camera_position.z = m_camera_position.z + 0.05f * direction;
	//m_camera_target_position.x = m_camera_target_position.x - 0.5f * direction;
	//m_camera_target_position.z = m_camera_target_position.z - 0.5f * direction;

}

void Renderer::setTurnHelper(float f) {
	turnHelper = f;
}

void Renderer::UpdateCamera(float dt)
{

	//glm::vec3 direction = glm::rotate(glm::mat4(1.f), glm::radians(10.f), glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(dfacing, 0.f);
	//m_camera_target_position = m_camera_position + direction * glm::distance(m_camera_position, m_camera_target_position);

	//akolouthei to skafos
	glm::vec3 direction = dfacing; //glm::normalize(m_camera_target_position - m_camera_position);

	//direction = glm::rotate(glm::mat4(1.f), glm::radians(angle) , glm::vec3(0.f, 0.f, 0.f)) * glm::vec4(direction, 0.f);

	m_camera_position = m_camera_position + (/*m_camera_movement.x*/ 1.5f * dt) * direction;
	// ayta gia skafos
	m_camera_target_position = m_camera_target_position + (/*m_camera_movement.x */ 1.5f * dt) * direction;

	/*m_camera_position = m_camera_position + (m_camera_movement.x * dt) * direction;

	m_camera_target_position = m_camera_target_position + (m_camera_movement.x * dt) * direction;*/

	glm::vec3 right = glm::normalize(glm::cross(direction, m_camera_up_vector));

	//m_camera_movement.y = d;
	//std::cout << m_camera_movement.y << std::endl;
	m_camera_position = m_camera_position - (m_camera_movement.y * dt) * right;
	m_camera_target_position = m_camera_target_position - (m_camera_movement.y * dt) * right;
	m_camera_movement.y = 0;
	float speed = glm::pi<float>() * 0.001; 
	//float speed = glm::pi<float>() * 0.01;
	/*std::cout << "camera angle dest" << std::endl;
	std::cout << m_camera_look_angle_destination.x << ", " << m_camera_look_angle_destination.y << std::endl;*/
 
	/*glm::mat4 rotation = glm::rotate(glm::mat4(1.f), glm::radians(-1.f) ,glm::vec3(-1 , 1, -1));

	direction = rotation * glm::vec4(direction,0.f);
	
	m_camera_target_position = m_camera_position + direction * glm::distance(m_camera_position, m_camera_target_position);*/
	// kwdikas gia to mouse
	/*glm::mat4 rotation = glm::rotate(glm::mat4(1.f), m_camera_look_angle_destination.y * speed, right);
	rotation *= glm::rotate(glm::mat4(1.f), m_camera_look_angle_destination.x * speed, m_camera_up_vector);
	m_camera_look_angle_destination = glm::vec2(0.f);

	direction = rotation * glm::vec4(direction, 0.f);
	direction = glm::rotate(glm::mat4(1.f), glm::radians(angle), glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(direction, 0.f);
	m_camera_target_position = m_camera_position + direction * glm::distance(m_camera_position, m_camera_target_position); //glm::distance(dfacing,m_camera_target_position);*/
	// telos kwdika gia to mouse


	m_view_matrix = glm::lookAt(m_camera_position, m_camera_target_position, m_camera_up_vector);

	//std::cout << m_camera_position.x << " " << m_camera_position.y << " " << m_camera_position.z << " " << std::endl;
	//std::cout << m_camera_target_position.x << " " << m_camera_target_position.y << " " << m_camera_target_position.z << " " << std::endl;
	//m_light.SetPosition(m_camera_position);
	//m_light.SetTarget(m_camera_target_position);
}

bool Renderer::ReloadShaders()
{
	m_geometry_program.ReloadProgram();
	m_post_program.ReloadProgram();
	m_deferred_program.ReloadProgram();
	m_spot_light_shadow_map_program.ReloadProgram();
	return true;
}

void Renderer::Render()
{
	RenderShadowMaps();
	RenderGeometry();
	RenderDeferredShading();
	RenderPostProcess();

	GLenum error = Tools::CheckGLError();

	if (error != GL_NO_ERROR)
	{
		printf("Reanderer:Draw GL Error\n");
		system("pause");
	}
}

void Renderer::RenderPostProcess()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.f, 0.8f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	m_post_program.Bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
	m_post_program.loadInt("uniform_texture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_light.GetShadowMapDepthTexture());
	m_post_program.loadInt("uniform_shadow_map", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_fbo_pos_texture);
	m_post_program.loadInt("uniform_tex_pos", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_fbo_normal_texture);
	m_post_program.loadInt("uniform_tex_normal", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_fbo_albedo_texture);
	m_post_program.loadInt("uniform_tex_albedo", 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_fbo_mask_texture);
	m_post_program.loadInt("uniform_tex_mask", 5);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	m_post_program.loadInt("uniform_tex_depth", 6);

	glBindVertexArray(m_vao_fbo);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_post_program.Unbind();
}

void Renderer::RenderStaticGeometry()
{
	glm::mat4 proj = m_projection_matrix * m_view_matrix * m_world_matrix;

	for (auto& node : this->m_nodes)
	{
		glBindVertexArray(node->m_vao);

		m_geometry_program.loadMat4("uniform_projection_matrix", proj * node->app_model_matrix);
		m_geometry_program.loadMat4("uniform_normal_matrix", glm::transpose(glm::inverse(m_world_matrix * node->app_model_matrix)));
		m_geometry_program.loadMat4("uniform_world_matrix", m_world_matrix * node->app_model_matrix);

		for (int j = 0; j < node->parts.size(); ++j)
		{
			m_geometry_program.loadVec3("uniform_diffuse", node->parts[j].diffuse);
			m_geometry_program.loadVec3("uniform_ambient", node->parts[j].ambient);
			m_geometry_program.loadVec3("uniform_specular", node->parts[j].specular);
			m_geometry_program.loadFloat("uniform_shininess", node->parts[j].shininess);
			m_geometry_program.loadInt("uniform_has_tex_diffuse", (node->parts[j].diffuse_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_emissive", (node->parts[j].emissive_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_mask", (node->parts[j].mask_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_normal", (node->parts[j].bump_textureID > 0 || node->parts[j].normal_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_is_tex_bumb", (node->parts[j].bump_textureID > 0) ? 1 : 0);

			glActiveTexture(GL_TEXTURE0);
			m_geometry_program.loadInt("uniform_tex_diffuse", 0);
			glBindTexture(GL_TEXTURE_2D, node->parts[j].diffuse_textureID);

			if (node->parts[j].mask_textureID > 0)
			{
				glActiveTexture(GL_TEXTURE1);
				m_geometry_program.loadInt("uniform_tex_mask", 1);
				glBindTexture(GL_TEXTURE_2D, node->parts[j].mask_textureID);
			}

			if ((node->parts[j].bump_textureID > 0 || node->parts[j].normal_textureID > 0))
			{
				glActiveTexture(GL_TEXTURE2);
				m_geometry_program.loadInt("uniform_tex_normal", 2);
				glBindTexture(GL_TEXTURE_2D, node->parts[j].bump_textureID > 0 ?
					node->parts[j].bump_textureID : node->parts[j].normal_textureID);
			}

			if (node->parts[j].emissive_textureID > 0)
			{
				glActiveTexture(GL_TEXTURE3);
				m_geometry_program.loadInt("uniform_tex_emissive", 3);
				glBindTexture(GL_TEXTURE_2D, node->parts[j].emissive_textureID);
			}

			glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);
		}

		glBindVertexArray(0);
	}
}

void Renderer::RenderCollidableGeometry()
{
	glm::mat4 proj = m_projection_matrix * m_view_matrix * m_world_matrix;

	//glm::vec4 dfacing4(dfacing,0.0);

	glm::vec3 camera_dir = dup; //normalize(dfacing - craft_position);

	glm::vec3 norm_craft_pos = normalize(craft_position); //normalize(craft_position); // ???? xtypaei apeytheias
	//std::cout << norm_craft_pos.x << " " << norm_craft_pos.y << " " << norm_craft_pos.z << std::endl;
	//std::cout << camera_dir.x << " " << camera_dir.y << " " << camera_dir.z << std::endl;
	//std::cout << dfacing.x << " " << dfacing.y << " " << dfacing.z << std::endl;


	for (auto& node : this->m_collidables_nodes)
	{
		//std::cout << node->node_name << std::endl;

		float_t isectT = 0.f;
		int32_t primID = -1;
		int32_t totalRenderedPrims = 0;

		
		//if (node->node_name == "col_ball_2") {
			/*if (node->intersectRay(norm_craft_pos, camera_dir, m_world_matrix, isectT, primID, 5.f, 4.f)) {
				hit = true;
				std::cout << "Hit" << std::endl;
				return;
				//ball2hit = true;
			}*/
		//}
		
			// col detection me craft position MONO.


		/*if (node->intersectRay(norm_craft_pos, camera_dir, m_world_matrix, isectT, primID,30.f,29.f)) {
			hit = true;
			return;
		};*/

		//node->intersectRay(norm_craft_pos, camera_dir, m_world_matrix, isectT, primID,40.f,0.f);

		hit = false;


		glBindVertexArray(node->m_vao);

		m_geometry_program.loadMat4("uniform_projection_matrix", proj * node->app_model_matrix);
		m_geometry_program.loadMat4("uniform_normal_matrix", glm::transpose(glm::inverse(m_world_matrix * node->app_model_matrix)));
		m_geometry_program.loadMat4("uniform_world_matrix", m_world_matrix * node->app_model_matrix);
		m_geometry_program.loadFloat("uniform_time", m_continous_time);

		for (int j = 0; j < node->parts.size(); ++j)
		{
			m_geometry_program.loadVec3("uniform_diffuse", node->parts[j].diffuse);
			m_geometry_program.loadVec3("uniform_ambient", node->parts[j].ambient);
			m_geometry_program.loadVec3("uniform_specular", node->parts[j].specular);
			m_geometry_program.loadFloat("uniform_shininess", node->parts[j].shininess);
			m_geometry_program.loadInt("uniform_has_tex_diffuse", (node->parts[j].diffuse_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_mask", (node->parts[j].mask_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_emissive", (node->parts[j].emissive_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_has_tex_normal", (node->parts[j].bump_textureID > 0 || node->parts[j].normal_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_is_tex_bumb", (node->parts[j].bump_textureID > 0) ? 1 : 0);
			m_geometry_program.loadInt("uniform_prim_id", primID - totalRenderedPrims);

			glActiveTexture(GL_TEXTURE0);
			m_geometry_program.loadInt("uniform_tex_diffuse", 0);
			glBindTexture(GL_TEXTURE_2D, node->parts[j].diffuse_textureID);

			if (node->parts[j].mask_textureID > 0)
			{
				glActiveTexture(GL_TEXTURE1);
				m_geometry_program.loadInt("uniform_tex_mask", 1);
				glBindTexture(GL_TEXTURE_2D, node->parts[j].mask_textureID);
			}

			if ((node->parts[j].bump_textureID > 0 || node->parts[j].normal_textureID > 0))
			{
				glActiveTexture(GL_TEXTURE2);
				m_geometry_program.loadInt("uniform_tex_normal", 2);
				glBindTexture(GL_TEXTURE_2D, node->parts[j].bump_textureID > 0 ?
					node->parts[j].bump_textureID : node->parts[j].normal_textureID);
			}

			if (node->parts[j].emissive_textureID > 0)
			{
				glActiveTexture(GL_TEXTURE3);
				m_geometry_program.loadInt("uniform_tex_emissive", 3);
				glBindTexture(GL_TEXTURE_2D, node->parts[j].emissive_textureID);
			}

			glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);
			totalRenderedPrims += node->parts[j].count;
		}

		glBindVertexArray(0);
	}
}

void Renderer::RenderDeferredShading()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);

	GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };

	glDrawBuffers(1, drawbuffers);

	glViewport(0, 0, m_screen_width, m_screen_height);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	
	glClear(GL_COLOR_BUFFER_BIT);

	m_deferred_program.Bind();

	m_deferred_program.loadVec3("uniform_light_color", m_light.GetColor());
	m_deferred_program.loadVec3("uniform_light_dir", m_light.GetDirection());
	m_deferred_program.loadVec3("uniform_light_pos", m_light.GetPosition());

	m_deferred_program.loadFloat("uniform_light_umbra", m_light.GetUmbra());
	m_deferred_program.loadFloat("uniform_light_penumbra", m_light.GetPenumbra());

	m_deferred_program.loadVec3("uniform_camera_pos", m_camera_position);
	m_deferred_program.loadVec3("uniform_camera_dir", normalize(m_camera_target_position - m_camera_position));

	m_deferred_program.loadMat4("uniform_light_projection_view", m_light.GetProjectionMatrix() * m_light.GetViewMatrix());
	m_deferred_program.loadInt("uniform_cast_shadows", m_light.GetCastShadowsStatus() ? 1 : 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_fbo_pos_texture);
	m_deferred_program.loadInt("uniform_tex_pos", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_fbo_normal_texture);
	m_deferred_program.loadInt("uniform_tex_normal", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_fbo_albedo_texture);
	m_deferred_program.loadInt("uniform_tex_albedo", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_fbo_mask_texture);
	m_deferred_program.loadInt("uniform_tex_mask", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_fbo_depth_texture);
	m_deferred_program.loadInt("uniform_tex_depth", 4);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, m_light.GetShadowMapDepthTexture());
	m_deferred_program.loadInt("uniform_shadow_map", 10);

	glBindVertexArray(m_vao_fbo);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	m_deferred_program.Unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDepthMask(GL_TRUE);
}

void Renderer::RenderGeometry()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_pos_texture, 0);

	GLenum drawbuffers[4] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3 };

	glDrawBuffers(4, drawbuffers);

	glViewport(0, 0, m_screen_width, m_screen_height);
	glClearColor(0.f, 0.8f, 1.f, 1.f);
	glClearDepth(1.f);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_geometry_program.Bind();
	RenderStaticGeometry();
	auto e = glGetError();
	RenderCollidableGeometry();

	m_geometry_program.Unbind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
}

void Renderer::RenderShadowMaps()
{
	if (m_light.GetCastShadowsStatus())
	{
		int m_depth_texture_resolution = m_light.GetShadowMapResolution();

		glBindFramebuffer(GL_FRAMEBUFFER, m_light.GetShadowMapFBO());
		glViewport(0, 0, m_depth_texture_resolution, m_depth_texture_resolution);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_DEPTH_BUFFER_BIT);

		// Bind the shadow mapping program
		m_spot_light_shadow_map_program.Bind();

		glm::mat4 proj = m_light.GetProjectionMatrix() * m_light.GetViewMatrix() * m_world_matrix;

		for (auto& node : this->m_nodes)
		{
			glBindVertexArray(node->m_vao);

			m_spot_light_shadow_map_program.loadMat4("uniform_projection_matrix", proj * node->app_model_matrix);

			for (int j = 0; j < node->parts.size(); ++j)
			{
				glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);
			}

			glBindVertexArray(0);
		}

		glm::vec3 camera_dir = normalize(m_camera_target_position - m_camera_position);
		float_t isectT = 0.f;
		int32_t primID;

		for (auto& node : this->m_collidables_nodes)
		{
			//if (node->intersectRay(m_camera_position, camera_dir, m_world_matrix, isectT, primID)) continue;
			node->intersectRay(m_camera_position, camera_dir, m_world_matrix, isectT, primID);

			glBindVertexArray(node->m_vao);

			m_spot_light_shadow_map_program.loadMat4("uniform_projection_matrix", proj * node->app_model_matrix);

			for (int j = 0; j < node->parts.size(); ++j)
			{
				glDrawArrays(GL_TRIANGLES, node->parts[j].start_offset, node->parts[j].count);
			}

			glBindVertexArray(0);
		}

		m_spot_light_shadow_map_program.Unbind();
		glDisable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void Renderer::CameraMoveForward(bool enable)
{
	m_camera_movement.x = (enable) ? 1 : 0;
}
void Renderer::CameraMoveBackWard(bool enable)
{
	m_camera_movement.x = (enable) ? -1 : 0;
}

void Renderer::CameraMoveLeft(bool enable)
{
	m_camera_movement.y = (enable) ? -1 : 0;
}
void Renderer::CameraMoveRight(bool enable)
{
	m_camera_movement.y = (enable) ? 1 : 0;
}

void Renderer::CameraLook(glm::vec2 lookDir)
{
	m_camera_look_angle_destination = lookDir;
}

void Renderer::resetLeft(bool flag) {

	float e = 0.01f; // helps with comparing floats

	// arxiko dianysma (0,0,-1) opote otan ftasoume sto (0.1,0,-1) tha exoume kanei plhrh counter clockwise peristrofh ouysiastika , ara reset
	if (/*LeftHelper == 0 && */(fabs(dfacing.x - 0.1) < e) && (fabs(dfacing.z - (-1.f)) < e)) {
		leftdir = 0.f;
		diagleftdir = 0.f;
		diagrightdir = 0.f;
		rightdir = 0.f;
		forwarddir = 0.f;
	}
	/*if (flag) {
		leftdir = 0.f;
		diagleftdir = 0.f;
		diagrightdir = 0.f;
		rightdir = 0.f;
		forwarddir = 0.f;
	}*/
}

void Renderer::resetRight(bool flag) {

	/*if (flag) {
		leftd = 0.f;
		diagleftd = 0.f;
		diagrightd = 0.f;
		rightd = 0.f;
		forwardd = 0.f;
	}*/

	float e = 0.01f; // helps with comparing floats

	// arxiko dianysma (0,0,-1) opote otan ftasoume sto (-0.1,0,-1) tha exoume kanei plhrh clockwise peristrofh ouysiastika , ara reset
	if (/*RightHelper == 0 &&*/ (fabs(dfacing.x - (-0.1)) < e) && (fabs(dfacing.z - (-1.f)) < e)) {
		leftd = 0.f;
		diagleftd = 0.f;
		diagrightd = 0.f;
		rightd = 0.f;
		forwardd = 0.f;
	}
}

void Renderer::CraftTurnLeft(bool enable) {

	LeftHelper	= (enable) ? 1 : 0;

	float e = 0.01f; // helps with comparing floats
	
	// counter clockwise movement implementation

	if (fabs(leftdir - (-1.f)) < e) { // if leftdir == -1 do this
		if (fabs(diagleftdir - 1.f) < e) { 
			if (fabs(diagrightdir - 1.f) < e) {
				rightdir += 0.1f;
				dfacing.z -= rightdir;
				dfacing.x -= 0.1f;
				//resetValues(leftdir);
				return; // mporoyme na kanoume kati wste na vgoyme pali apo to if? gia na ginetai ep aoriston cc movement?
			}
			diagrightdir += 0.1f;
			dfacing.x += diagrightdir;
			dfacing.z -= 0.1f;
			//resetValues(diagrightdir);
			return;
		}
		diagleftdir += 0.1f;
		dfacing.z += diagleftdir; // pame mexri dfacing.z = 1
		//resetValues(diagleftdir);
		return;
	}
	leftdir = leftdir - 0.1f;
	dfacing.x = leftdir; // pame mexri dfacing.x = -1
	dfacing.z = -1;	
	//resetValues(leftdir);

	

}

void Renderer::CraftTurnRight(bool enable) {

	// clockwise movement implementation

	RightHelper = (enable) ? 1 : 0;
	float e = 0.01f; // helps with comparing floats

	

	if (fabs(rightd - 1.0f) < e) {
		if (fabs(diagrightd - 1.0f) < e) {
			if (fabs(diagleftd - 1.0f) < e) {
				if (fabs(leftd - (-1.0f)) < e) {
					forwardd = forwardd - 0.1f;
					dfacing.z += forwardd; // -1 
					dfacing.x += 0.1f; // 0 eytheia
					//resetValues(rightd);
					return;
				}
				leftd = leftd - 0.1f;
				dfacing.x += leftd; // -1
				dfacing.z -= 0.1f; // 0 aristera
				//resetValues(leftd);
				return;
			}
			diagleftd += 0.1f;
			dfacing.z += diagleftd; // 1
			dfacing.x -= 0.1f; // 0 opisthen
			//resetValues(diagleftd);
			return;
		}
		diagrightd += 0.1f;
		dfacing.x += diagrightd; // 1 dexia
		//resetValues(diagrightd);
		//resetValues(true);
		return;
	}
	rightd = rightd + 0.1f;
	dfacing.x = rightd;
	dfacing.z = -1;

}

void Renderer::CraftTurnUp(bool enable) {
	dfacing.y = (enable) ? 1.f : 0.f;
}

void Renderer::CraftTurnDown(bool enable) {
	dfacing.y = (enable) ? -1.f : 0.f;
}