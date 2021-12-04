#include "animation_system.h"
#include "..\ragebot\aim.h"
#include "..\ragebot\aim.h"

int side;

void resolver::initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch)
{
	player = e;
	player_record = record;

	original_goal_feet_yaw = math::normalize_yaw(goal_feet_yaw);
	original_pitch = math::normalize_pitch(pitch);
}

void resolver::reset()
{
	player = nullptr;
	player_record = nullptr;

	side = false;
	fake = false;

	was_first_bruteforce = false;
	was_second_bruteforce = false;

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
}

void resolver::resolve_yaw()
{
	auto animstate = player->get_animation_state();
	float lby_delta = math::normalize_yaw(player->m_angEyeAngles().y - animstate->m_flGoalFeetYaw);
	float delta = lby_delta;

	player_info_t e_info;

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &e_info))
		return;

	if (!animstate)
		return;

	if (player->m_angEyeAngles().x == -540.f || player->m_angEyeAngles().x == -1620.f)
		player->m_angEyeAngles().x = 88.8;

float yawstate = animstate->m_flGoalFeetYaw;
float yaw = player->m_angEyeAngles().y - yawstate;
if (yaw > 0.f)
side = 1;
else
side = -1;
	
	/*AnimationLayer right_layers[13];
	AnimationLayer left_layers[13];
	AnimationLayer center_layers[13];

	float speed = player_record->velocity.Length2D();
	float right = delta >= player->get_max_desync_delta();
	float left = delta <= player->get_max_desync_delta();

	if (speed > 0.15f && player_record->layers[3].m_flCycle > 0.9f && player_record->layers[3].m_flWeight > 0.1f)
	{
		float d1 = fabsf(player_record->layers[6].m_flPlaybackRate - left_layers[3].m_flPlaybackRate);
		float d2 = fabsf(player_record->layers[6].m_flPlaybackRate - right_layers[3].m_flPlaybackRate);

		delta = player->get_max_desync_delta();

		if (d1 > d2 && d1 < d2)
			side = 1;
		else if (d1 < d2 && d1 > d2)
			side = -1;
	}
	else if (speed > 1.0f && (player_record->layers[6].m_flWeight * 1000.f) == (center_layers[6].m_flWeight * 1000.f))
		delta <= player->get_max_desync_delta() / 2;
	else if (!(player_record->layers[12].m_flWeight * 1000.f) && (player_record->layers[6].m_flWeight * 1000.f) == (center_layers[6].m_flWeight * 1000.f))
	{
		float d1 = fabsf(player_record->layers[6].m_flPlaybackRate - center_layers[6].m_flPlaybackRate);
		float d2 = fabsf(player_record->layers[6].m_flPlaybackRate - left_layers[6].m_flPlaybackRate);
		float d3 = fabsf(player_record->layers[6].m_flPlaybackRate - right_layers[6].m_flPlaybackRate);

		if ((d1 - d2) <= d3)
		{
			if (-d3 <= (d1 - d2))
			{
				side = 1;
delta = left;
			}
			else
			{
				side = -1;
delta = right;
		         }
		}
	}
	else
	{
		if (lby_delta < 0.f)
			delta = right;
		else if (lby_delta > 0.f)
			delta = left;
	}*/

	if (side == 1)
		animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + player->get_max_desync_delta());
	else if (side == -1)
		animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - player->get_max_desync_delta());
}

float resolver::resolve_pitch()
{
	if
		(fabs(original_pitch) > 87.0f)
		fake = true;
	else if (!fake)
	{
		player_record->side = RESOLVER_ORIGINAL;
		return original_pitch;
	}

	return original_pitch;
}
