#include <DxCommon/If/BackendD3D12.h>

#if defined(_WIN32) && defined(DXCOMMON_ENABLE_D3D12)

#include <cstring>

namespace DirectX
{

D3D12RenderBackend::D3D12RenderBackend()
    : m_initialized(false), m_frameOpen(false)
{
    std::memset(&m_context, 0, sizeof(m_context));
    m_frame.width = 0;
    m_frame.height = 0;
    m_clearColor[0] = 0.0f;
    m_clearColor[1] = 0.0f;
    m_clearColor[2] = 0.0f;
    m_clearColor[3] = 1.0f;
}

D3D12RenderBackend::~D3D12RenderBackend()
{
    shutdown();
}

bool D3D12RenderBackend::initialize(void* nativeDevice,
                                    const BackendFrameInfo& frame)
{
    if (!nativeDevice)
        return false;

    // The generic contract carries the device only. A swap chain and command
    // objects must be supplied through initializeFromContext before rendering.
    D3D12BackendContext context;
    std::memset(&context, 0, sizeof(context));
    context.device = static_cast<ID3D12Device*>(nativeDevice);
    return initializeFromContext(context, frame);
}

bool D3D12RenderBackend::initializeFromContext(
    const D3D12BackendContext& context, const BackendFrameInfo& frame)
{
    if (!context.device || !context.queue || !context.swapChain ||
        !context.allocator || !context.commandList || !context.renderTarget)
        return false;

    m_context = context;
    m_frame = frame;
    m_initialized = true;
    m_frameOpen = false;
    return true;
}

bool D3D12RenderBackend::beginFrame(unsigned long clearColor)
{
    if (!m_initialized || m_frameOpen)
        return false;

    m_clearColor[0] = ((clearColor >> 16) & 0xff) / 255.0f;
    m_clearColor[1] = ((clearColor >> 8) & 0xff) / 255.0f;
    m_clearColor[2] = (clearColor & 0xff) / 255.0f;
    m_clearColor[3] = 1.0f;

    if (FAILED(m_context.allocator->Reset()) ||
        FAILED(m_context.commandList->Reset(m_context.allocator, 0)))
        return false;

    D3D12_RESOURCE_BARRIER barrier;
    std::memset(&barrier, 0, sizeof(barrier));
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_context.renderTarget;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_context.commandList->ResourceBarrier(1, &barrier);
    m_context.commandList->OMSetRenderTargets(1, &m_context.rtv, FALSE, 0);
    m_context.commandList->ClearRenderTargetView(m_context.rtv, m_clearColor, 0, 0);
    m_frameOpen = true;
    return true;
}

void D3D12RenderBackend::endFrame()
{
    if (!m_initialized || !m_frameOpen)
        return;

    D3D12_RESOURCE_BARRIER barrier;
    std::memset(&barrier, 0, sizeof(barrier));
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = m_context.renderTarget;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    m_context.commandList->ResourceBarrier(1, &barrier);
    m_context.commandList->Close();

    ID3D12CommandList* lists[] = { m_context.commandList };
    m_context.queue->ExecuteCommandLists(1, lists);
    m_context.swapChain->Present(1, 0);
    m_frameOpen = false;
}

void D3D12RenderBackend::shutdown()
{
    std::memset(&m_context, 0, sizeof(m_context));
    m_initialized = false;
    m_frameOpen = false;
}

void* D3D12RenderBackend::nativeDevice() const
{
    return m_context.device;
}

bool D3D12RenderBackend::ready() const
{
    return m_initialized;
}

} // namespace DirectX
#endif
