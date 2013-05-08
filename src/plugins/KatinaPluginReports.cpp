
#include "KatinaPluginReports.h"

#include <katina/KatinaPlugin.h>
#include "KatinaPluginStats.h"

#include <katina/Database.h>
#include <katina/GUID.h>

#include <katina/types.h>
#include <katina/log.h>
#include <katina/codes.h>


#include <gcrypt.h>

namespace katina { namespace plugin {

using namespace oastats::log;
using namespace oastats::data;
using namespace oastats::types;

KATINA_PLUGIN_TYPE(KatinaPluginReports);
KATINA_PLUGIN_INFO("katina::report", "Katina Reports", "0.1");

const str HUD_FLAG_P = "⚑";
const str HUD_FLAG_DIE = "*";
const str HUD_FLAG_CAP = "Y";
const str HUD_FLAG_NONE = ".";
const str HUD_FLAG_RETURN = "^";

const siz TEAM_U = 0;
const siz TEAM_R = 1;
const siz TEAM_B = 2;
const siz TEAM_S = 3;

str hud_flag[2] = {HUD_FLAG_NONE, HUD_FLAG_NONE};
const str flag[2] = {"^1RED", "^4BLUE"};

str weapons[] =
{
	"unknown weapon"
	, "shotgun"
	, "gauntlet"
	, "machinegun"
	, "grenade"
	, "grenade schrapnel"
	, "rocket"
	, "rocket blast"
	, "plasma"
	, "plasma splash"
	, "railgun"
	, "lightening"
	, "BFG"
	, "BFG fallout"
	, "dround"
	, "slimed"
	, "burnt up in lava"
	, "crushed"
	, "telefraged"
	, "falling to far"
	, "suicide"
	, "target lazer"
	, "inflicted pain"
	, "nailgun"
	, "chaingun"
	, "proximity mine"
	, "kamikazi"
	, "juiced"
	, "grappled"
};

siz map_get(const siz_map& m, siz key)
{
	return m.find(key) == m.end() ? 0 : m.at(key);
}

str get_hud(siz m, siz s, str hud_flag[2])
{
	soss oss;
	oss << "00[15" << (m < 10?"0":"") << m << "00:15" << (s < 10?"0":"") << s << " ";
	oss << "04" << hud_flag[FL_RED];
	oss << "02" << hud_flag[FL_BLUE];
	oss << "00]";
	return oss.str();
}

/**
 *
 * @param var
 * @param w
 * @param j - junk (control codes not included in final width)
 */
void set_width(str& var, siz w, siz j)
{
	w += j;
	if(var.size() < w)
		var = str(w - var.size(), ' ') + var;
}

bool KatinaPluginReports::open()
{
	if((stats = dynamic_cast<KatinaPluginStats*>(katina.get_plugin("katina::stats", "0.0"))))
		plog("Found: " << stats->get_name());

	delete client;
	
	if(!(client = new RemoteClientList(katina)))
	{
		plog("FATAL: unable to crate remote client list");
		return false;
	}
	
	str_vec clients = katina.get_vec("remote.irc.client");
	
	for(siz i = 0; i < clients.size(); ++i)
		client->add(RemoteClient::create(katina, clients[i]));
	
	client->chat('*', "^3Stats Reporting System v^7" + get_version() + " - ^1ONLINE");

	return true;
}

str KatinaPluginReports::get_id() const
{
	return "katina::reports";
}

str KatinaPluginReports::get_name() const
{
	return "Stats Reporting";
}

str KatinaPluginReports::get_version() const
{
	return "0.1-dev";
}

bool KatinaPluginReports::exit(siz min, siz sec)
{
	client->chat('*', "^3Game Over");

	// erase non spam marked messages
	for(str_siz_map_iter i = spam.begin(); i != spam.end();)
	{
		if(i->second < spam_limit)
		{
			spam.erase(i->first);
			i = spam.begin();
		}
		else
			++i;
	}

	std::multimap<double, str> skivvy_scores;

	soss oss;
	oss.str("");
	str sep;
	if(stats_cols & RSC_TIME)
		{ oss << sep << "^3time "; sep = "^2|"; }
	if(stats_cols & RSC_FPH)
		{ oss << sep << "^3fph"; sep = "^2|"; }
	if(stats_cols & RSC_TIME)
		{ oss << sep << "^3cph"; sep = "^2|"; }
	if(stats_cols & RSC_TIME)
		{ oss << sep << "^3fpd  "; sep = "^2|"; }
	if(stats_cols & RSC_TIME)
		{ oss << sep << "^3cpd  "; sep = "^2|"; }
	client->chat('s', oss.str());

	for(guid_stat_citer p = stats->stats.begin(); p != stats->stats.end(); ++p)
	{
		const str& player = katina.players.at(p->first);
		con("player: " << player);
		con("\t  caps: " << map_get(p->second.flags, FL_CAPTURED));
		con("\t kills: " << map_get(p->second.kills, MOD_RAILGUN));
		con("\tdeaths: " << map_get(p->second.deaths, MOD_RAILGUN));
		con("\t  defs: " << map_get(p->second.awards, AW_DEFENCE));
		con("\t gaunt: " << map_get(p->second.awards, AW_GAUNTLET));
		con("\t  time: " << p->second.logged_time << 's');
		// TODO: modify this to add AW options as well as insta

		siz c = map_get(p->second.flags, FL_CAPTURED);

		siz k = 0;
		for(siz i = 0; i < MOD_MAXVALUE; ++i)
			k += map_get(p->second.kills, i);

		siz d = 0;
		for(siz i = 0; i < MOD_MAXVALUE; ++i)
			d += map_get(p->second.deaths, i);

//			siz k = map_get(p->second.kills, MOD_RAILGUN);
//			k += map_get(p->second.kills, MOD_GAUNTLET);
//			siz d = map_get(p->second.deaths, MOD_RAILGUN);
//			d += map_get(p->second.deaths, MOD_GAUNTLET);
		siz h = p->second.logged_time;
		con("c: " << c);
		con("k: " << k);
		con("d: " << d);

		double rkd = 0.0;
		double rcd = 0.0;
		siz rkh = 0;
		siz rch = 0;
		str kd, cd, kh, ch;
		if(!d)
		{
			if(k)
				kd = "perf ";
			if(c)
				cd = "perf  ";
		}
		else
		{
			rkd = double(k) / d;
			rcd = double(c * 100) / d;
			rkh = k * 60 * 60 / h;
			rch = c * 60 * 60 / h;

			kd = to_string(rkd, 5);
			cd = to_string(rcd, 6);
			kh = to_string(rkh, 3);
			ch = to_string(rch, 2);
		}
		if(k || c || d)
		{
			str mins, secs;
			siz m = p->second.logged_time / 60;
			siz s = p->second.logged_time % 60;
			oss.str("");
			oss << m;
			mins = oss.str();
			oss.str("");
			oss << s;
			secs = oss.str();
			if(mins.size() < 2)
				mins = str(2 - mins.size(), ' ') + mins;
			if(secs.size() < 2)
				secs = str(2 - secs.size(), '0') + secs;

			oss.str("");
			str sep, col;
			if(stats_cols & RSC_TIME)
			{
				col = "^7" + mins + "^3:^7" + secs;
				set_width(col, 5, 6);
				oss << sep << col;
				sep = "^2|";
			}
			if(stats_cols & RSC_FPH)
			{
				col = "^7" + kh;
				set_width(col, 3, 2);
				oss << sep << col;
				sep = "^2|";
			}
			if(stats_cols & RSC_CPH)
			{
				col = "^7" + ch;
				set_width(col, 3, 2);
				oss << sep << col;
				sep = "^2|";
			}
			if(stats_cols & RSC_KPD)
			{
				col = "^7" + kd;
				set_width(col, 5, 2);
				oss << sep << col;
				sep = "^2|";
			}
			if(stats_cols & RSC_CPD)
			{
				col = "^7" + cd;
				set_width(col, 5, 2);
				oss << sep << col;
				sep = "^2|";
			}

			oss << sep << "^7" << player;
//				oss << "^3time: ^7" << mins << "^3:^7" << secs << " " << "^3kills^7/^3d ^5(^7" << kd << "^5) ^3caps^7/^3d ^5(^7" << cd << "^5)^7: " + player;
			skivvy_scores.insert(std::make_pair(rkh, oss.str()));
		}
	}
	for(std::multimap<double, str>::reverse_iterator r = skivvy_scores.rbegin(); r != skivvy_scores.rend(); ++r)
		client->chat('s', r->second);

	return true;
}

bool KatinaPluginReports::shutdown_game(siz min, siz sec)
{
	return true;
}

bool KatinaPluginReports::warmup(siz min, siz sec)
{
	return true;
}

bool KatinaPluginReports::client_userinfo_changed(siz min, siz sec, siz num, siz team, const GUID& guid, const str& name)
{
	return true;
}
bool KatinaPluginReports::client_connect(siz min, siz sec, siz num)
{

}
bool KatinaPluginReports::client_disconnect(siz min, siz sec, siz num)
{
	return true;
}
bool KatinaPluginReports::kill(siz min, siz sec, siz num1, siz num2, siz weap)
{
	if(weap != MOD_SUICIDE && katina.clients.find(num1) != katina.clients.end() && katina.clients.find(num2) != katina.clients.end())
		client->chat('k', "^7" + katina.players[katina.clients[num1]] + " ^4killed ^7" + katina.players[katina.clients[num2]]
			+ " ^4with a ^7" + weapons[weap]);

	return true;
}
bool KatinaPluginReports::ctf(siz min, siz sec, siz num, siz team, siz act)
{
	siz pcol = team - 1; // make 0-1 for array index
	siz ncol = pcol ? 0 : 1;

	str nums_team = "^7[^2U^7]"; // unknown team
	str nums_nteam = "^7[^2U^7]"; // unknown team

	if(katina.teams[katina.clients[num]] == TEAM_R)
		nums_team = "^7[^1R^7]";
	else if(katina.teams[katina.clients[num]] == TEAM_B)
		nums_team = "^7[^4B^7]";
	if(katina.teams[katina.clients[num]] == TEAM_B)
		nums_nteam = "^7[^1R^7]";
	else if(katina.teams[katina.clients[num]] == TEAM_R)
		nums_nteam = "^7[^4B^7]";

	str hud;

	if(act == FL_CAPTURED) // In Game Announcer
	{
		if(client)
		{
			siz caps = map_get(stats->stats[katina.clients[num]].flags, FL_CAPTURED);
			str msg = katina.players[katina.clients[num]]
				+ "^3 has ^7" + to_string(caps) + "^3 flag" + (caps==1?"":"s") + "!";
			if(do_flags_hud)
			{
				hud_flag[pcol] = HUD_FLAG_CAP;
				hud = get_hud(min, sec, hud_flag);
			}
			client->raw_chat('f', hud + oa_to_IRC(nums_team + " " + msg));
			if(do_flags_hud)
			{
				hud_flag[pcol] = HUD_FLAG_NONE;
				hud = get_hud(min, sec, hud_flag);
			}
			client->raw_chat('f', hud + oa_to_IRC("^7[ ] ^1RED^3: ^7" + to_string(flags[FL_BLUE]) + " ^3v ^4BLUE^3: ^7" + to_string(flags[FL_RED])));
		}
	}
	else if(act == FL_TAKEN)
	{
		if(client && do_flags)
		{
			if(do_flags_hud)
			{
				hud_flag[pcol] = HUD_FLAG_P;
				hud = get_hud(min, sec, hud_flag);
			}
			client->raw_chat('f', hud + oa_to_IRC(nums_team + " ^7" + katina.players[katina.clients[num]] + "^3 has taken the " + flag[pcol] + " ^3flag!"));
		}
	}
	else if(act == FL_DROPPED)
	{
		if(client && do_flags)
		{
			if(do_flags_hud)
			{
				hud_flag[ncol] = HUD_FLAG_DIE;
				hud = get_hud(min, sec, hud_flag);
				hud_flag[ncol] = HUD_FLAG_NONE;
			}
			client->raw_chat('f', hud + oa_to_IRC(nums_team + " ^7" + katina.players[katina.clients[num]] + "^3 has killed the " + flag[ncol] + " ^3flag carrier!"));
		}
	}
	else if(act == FL_RETURNED)
	{
		if(client && do_flags)
		{
			if(do_flags_hud)
			{
				hud_flag[pcol] = HUD_FLAG_RETURN;
				hud = get_hud(min, sec, hud_flag);
				hud_flag[pcol] = HUD_FLAG_NONE;
			}
			client->raw_chat('f', hud + oa_to_IRC(nums_team + " ^7" + katina.players[katina.clients[num]] + "^3 has returned the " + flag[pcol] + " ^3flag!"));
		}
	}

	return true;
}

bool KatinaPluginReports::award(siz min, siz sec, siz num, siz awd)
{
	return true;
}

bool KatinaPluginReports::init_game(siz min, siz sec)
{
	flags[FL_RED] = 0;
	flags[FL_BLUE] = 0;

	if(client && do_infos && katina.mapname != old_mapname)
	{
		// TODO: add this after writing KainaPluginVotes
//		siz love = 0;
//		siz hate = 0;
//		for(guid_int_map_iter i = map_votes.begin(); i != map_votes.end(); ++i)
//		{
//			if(i->second > 0)
//				++love;
//			else
//				++hate;
//		}
		client->chat('i', ".");
		client->chat('i', "^3== Playing Map: ^7" + katina.mapname + "^3 == ^7");// + to_string(love)
//			+ " ^1LOVE ^7" + to_string(hate) + " ^2HATE ^3==");
		old_mapname = katina.mapname;
	}

	return true;
}

bool KatinaPluginReports::say(siz min, siz sec, const GUID& guid, const str& text)
{
	if(do_chats)
	{
		if(!spamkill || ++spam[text] < spam_limit)
			client->chat('c', "^7say: " + katina.players[guid] + " ^2" + text);
	}

	return true;
}

//bool KatinaPluginReports::unknown(siz min, siz sec, const str& cmd, const str& params)
//{
//
//}

void KatinaPluginReports::close()
{

}

}} // katina::plugin