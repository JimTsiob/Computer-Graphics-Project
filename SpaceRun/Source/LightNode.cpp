#include "LightNode.h"
#include "glm\gtc\matrix_transform.hpp"
#include "Tools.h"

// Spot Light
LightNode::LightNode()
{
	m_name = "defaultSpotLight1";

	m_light_direction = glm::normalize(glm::vec3(-1, -1, 0));
	m_light_position = glm::vec3(5, 3, 0);
	m_light_color = glm::vec3(1.0f);
	SetPosition(m_light_position);
	SetConeSize(60, 60);

	m_cast_shadow = false;
	m_shadow_map_resolution = 1024;
	m_shadow_map_bias = 0.001;
	m_shadow_map_texture = 0;
	m_shadow_map_fbo = 0;
}

LightNode::~LightNode()
{
	glDeleteFramebuffers(1, &m_shadow_map_fbo);
	glDeleteTextures(1, &m_shadow_map_texture);
}

void LightNode::CastShadow(bool cast)
{
	m_cast_shadow = cast;

	if (cast)
	{
		if (m_shadow_map_texture == 0)
			glGenTextures(1, &m_shadow_map_texture);
		// Depth buffer
		glBindTexture(GL_TEXTURE_2D, m_shadow_map_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_shadow_map_resolution, m_shadow_map_resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_2D, 0);

		if (m_shadow_map_fbo == 0)
			glGenFramebuffers(1, &m_shadow_map_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_shadow_map_fbo);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadow_map_texture, 0);
		glDrawBuffer(GL_NONE);
		//glReadBuffer(GL_NONE);

		GLenum status = Tools::CheckFramebufferStatus(m_shadow_map_fbo);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			printf("Error in Spotlight shadow FB generation.\n");
			return;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}


void LightNode::SetColor(const glm::vec3 & color)
{
	m_light_color = color;
}

void LightNode::SetPosition(const glm::vec3 & pos)
{
	m_light_position = pos;
	m_light_direction = glm::normalize(m_light_target - m_light_position);
	m_view_matrix = glm::lookAt(m_light_position, m_light_target, glm::vec3(0, 1, 0));
	m_view_inverse_matrix = glm::inverse(m_view_matrix);
}

void LightNode::SetTarget(const glm::vec3 & target)
{
	m_light_target = target;
	m_light_direction = glm::normalize(m_light_target - m_light_position);
	m_view_matrix = glm::lookAt(m_light_position, m_light_target, glm::vec3(0, 1, 0));
	m_view_inverse_matrix = glm::inverse(m_view_matrix);
}

void LightNode::SetConeSize(float umbra, float penumbra)
{
	m_umbra = umbra;
	m_penumbra = penumbra;

	float near_clipping_range = 0.1f;
	float far_clipping_range = 30.f;
	
	float h = near_clipping_range * glm::tan(glm::radians(m_penumbra * 0.5f));
	m_projection_matrix = glm::frustum(-h, h, -h, h, near_clipping_range, far_clipping_range);
	m_projection_inverse_matrix = glm::inverse(m_projection_matrix);
}

glm::vec3 LightNode::GetPosition()
{
	return m_light_position;
}

glm::vec3 LightNode::GetDirection()
{
	return m_light_direction;
}

glm::vec3 LightNode::GetColor()
{
	return m_light_color;
}

float LightNode::GetUmbra()
{
	return m_umbra;
}

float LightNode::GetPenumbra()
{
	return m_penumbra;
}

bool LightNode::GetCastShadowsStatus()
{
	return m_cast_shadow;
}

GLuint LightNode::GetShadowMapFBO()
{
	return m_shadow_map_fbo;
}

GLuint LightNode::GetShadowMapDepthTexture()
{
	return m_shadow_map_texture;
}

int LightNode::GetShadowMapResolution()
{
	return m_shadow_map_resolution;;
}

glm::mat4 LightNode::GetProjectionMatrix()
{
	return m_projection_matrix;
}

glm::mat4 LightNode::GetViewMatrix()
{
	return m_view_matrix;
}