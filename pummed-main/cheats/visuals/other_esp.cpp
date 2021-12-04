// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "other_esp.h"
#include "..\autowall\autowall.h"
#include "..\ragebot\antiaim.h"
#include "..\misc\logs.h"
#include "..\misc\misc.h"
#include "..\lagcompensation\local_animations.h"

bool can_penetrate(weapon_t* weapon)
{
	auto weapon_info = weapon->get_csweapon_info();

	if (!weapon_info)
		return false;

	Vector view_angles;
	m_engine()->GetViewAngles(view_angles);

	Vector direction;
	math::angle_vectors(view_angles, direction);

	CTraceFilter filter;
	filter.pSkip = g_ctx.local();

	trace_t trace;
	util::trace_line(g_ctx.globals.eye_pos, g_ctx.globals.eye_pos + direction * weapon_info->flRange, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter, &trace);

	if (trace.fraction == 1.0f) //-V550
		return false;

	auto eye_pos = g_ctx.globals.eye_pos;
	auto hits = 1;
	auto damage = (float)weapon_info->iDamage;
	auto penetration_power = weapon_info->flPenetration;

	static auto damageReductionBullets = m_cvar()->FindVar(crypt_str("ff_damage_reduction_bullets"));
	static auto damageBulletPenetration = m_cvar()->FindVar(crypt_str("ff_damage_bullet_penetration"));

	return autowall::get().handle_bullet_penetration(weapon_info, trace, eye_pos, direction, hits, damage, penetration_power, damageReductionBullets->GetFloat(), damageBulletPenetration->GetFloat());
}

void otheresp::penetration_reticle()
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.penetration_reticle)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	auto color = Color::Red;

	if (!weapon->is_non_aim() && weapon->m_iItemDefinitionIndex() != WEAPON_TASER && can_penetrate(weapon))
		color = Color::Green;

	static int width, height;
	m_engine()->GetScreenSize(width, height);
	
	render::get().rect_filled(width / 2, height / 2 - 1, 1, 3, color);
	render::get().rect_filled(width / 2 - 1, height / 2, 3, 1, color);
}

void otheresp::indicators()
{
	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (g_cfg.esp.indicators[INDICATOR_FAKE] && (antiaim::get().type == ANTIAIM_LEGIT || g_cfg.antiaim.type[antiaim::get().type].desync))
	{
		auto color = Color(130, 20, 20);
		auto animstate = g_ctx.local()->get_animation_state();

		if (animstate && local_animations::get().local_data.animstate)
		{
			auto delta = fabs(math::normalize_yaw(animstate->m_flGoalFeetYaw - local_animations::get().local_data.animstate->m_flGoalFeetYaw));
			auto desync_delta = max(g_ctx.local()->get_max_desync_delta(), 58.0f);

			color = Color(130, 20 + (int)(min(delta / desync_delta, 1.0f) * 150.0f), 20);
		}


		//m_indicators.push_back(m_indicator("FAKE", color));
	}

	auto choke_indicator = false;

	if (g_cfg.esp.indicators[INDICATOR_CHOKE] && !fakelag::get().condition && !misc::get().double_tap_enabled && !misc::get().hide_shots_enabled)
	{
		m_indicators.push_back(m_indicator("FAKELAG", Color(20, 255, 20)));
		choke_indicator = true;
	}

	if (g_cfg.esp.indicators[INDICATOR_HITCHANCE] && g_ctx.globals.current_weapon != -1 && key_binds::get().get_key_bind_state(25 + g_ctx.globals.current_weapon) && !weapon->is_non_aim())
	{
		if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitchance_override)
			m_indicators.push_back(m_indicator(("HC: " + std::to_string(g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].hitchance_override_amount)), Color(180, 184, 255)));
	}

	if (g_cfg.esp.indicators[INDICATOR_DAMAGE] && g_ctx.globals.current_weapon != -1 && key_binds::get().get_key_bind_state(4 + g_ctx.globals.current_weapon) && !weapon->is_non_aim())
	{
		if (g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_override_damage > 100)
			m_indicators.push_back(m_indicator(("DMG: HP + " + std::to_string(g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_override_damage - 100)), Color(180, 180, 180)));
		else
			m_indicators.push_back(m_indicator(("DMG: " + std::to_string(g_cfg.ragebot.weapon[g_ctx.globals.current_weapon].minimum_override_damage)), Color(180, 180, 180)));
	}

	if (g_cfg.esp.indicators[INDICATOR_SAFE_POINTS] && key_binds::get().get_key_bind_state(18))
		m_indicators.push_back(m_indicator("ASSIST", Color(230, 230, 230)));

	if (g_cfg.esp.indicators[INDICATOR_SAFE_POINTS] && key_binds::get().get_key_bind_state(3) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("SAFE", Color(20, 255, 20)));

	if (g_cfg.esp.indicators[INDICATOR_SAFE_POINTS] && key_binds::get().get_key_bind_state(23) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("EDGE BUG", Color(20, 20, 255)));

	if (g_cfg.esp.indicators[INDICATOR_SAFE_POINTS] && key_binds::get().get_key_bind_state(13) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("EDGE JUMP", Color(20, 20, 255)));

	if (g_cfg.esp.indicators[INDICATOR_SAFE_POINTS] && key_binds::get().get_key_bind_state(24) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("JUMP BUG", Color(20, 20, 255)));

	if (g_cfg.esp.indicators[INDICATOR_BODY_AIM] && key_binds::get().get_key_bind_state(22) && !weapon->is_non_aim())
		m_indicators.push_back(m_indicator("BAIM", Color(255, 0, 0)));

	if (g_cfg.esp.indicators[INDICATOR_DUCK] && key_binds::get().get_key_bind_state(20))
		m_indicators.push_back(m_indicator("DUCK", Color(230, 230, 230)));

	if (g_cfg.esp.indicators[INDICATOR_DESYNC_SIDE] && (antiaim::get().type == ANTIAIM_LEGIT && g_cfg.antiaim.desync == 1 || antiaim::get().type != ANTIAIM_LEGIT && g_cfg.antiaim.type[antiaim::get().type].desync == 1) && !antiaim::get().condition(g_ctx.get_command()))
	{
		auto side = antiaim::get().desync_angle > 0.0f ? "AA RIGHT" : "AA LEFT";

		if (antiaim::get().type == ANTIAIM_LEGIT)
			side = antiaim::get().desync_angle > 0.0f ? "AA LEFT" : "AA RIGHT";

		m_indicators.push_back(m_indicator(side, Color(255, 255, 255)));
	}

	if (g_cfg.esp.indicators[INDICATOR_DT] && g_cfg.ragebot.double_tap && g_cfg.ragebot.double_tap_key.key > KEY_NONE && g_cfg.ragebot.double_tap_key.key < KEY_MAX && misc::get().double_tap_key)
		m_indicators.push_back(m_indicator("DT", !g_ctx.local()->m_bGunGameImmunity() && !(g_ctx.local()->m_fFlags() & FL_FROZEN) && !antiaim::get().freeze_check && misc::get().double_tap_enabled && !weapon->is_grenade() && weapon->m_iItemDefinitionIndex() != WEAPON_TASER && weapon->m_iItemDefinitionIndex() != WEAPON_REVOLVER && weapon->can_fire(false) ? Color(20, 255, 20) : Color(130, 20, 20)));

	if (g_cfg.esp.indicators[INDICATOR_HS] && misc::get().hide_shots_key)
		m_indicators.push_back(m_indicator("ONSHOT", !g_ctx.local()->m_bGunGameImmunity() && !(g_ctx.local()->m_fFlags() & FL_FROZEN) && !antiaim::get().freeze_check && misc::get().hide_shots_enabled ? Color(20, 255, 20) : Color(130, 20, 20)));
}
void otheresp::draw_indicators()
{
	if (!g_ctx.local()->is_alive())
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto h = height / 2 + 100;

	for (auto& indicator : m_indicators)
	{
		render::get().gradient(5, h - 15, 30, 30, Color(0, 0, 0, 0), Color(g_cfg.esp.indicators_left_color), GRADIENT_HORIZONTAL);
		render::get().gradient(35, h - 15, 30, 30, Color(g_cfg.esp.indicators_right_color), Color(0, 0, 0, 0), GRADIENT_HORIZONTAL);
		//render::get().gradient(5, h - 15, 85, 30, Color(g_cfg.esp.indicators_left_color), Color(g_cfg.esp.indicators_right_color), GRADIENT_HORIZONTAL);
		render::get().text(fonts[INDICATORFONT], 10, h, indicator.m_color, HFONT_CENTERED_Y, indicator.m_text.c_str());
		h -= 35;
	}

	m_indicators.clear();
}


void otheresp::keybinds()
{
	if (!g_cfg.misc.keybinds)
	return;

	if (!g_ctx.local()->is_alive()) //-V807
		return;

	static int width, height;
	m_engine()->GetScreenSize(width, height);

	auto color = g_cfg.menu.menu_theme;
	int x{ 15 };
	static int offset = 1;

	std::string text;

	// condition dt
	if (misc::get().double_tap_key)
	{
		text += crypt_str("Double Tap Enabled\n");
	}

	// hs
	if (misc::get().hide_shots_key)
	{
		text += crypt_str("Hide Shots Enabled\n");
	}

	// mindmg 
	if (key_binds::get().get_key_bind_state(4 + g_ctx.globals.current_weapon))
	{
		text += crypt_str("Min Damage Enabled\n");
	}

	// baim
	if (key_binds::get().get_key_bind_state(22))
	{
		text += crypt_str("Smart Baim Enabled\n");
	}

	// resolver override
	if (key_binds::get().get_key_bind_state(24))
	{
		text += crypt_str("Resolver Override Enabled\n");
	}

	// thirdperson
	if (key_binds::get().get_key_bind_state(17))
	{
		text += crypt_str("Thirdperson Enabled\n");
	}

	// slowwalk
	if (key_binds::get().get_key_bind_state(21))
	{
		text += crypt_str("Slowwalk Enabled\n");
	}

	// fakeduck
	if (key_binds::get().get_key_bind_state(20))
	{
		text += crypt_str("Fake Duck Enabled\n");
	}

	// edgejump
	if (key_binds::get().get_key_bind_state(19))
	{
		text += crypt_str("Edge Jump Enabled\n");
	}

	// auto peek 
	if (key_binds::get().get_key_bind_state(18))
	{
		text += crypt_str("Auto Peek Enabled\n");
	}

	// auto peek 
	if (key_binds::get().get_key_bind_state(16))
	{
		text += crypt_str("Desync Invert Enabled\n");
	}

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.f, 1.f, 1.f, 0.05f));

	DWORD flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiColumnsFlags_NoResize;
	ImGui::SetNextWindowSize(ImVec2(200, 30 + (15 * 11)));
	ImGui::Begin(crypt_str("Indicators"), nullptr, flags);

	ImGui::SetCursorPos(ImVec2(0, 0));
	ImGui::BeginChild("Head", ImVec2(200, 20), false, ImGuiWindowFlags_NoBackground);

	ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetWindowPos(), ImVec2(ImGui::GetWindowPos().x + 200, ImGui::GetWindowPos().y + 20), ImColor(41, 46, 57, 255));

	ImGui::SetCursorPos(ImVec2(100 - ImGui::CalcTextSize(crypt_str("Keybinds List")).x / 2, 10 - ImGui::CalcTextSize(crypt_str("Keybind List")).y / 2));
	ImGui::Text(crypt_str("Keybind List"));

	ImGui::EndChild();

	ImGui::SetCursorPos(ImVec2(10, 25));
	ImGui::BeginChild(crypt_str("Body"), ImVec2(180, -1), false, ImGuiWindowFlags_NoBackground);

	ImGui::Text(text.c_str());

	ImGui::EndChild();

	ImGui::End();

	ImGui::PopStyleColor();
}

void otheresp::hitmarker_paint()
{
	if (!g_cfg.esp.hitmarker[0] && !g_cfg.esp.hitmarker[1])
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (!g_ctx.local()->is_alive())
	{
		hitmarker.hurt_time = FLT_MIN;
		hitmarker.point = ZERO;
		return;
	}

	if (hitmarker.hurt_time + 0.7f > m_globals()->m_curtime)
	{
		if (g_cfg.esp.hitmarker[0])
		{
			static int width, height;
			m_engine()->GetScreenSize(width, height);

			auto alpha = (int)((hitmarker.hurt_time + 0.7f - m_globals()->m_curtime) * 255.0f);
			hitmarker.hurt_color.SetAlpha(alpha);

			auto offset = 7.0f - (float)alpha / 255.0f * 7.0f;

			render::get().line(width / 2 + 5 + offset, height / 2 - 5 - offset, width / 2 + 12 + offset, height / 2 - 12 - offset, hitmarker.hurt_color);
			render::get().line(width / 2 + 5 + offset, height / 2 + 5 + offset, width / 2 + 12 + offset, height / 2 + 12 + offset, hitmarker.hurt_color);
			render::get().line(width / 2 - 5 - offset, height / 2 + 5 + offset, width / 2 - 12 - offset, height / 2 + 12 + offset, hitmarker.hurt_color);
			render::get().line(width / 2 - 5 - offset, height / 2 - 5 - offset, width / 2 - 12 - offset, height / 2 - 12 - offset, hitmarker.hurt_color);
		}

		if (g_cfg.esp.hitmarker[1])
		{
			Vector world;

			if (math::world_to_screen(hitmarker.point, world))
			{
				auto alpha = (int)((hitmarker.hurt_time + 0.7f - m_globals()->m_curtime) * 255.0f);
				hitmarker.hurt_color.SetAlpha(alpha);

				auto offset = 7.0f - (float)alpha / 255.0f * 7.0f;

				render::get().line(world.x + 5 + offset, world.y - 5 - offset, world.x + 12 + offset, world.y - 12 - offset, hitmarker.hurt_color);
				render::get().line(world.x + 5 + offset, world.y + 5 + offset, world.x + 12 + offset, world.y + 12 + offset, hitmarker.hurt_color);
				render::get().line(world.x - 5 - offset, world.y + 5 + offset, world.x - 12 - offset, world.y + 12 + offset, hitmarker.hurt_color);
				render::get().line(world.x - 5 - offset, world.y - 5 - offset, world.x - 12 - offset, world.y - 12 - offset, hitmarker.hurt_color);
			}
		}
	}
}

void otheresp::damage_marker_paint()
{
	for (auto i = 1; i < m_globals()->m_maxclients; i++) //-V807
	{
		if (damage_marker[i].hurt_time + 2.0f > m_globals()->m_curtime)
		{
			Vector screen;

			if (!math::world_to_screen(damage_marker[i].position, screen))
				continue;

			auto alpha = (int)((damage_marker[i].hurt_time + 2.0f - m_globals()->m_curtime) * 127.5f);
			damage_marker[i].hurt_color.SetAlpha(alpha);

			render::get().text(fonts[DAMAGE_MARKER], screen.x, screen.y, damage_marker[i].hurt_color, HFONT_CENTERED_X | HFONT_CENTERED_Y, "%i", damage_marker[i].damage);
		}
	}
}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device);

void otheresp::spread_crosshair(LPDIRECT3DDEVICE9 device)
{
	if (!g_cfg.player.enable)
		return;

	if (!g_cfg.esp.show_spread)
		return;

	if (!g_ctx.local()->is_alive())
		return;

	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (weapon->is_non_aim())
		return;

	int w, h;
	m_engine()->GetScreenSize(w, h);

	draw_circe((float)w * 0.5f, (float)h * 0.5f, g_ctx.globals.inaccuracy * 500.0f, 50, D3DCOLOR_RGBA(g_cfg.esp.show_spread_color.r(), g_cfg.esp.show_spread_color.g(), g_cfg.esp.show_spread_color.b(), g_cfg.esp.show_spread_color.a()), D3DCOLOR_RGBA(0, 0, 0, 0), device);
}

void draw_circe(float x, float y, float radius, int resolution, DWORD color, DWORD color2, LPDIRECT3DDEVICE9 device)
{
	LPDIRECT3DVERTEXBUFFER9 g_pVB2 = nullptr;
	std::vector <CUSTOMVERTEX2> circle(resolution + 2);

	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0.0f;

	circle[0].rhw = 1.0f;
	circle[0].color = color2;

	for (auto i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - radius * cos(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - radius * sin(D3DX_PI * ((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0.0f;

		circle[i].rhw = 1.0f;
		circle[i].color = color;
	}

	device->CreateVertexBuffer((resolution + 2) * sizeof(CUSTOMVERTEX2), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB2, nullptr); //-V107

	if (!g_pVB2)
		return;

	void* pVertices;

	g_pVB2->Lock(0, (resolution + 2) * sizeof(CUSTOMVERTEX2), (void**)&pVertices, 0); //-V107
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(CUSTOMVERTEX2));
	g_pVB2->Unlock();

	device->SetTexture(0, nullptr);
	device->SetPixelShader(nullptr);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	device->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX2));
	device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	device->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);

	g_pVB2->Release();
}

void otheresp::automatic_peek_indicator()
{
	auto weapon = g_ctx.local()->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	static auto position = ZERO;

	if (!g_ctx.globals.start_position.IsZero())
		position = g_ctx.globals.start_position;

	if (position.IsZero())
		return;

	static auto alpha = 4.0f;

	if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18) || alpha)
	{
		if (!weapon->is_non_aim() && key_binds::get().get_key_bind_state(18))
			alpha += 9.0f * m_globals()->m_frametime; //-V807
		else
			alpha -= 9.0f * m_globals()->m_frametime;

		alpha = math::clamp(alpha, 0.0f, 1.0f);
		render::get().Draw3DFilledCircle(position, alpha * 20.f, g_ctx.globals.fired_shot ? Color(111, 45, 159, (int)(alpha * 135.0f)) : Color(111, 45, 159, (int)(alpha * 125.0f)));
		render::get().Draw3DCircle(position, alpha * 20.f, g_ctx.globals.fired_shot ? Color(129, 86, 159, (int)(alpha * 135.0f)) : Color(129, 86, 159, (int)(alpha * 125.0f)));

		Vector screen;

		if (math::world_to_screen(position, screen))
		{
			static auto offset = 30.0f;

			if (!g_ctx.globals.fired_shot)
			{
				static auto switch_offset = false;

				if (offset <= 30.0f || offset >= 55.0f)
					switch_offset = !switch_offset;

				offset += switch_offset ? 22.0f * m_globals()->m_frametime : -22.0f * m_globals()->m_frametime;
				offset = math::clamp(offset, 30.0f, 55.0f);
			}
		}
	}
}