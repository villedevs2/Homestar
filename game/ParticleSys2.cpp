#include "ParticleSys2.h"

ParticleSys2::ParticleSys2(int num, TextureSet* texset)
{
	m_texset = texset;

	m_particle = new Particle[num];

	m_num_active = 0;
	m_num_max = num;

	// set default emitter
	m_emitter.position = glm::vec2(0.0f, 0.0f);
	m_emitter.angle = 90.0f;
	m_emitter.angle_range = 0.0f;
	m_emitter.time = 5000;
	m_emitter.time_range = 0;
	m_emitter.width = 1.0f;
	m_emitter.height = 1.0f;
	m_emitter.particle_speed = 1.0f;
	m_emitter.particle_speed_range = 0.0f;


	m_gravity = glm::vec2(0.0f, 10.0f);
	m_grav_enable = false;

	m_particle_size = 1.0f;
}

ParticleSys2::~ParticleSys2()
{
	delete [] m_particle;
}

void ParticleSys2::setEmitter(const ParticleSys2::Emitter& emitter)
{
	m_emitter.position = emitter.position;
	m_emitter.angle = emitter.angle;
	m_emitter.angle_range = emitter.angle_range;
	m_emitter.time = emitter.time;
	m_emitter.time_range = emitter.time_range;
	m_emitter.width = emitter.width;
	m_emitter.height = emitter.height;
	m_emitter.particle_speed = emitter.particle_speed;
	m_emitter.particle_speed_range = emitter.particle_speed_range;
}

void ParticleSys2::setTexture(TextureSet* texset)
{
	m_texset = texset;
}

void ParticleSys2::setGravity(glm::vec2 gravity)
{
	m_gravity = gravity;
}

void ParticleSys2::setGravity(float gx, float gy)
{
	m_gravity.x = gx;
	m_gravity.y = gy;
}

void ParticleSys2::enableGravity(bool enable)
{
	m_grav_enable = enable;
}

void ParticleSys2::setParticleSize(float size)
{
	m_particle_size = size;
}

void ParticleSys2::emit(int num)
{
	// no space, just return
	if (m_num_active >= m_num_max)
		return;

	// if we would overflow, cap to max
	if (m_num_active + num > m_num_max)
	{
		num = m_num_max - m_num_active;
	}

	for (int i=0; i < num; i++)
	{
		int index = i + m_num_active;

		// make random speed for this particle
		float speed_variation = (((float)(rand() % 2000) / 1000.0f) * m_emitter.particle_speed_range) - (m_emitter.particle_speed_range * 0.5f);
		float speed = m_emitter.particle_speed + speed_variation;

		// make life time for this particle (base_time +- (time_range / 2))
		int time;
		if (m_emitter.time_range > 0)
		{
			time = m_emitter.time + ((rand() % m_emitter.time_range) - (m_emitter.time_range / 2));
		}
		else
		{
			time = m_emitter.time;
		}

		// make random texture for this particle
		int texture = (rand() % m_texset->getNumTextures());
		const Texture* tex = m_texset->getTexture(texture);

		// make direction with minor angle variation for this particle
		float angvar = (((float)(rand() % 1000) / 1000.0f) * m_emitter.angle_range) - (m_emitter.angle_range * 0.5f);
		float angrad = (m_emitter.angle + angvar) * M_PI / 180.0f;

		glm::vec2 dir = glm::vec2(cos(angrad), sin(angrad));

		dir *= speed;

		float px = 0.0f;
		float py = 0.0f;

		if (m_emitter.width > 0.0f)
		{
			px = (((rand() % 1000) / 1000.0f) * m_emitter.width) - (m_emitter.width * 0.5f);
		}
		if (m_emitter.height > 0.0f)
		{
			py = (((rand() % 1000) / 1000.0f) * m_emitter.height) - (m_emitter.height * 0.5f);
		}

		glm::vec2 pos = m_emitter.position + glm::vec2(px, py);

		float tbx = tex->m_left;
		float tby = tex->m_top;
		float tbw = tex->m_right - tex->m_left;
		float tbh = tex->m_bottom - tex->m_top;
		
		m_particle[index].timer = time * 1000000;
		m_particle[index].pos = pos;						// TODO: randomness, other shape emitters (disc, box, plane, etc.)
		m_particle[index].dir = dir;						// TODO: randomness in direction/speed
		m_particle[index].texture = glm::vec4(tbx, tby, tbw, tbh);
	}

	m_num_active += num;
}

void ParticleSys2::emitArea(const glm::vec2* p, int num_points, int num_particles)
{
	// no space, just return
	if (m_num_active >= m_num_max)
		return;

	// if we would overflow, cap to max
	if (m_num_active + num_particles > m_num_max)
	{
		num_particles = m_num_max - m_num_active;
	}

	assert(num_points > 2);

	// divide num of particles evenly into triangles
	int num_tris = num_points - 2;
	int parts_per_tri = num_particles / num_tris;


	for (int t=0; t < num_tris; t++)
	{
		glm::vec2 v1 = p[0];
		glm::vec2 v2 = p[(t+2)-1];
		glm::vec2 v3 = p[(t+2)];

		for (int i=0; i < parts_per_tri; i++)
		{
			int index = i + m_num_active;

			// make random speed for this particle
			float speed_variation = (((float)(rand() % 2000) / 1000.0f) * m_emitter.particle_speed_range) - (m_emitter.particle_speed_range * 0.5f);
			float speed = m_emitter.particle_speed + speed_variation;

			// make life time for this particle (base_time +- (time_range / 2))
			int time;
			if (m_emitter.time_range > 0)
			{
				time = m_emitter.time + ((rand() % m_emitter.time_range) - (m_emitter.time_range / 2));
			}
			else
			{
				time = m_emitter.time;
			}

			// make random texture for this particle
			int texture = (rand() % m_texset->getNumTextures());
			const Texture* tex = m_texset->getTexture(texture);

			// make direction with minor angle variation for this particle
			float angvar = (((float)(rand() % 1000) / 1000.0f) * m_emitter.angle_range) - (m_emitter.angle_range * 0.5f);
			float angrad = (m_emitter.angle + angvar) * M_PI / 180.0f;

			glm::vec2 dir = glm::vec2(cos(angrad), sin(angrad));

			dir *= speed;

			float px = 0.0f;
			float py = 0.0f;

			// interpolate random point inside triangle
			float randomu = (float)(rand() % 1000) / 1000.0f;
			float randomv = (float)(rand() % 1000) / 1000.0f;

			glm::vec2 itp1 = v2 + ((v3 - v2) * randomv);
			glm::vec2 itp2 = v1 + ((itp1 - v1) * randomu);

			glm::vec2 pos = m_emitter.position + itp2;

			float tbx = tex->m_left;
			float tby = tex->m_top;
			float tbw = tex->m_right - tex->m_left;
			float tbh = tex->m_bottom - tex->m_top;
		
			m_particle[index].timer = time * 1000000;
			m_particle[index].pos = pos;
			m_particle[index].dir = dir;
			m_particle[index].texture = glm::vec4(tbx, tby, tbw, tbh);
		}

		m_num_active += parts_per_tri;
	}

}

void ParticleSys2::copy(int from, int to)
{
	m_particle[to].pos = m_particle[from].pos;
	m_particle[to].dir = m_particle[from].dir;
	m_particle[to].texture = m_particle[from].texture;
	m_particle[to].timer = m_particle[from].timer;
}

int ParticleSys2::getNumActive()
{
	return m_num_active;
}

void ParticleSys2::update(int time)
{
	float dt = (float)(time) / 1000000000.0f;

	int i = 0;
	while (i < m_num_active)
	{
		// apply gravity
		if (m_grav_enable)
			m_particle[i].dir += (m_gravity * dt);

		// update particle position
		m_particle[i].pos += (m_particle[i].dir * dt);

		// update timer
		m_particle[i].timer -= time;
		
		if (m_particle[i].timer <= 0)		// is the particle about to die?
		{
			// swap to the end
			if (i < m_num_active - 1)		// if we are already at end, just discard it
			{
				copy(m_num_active-1, i);
				// the current location got replaced, so we need to process it again (so don't increment i here)
			}
			else
			{
				// just proceed since we didn't copy anything
				i++;
			}
			m_num_active--;
		}
		else
		{
			// just proceed to next one
			i++;
		}
	};
}

void ParticleSys2::render(const Shaders::ParticleShaderContext* context)
{
	if (m_num_active > 0)
	{
		m_texset->apply(0);

		glUniform1f(context->part_size, m_particle_size);

		glVertexAttribPointer(context->position, 2, GL_FLOAT, false, sizeof(Particle), (char*)(m_particle) + offsetof(Particle, pos));
		glEnableVertexAttribArray(context->position);
//		glVertexAttribPointer(context->direction, 2, GL_FLOAT, false, sizeof(Particle), (char*)(m_particle) + offsetof(Particle, dir));
//		glEnableVertexAttribArray(context->direction);
		glVertexAttribPointer(context->texture, 4, GL_FLOAT, false, sizeof(Particle), (char*)(m_particle) + offsetof(Particle, texture));
		glEnableVertexAttribArray(context->texture);

		glDrawArrays(GL_POINTS, 0, m_num_active);

		glDisableVertexAttribArray(context->position);
//		glDisableVertexAttribArray(context->direction);
		glDisableVertexAttribArray(context->texture);
	}
}