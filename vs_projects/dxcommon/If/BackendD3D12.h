#ifndef __DXCOMMON_BACKEND_D3D12_H__
#define __DXCOMMON_BACKEND_D3D12_H__

#include <DxCommon/If/Backend.h>

#if defined(_WIN32) && defined(DXCOMMON_ENABLE_D3D12)
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

namespace DirectX
{

struct D3D12BackendContext
{
    ID3D12Device* device;
    ID3D12CommandQueue* queue;
    IDXGISwapChain3* swapChain;
    ID3D12DescriptorHeap* rtvHeap;
    ID3D12Resource* renderTarget;
    ID3D12CommandAllocator* allocator;
    ID3D12GraphicsCommandList* commandList;
    D3D12_CPU_DESCRIPTOR_HANDLE rtv;
    UINT rtvSize;
};

/**
 * DirectX 12 render backend boundary.
 * Resource creation is owned by the host/bootstrapper; this adapter does not
 * release borrowed COM interfaces. It records a clear and presents one frame.
 */
class D3D12RenderBackend : public IRenderBackend
{
public:
    D3D12RenderBackend();
    virtual ~D3D12RenderBackend();

    virtual bool initialize(void* nativeDevice, const BackendFrameInfo& frame);
    bool initializeFromContext(const D3D12BackendContext& context,
                               const BackendFrameInfo& frame);
    virtual bool beginFrame(unsigned long clearColor);
    virtual void endFrame();
    virtual void shutdown();
    virtual void* nativeDevice() const;

    bool ready() const;

private:
    D3D12BackendContext m_context;
    BackendFrameInfo m_frame;
    bool m_initialized;
    bool m_frameOpen;
    float m_clearColor[4];
};

} // namespace DirectX
#endif

#endif /* __DXCOMMON_BACKEND_D3D12_H__ */
