#pragma once

#include <windows.h>
#include <cstdint>

constexpr DWORD ioctl_map_phys = 0x80102040;
constexpr DWORD ioctl_unmap_phys = 0x80102044;

struct phys_mem_buf_t {
    uint64_t mapping_size;
    uint64_t physical_address;
    uint64_t section_handle;
    uint64_t virtual_address;
    uint64_t section_object;
};
static_assert(sizeof(phys_mem_buf_t) == 40);

class axeapi3_t {
public:
    axeapi3_t() {
        handle_ = ::CreateFileW(
            L"\\\\.\\AxEapi",
            GENERIC_READ | GENERIC_WRITE,
            0, nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
    }

    ~axeapi3_t() {
        if (handle_ != INVALID_HANDLE_VALUE)
            ::CloseHandle(handle_);
    }

    [[nodiscard]] bool is_open() const noexcept {
        return handle_ != INVALID_HANDLE_VALUE;
    }

    bool map_physical(phys_mem_buf_t& buf) const {
        DWORD bytes = 0;
        return ::DeviceIoControl(
            handle_, ioctl_map_phys,
            &buf, sizeof(buf),
            &buf, sizeof(buf),
            &bytes, nullptr) != FALSE;
    }

    bool unmap_physical(const phys_mem_buf_t& buf) const {
        DWORD bytes = 0;
        return ::DeviceIoControl(
            handle_, ioctl_unmap_phys,
            const_cast<phys_mem_buf_t*>(&buf), sizeof(buf),
            nullptr, 0,
            &bytes, nullptr) != FALSE;
    }

private:
    HANDLE handle_ = INVALID_HANDLE_VALUE;
};