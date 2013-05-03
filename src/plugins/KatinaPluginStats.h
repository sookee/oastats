#pragma once
#ifndef _OASTATS_KATINA_PLUGIN_STATS_H
#define	_OASTATS_KATINA_PLUGIN_STATS_H
/*
 * File:   KatinaPluginStats.h
 * Author: SooKee oasookee@gmail.com
 *
 * Created on April 27, 2013, 10:02 AM
 */

#include <map>
#include <utility>

#include <katina/Katina.h>
#include <katina/KatinaPlugin.h>

#include <katina/Database.h>
#include <katina/GUID.h>

#include <katina/types.h>
#include <katina/log.h>

namespace katina { namespace plugin {

using namespace oastats;
using namespace oastats::log;
using namespace oastats::data;
using namespace oastats::types;

class KatinaPluginStats
: public KatinaPlugin
{
public:
	struct stats
	{
		siz_map kills;
		siz_map deaths;
		siz_map flags;
		siz_map awards;

		time_t joined_time;
		siz logged_time;

		stats(): kills(), deaths(), flags(), awards(), joined_time(0), logged_time(0) {}
	};

	typedef std::map<GUID, guid_siz_map> onevone_map;
	typedef std::pair<const GUID, guid_siz_map> onevone_pair;
	typedef std::map<GUID, guid_siz_map>::iterator onevone_iter;
	typedef std::map<GUID, guid_siz_map>::const_iterator onevone_citer;

	typedef std::map<GUID, stats> guid_stat_map;
	typedef std::pair<const GUID, stats> guid_stat_pair;
	typedef std::map<GUID, stats>::iterator guid_stat_iter;
	typedef std::map<GUID, stats>::const_iterator guid_stat_citer;


private:
	Database db;

	str host;
	str port;

	onevone_map onevone; // GUID -> GUID -> <count> //
	guid_stat_map stats; // GUID -> <stat>

	bool in_game;

	std::set<siz> db_weaps; // what weapons to record

public:
	KatinaPluginStats(Katina& katina): KatinaPlugin(katina), in_game(false) {}

	// API

	const guid_stat_map& get_stats_ref() const { return stats; }

	// INTERFACE: KatinaPlugin

	virtual bool open();

	virtual str get_id() const;
	virtual str get_name() const;
	virtual str get_version() const;

	virtual bool exit();
	virtual bool shutdown_game();
	virtual bool warmup();
	virtual bool client_userinfo_changed(siz num, siz team, const GUID& guid, const str& name);
	virtual bool client_connect(siz num);
	virtual bool client_disconnect(siz num);
	virtual bool kill(siz num1, siz num2, siz weap);
	virtual bool ctf(siz num, siz team, siz act);
	virtual bool award(siz num, siz awd);
	virtual bool init_game();
	virtual bool say(const GUID& guid, const str& text);
	virtual bool unknown(const str& line);

	virtual void close();
};

}} // katina::plugin

#endif // _OASTATS_KATINA_PLUGIN_STATS_H

