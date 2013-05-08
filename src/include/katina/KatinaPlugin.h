#pragma once
#ifndef _OASTATS_KATINA_PLUGIN_H
#define	_OASTATS_KATINA_PLUGIN_H
/*
 * File:   KatinaPlugin.h
 * Author: SooKee oasookee@gmail.com
 *
 * Created on April 27, 2013, 5:08 AM
 */

#include <memory>
#include <vector>

//#include <boost/shared_ptr.hpp>

#include "types.h"
#include "GUID.h"

namespace oastats {

using namespace oastats::types;

//struct GameInfo
//{
//	str mapname;
//	siz_guid_map clients; // slot -> GUID
//	guid_str_map players; // GUID -> name
//	guid_siz_map teams; // GUID -> 'R' | 'B'
//};

class KatinaPlugin
{
	friend class Katina;

private:
	void* dl;

protected:
	class Katina& katina;

public:
	KatinaPlugin(Katina& katina): dl(0), katina(katina) {}
	virtual ~KatinaPlugin() {}

	/**
	 * This provides an opportunity for a plugin to initialise
	 * itself.
	 *
	 * @return false on failure
	 */
	virtual bool open() = 0;

	/**
	 * Return the unique id of the plugin.
	 */
	virtual str get_id() const = 0;

	/**
	 * Return the name of the plugin.
	 */
	virtual str get_name() const = 0;

	/**
	 * Return the version of the plugin.
	 */
	virtual str get_version() const = 0;
	
	// cvar events
	//virtual void cvar_event(const str& name, const str& value) {}

	// Game server log events
	virtual bool init_game(siz min, siz sec) {}
	virtual bool warmup(siz min, siz sec) {}
	virtual bool client_connect(siz min, siz sec, siz num) {}
	virtual bool client_disconnect(siz min, siz sec, siz num) {}
	virtual bool client_userinfo_changed(siz min, siz sec, siz num, siz team, const GUID& guid, const str& name) {}
	virtual bool kill(siz min, siz sec, siz num1, siz num2, siz weap) {}
	virtual bool ctf(siz min, siz sec, siz num, siz team, siz act) {}
	virtual bool award(siz min, siz sec, siz num, siz awd) {}
	virtual bool say(siz min, siz sec, const GUID& guid, const str& text) {}
	virtual bool shutdown_game(siz min, siz sec) {}
	virtual bool exit(siz min, siz sec) {}
	virtual bool unknown(siz min, siz sec, const str& cmd, const str& params) {}

	/**
	 * This provides an opportunity for a plugin to clean
	 * itself up. It is called when the IrcBot is closed down.
	 * This is a good place to clean up any threads, close files etc.
	 */
	virtual void close() = 0;
};

//typedef boost::shared_ptr<KatinaPlugin> KatinaPluginSPtr;
typedef std::vector<KatinaPlugin*> plugin_vec;
typedef plugin_vec::iterator plugin_vec_iter;
typedef plugin_vec::const_iterator plugin_vec_citer;

typedef std::map<str, KatinaPlugin*> plugin_map;
typedef plugin_map::value_type plugin_map_vt;
typedef plugin_map::iterator plugin_map_iter;
typedef plugin_map::const_iterator plugin_map_citer;

/**
 * The plugin implementation source should
 * use this macro to make itself loadable as
 * a plugin.
 *
 * The macro variable should be set to the name
 * of the plugin class to instantiate.
 *
 * The plugin class should derive from KatinaPlugin.
 *
 */

#define KATINA_PLUGIN_TYPE(type) \
extern "C" KatinaPlugin* katina_plugin_factory(Katina& katina) \
{ \
	return new type(katina); \
} extern int _missing_semicolon_()

#define KATINA_PLUGIN_INFO(I, N, V) \
static const char* ID = I; \
static const char* NAME = N; \
static const char* VERSION = V

#define plog(m) log(ID << ": " << m)

} // oastats

#endif // _OASTATS_KATINA_PLUGIN_H
