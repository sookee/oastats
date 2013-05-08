/*
 * File:   KatinaPluginReports.h
 * Author: SooKee oasookee@gmail.com
 *
 * Created on April 27, 2013, 10:02 AM
 */

#ifndef KATINA_PLUGIN_REPORTS_H
#define	KATINA_PLUGIN_REPORTS_H

#include <map>
#include <utility>

#include "KatinaPluginStats.h"
#include <katina/Database.h>
#include <katina/RemoteClient.h>
#include <katina/GUID.h>

#include <katina/types.h>
#include <katina/log.h>

#include <katina/PKI.h>

namespace katina { namespace plugin {

using namespace oastats::pki;
using namespace oastats::log;
using namespace oastats::data;
using namespace oastats::types;

class RemoteClientList
: public RemoteClient
{
	std::vector<RemoteClient*> clients;
	
public:
	RemoteClientList(Katina& katina): RemoteClient(katina) {}
	~RemoteClientList()
	{
		for(siz i = 0; i < clients.size(); ++i)
			delete clients[i];
	}

	// API
	
	void add(RemoteClient* client) { clients.push_back(client); }
	
	// RemoteClient Interface

	virtual bool send(const str& cmd, str& res);
};

class KatinaPluginReports
: public KatinaPlugin
{
public:

private:
	KatinaPluginStats* stats;
	RemoteClientList* client;

	enum
	{
		RSC_TIME = 0b00000001
		, RSC_FPH = 0b00000010 // frags/hour
		, RSC_CPH = 0b00000100 // flags/hour
		, RSC_KPD = 0b00001000 // kills/deaths
		, RSC_CPD = 0b00010000 // caps/deaths
	};

	bool active;
	bool do_flags;
	bool do_flags_hud;
	bool do_chats;
	bool do_kills;
	bool do_infos;
	bool do_stats;
	siz stats_cols;
	bool spamkill;
	str chans;

	str_siz_map spam;
	siz spam_limit;

	siz flags[2];

	str old_mapname;

	str get_stats_cols() const
	{
		str cols, sep;
		if(stats_cols & RSC_TIME)
			{ cols += sep + "TIME"; sep = " "; }
		if(stats_cols & RSC_FPH)
			{ cols += sep + "FPH"; sep = " "; }
		if(stats_cols & RSC_CPH)
			{ cols += sep + "CPH"; sep = " "; }
		if(stats_cols & RSC_KPD)
			{ cols += sep + "KPD"; sep = " "; }
		if(stats_cols & RSC_CPD)
			{ cols += sep + "CPD"; sep = " "; }
		return cols;
	}

public:
	KatinaPluginReports(Katina& katina)
	: KatinaPlugin(katina)
	, stats(0)
	, client(0)
	, active(false)
	, do_flags(false)
	, do_flags_hud(false)
	, do_chats(false)
	, do_kills(false)
	, do_infos(false)
	, do_stats(false)
	, stats_cols(0)
	, spamkill(false)
	, spam_limit(2)
	{
	}

	// INTERFACE: KatinaPlugin

	virtual bool open();

	virtual str get_id() const;
	virtual str get_name() const;
	virtual str get_version() const;

	virtual bool init_game(siz min, siz sec);
	virtual bool warmup(siz min, siz sec);
	virtual bool client_connect(siz min, siz sec, siz num);
	virtual bool client_disconnect(siz min, siz sec, siz num);
	virtual bool client_userinfo_changed(siz min, siz sec, siz num, siz team, const GUID& guid, const str& name);
	virtual bool kill(siz min, siz sec, siz num1, siz num2, siz weap);
	virtual bool ctf(siz min, siz sec, siz num, siz team, siz act);
	virtual bool award(siz min, siz sec, siz num, siz awd);
	virtual bool say(siz min, siz sec, const GUID& guid, const str& text);
	virtual bool shutdown_game(siz min, siz sec);
	virtual bool exit(siz min, siz sec);
//	virtual bool unknown(siz min, siz sec, const str& cmd, const str& params);

	virtual void close();
};

}} // katina::plugin

#endif	// KATINA_PLUGIN_REPORTS_H
