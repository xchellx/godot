#include "dir_access_wii.h"

#include <unistd.h>
#include <errno.h>
#include <stdio.h>

Error DirAccessWii::list_dir_begin()
{
    list_dir_end(); //close any previous dir opening!

	//char real_current_dir_name[2048]; //is this enough?!
	//getcwd(real_current_dir_name,2048);
	//chdir(current_path.utf8().get_data());
	dir_stream = opendir(current_dir.utf8().get_data());
	//chdir(real_current_dir_name);
	if (!dir_stream)
		return ERR_CANT_OPEN; //error!

	return OK;
}

bool DirAccessWii::file_exists(String p_file) {

	GLOBAL_LOCK_FUNCTION

	if (p_file.is_rel_path())
		p_file = current_dir.plus_file(p_file);

	p_file = fix_path(p_file);

	struct stat flags;
	bool success = (stat(p_file.utf8().get_data(), &flags) == 0);

	if (success && S_ISDIR(flags.st_mode)) {
		success = false;
	}

	return success;
}

bool DirAccessWii::dir_exists(String p_dir) {

	GLOBAL_LOCK_FUNCTION

	if (p_dir.is_rel_path())
		p_dir = get_current_dir().plus_file(p_dir);

	p_dir = fix_path(p_dir);

	struct stat flags;
	bool success = (stat(p_dir.utf8().get_data(), &flags) == 0);

	return (success && S_ISDIR(flags.st_mode));
}

uint64_t DirAccessWii::get_modified_time(String p_file) {

	if (p_file.is_rel_path())
		p_file = current_dir.plus_file(p_file);

	p_file = fix_path(p_file);

	struct stat flags;
	bool success = (stat(p_file.utf8().get_data(), &flags) == 0);

	if (success) {
		return flags.st_mtime;
	} else {

		ERR_FAIL_V(0);
	};
	return 0;
};

String DirAccessWii::get_next() {

	if (!dir_stream)
		return "";

	dirent *entry = readdir(dir_stream);

	if (entry == NULL) {
		list_dir_end();
		return "";
	}

	String fname = fix_unicode_name(entry->d_name);

	// Look at d_type to determine if the entry is a directory, unless
	// its type is unknown (the file system does not support it) or if
	// the type is a link, in that case we want to resolve the link to
	// known if it points to a directory. stat() will resolve the link
	// for us.
	if (entry->d_type == DT_UNKNOWN || entry->d_type == DT_LNK) {
		String f = current_dir.plus_file(fname);

		struct stat flags;
		if (stat(f.utf8().get_data(), &flags) == 0) {
			_cisdir = S_ISDIR(flags.st_mode);
		} else {
			_cisdir = false;
		}
	} else {
		_cisdir = (entry->d_type == DT_DIR);
	}

	_cishidden = (fname != "." && fname != ".." && fname.begins_with("."));

	return fname;
}

bool DirAccessWii::current_is_dir() const {

	return _cisdir;
}

bool DirAccessWii::current_is_hidden() const {

	return _cishidden;
}

void DirAccessWii::list_dir_end() {

	if (dir_stream)
		closedir(dir_stream);
	dir_stream = 0;
	_cisdir = false;
}

int DirAccessWii::get_drive_count()
{
    return 0;
}

String DirAccessWii::get_drive(int p_drive)
{
    return "";
}

Error DirAccessWii::change_dir(String p_dir)
{
    GLOBAL_LOCK_FUNCTION

    p_dir = fix_path(p_dir);

	// prev_dir is the directory we are changing out of
	String prev_dir;
	char real_current_dir_name[2048];
	ERR_FAIL_COND_V(getcwd(real_current_dir_name, 2048) == NULL, ERR_BUG);
	if (prev_dir.parse_utf8(real_current_dir_name))
		prev_dir = real_current_dir_name; //no utf8, maybe latin?

	// try_dir is the directory we are trying to change into
	String try_dir = "";
	if (p_dir.is_rel_path()) {
		String next_dir = current_dir.plus_file(p_dir);
		next_dir = next_dir.simplify_path();
		try_dir = next_dir;
	} else {
		try_dir = p_dir;
	}

	bool worked = (chdir(try_dir.utf8().get_data()) == 0); // we can only give this utf8
	if (!worked) {
		return ERR_INVALID_PARAMETER;
	}

	String base = _get_root_path();
	if (base != String() && !try_dir.begins_with(base)) {
		ERR_FAIL_COND_V(getcwd(real_current_dir_name, 2048) == NULL, ERR_BUG);
		String new_dir;
		new_dir.parse_utf8(real_current_dir_name);

		if (!new_dir.begins_with(base)) {
			try_dir = current_dir; //revert
		}
	}

	// the directory exists, so set current_dir to try_dir
	current_dir = try_dir;
	ERR_FAIL_COND_V(chdir(prev_dir.utf8().get_data()) != 0, ERR_BUG);
	return OK;
}

String DirAccessWii::get_current_dir()
{
    String base = _get_root_path();
	if (base != "") {

		String bd = current_dir.replace_first(base, "");
		if (bd.begins_with("/"))
			return _get_root_string() + bd.substr(1, bd.length());
		else
			return _get_root_string() + bd;
	}
	return current_dir;
}

Error DirAccessWii::make_dir(String p_dir)
{
    GLOBAL_LOCK_FUNCTION

	if (p_dir.is_rel_path())
		p_dir = get_current_dir().plus_file(p_dir);

	p_dir = fix_path(p_dir);

	bool success = (mkdir(p_dir.utf8().get_data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0);
	int err = errno;

	if (success) {
		return OK;
	};

	if (err == EEXIST) {
		return ERR_ALREADY_EXISTS;
	};

	return ERR_CANT_CREATE;
}

size_t DirAccessWii::get_space_left() {
	struct statvfs vfs;
	if (statvfs(current_dir.utf8().get_data(), &vfs) != 0) {
		return 0;
	};

	return vfs.f_bfree * vfs.f_bsize;
};

Error DirAccessWii::rename(String p_path, String p_new_path)
{
    if (p_path.is_rel_path())
		p_path = get_current_dir().plus_file(p_path);

	p_path = fix_path(p_path);

	if (p_new_path.is_rel_path())
		p_new_path = get_current_dir().plus_file(p_new_path);

	p_new_path = fix_path(p_new_path);

	return ::rename(p_path.utf8().get_data(), p_new_path.utf8().get_data()) == 0 ? OK : FAILED;
}

Error DirAccessWii::remove(String p_path) {

	if (p_path.is_rel_path())
		p_path = get_current_dir().plus_file(p_path);

	p_path = fix_path(p_path);

	struct stat flags;
	if ((stat(p_path.utf8().get_data(), &flags) != 0))
		return FAILED;

	if (S_ISDIR(flags.st_mode))
		return ::rmdir(p_path.utf8().get_data()) == 0 ? OK : FAILED;
	else
		return ::unlink(p_path.utf8().get_data()) == 0 ? OK : FAILED;
}

String DirAccessWii::get_filesystem_type() const {
	return ""; //TODO this should be implemented
}

DirAccessWii::DirAccessWii() {

	dir_stream = 0;
	_cisdir = false;

	/* determine drive count */

	// set current directory to an absolute path of the current directory
	char real_current_dir_name[2048];
	ERR_FAIL_COND(getcwd(real_current_dir_name, 2048) == NULL);
	if (current_dir.parse_utf8(real_current_dir_name))
		current_dir = real_current_dir_name;

	change_dir(current_dir);
}

DirAccessWii::~DirAccessWii() {

	list_dir_end();
}