#ifndef BIM_ENGINE_RENDERER_H
#define BIM_ENGINE_RENDERER_H

#include "GLEW\glew.h"
#include "glm\glm.hpp"
#include <vector>
#include "ShaderProgram.h"
#include "GeometryNode.h"
#include "CollidableNode.h"
#include "LightNode.h"

class Renderer
{
public:
	// Empty

protected:
	int												m_screen_width, m_screen_height;
	glm::mat4										m_world_matrix;
	glm::mat4										m_view_matrix;
	glm::mat4										m_projection_matrix;
	glm::vec3										m_camera_position;
	glm::vec3										m_camera_target_position;
	glm::vec3										m_camera_up_vector;
	glm::vec2										m_camera_movement;
	glm::vec2										m_camera_look_angle_destination;
	
	float m_continous_time;
	float u = 0.f;
	float v = 0.f; // variables used for turning the spacecraft

	


	// variables used for changing the direction of the aircraft when it's turning left or right

	int LeftHelper = 0; 
	int RightHelper = 0;

	// variables used for clockwise and counter clockwise spacecraft movement

	float leftdir = 0.f; 
	float diagleftdir = 0.f;
	float diagrightdir = 0.f;
	float rightdir = 0.f;
	float forwarddir = 0.f;

	// na dokimasw diaforetikous metrhtes px gia right kai left
	float leftd = 0.f;
	float diagleftd = 0.f;
	float diagrightd = 0.f;
	float rightd = 0.f;
	float forwardd = 0.f;

	// helper variable that helps with the left and right transformations of the spacecraft
	float angle = 0.f;
	float turnHelper = -1.f;



	// Protected Functions
	bool InitShaders();
	bool InitGeometricMeshes();
	bool InitCommonItems();
	bool InitLights();
	bool InitIntermediateBuffers();
	void BuildWorld();
	void InitCraft();
	void InitCamera();
	void RenderGeometry();
	void RenderDeferredShading();
	void RenderStaticGeometry();
	void RenderCollidableGeometry();
	void RenderShadowMaps();
	void RenderPostProcess();

	enum OBJECTS
	{
		/*CRAFT_1 = 0,
		CRAFT_2 = 0,*/
		CRAFT = 0,
		COLLISION_HULL,
		TERRAIN = 1,
		BALL = 2,
		BALL2 = 3,
		BALL3 = 4,
		BALL4 = 5,
		BALL5 = 6,
		SIZE_ALL
	};

	std::vector<GeometryNode*> m_nodes;
	std::vector<CollidableNode*> m_collidables_nodes;

	LightNode									m_light;
	ShaderProgram								m_geometry_program;
	ShaderProgram								m_deferred_program;
	ShaderProgram								m_post_program;
	ShaderProgram								m_spot_light_shadow_map_program;

	// Second light source variable and shader programs
	LightNode									secondLightSource;

	GLuint m_fbo;
	GLuint m_vao_fbo;
	GLuint m_vbo_fbo_vertices;

	GLuint m_fbo_texture;

	GLuint m_fbo_depth_texture;
	GLuint m_fbo_pos_texture;
	GLuint m_fbo_normal_texture;
	GLuint m_fbo_albedo_texture;
	GLuint m_fbo_mask_texture;

public:

	glm::vec3										dfacing; // vectors used for spacecraft movement
	glm::vec3										dup;
	glm::vec3										dright; // public because dfacing must be used in main.cpp
	glm::vec3										craft_movement;
	glm::vec3										craft_position;
	glm::vec3										craft_target_position;

	bool											hit; // global variable that detects if the spacecraft hit the surface 

	// global variables used for removing the balls from the terrain
	bool ball1hit = false;
	bool ball2hit = false;
	bool ball3hit = false;
	bool ball4hit = false;
	bool ball5hit = false;

	// global int variables used for counting the score and ending the game.
	int score1 = 0; 
	int score2 = 0;
	int score3 = 0;
	int score4 = 0;
	int score5 = 0;

	Renderer();
	~Renderer();
	bool										Init(int SCREEN_WIDTH, int SCREEN_HEIGHT);
	void										Update(float dt);
	bool										ResizeBuffers(int SCREEN_WIDTH, int SCREEN_HEIGHT);
	void										UpdateGeometry(float dt);
	void										UpdateCamera(float dt);
	void										UpdateCraft(float dt); // function that helps with the movement of the spacecraft.
	bool										ReloadShaders();
	void										Render();
	void										resetLeft(bool flag);
	void										resetRight(bool flag);
	void										UpdateAngle(bool left); // updates angle of spacecraft , used for permanent transformation after turn
	void										UpdateDirection(int direction);
	void										setTurnHelper(float f);
	void										UpdateBall();			// updates the balls in the terrain. Once the player hits a ball , it "disappears".
	void										RockCollision();		// function that detects collisions with rocks inside the terrain.

	void										CameraMoveForward(bool enable);
	void										CameraMoveBackWard(bool enable);
	void										CameraMoveLeft(bool enable);
	void										CameraMoveRight(bool enable);
	void										CameraLook(glm::vec2 lookDir);

	void										CraftTurnLeft(bool enable); // functions for craft movement by the player.
	void										CraftTurnRight(bool enable);
	void										CraftTurnUp(bool enable);
	void										CraftTurnDown(bool enable);

	void										printMessage(int x, int y, float r, float g, float b, int font, char* string); // function that outputs a message to the screen.
};

#endif
