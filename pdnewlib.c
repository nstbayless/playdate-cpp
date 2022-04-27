#include "pd_api.h"

#if TARGET_PLAYDATE
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#ifdef errno
	#undef errno
#endif
extern int errno;

static PlaydateAPI* pd;

static int _is_init = 0;

__attribute__((constructor))
static void _init(void)
{
	_is_init = 1;
}

int eventHandler_pdnewlib(PlaydateAPI* _pd, PDSystemEvent event, uint32_t arg)
{
	if (event == kEventInit)
	{
		pd = _pd;
		if (!_is_init)
		{
			pd->system->error("Error: .init is not implemented.\nIf you are a developer, please ensure that the event handler in setup.c is up-to-date and implements .init_array execution.");
		}
	}
	return 0;
}

void _exit(int code)
{
	while (1)
	{
		pd->system->error("exited with code %d.", code);
	}
}

void __exit(int code)
{
	_exit(code);
}

int _kill(int pid, int sig)
{
	return 0;
}
int _getpid(void)
{
	return 1;
}

#define HANDLE_STDIN 0
#define HANDLE_STDOUT 1
#define HANDLE_STDERR 2

#define MAXFILES 32
#define FILEHANDLEOFF 3
SDFile* openfiles[MAXFILES];

typedef struct {
	char* v;
	size_t c;
	const char* fmt;
} buf_t;

// stdout, stderr
buf_t iobuffs[2] = {{NULL, 0, NULL}, {NULL, 0, "\e[0;31m%s\e[0m"}};

static void write_buf(buf_t* wbuf, char* data, int size)
{
	wbuf->v = realloc(wbuf->v, wbuf->c + size + 1);
	memcpy(wbuf->v + wbuf->c, data, size);
	wbuf->c += size;
	wbuf->v[wbuf->c] = 0;
}

// logs as far as the last newline.
static void log_buf_to_console(buf_t* buf)
{
	char* start = buf->v;
	while (1)
	{
		char* end = strchr(start, '\n');
		if (end == NULL)
		{
			break;
		}
		*end = 0;
		pd->system->logToConsole(buf->fmt ? buf->fmt : "%s", start);
		start = end + 1;
	}
	if (start != buf->v)
	{
		size_t newsize = buf->c + buf->v - start;
		memcpy(buf->v, start, newsize);
		buf->c = newsize;
		buf->v = realloc(buf->v, newsize ? newsize + 1 : 0);
	}
}

int _wait(int *status) {
  errno = ECHILD;
  return -1;
}

int _fork(void) {
  errno = ENOMEM;
  return -1;
}

int _execve(char *name, char **argv, char **env)
{
  errno = ENOMEM;
  return -1;
}

int _write(int handle, char* data, int size)
{
	if (size == 0 || data == NULL) return 0;
	
	if (handle == HANDLE_STDOUT || handle == HANDLE_STDERR)
	{
		buf_t* wbuf = &iobuffs[handle == HANDLE_STDERR];
		write_buf(wbuf, data, size);
		log_buf_to_console(wbuf);
		return size;
	}
	else if (handle >= FILEHANDLEOFF && handle < FILEHANDLEOFF + MAXFILES)
	{
		SDFile* file = openfiles[handle - FILEHANDLEOFF];
		
		if (file)
		{
			int s = pd->file->write(file, data, size);
			if (s < 0)
			{
				// TODO: errno
				return -1;
			}
			return s;
		}
	}
	
	// TODO: errno (no such open file)
	return -1;	
}

int _read(int file, char* ptr, int len)
{
	return 0;
}

int _open(const char *name, int flags, int mode)
{
  	for (size_t i = 0; i < MAXFILES; ++i)
	{
		if (!openfiles[i])
		{
			// TODO: convert mode
			openfiles[i] = pd->file->open(name, mode);
			if (openfiles[i] == NULL)
			{
				errno = ENOENT;
				return -1;
			}
			
			return FILEHANDLEOFF + i;
		}
	}
	
	errno = ENFILE;
	return -1;
}

int _close(int file)
{
	if (file >= FILEHANDLEOFF && file < FILEHANDLEOFF + MAXFILES)
	{
		file -= FILEHANDLEOFF;
		SDFile* f = openfiles[file];
		if (f != NULL)
		{
			if (pd->file->close(f))
			{
				// TODO: errno
				return -1;
			}
			else
			{
				openfiles[file] = NULL;
				return 0;
			}
		}
	}
	
	// TODO: errno (no such file handle)
	return -1;
}

int _mkdir(char* dir)
{
	return pd->file->mkdir(dir);
}

int _rename(char* src, char* dst)
{
	if (pd->file->rename(src, dst))
	{
		errno = ENOENT;
		return -1;
	}
	return 0;
}

int _unlink(char* p)
{
	if (pd->file->unlink(p, 1))
	{
		errno = ENOENT;
		return -1;
	}
	return 0;
}

/*
int _link(char* old, char* new) {
	// TODO
	errno = EMLINK;
	return -1;
}
*/

int _isatty(int file)
{
	return file >= 0 && file <= HANDLE_STDERR;
}

int _lseek(int file, int pos, int whence) {
	if (file < FILEHANDLEOFF || file >= MAXFILES + FILEHANDLEOFF)
	{
		SDFile* f = openfiles[file - FILEHANDLEOFF];
		if (f)
		{
			if (pd->file->seek(f, pos, whence))
			{
				// TODO: errno
				return -1;
			}
			return 0;
		}
	}
	
	// TODO: errno (no such file)
	return -1;
}

int _fstat(int file, struct stat *st) {
	memset(stat, 0, sizeof(stat));
	if (_isatty(file))
	{
    	st->st_mode = S_IFCHR;
	}
	else
	{
		// TODO: get filepath and then call _stat(...)
		errno = ENFILE;
		return -1;
	}
    return 0;
}

int _stat(char *file, struct stat *st) {
	memset(stat, 0, sizeof(stat));
	FileStat pdstat;
	
	if (pd->file->stat(file, &pdstat))
	{
		errno = ENOENT;
		return -1;
	}
	
	st->st_mode = pdstat.isdir ? S_IFDIR : S_IFREG;
	st->st_size = pdstat.size;
	st->st_blksize = 512;
	st->st_blocks = (pdstat.size + st->st_blksize - 1) / st->st_blksize;
	st->st_atime = (0); // TODO
	st->st_mtime = st->st_atime;
	st->st_ctime = st->st_ctime;
  	return 0;
}

static char *__env[1] = { 0 };
char **_environ = __env;

#else

int eventHandler_pdnewlib(PlaydateAPI* p, PDSystemEvent e, uint32_t a)
{
	return 0;
}

#endif