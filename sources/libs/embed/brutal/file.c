#include <brutal/debug.h>
#include <embed/file.h>

MaybeError embed_file_open(IoFile *, Str, FileInitFlags)
{
    panic$("embed_file_open not implemented");
}

MaybeError embed_file_create(IoFile *, Str)
{
    panic$("embed_file_create not implemented");
}

MaybeError embed_file_close(IoFile *)
{
    panic$("embed_file_close not implemented");
}

IoResult embed_file_seek(IoFile *, IoSeek)
{
    panic$("embed_file_seek not implemented");
}

IoResult embed_file_read(IoFile *, uint8_t *, size_t)
{
    panic$("embed_file_read not implemented");
}

IoResult embed_file_write(IoFile *, uint8_t const *, size_t)
{
    panic$("embed_file_write not implemented");
}