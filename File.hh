//
// Copyright 2009-2017 M. Shulhan (ms@kilabit.info). All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#ifndef _LIBVOS_FILE_HH
#define _LIBVOS_FILE_HH 1

#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include "Buffer.hh"

using vos::Buffer;

namespace vos {

extern Error ErrFileEmpty;
extern Error ErrFileEnd;
extern Error ErrFileExist;
extern Error ErrFileNameEmpty;
extern Error ErrFileNotFound;
extern Error ErrFileReadOnly;
extern Error ErrFileWriteOnly;

#define LF '\n'
#define CR '\r'

enum file_eol_mode {
	FILE_EOL_NIX	= 0,
	FILE_EOL_DOS	= 1,
	N_FILE_EOL_MODE
};

extern const char* __eol[N_FILE_EOL_MODE];

enum file_open_mode {
	FILE_OPEN_NO	= -1
,	FILE_OPEN_RO	= O_RDONLY			// 0
,	FILE_OPEN_WO	= O_WRONLY			// 1
,	FILE_OPEN_WOCA	= O_WRONLY | O_CREAT | O_APPEND	// 1089
,	FILE_OPEN_WOCT	= O_WRONLY | O_CREAT | O_TRUNC	// 577
,	FILE_OPEN_WOCX	= O_WRONLY | O_CREAT | O_EXCL	// 193
,	FILE_OPEN_RWCA	= O_RDWR | O_CREAT | O_APPEND	// 1090
,	FILE_OPEN_RWCT	= O_RDWR | O_CREAT | O_TRUNC	// 578
,	FILE_OPEN_RWCX	= O_RDWR | O_CREAT | O_EXCL	// 194
,	FILE_OPEN_SOCK	= O_RDWR | O_SYNC		// 1052674
};

enum flush_mode {
	FLUSH_NO	= 0
,	FLUSH_FIRST	= 1
,	FLUSH_LAST	= 2
};

/**
 * Class File represent file in system as object.
 *
 * To walk through buffer use '_p' or for marking the position that you
 * have already checked/processed in buffer. So, '_p' <= '_i' <= '_l'.
 *
 * '_p' is also used to mark the end of line when used to read one line,
 * using get_line() method, from buffer; or, used as pointer, iterating
 * through buffer '_v' from 0 until '_i'.
 *
 * Field DFLT_SIZE define default buffer size for file.
 *
 * Field _d contains file descriptor.
 * Field _p contains file buffer iterator.
 * Field _status contains status of open file (read only, write only, or
 * read-write).
 * Field _perm contains current file permission mode.
 * Field _size contains current file size.
 * Field _eol contains end of line as a character.
 * Field _eols contains end of line as a string.
 * Field _name contains file name, with or without path, depends on how user
 * called at opening it.
 */
class File : public Buffer {
public:
	static const char* __CNAME;
	static uint16_t DFLT_SIZE;

	static Error GET_SIZE(const char* path, off_t* size);
	static int IS_EXIST(const char* path, int access_mode = O_RDWR);
	static Error BASENAME(Buffer* name, const char* path);
	static Error COPY(const char* src, const char* dst);
	static Error TOUCH(const char* file, int mode = FILE_OPEN_WOCA
		, int perm = S_IRUSR | S_IWUSR);
	static Error WRITE_PID(const char* file);

	explicit File(const size_t bfr_size = File::DFLT_SIZE);
	~File();

	void as_stdout();

	Error open(const char* path);
	Error open_ro(const char* path);
	Error open_wo(const char* path);
	Error open_wt(const char* path);
	Error open_wx(const char* path);

	Error truncate(enum flush_mode mode = FLUSH_LAST);

	int is_open();

	void set_eol(enum file_eol_mode mode);

	Error read(size_t n = 0);
	Error get_line(Buffer* line);

	Error write(const Buffer* bfr);
	Error write_raw(const char* bfr, size_t len = 0);
	Error writef(const char* fmt, ...);
	Error writec(const char c);

	int is_readable(fd_set* read_fds, fd_set* all_fds);
	void set_add(fd_set* fds, int* maxfd);
	void set_clear(fd_set* fds);

	Error flush();
	void close();

	int fd();
	const char* name();
	off_t size();
	int status();
	const char* eol();

protected:
	int		_d;
	size_t		_p;
	int		_status;
	int		_perm;
	off_t		_size;
	const char*	_eol;
	Buffer		_name;

	Error refill(size_t read_min = 0);

private:
	File(const File&);
	void operator=(const File&);

	Error open(const char* path, const enum file_open_mode
		, const int perm = S_IRUSR | S_IWUSR);

	off_t get_size();
};

} // namespace::vos
#endif
// vi: ts=8 sw=8 tw=80:
