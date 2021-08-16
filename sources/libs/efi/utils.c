#include "utils.h"
#include "brutal/base/error.h"
#include "efi/file.h"
#include "efi/lip.h"
#include "efi/st.h"
#include "misc.h"

EFISystemTable *st;
EFIHandle image_handle;

static EFIFileProtocol *rootdir;
static EFISimpleFileSystemProtocol *rootfs;

static File current_file;

void init_lib(EFISystemTable *st2, EFIHandle image_handle2)
{
    st = st2;
    image_handle = image_handle2;
}

static EFIStatus get_rootdir(EFISimpleFileSystemProtocol *rootfs, EFIFileProtocol **rootdir)
{
    return rootfs->open_volume(rootfs, rootdir);
}

static EFIStatus get_rootfs(EFIHandle loader, EFISystemTable *system, EFIHandle device, EFISimpleFileSystemProtocol **rootfs)
{
    EFIGUID guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    return system->boot_services->open_protocol(
        device,
        &guid,
        (void **)rootfs,
        loader,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
}

static EFIStatus get_loader_image(EFIHandle loader, EFISystemTable *system, EFILoadedImage **image)
{
    EFIGUID guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    return system->boot_services->open_protocol(
        loader,
        &guid,
        (void **)image,
        loader,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
}

File open_file(u16 *path)
{
    File f;

    f.status = 0;

    EFILoadedImage *image;

    get_loader_image(image_handle, st, &image);
    get_rootfs(image_handle, st, image->device_handle, &rootfs);
    get_rootdir(rootfs, &rootdir);

    /* Open the file as readonly */
    f.status = rootdir->open(rootdir, &f.efi_file, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

    current_file = f;

    return f;
}

EFIFileInfo get_file_info(File *file)
{

    EFIGUID guid = EFI_FILE_INFO_ID;
    u64 info_size = sizeof(file->info);

    file->efi_file->get_info(file->efi_file, &guid, &info_size, &file->info);

    return file->info;
}

void read_file(File *file, void *buffer)
{

    u64 file_size = file->info.file_size;

    file->buffer = buffer;

    file->status = file->efi_file->read(file->efi_file, &file_size, buffer);
}

void close_file(File *file)
{

    file->status = rootdir->close(file->efi_file);
}

Error host_mem_acquire(size_t size, void **out_result, MAYBE_UNUSED enum host_mem_flag flags)
{
    auto status = st->boot_services->allocate_pool(EFI_BOOT_SERVICES_DATA, size, out_result);

    if (status != EFI_SUCCESS)
    {
        return ERR_UNDEFINED;
    }

    return ERR_SUCCESS;
}

Error host_mem_release(void *addr, MAYBE_UNUSED size_t size)
{
    st->boot_services->free_pool(addr);

    return ERR_SUCCESS;
}

void host_mem_lock(void)
{
    return;
}

void host_mem_unlock(void)
{
    return;
}

static void *_ptr;

void *to_utf16(char *buffer)
{
    _ptr = alloc_malloc(alloc_global(), strlen(buffer) << 1);

    memset(_ptr, 0, sizeof(*_ptr));

    for (size_t i = 0; i < strlen(buffer); i++)
    {
        *(char16 *)(_ptr + (i << 1)) = buffer[i];
    }

    return _ptr;
}

void set_attribute(u64 attribute)
{
    st->console_out->set_attribute(st->console_out, attribute);
}
void clear_screen()
{
    st->console_out->clear_screen(st->console_out);
}

IoWriteResult efi_write(MAYBE_UNUSED IoWriter *writer, char const *text, size_t size)
{

    u16 *ptr = (u16 *)to_utf16((char *)text);

    ptr[u16strlen(ptr)] = '\0';

    st->console_out->output_string(st->console_out, ptr);

    return OK(IoWriteResult, size);
}

static IoWriter efi_writer;

IoWriter *host_log_writer()
{
    efi_writer.write = efi_write;

    return &efi_writer;
}

void host_log_lock() { return; }

void host_log_unlock() { return; }

void efi_print_impl(Str fmt, struct print_args args)
{
    print_impl(host_log_writer(), fmt, args);
    print(host_log_writer(), "\r\n");
}

void host_log_panic()
{
    while (1)
        ;
}

u64 read_unaligned64(const u64 *buffer)
{
    return *buffer;
}

bool compare_guids(EFIGUID *guid1, EFIGUID *guid2)
{

    u64 low_guid1 = read_unaligned64((const u64 *)guid1);
    u64 low_guid2 = read_unaligned64((const u64 *)guid2);
    u64 high_guid1 = read_unaligned64((const u64 *)guid1 + 1);
    u64 high_guid2 = read_unaligned64((const u64 *)guid2 + 1);

    return (low_guid1 == low_guid2 && high_guid1 == high_guid2);
}

EFIStatus get_system_config_table(EFIGUID *table_guid, void **table)
{

    for (u64 i = 0; i < st->num_table_entries; i++)
    {
        if (compare_guids(table_guid, &st->config_table[i].vendor_guid))
        {
            *table = st->config_table[i].vendor_table;
            return EFI_SUCCESS;
        }
    }

    return EFI_NOT_FOUND;
}
