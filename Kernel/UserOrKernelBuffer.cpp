/*
 * Copyright (c) 2020, the SerenityOS developers.
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/UserOrKernelBuffer.h>

namespace Kernel {

bool UserOrKernelBuffer::is_kernel_buffer() const
{
    return !Memory::is_user_address(VirtualAddress(m_buffer));
}

KResultOr<NonnullOwnPtr<KString>> UserOrKernelBuffer::try_copy_into_kstring(size_t size) const
{
    if (!m_buffer)
        return EINVAL;
    if (Memory::is_user_address(VirtualAddress(m_buffer))) {
        char* buffer;
        auto kstring = TRY(KString::try_create_uninitialized(size, buffer));
        TRY(copy_from_user(buffer, m_buffer, size));
        return kstring;
    }

    return KString::try_create(ReadonlyBytes { m_buffer, size });
}

bool UserOrKernelBuffer::write(const void* src, size_t offset, size_t len)
{
    if (!m_buffer)
        return false;

    if (Memory::is_user_address(VirtualAddress(m_buffer)))
        return copy_to_user(m_buffer + offset, src, len).is_success();

    memcpy(m_buffer + offset, src, len);
    return true;
}

bool UserOrKernelBuffer::read(void* dest, size_t offset, size_t len) const
{
    if (!m_buffer)
        return false;

    if (Memory::is_user_address(VirtualAddress(m_buffer)))
        return copy_from_user(dest, m_buffer + offset, len).is_success();

    memcpy(dest, m_buffer + offset, len);
    return true;
}

bool UserOrKernelBuffer::memset(int value, size_t offset, size_t len)
{
    if (!m_buffer)
        return false;

    if (Memory::is_user_address(VirtualAddress(m_buffer)))
        return memset_user(m_buffer + offset, value, len).is_success();

    ::memset(m_buffer + offset, value, len);
    return true;
}

}
