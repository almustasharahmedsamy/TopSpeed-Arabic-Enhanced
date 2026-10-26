#ifndef __DXCOMMON_D3D12_BOOTSTRAPPER_H__
#define __DXCOMMON_D3D12_BOOTSTRAPPER_H__

#include <DxCommon/If/BackendD3D12.h>

#if defined(_WIN32) && defined(DXCOMMON_ENABLE_D3D12)
namespace DirectX
{

class D3D12Bootstrapper
{
public:
    D3D12Bootstrapper();
    ~D3D12Bootstrapper();

    bool initialize(HWND window, unsigned int width, unsigned int height);
    void shutdown();
    bool ready() const;
    const D3D12BackendContext& context() const;

private:
    bool createDevice();
    bool createQueue();
    bool createSwapChain(HWND window, unsigned int width, unsigned int height);
    bool createFrameResources();
    void releaseResources();

    D3D12BackendContext m_context;
    IDXGIFactory4* m_factory;
    ID3D12DescriptorHeap* m_heap;
    bool m_ready;
};

} // namespace DirectX
#endif

#endif /* __DXCOMMON_D3D12_BOOTSTRAPPER_H__ */
