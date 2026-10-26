#include <DxCommon/If/D3D12Bootstrapper.h>

#if defined(_WIN32) && defined(DXCOMMON_ENABLE_D3D12)

#include <cstring>

namespace DirectX
{

D3D12Bootstrapper::D3D12Bootstrapper()
    : m_factory(0), m_heap(0), m_ready(false)
{
    std::memset(&m_context, 0, sizeof(m_context));
}

D3D12Bootstrapper::~D3D12Bootstrapper()
{
    shutdown();
}

bool D3D12Bootstrapper::initialize(HWND window, unsigned int width,
                                   unsigned int height)
{
    shutdown();
    if (!window || width == 0 || height == 0)
        return false;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory))))
        return false;
    if (!createDevice() || !createQueue() ||
        !createSwapChain(window, width, height) || !createFrameResources())
    {
        shutdown();
        return false;
    }
    m_ready = true;
    return true;
}

bool D3D12Bootstrapper::createDevice()
{
    IDXGIAdapter1* adapter = 0;
    for (UINT i = 0; m_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
        {
            adapter->Release();
            adapter = 0;
            continue;
        }
        if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0,
                                        IID_PPV_ARGS(&m_context.device))))
        {
            adapter->Release();
            return true;
        }
        adapter->Release();
        adapter = 0;
    }
    return false;
}

bool D3D12Bootstrapper::createQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc;
    std::memset(&desc, 0, sizeof(desc));
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    return SUCCEEDED(m_context.device->CreateCommandQueue(
        &desc, IID_PPV_ARGS(&m_context.queue)));
}

bool D3D12Bootstrapper::createSwapChain(HWND window, unsigned int width,
                                        unsigned int height)
{
    DXGI_SWAP_CHAIN_DESC1 desc;
    std::memset(&desc, 0, sizeof(desc));
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferCount = 2;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.SampleDesc.Count = 1;

    IDXGISwapChain1* swap1 = 0;
    HRESULT hr = m_factory->CreateSwapChainForHwnd(
        m_context.queue, window, &desc, 0, 0, &swap1);
    if (FAILED(hr))
        return false;
    hr = swap1->QueryInterface(IID_PPV_ARGS(&m_context.swapChain));
    swap1->Release();
    return SUCCEEDED(hr);
}

bool D3D12Bootstrapper::createFrameResources()
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
    std::memset(&heapDesc, 0, sizeof(heapDesc));
    heapDesc.NumDescriptors = 1;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    if (FAILED(m_context.device->CreateDescriptorHeap(
            &heapDesc, IID_PPV_ARGS(&m_heap))))
        return false;

    m_context.rtvHeap = m_heap;
    m_context.rtvSize = m_context.device->GetDescriptorHandleIncrementSize(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_context.rtv = m_heap->GetCPUDescriptorHandleForHeapStart();
    if (FAILED(m_context.swapChain->GetBuffer(
            0, IID_PPV_ARGS(&m_context.renderTarget))))
        return false;
    m_context.device->CreateRenderTargetView(m_context.renderTarget, 0,
                                             m_context.rtv);
    if (FAILED(m_context.device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_context.allocator))))
        return false;
    if (FAILED(m_context.device->CreateCommandList(
            0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_context.allocator, 0,
            IID_PPV_ARGS(&m_context.commandList))))
        return false;
    return SUCCEEDED(m_context.commandList->Close());
}

void D3D12Bootstrapper::releaseResources()
{
    if (m_context.commandList) m_context.commandList->Release();
    if (m_context.allocator) m_context.allocator->Release();
    if (m_context.renderTarget) m_context.renderTarget->Release();
    if (m_heap) m_heap->Release();
    if (m_context.swapChain) m_context.swapChain->Release();
    if (m_context.queue) m_context.queue->Release();
    if (m_context.device) m_context.device->Release();
    if (m_factory) m_factory->Release();
    std::memset(&m_context, 0, sizeof(m_context));
    m_factory = 0;
    m_heap = 0;
}

void D3D12Bootstrapper::shutdown()
{
    m_ready = false;
    releaseResources();
}

bool D3D12Bootstrapper::ready() const
{
    return m_ready;
}

const D3D12BackendContext& D3D12Bootstrapper::context() const
{
    return m_context;
}

} // namespace DirectX
#endif
