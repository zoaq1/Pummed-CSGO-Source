#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"

enum
{
	MAIN,
	NONE,
	FIRST,
	SECOND
};

enum e_anim_layer {
	ANIMATION_LAYER_AIMMATRIX,
	ANIMATION_LAYER_WEAPON_ACTION,
	ANIMATION_LAYER_WEAPON_ACTION_RECROUCH,
	ANIMATION_LAYER_ADJUST,
	ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL,
	ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB,
	ANIMATION_LAYER_MOVEMENT_MOVE,
	ANIMATION_LAYER_MOVEMENT_STRAFECHANGE,
	ANIMATION_LAYER_WHOLE_BODY,
	ANIMATION_LAYER_FLASHED,
	ANIMATION_LAYER_FLINCH,
	ANIMATION_LAYER_ALIVELOOP,
	ANIMATION_LAYER_LEAN
};

enum resolver_side
{
	RESOLVER_ORIGINAL,
	RESOLVER_ZERO,
	SIDE_R,
	SIDE_L,
	SIDE_N,
	RESOLVER_FIRST,
	RESOLVER_SECOND,
	RESOLVER_LOW_FIRST,
	RESOLVER_LOW_SECOND,
	RESOLVER_MIDH_SECOND,
	RESOLVER_MIDH_FIRST,///////igN//////
	RESOLVER_MID_FIRST,
	RESOLVER_MID_SECOND,
	RESOLVER_HIGHT_FIRST,
	RESOLVER_HIGHT_SECOND,
	RESOLVER_THIRD_FISRT,
	RESOLVER_THIRD_SECOND,
	RESOLVER_FOURTH_FIRST,
	RESOLVER_FOURTH_SECOND,
	RESOLVER_LDELTA_FIRST,
	RESOLVER_LDELTA_SECOND,
	RESOLVER_BDELTA_FIRST,
	RESOLVER_BDELTA_SECOND,
	RESOLVER_FIFTH_FIRST,
	RESOLVER_FIFTH_SECOND,
	RESOLVER_HHIGH_FIRST,
	RESOLVER_HHIGH_SECOND,
	RESOLVER_LDELTA1,
	RESOLVER_LDELTA2
};

enum resolver_history
{
	HISTORY_UNKNOWN = -1,
	HISTORY_ORIGINAL,
	HISTORY_ZERO,
	HISTORY_DEFAULT,
	HISTORY_LOW
};

enum resolver_type
{
	ORIGINAL,
	BRUTEFORCE,
	LBY,
	LAYERS,
	TRACE,
	DIRECTIONAL,
	NON_RESOLVED
};

struct player_settings
{
	__int64 id;
	resolver_history res_type;
	bool faking;
	int neg;
	int pos;

	player_settings(__int64 id, resolver_history res_type, bool faking, int left, int right) noexcept : id(id), res_type(res_type), faking(faking), neg(neg), pos(pos)
	{

	}
};



struct matrixes
{
	matrix3x4_t main[MAXSTUDIOBONES];
	matrix3x4_t zero[MAXSTUDIOBONES];
	matrix3x4_t first[MAXSTUDIOBONES];
	matrix3x4_t second[MAXSTUDIOBONES];
};

class adjust_data;

class resolver
{
	player_t* player = nullptr;
	adjust_data* player_record = nullptr;

	bool side = false;
	bool fake = false;
	bool was_first_bruteforce = false;
	bool was_second_bruteforce = false;
	bool was_first_low_bruteforce = false;
	bool was_second_low_bruteforce = false;
	bool  resolve_one = false;
	bool resolve_two = false;
	bool is_player_faking = false;
	float lock_side = 0.0f;
	float original_goal_feet_yaw = 0.0f;
	float original_pitch = 0.0f;
	int FreestandSide[64];
	
public:
	void initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch);
	void reset();
	void resolve_yaw();
	float resolve_pitch();

	resolver_side last_side = RESOLVER_ORIGINAL;
};


class adjust_data //-V730
{
public:
	player_t* player;
	int i;

	AnimationLayer layers[13];
	AnimationLayer side_layers[3][13];
	AnimationLayer previous_layers[13];
	matrixes matrixes_data;
	AnimationLayer resolver_anim_layer[3][13];
	std::string log_anim = "0";
	resolver_type type;
	resolver_side side;
	int m_missed_shots;
	//resolver_type type;
	//resolver_side side;
	int animations_index = -1;
	int reverse_index = -1;
	int anims_pre_resolving = -1;
	int anim_time = 0;
	bool did_anims_update = false;
	bool animations_updated = false;
	bool invalid;
	bool immune;
	bool dormant;
	bool bot;
	bool shot;
	float last_shot_time;
	int flags;
	int bone_count;
	int index;
	Vector m_angEyeAngles;
	Vector eye_angles;
	bool shot_this_tick = false;
	int entity_flags = 0;
	float simulation_time;
	float duck_amount;
	float lby;
	float m_flSimulationTime;
	float m_flInterpTime;
	float m_flDuck;
	float m_flLowerBodyYawTarget;
	float m_flLastShotTime;
	float m_flSpawnTime;
	int m_iChoked;
	int m_fFlags;
	bool m_extending;
	float left_side;
	float right_side;
	float m_pPoses[24];
	float last_lby_update_time;

	Vector m_vecVelocity;
	Vector m_vecOrigin;
	Vector m_vecAbsOrigin;
	//Vector m_angEyeAngles;
	Vector m_vecMins;
	Vector m_vecMaxs;
	Vector m_angLastReliableAngle;
	//Vector m_angEyeAngles;
	Vector m_angAbsAngles;
	Vector angles;
	c_baseplayeranimationstate frame_state;
	matrix3x4_t leftmatrixes[256] = {};
	matrix3x4_t rightmatrixes[256] = {};
	std::array<float, 24> left_poses = {};
	std::array<float, 24> right_poses = {};
	Vector abs_angles;
	Vector velocity;
	Vector origin;
	Vector mins;
	Vector maxs;

	adjust_data() //-V730
	{
		reset();
	}

	void reset()
	{
		player = nullptr;
		i = -1;

		//type = ORIGINAL;
		side = RESOLVER_ORIGINAL;

		invalid = false;
		immune = false;
		dormant = false;
		bot = false;

		flags = 0;
		bone_count = 0;

		simulation_time = 0.0f;
		duck_amount = 0.0f;
		lby = 0.0f;
		last_shot_time = 0.0f;
		angles.Zero();
		abs_angles.Zero();
		velocity.Zero();
		origin.Zero();
		mins.Zero();
		maxs.Zero();
	}

	adjust_data(player_t* e, bool store = true)
	{
		//type = ORIGINAL;
		side = RESOLVER_ORIGINAL;

		invalid = false;
		store_data(e, store);
	}
	//last_shot_time = player->m_hActiveWeapon() ? player->m_hActiveWeapon()->m_fLastShotTime() : 0.f;
	void store_data(player_t* e, bool store = true)
	{
		if (!e->is_alive())
			return;

		player = e;
		i = player->EntIndex();

		if (store)
		{
			memcpy(layers, e->get_animlayers(), e->animlayer_count() * sizeof(AnimationLayer));
			memcpy(matrixes_data.main, player->m_CachedBoneData().Base(), player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));
		}

		immune = player->m_bGunGameImmunity() || player->m_fFlags() & FL_FROZEN;
		dormant = player->IsDormant();

#if RELEASE
		player_info_t player_info;
		m_engine()->GetPlayerInfo(i, &player_info);

		bot = player_info.fakeplayer;
#else
		bot = false;
#endif

		flags = player->m_fFlags();
		bone_count = player->m_CachedBoneData().Count();

		simulation_time = player->m_flSimulationTime();
		duck_amount = player->m_flDuckAmount();
		lby = player->m_flLowerBodyYawTarget();

		angles = player->m_angEyeAngles();
		abs_angles = player->GetAbsAngles();
		velocity = player->m_vecVelocity();
		origin = player->m_vecOrigin();
		mins = player->GetCollideable()->OBBMins();
		maxs = player->GetCollideable()->OBBMaxs();
	}

	void adjust_player()
	{
		if (!valid(false))
			return;

		memcpy(player->get_animlayers(), layers, player->animlayer_count() * sizeof(AnimationLayer));
		memcpy(player->m_CachedBoneData().Base(), matrixes_data.main, player->m_CachedBoneData().Count() * sizeof(matrix3x4_t)); //-V807

		player->m_fFlags() = flags;
		player->m_CachedBoneData().m_Size = bone_count;

		player->m_flSimulationTime() = simulation_time;
		player->m_flDuckAmount() = duck_amount;
		player->m_flLowerBodyYawTarget() = lby;

		player->m_angEyeAngles() = angles;
		player->set_abs_angles(abs_angles);
		player->m_vecVelocity() = velocity;
		player->m_vecOrigin() = origin;
		player->set_abs_origin(origin);
		player->GetCollideable()->OBBMins() = mins;
		player->GetCollideable()->OBBMaxs() = maxs;
	}

	void increment_layer_cycle(int layer, bool is_looping)
	{
		AnimationLayer pLayer = layers[layer];
		if (fabs(pLayer.m_flPlaybackRate) <= 0.0f)
			return;

		float newcycle = (pLayer.m_flPlaybackRate * player->get_animation_state()->m_flUpdateTimeDelta) + pLayer.m_flCycle;

		if (!is_looping && newcycle >= 1.0f)
			newcycle = 0.999f;

		newcycle -= (float)(int)newcycle; //round to integer

		if (newcycle < 0.0f)
			newcycle += 1.0f;

		if (newcycle > 1.0f)
			newcycle -= 1.0f;

		pLayer.m_flCycle = newcycle;
	}

	void set_layer_weight(int layer, float weight)
	{
		AnimationLayer pLayer = layers[layer];

		weight = std::clamp(weight, 0.0f, 1.0f);

		pLayer.m_flWeight = weight;
	}

	void set_layers_wight_delta_rate(int layer, float oldweight)
	{
		if (player->get_animation_state()->m_flUpdateTimeDelta != 0.0f)
		{
			AnimationLayer pLayer = layers[layer];
			float weightdeltarate = (pLayer.m_flWeight - oldweight) / player->get_animation_state()->m_flUpdateTimeDelta;
			if (pLayer.m_flWeightDeltaRate != weightdeltarate)
				pLayer.m_flWeightDeltaRate = weightdeltarate;
		}
	}

	bool sequence_unfinished(int layer)
	{
		AnimationLayer pLayer = layers[layer];
		return (player->get_animation_state()->m_flUpdateTimeDelta * pLayer.m_flPlaybackRate) + pLayer.m_flCycle >= 1.0f;
		return false;
	}

	bool valid(bool extra_checks = true)
	{
		if (!this) //-V704
			return false;

		if (i > 0)
			player = (player_t*)m_entitylist()->GetClientEntity(i);

		if (!player)
			return false;

		if (player->m_lifeState() != LIFE_ALIVE)
			return false;

		if (immune)
			return false;

		if (dormant)
			return false;

		if (!extra_checks)
			return true;

		if (invalid)
			return false;

		auto net_channel_info = m_engine()->GetNetChannelInfo();

		if (!net_channel_info)
			return false;

		static auto sv_maxunlag = m_cvar()->FindVar(crypt_str("sv_maxunlag"));

		auto outgoing = net_channel_info->GetLatency(FLOW_OUTGOING);
		auto incoming = net_channel_info->GetLatency(FLOW_INCOMING);

		auto correct = math::clamp(outgoing + incoming + util::get_interpolation(), 0.0f, sv_maxunlag->GetFloat());

		auto curtime = g_ctx.local()->is_alive() ? TICKS_TO_TIME(g_ctx.globals.fixed_tickbase) : m_globals()->m_curtime; //-V807
		auto delta_time = correct - (curtime - simulation_time);

		if (fabs(delta_time) > 0.2f)
			return false;

		auto extra_choke = 0;

		if (g_ctx.globals.fakeducking)
			extra_choke = 14 - m_clientstate()->iChokedCommands;

		auto server_tickcount = extra_choke + m_globals()->m_tickcount + TIME_TO_TICKS(outgoing + incoming);
		auto dead_time = (int)(TICKS_TO_TIME(server_tickcount) - sv_maxunlag->GetFloat());

		if (simulation_time < (float)dead_time)
			return false;

		return true;
	}
};

class optimized_adjust_data
{
public:
	int i;
	player_t* player;

	float simulation_time;
	float duck_amount;
	float speed;
	bool shot;

	Vector angles;
	Vector origin;

	optimized_adjust_data() //-V730
	{
		reset();
	}

	void reset()
	{
		i = 0;
		player = nullptr;

		simulation_time = 0.0f;
		duck_amount = 0.0f;

		angles.Zero();
		origin.Zero();
	}
};

extern std::deque <adjust_data> player_records[65];


class lagcompensation : public singleton <lagcompensation>
{
public:
	void fsn(ClientFrameStage_t stage);
	bool valid(int i, player_t* e);
	void update_player_animations(player_t* e);
	void extrapolate(player_t* player, Vector& origin, Vector& velocity, int& flags, bool wasonground);
	void BuildBones(player_t* player, bool resolve);
	void UpdatePlayer(player_t* pEntity);
	//	void update_animation_system(player_t* player, adjust_data* record, C_Tickrecord * previous, int resolver_side);
	void extrapolation(player_t* player, Vector& origin, Vector& velocity, int& flags, bool on_ground);
	void player_extrapolation(player_t* e, Vector& vecorigin, Vector& vecvelocity, int& fFlags, bool bOnGround, int ticks);
	void Apply(player_t* player, adjust_data* record);
	void apply_interpolation_flags(player_t* e, bool flag);
	void UpdateAnimations(player_t* player, adjust_data* pRecord);

	resolver player_resolver[65];
	std::vector<player_settings> player_sets;
	player_info_t player_info;

	float get_interpolation();
	bool is_dormant[65];
	float previous_goal_feet_yaw[65];
};
