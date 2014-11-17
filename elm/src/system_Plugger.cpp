/*
 *	Plugger class interface
 *
 *	This file is part of OTAWA
 *	Copyright (c) 2005-07, IRIT UPS.
 *
 *	OTAWA is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	OTAWA is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with OTAWA; if not, write to the Free Software
 *	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "../config.h"
#include <elm/deprecated.h>
#include <elm/assert.h>
#include <stdlib.h>
#if defined(__WIN32) || defined(__WIN64)
#	include <windows.h>
#	undef min
#	undef max
#elif defined(WITH_LIBTOOL)
#	include <ltdl.h>
#else
#	include <dlfcn.h>
#endif
#include <elm/sys/Plugger.h>
#include <elm/sys/System.h>
#include <elm/io.h>
#include <elm/ini.h>
#include <elm/util/AutoDestructor.h>

namespace elm { namespace sys {

static cstring ELD_EXT = "eld";
static cstring
	SECTION_NAME = "elm-plugin",
	// NAME_ATT = "name",
	PATH_ATT = "path",
	// DESCRIPTION = "description",
	RPATH_ATT = "rpath",
	LIBS_ATT = "libs",
	DEPS_ATT = "deps";

static cstring PLUG_EXT =
#ifdef WITH_LIBTOOL
		"la";
#elif defined(__WIN32) || defined(__WIN64)
		"dll";
#elif defined(__APPLE__)
		"dylib";
#else
		"so";
#endif


/**
 * @class Plugger
 * This class is used for connecting Plugin object from dynamic loaded code
 * unit. The Plugin object is retrieved as a global data matching the hook name
 * passed to the plugger creation. The dynamic loaded code units are retrieved
 * from the paths given to the plugger object. See @ref Plugin.
 * @ingroup plugins
 */


/**
 * Current active pluggers.
 */
genstruct::Vector<Plugger *> Plugger::pluggers;


/**
 * Build a new plugger.
 * @param hook				Hook name for retrieving plugin in the code unit.
 * @param plugger_version	Plugger version for compatibility checking.
 * @param _paths			List of path separated by ":" for retrieving
 * 							the plugin. Pass "*" for initializing with the
 * 							default system paths.
 */
Plugger::Plugger(CString hook, const Version& plugger_version, String _paths)
: _hook(hook), per_vers(plugger_version), err(OK) {

	// Initialize DL library
	#if !defined(__WIN32) && !defined(__WIN64) && defined(WITH_LIBTOOL)
		static bool preloaded = false;
		if(!preloaded) {
			lt_dlinit();
			preloaded = true;
		}
	#endif

	// Look in the system paths
	if(_paths == "*")
#if defined(__APPLE__)
		_paths = getenv("DYLD_LIBRARY_PATH");
#else
		_paths = getenv("LD_LIBRARY_PATH");
#endif

	// Scan the paths
	int index = _paths.indexOf(elm::sys::Path::PATH_SEPARATOR);
	while(index >= 0) {
		if(index)
			paths.add(_paths.substring(0, index));
		_paths = _paths.substring(index + 1);
		index = _paths.indexOf(elm::sys::Path::PATH_SEPARATOR);
	}
	if(_paths)
		paths.add(_paths);

	// Add to active pluggers
	pluggers.add(this);
}


/**
 */
Plugger::~Plugger(void) {
	pluggers.remove(this);
	#if !defined(__WIN32) && !defined(__WIN64) && defined(WITH_LIBTOOL)
		lt_dlexit();
	#endif
}


/**
 * For internal use only.
 */
void Plugger::leave(Plugin *plugin) {
	for(int i = 0; i < pluggers.count(); i++)
		pluggers[i]->plugins.remove(plugin);
}


/**
 * Add new path for retrieving plugins.
 * @param path	Added path.
 */
void Plugger::addPath(String path) {
	paths.add(path);
}


/**
 * Remove a path.
 * @param path	Removed path.
 */
void Plugger::removePath(String path) {
	paths.remove(path);
}


/**
 * Remove all paths.
 */
void Plugger::resetPaths(void) {
	paths.clear();
}


/**
 * Get a plugin by its name.
 * @param name	Name of the plugin to get (possibly containing '/'
 * 				to access sub-directories).
 * @return		Found plugin or null.
 */
Plugin *Plugger::plug(const string& name) {
	err = OK;

	// is it a path ?
	if(name.startsWith("/"))
		return plugFile(name);

	// Look in opened plugins
	for(int i = 0; i < plugins.count(); i++)
		if(plugins[i]->matches(name)) {
			plugins[i]->plug(0);
			return plugins[i];
		}

	// Look in static plugins
	Plugin *plugin = Plugin::get(_hook, name);
	if(plugin)
		return plug(plugin, 0);

	// Load the plugin
	for(int i = 0; i < paths.count(); i++) {
		StringBuffer buf;
		buf << paths[i] << "/" << name << "." << PLUG_EXT;
		error_t old_err = err;
		Plugin *plugin = plugFile(buf.toString());
		if(plugin)
			return plugin;
		if(old_err != OK && err == NO_PLUGIN)
			err = old_err;
	}

	// No plugin available
	return 0;
}


/**
 */
Plugin *Plugger::plug(Plugin *plugin, void *handle) {
	plugin->plug(handle);
	if(!plugins.contains(plugin))
		plugins.add(plugin);
	return plugin;
}


/**
 * Evaluate the given path. For now, replaced only prefix "$ORIGIN" by current plugin
 * path.
 * @param plugin_path	Path to the current plugin.
 * @param path			Path to transform.
 * @return				Transformed path.
 */
static inline sys::Path evaluate(sys::Path plugin_path, sys::Path path) {
	string p = path;
	if(p.startsWith("$ORIGIN")) {
		return plugin_path.dirPart() / p.substring(7);
	}
	else
		return path;
}


/**
 * Link an OS library.
 * @param lib		Library to link (OS extension is automatically added if required).
 * @return			Opened library or null.
 */
void *Plugger::link(sys::Path lib) {
#	if defined(__WIN32) || defined(__WIN64)
		return LoadLibrary(&lib);
#	elif defined(WITH_LIBTOOL)
		return lt_dlopen(&lib);
#	else
		return dlopen(&lib, RTLD_LAZY);
#	endif
}


/**
 * Look and load a library.
 * @param lib	Library path (OS extension automatically added).
 * @param rpath	List of paths to look in (if empty, look in OS pathes).
 * @return		Hook on the library.
 */
void *Plugger::lookLibrary(sys::Path lib, genstruct::Vector<string> rpath) {
	lib = lib.setExtension(PLUG_EXT);
	if(lib.isAbsolute() || !rpath)
		return link(lib);
	else {
		for(int i = 0; i < rpath.count(); i++) {
			void *handle = link(sys::Path(rpath[i]) / lib);
			if(handle)
				return handle;
		}
		return 0;
	}
}


/**
 * Look for a symbol in the given library.
 * @param handle	Handle of the library to look in.
 * @param name		Name of the looked symbol.
 * @return			Address of the symbol or null.
 */
void *Plugger::lookSymbol(void *handle, cstring name) {
#	if defined(__WIN32) || defined(__WIN64)
		return (void *)(GetProcAddress(reinterpret_cast<HINSTANCE&>(handle), &name));
#	elif defined(WITH_LIBTOOL)
		return lt_dlsym((lt_dlhandle)handle, &name);
#	else
		return dlsym(handle, &name);
#	endif
}


/**
 * Plug the given file in the plugger.
 * @param path	Path of file to plug.
 * @return		Plugin or null if there is an error.
 */
Plugin *Plugger::plugFile(sys::Path path) {
	err = OK;

	// look for ELD file
	sys::Path ppath = path;
	if(ppath.extension() == PLUG_EXT)
		ppath = ppath.setExtension(ELD_EXT);
	else if(ppath.extension() != ELD_EXT)
		ppath = _ << ppath << "." << ELD_EXT;
	try {
		AutoDestructor<ini::File> file(ini::File::load(ppath));
		ini::Section *sect = file->get(SECTION_NAME);
		if(sect) {

			// just renaming
			sys::Path npath = sect->get(PATH_ATT);
			if(npath)
				return plugFile(evaluate(ppath, npath).setExtension(PLUG_EXT));

			// pre-link other plugins
			genstruct::Vector<string> deps;
			sect->getList(DEPS_ATT, deps);
			for(int i = 0; i < deps.count(); i++) {
				if(!plug(deps[i])) {
					onError(level_error, _ << "cannot plug " << deps[i]);
					return 0;
				}
			}

			// pre-link libraries
			genstruct::Vector<string> libs;
			sect->getList(LIBS_ATT, libs);
			if(libs) {

				// add the RPATH if any
				genstruct::Vector<string> rpaths;
				sect->getList(RPATH_ATT, rpaths);
				for(int i = 0; i < rpaths.count(); i++)
					rpaths[i] = evaluate(ppath, rpaths[i]);

				// link the libraries
				for(int i = 0; i < libs.count(); i++)
					if(!lookLibrary(evaluate(ppath, libs[i]), rpaths)) {
						onError(level_error, _ << "cannot link " << libs[i]);
						return 0;
					}
			}
		}
	}
	catch(ini::Exception& e) {
	}

	// Check existence of the file
	sys::FileItem *file = 0;
	try {
		file = sys::FileItem::get(path);
	}
	catch(SystemException& exn) {
	}
#	if defined(__WIN32) || defined(__WIN64)
		if(!file) {
			Path rpath = path;
			rpath = rpath.setExtension("link");
			file = sys::FileItem::get(rpath);
			if(file) {
				file->release();
				file = 0;
				io::InStream *in = 0;
				try {
					in = System::readFile(rpath);
					io::Input input(*in);
					String npath;
					input >> npath;
					path = rpath.parent() / npath;
					file = sys::FileItem::get(path);
				}
				catch(io::IOException& e) {
					onError(level_error, e.message());
					if(in)
						delete in;
				}
				
			}
		}
#	endif
	if(!file) {
		err = NO_PLUGIN;
		return 0;
	}
	file->release();

	// Open shared library
	/*#if defined(__WIN32) || defined(__WIN64)
		void *handle = LoadLibrary(&path);
	#elif defined(WITH_LIBTOOL)
		void *handle = lt_dlopen(&path);
	#else
		void *handle = dlopen(&path, RTLD_LAZY);
	#endif*/
	void *handle = link(path);
	if(!handle) {
		err = BAD_PLUGIN;
		onError(level_warning, _ << "invalid plugin found at \"" << path << "\" (no handle): "
#			if defined(__WIN32) || defined(__WIN64)
				<< GetLastError());
#			elif defined(WITH_LIBTOOL)
				<< lt_dlerror());
#			else
				<< dlerror());
#			endif
		return 0;
	}

	// Look for the plugin symbol
	/*#if defined(__WIN32) || defined(__WIN64)
		Plugin *plugin = (Plugin *)GetProcAddress(reinterpret_cast<HINSTANCE&>(handle),&_hook);
	#elif defined(WITH_LIBTOOL)
		Plugin *plugin = (Plugin *)lt_dlsym((lt_dlhandle)handle, &_hook);
	#else
		Plugin *plugin = (Plugin *)dlsym(handle, &_hook);
	#endif*/
	Plugin *plugin = static_cast<Plugin *>(lookSymbol(handle, &_hook));
	if(!plugin) {
		err = NO_HOOK;
		onError(level_warning, _ << "invalid plugin found at \"" << path << "\" (no hook)");
		return 0;
	}

	// Check the magic
	if(plugin->magic != Plugin::MAGIC) {
		err = NO_MAGIC;
		onError(level_warning, _ << "invalid plugin found at \"" << path << "\" (bad magic)");
		return 0;
	}

	// Check plugger version
	if(!per_vers.accepts(plugin->pluggerVersion())) {
		err = BAD_VERSION;
		onError(level_warning, _ << "bad version plugin found at \"" << path << "\" (required: " << per_vers << ", provided: " << plugin->pluggerVersion() << ")");
		return 0;
	}

	// Plug it
	plugin->setPath(path);
	return plug(plugin, handle);
}


/**
 * @fn error_t Plugger::lastError(void);
 * Get the last error.
 * @return	Last error.
 */
Plugger::error_t Plugger::lastError(void) {
	DEPRECATED
	return err;
}


/**
 * Get the message for the last error.
 * @return	Error message.
 * @deprecated
 */
String Plugger::lastErrorMessage(void) {
	DEPRECATED
	return getLastError();
}


/**
 * Return the last produced error.
 * @return		Last error message.
 * @deprecated
 */
string Plugger::getLastError() {
	switch(err) {
	case OK:
		return "Success.";
	case BAD_PLUGIN: {
			StringBuffer buf;
			#if defined(__WIN32) || defined(__WIN64)
				buf << "cannot open the plugin(" << GetLastError() << ").";
			#elif defined(WITH_LIBTOOL)
				const char *msg = lt_dlerror();
				buf << "cannot open the plugin(" << (msg ? msg : "") << ").";
			#else
				buf << "cannot open the plugin(" << dlerror() << ").";
			#endif
			return buf.toString();
		}
	case NO_PLUGIN:
		return "cannot find any plugin matching the given name";
	case NO_HOOK:
		return "Found plugin does not contain a hook symbol.";
	case BAD_VERSION:
		return "Found plug-in is incompatible.";
	case NO_MAGIC:
		return "Found plugin does not match the plugin signature.";
	default:
		ASSERTP(0, "unknown error");
		return "";
	}
}


/**
 * This method is called when an error arises to let the user display or not
 * the message. As default, the message is displayed on standard error.
 * @param message	Message of the error.
 * @deprecated
 */
void Plugger::onError(String message) {
	cerr << "ERROR: " << message << io::endl;
}


/**
 * Generate an error for the error handler.
 * @param level		Level of error.
 * @param message	Error message.
 */
void Plugger::onError(error_level_t level, const string& message) {
	ErrorBase::onError(level, message);
	if(level == level_error)
		onError(message);
	else if(level == level_warning)
		onWarning(message);
}


/**
 * This method is called when a warning arises to let the user display or not
 * the message. As default, the message is displayed on standard error.
 * @param message	Message of the warning.
 * @deprecated
 */
void Plugger::onWarning(String message) {
	cerr << "WARNING: " << message << io::endl;
}


/**
 * @class Plugger::Iterator
 * Used for exploring plugins hookable on the current plugger.
 */


/**
 */
void Plugger::Iterator::go(void) {

	// Look in statics
	if(i < statics.count()) {
		i++;
		while(i < statics.count()) {
			if(statics[i]->hook() == plugger.hook())
				return;
			i++;
		}
	}

	// Look in files
	while(true) {

		// Next file
		if(file)
			file->next();

		// Next path
		if(!file || file->ended()) {
			if(file) {
				delete file;
				file = 0;
			}
			_path++;
			if(_path >= plugger.paths.count())
				break;
			FileItem *item = FileItem::get(Path(plugger.paths[_path]));
			if(!item || !item->toDirectory())
				continue;
			else {
				file = new Directory::Iterator(item->toDirectory());
				if(file->ended())
					continue;
			}
		}

		// Look current file
#if defined(WITH_LIBTOOL)
		if(file->item()->path().toString().endsWith(".la"))
#elif defined(__APPLE__)
		if(file->item()->path().toString().endsWith(".dylib"))
#elif defined(__unix)
		if(file->item()->path().toString().endsWith(".so"))
#elif defined(__WIN32) || defined(__WIN64)
		if(file->item()->path().toString().endsWith(".dll"))
#endif
			break;
	}
}


/**
 * Build a new iterator.
 * @param plugger	Used plugger.
 */
Plugger::Iterator::Iterator(Plugger& _plugger)
:	plugger(_plugger),
	statics(_plugger.statics()),
	i(-1),
	_path(-1),
	file(0)
{
	go();
}


/**
 */
Plugger::Iterator::~Iterator(void) {
	if(file)
		delete file;
}


/**
 * Test if the iteration is ended.
 * @return	True if it is ended.
 */
bool Plugger::Iterator::ended(void) const {
	return _path >= plugger.paths.count();
}


/**
 * Get the current plugin name.
 * @return	Current plugin name.
 */
String Plugger::Iterator::item(void) const {
	if(i < statics.count())
		return statics[i]->name();
	else {
		Path path = (*file)->path();
		String name = path.namePart();
		name = name.substring(0, name.length() - 3);
		return name;
	}
}


/**
 * Get the path of the current plug-in.
 * @return	Plug-in path.
 */
Path Plugger::Iterator::path(void) const {
	if(i < statics.count())
		return "<static>";
	else
		return (*file)->path();
}


/**
 * Go to the next plugin.
 */
void Plugger::Iterator::next(void) {
	go();
}


/**
 * Plug the current plugin.
 * @return	Matching plugin.
 */
Plugin *Plugger::Iterator::plug(void) const {
	if(i < statics.count())
		return plugger.plug(statics[i], 0);
	else
		return plugger.plugFile(file->item()->path().toString());
}

} }	// elm::sys
