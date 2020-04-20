#include "cstdio.h"

using namespace shim;

bionic::FILE bionic::standard_files[3];
int bionic::io_isthreaded = 1;

void bionic::init_standard_files() {
    standard_files[0]._p = "stdin";
    standard_files[0].wrapped = stdin;
    standard_files[1]._p = "stdout";
    standard_files[1].wrapped = stdout;
    standard_files[2]._p = "stderr";
    standard_files[2].wrapped = stderr;
}

static bionic::FILE *wrap_file(::FILE *file) {
    if (!file)
        return nullptr;

    auto ret = new bionic::FILE;
    ret->wrapped = file;
    ret->_p = "Internal";
    ret->_r = ret->_w = 0;
#if defined(__LP64__)
    ret->_file = fileno(file);
#else
    ret->_file = (short) fileno(file);
#endif
    ret->_flags = 0;
    return ret;
}

bionic::FILE *shim::fopen(const char *filename, const char *mode) {
    return wrap_file(::fopen(filename, mode));
}

bionic::FILE* shim::fdopen(int fd, const char *mode) {
    return wrap_file(::fdopen(fd, mode));
}

bionic::FILE* shim::freopen(const char *filename, const char *mode, bionic::FILE *stream) {
    return wrap_file(::freopen(filename, mode, stream->wrapped));
}

bionic::FILE* shim::tmpfile() {
    return wrap_file(::tmpfile());
}

bionic::FILE* shim::popen(const char *command, const char *mode) {
    return wrap_file(::popen(command, mode));
}

int shim::fclose(bionic::FILE *file) {
    int ret = fclose(file->wrapped);
    if (file == &bionic::standard_files[0] ||
        file == &bionic::standard_files[1] ||
        file == &bionic::standard_files[2])
        return ret;
    delete file;
    return ret;
}

int shim::pclose(bionic::FILE *file) {
    int ret = ::pclose(file->wrapped);
    delete file;
    return ret;
}

int shim::fprintf(bionic::FILE *fp, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vfprintf(fp->wrapped, fmt, args);
    va_end(args);
    return ret;
}

int shim::fscanf(bionic::FILE *fp, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vfscanf(fp->wrapped, fmt, args);
    va_end(args);
    return ret;
}

void shim::add_cstdio_shimmed_symbols(std::vector<shim::shimmed_symbol> &list) {
    bionic::init_standard_files();

    list.insert(list.end(), {
        {"__sF", (void*) &bionic::standard_files},
        {"__isthreaded", (void*) &bionic::io_isthreaded},

        {"fopen", fopen},
        {"fdopen", fdopen},
        {"freopen", freopen},
        {"tmpfile", tmpfile},
        {"popen", popen},
        {"fclose", fclose},
        {"pclose", pclose},
        {"clearerr", AutoArgRewritten(::clearerr)},
        {"feof", AutoArgRewritten(::feof)},
        {"fseek", AutoArgRewritten(::fseek)},
        {"ftell", AutoArgRewritten(::ftell)},
        {"ferror", AutoArgRewritten(::ferror)},
        {"ferror", AutoArgRewritten(::ferror)},
        {"fflush", AutoArgRewritten(::fflush)},
        {"fgetc", AutoArgRewritten(::fgetc)},
        {"fgets", AutoArgRewritten(::fgets)},
        {"fputc", AutoArgRewritten(::fputc)},
        {"fputs", AutoArgRewritten(::fputs)},
        {"fread", AutoArgRewritten(::fread)},
        {"fwrite", AutoArgRewritten(::fwrite)},
        {"fprintf", fprintf},
        {"vfprintf", AutoArgRewritten(::vfprintf)},
        {"fscanf", fscanf},
        {"vfscanf", AutoArgRewritten(::vfscanf)},
        {"getc", AutoArgRewritten(::getc)},
        {"getc_unlocked", AutoArgRewritten(::getc_unlocked)},
        {"getdelim", AutoArgRewritten(::getdelim)},
        {"getline", AutoArgRewritten(::getline)},
        {"putc", AutoArgRewritten(::putc)},
        {"putc_unlocked", AutoArgRewritten(::putc_unlocked)},
        {"rewind", AutoArgRewritten(::rewind)},
        {"setbuf", AutoArgRewritten(::setbuf)},
        {"setvbuf", AutoArgRewritten(::setvbuf)},
        {"setbuffer", AutoArgRewritten(::setbuffer)},
        {"setlinebuf", AutoArgRewritten(::setlinebuf)},
        {"ungetc", AutoArgRewritten(::ungetc)},
        {"fileno", AutoArgRewritten(::fileno)},
        {"pclose", AutoArgRewritten(::pclose)},
        {"flockfile", AutoArgRewritten(::flockfile)},
        {"ftrylockfile", AutoArgRewritten(::ftrylockfile)},
        {"funlockfile", AutoArgRewritten(::funlockfile)},
        {"getw", AutoArgRewritten(::getw)},
        {"putw", AutoArgRewritten(::putw)},

        {"remove", ::remove},
        {"rename", ::rename},

        {"putchar", ::putchar},
        {"puts", ::puts},
        {"vprintf", ::vprintf},
        {"sprintf", ::sprintf},
        {"asprintf", ::asprintf},
        {"vasprintf", ::vasprintf},
        {"snprintf", ::snprintf},
        {"vsprintf", ::vsprintf},
        {"vsnprintf", ::vsnprintf},
    });
}