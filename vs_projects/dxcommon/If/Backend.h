/**
* Backend contracts for the modernized Top Speed engine.
* The legacy DirectX 8 adapter keeps the current simulation behavior intact.
*/
#ifndef __DXCOMMON_BACKEND_H__
#define __DXCOMMON_BACKEND_H__

#include <d3d8.h>

namespace DirectX
{

struct BackendFrameInfo
{
    unsigned int width;
    unsigned int height;
};

class IRenderBackend
{
public:
    virtual ~IRenderBackend() { }
    virtual bool initialize(void* nativeDevice, const BackendFrameInfo& frame) = 0;
    virtual bool beginFrame(unsigned long clearColor) = 0;
    virtual void endFrame() = 0;
    virtual void shutdown() = 0;
    virtual void* nativeDevice() const = 0;
};

class IAudioBackend
{
public:
    virtual ~IAudioBackend() { }
    virtual bool initialize(void* nativeWindow) = 0;
    virtual void shutdown() = 0;
    virtual bool play(unsigned int soundId, bool looped) = 0;
    virtual void stop(unsigned int soundId) = 0;
};

class IInputBackend
{
public:
    virtual ~IInputBackend() { }
    virtual bool initialize(void* nativeWindow) = 0;
    virtual void update() = 0;
    virtual void shutdown() = 0;
};

class INetworkBackend
{
public:
    virtual ~INetworkBackend() { }
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool send(const void* data, unsigned int size) = 0;
};

/**
 * Compatibility adapter for the existing Direct3D 8 device.
 * It owns no COM reference; Game remains the owner of the device.
 */
class LegacyD3D8RenderBackend : public IRenderBackend
{
public:
    LegacyD3D8RenderBackend() : m_device(0), m_initialized(false) { }

    virtual bool initialize(void* nativeDevice, const BackendFrameInfo& frame)
    {
        m_device = static_cast<LPDIRECT3DDEVICE8>(nativeDevice);
        m_frame = frame;
        m_initialized = (m_device != 0);
        return m_initialized;
    }

    virtual bool beginFrame(unsigned long clearColor)
    {
        if (!m_initialized || !m_device)
            return false;
        m_device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                        clearColor, 1.0f, 0x00000000);
        return SUCCEEDED(m_device->BeginScene());
    }

    virtual void endFrame()
    {
        if (!m_initialized || !m_device)
            return;
        m_device->EndScene();
        m_device->Present(0, 0, 0, 0);
    }

    virtual void shutdown()
    {
        // Game owns the COM device. Do not Release it here.
        m_device = 0;
        m_initialized = false;
    }

    virtual void* nativeDevice() const { return m_device; }
    const BackendFrameInfo& frame() const { return m_frame; }

private:
    LPDIRECT3DDEVICE8 m_device;
    BackendFrameInfo  m_frame;
    bool              m_initialized;
};

} // namespace DirectX

#endif /* __DXCOMMON_BACKEND_H__ */
