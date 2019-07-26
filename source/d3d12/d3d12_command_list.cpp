/**
 * Copyright (C) 2014 Patrick Mours. All rights reserved.
 * License: https://github.com/crosire/reshade#license
 */

#include "log.hpp"
#include "d3d12_device.hpp"
#include "d3d12_command_list.hpp"
#include <assert.h>
#include "runtime_d3d12.hpp"

D3D12CommandList::D3D12CommandList(D3D12Device *device, UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator *pCommandAllocator, ID3D12PipelineState *pInitialState, ID3D12GraphicsCommandList *original) :
	_orig(original),
	_interface_version(0),
	_device(device),
	_nodeMask(nodeMask),
	_type(type),
	_commandAllocator(pCommandAllocator),
	_initialState(pInitialState) {
	assert(original != nullptr);
}

D3D12CommandList::D3D12CommandList(D3D12Device *device, UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator *pCommandAllocator, ID3D12PipelineState *pInitialState, ID3D12GraphicsCommandList1 *original) :
	_orig(original),
	_interface_version(1),
	_device(device),
	_nodeMask(nodeMask),
	_type(type),
	_commandAllocator(pCommandAllocator),
	_initialState(pInitialState) {
	assert(original != nullptr);
}

D3D12CommandList::D3D12CommandList(D3D12Device *device, UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator *pCommandAllocator, ID3D12PipelineState *pInitialState, ID3D12GraphicsCommandList2 *original) :
	_orig(original),
	_interface_version(2),
	_device(device),
	_nodeMask(nodeMask),
	_type(type),
	_commandAllocator(pCommandAllocator),
	_initialState(pInitialState) {
	assert(original != nullptr);
}

D3D12CommandList::D3D12CommandList(D3D12Device *device, UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator *pCommandAllocator, ID3D12PipelineState *pInitialState, ID3D12GraphicsCommandList3 *original) :
	_orig(original),
	_interface_version(3),
	_device(device),
	_nodeMask(nodeMask),
	_type(type),
	_commandAllocator(pCommandAllocator),
	_initialState(pInitialState) {
	assert(original != nullptr);
}

D3D12CommandList::D3D12CommandList(D3D12Device *device, UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator *pCommandAllocator, ID3D12PipelineState *pInitialState, ID3D12GraphicsCommandList4 *original) :
	_orig(original),
	_interface_version(4),
	_device(device),
	_nodeMask(nodeMask),
	_type(type),
	_commandAllocator(pCommandAllocator),
	_initialState(pInitialState) {
	assert(original != nullptr);
}

D3D12CommandList::D3D12CommandList(D3D12Device *device, UINT nodeMask, D3D12_COMMAND_LIST_TYPE type, ID3D12CommandAllocator *pCommandAllocator, ID3D12PipelineState *pInitialState, ID3D12GraphicsCommandList5 *original) :
	_orig(original),
	_interface_version(5),
	_device(device),
	_nodeMask(nodeMask),
	_type(type),
	_commandAllocator(pCommandAllocator),
	_initialState(pInitialState) {
	assert(original != nullptr);
}

bool D3D12CommandList::check_and_upgrade_interface(REFIID riid)
{
	static const IID iid_lookup[] = {
		__uuidof(ID3D12GraphicsCommandList),
		__uuidof(ID3D12GraphicsCommandList1),
		__uuidof(ID3D12GraphicsCommandList2),
		__uuidof(ID3D12GraphicsCommandList3),
		__uuidof(ID3D12GraphicsCommandList4),
		__uuidof(ID3D12GraphicsCommandList5),
	};

	for (unsigned int new_version = _interface_version + 1; new_version < ARRAYSIZE(iid_lookup); ++new_version)
	{
		if (riid == iid_lookup[new_version])
		{
			IUnknown *new_interface = nullptr;
			if (FAILED(_orig->QueryInterface(riid, reinterpret_cast<void **>(&new_interface))))
				return false;
#if RESHADE_VERBOSE_LOG
			LOG(DEBUG) << "Upgraded ID3D12GraphicsCommandList" << _interface_version << " object " << this << " to ID3D12GraphicsCommandList" << new_version << '.';
#endif
			_orig->Release();
			_orig = static_cast<ID3D12GraphicsCommandList *>(new_interface);
			_interface_version = new_version;
			break;
		}
	}

	return true;
}

HRESULT STDMETHODCALLTYPE D3D12CommandList::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(this) ||
		riid == __uuidof(IUnknown) ||
		riid == __uuidof(ID3D12Object) ||
		riid == __uuidof(ID3D12DeviceChild) ||
		riid == __uuidof(ID3D12CommandList) ||
		riid == __uuidof(ID3D12GraphicsCommandList) ||
		riid == __uuidof(ID3D12GraphicsCommandList1) ||
		riid == __uuidof(ID3D12GraphicsCommandList2) ||
		riid == __uuidof(ID3D12GraphicsCommandList3) ||
		riid == __uuidof(ID3D12GraphicsCommandList4) ||
		riid == __uuidof(ID3D12GraphicsCommandList5))
	{
		if (!check_and_upgrade_interface(riid))
			return E_NOINTERFACE;

		AddRef();
		*ppvObj = this;
		return S_OK;
	}

	return _orig->QueryInterface(riid, ppvObj);
}
ULONG STDMETHODCALLTYPE D3D12CommandList::AddRef()
{
	++_ref;

	return _orig->AddRef();
}
ULONG STDMETHODCALLTYPE D3D12CommandList::Release()
{
	--_ref;

	const ULONG ref = _orig->Release();

	if (ref != 0 && _ref != 0)
		return ref;
	else if (ref != 0)
		LOG(WARN) << "Reference count for ID3D12GraphicsCommandList" << _interface_version << " object " << this << " is inconsistent: " << ref << ", but expected 0.";

	assert(_ref <= 0);
#if RESHADE_VERBOSE_LOG
	LOG(DEBUG) << "Destroyed ID3D12GraphicsCommandList" << _interface_version << " object " << this << '.';
#endif
	delete this;
	return 0;
}

HRESULT STDMETHODCALLTYPE D3D12CommandList::GetPrivateData(REFGUID guid, UINT *pDataSize, void *pData)
{
	return _orig->GetPrivateData(guid, pDataSize, pData);
}
HRESULT STDMETHODCALLTYPE D3D12CommandList::SetPrivateData(REFGUID guid, UINT DataSize, const void *pData)
{
	return _orig->SetPrivateData(guid, DataSize, pData);
}
HRESULT STDMETHODCALLTYPE D3D12CommandList::SetPrivateDataInterface(REFGUID guid, const IUnknown *pData)
{
	return _orig->SetPrivateDataInterface(guid, pData);
}
HRESULT STDMETHODCALLTYPE D3D12CommandList::SetName(LPCWSTR Name)
{
	return _orig->SetName(Name);
}

HRESULT STDMETHODCALLTYPE D3D12CommandList::GetDevice(REFIID riid, void **ppvDevice)
{
	return _device->QueryInterface(riid, ppvDevice);
}

D3D12_COMMAND_LIST_TYPE __stdcall D3D12CommandList::GetType(void)
{
	return _orig->GetType();
}

HRESULT STDMETHODCALLTYPE D3D12CommandList::Close()
{
	return _orig->Close();
}
HRESULT STDMETHODCALLTYPE D3D12CommandList::Reset(ID3D12CommandAllocator *pAllocator, ID3D12PipelineState *pInitialState)
{
	return _orig->Reset(pAllocator, pInitialState);
}
void STDMETHODCALLTYPE D3D12CommandList::ClearState(ID3D12PipelineState *pPipelineState)
{
	_orig->ClearState(pPipelineState);
}
void STDMETHODCALLTYPE D3D12CommandList::DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
{
	_orig->DrawInstanced(VertexCountPerInstance, InstanceCount, StartVertexLocation, StartInstanceLocation);
}
void STDMETHODCALLTYPE D3D12CommandList::DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
{
	_orig->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
}
void STDMETHODCALLTYPE D3D12CommandList::Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ)
{
	_orig->Dispatch(ThreadGroupCountX, ThreadGroupCountY, ThreadGroupCountZ);
}
void STDMETHODCALLTYPE D3D12CommandList::CopyBufferRegion(ID3D12Resource *pDstBuffer, UINT64 DstOffset, ID3D12Resource *pSrcBuffer, UINT64 SrcOffset, UINT64 NumBytes)
{
	_orig->CopyBufferRegion(pDstBuffer, DstOffset, pSrcBuffer, SrcOffset, NumBytes);
}
void STDMETHODCALLTYPE D3D12CommandList::CopyTextureRegion(const D3D12_TEXTURE_COPY_LOCATION *pDst, UINT DstX, UINT DstY, UINT DstZ, const D3D12_TEXTURE_COPY_LOCATION *pSrc, const D3D12_BOX *pSrcBox)
{
	_orig->CopyTextureRegion(pDst, DstX, DstY, DstZ, pSrc, pSrcBox);
}
void STDMETHODCALLTYPE D3D12CommandList::CopyResource(ID3D12Resource *pDstResource, ID3D12Resource *pSrcResource)
{
	_orig->CopyResource(pDstResource, pSrcResource);
}
void STDMETHODCALLTYPE D3D12CommandList::CopyTiles(ID3D12Resource *pTiledResource, const D3D12_TILED_RESOURCE_COORDINATE *pTileRegionStartCoordinate, const D3D12_TILE_REGION_SIZE *pTileRegionSize, ID3D12Resource *pBuffer, UINT64 BufferStartOffsetInBytes, D3D12_TILE_COPY_FLAGS Flags)
{
	_orig->CopyTiles(pTiledResource, pTileRegionStartCoordinate, pTileRegionSize, pBuffer, BufferStartOffsetInBytes, Flags);
}
void STDMETHODCALLTYPE D3D12CommandList::ResolveSubresource(ID3D12Resource *pDstResource, UINT DstSubresource, ID3D12Resource *pSrcResource, UINT SrcSubresource, DXGI_FORMAT Format)
{
	_orig->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, Format);
}
void STDMETHODCALLTYPE D3D12CommandList::IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology)
{
	_orig->IASetPrimitiveTopology(PrimitiveTopology);
}
void STDMETHODCALLTYPE D3D12CommandList::RSSetViewports(UINT NumViewports, const D3D12_VIEWPORT *pViewports)
{
	_orig->RSSetViewports(NumViewports, pViewports);
}
void STDMETHODCALLTYPE D3D12CommandList::RSSetScissorRects(UINT NumRects, const D3D12_RECT *pRects)
{
	_orig->RSSetScissorRects(NumRects, pRects);
}
void STDMETHODCALLTYPE D3D12CommandList::OMSetBlendFactor(const FLOAT BlendFactor[4])
{
	_orig->OMSetBlendFactor(BlendFactor);
}
void STDMETHODCALLTYPE D3D12CommandList::OMSetStencilRef(UINT StencilRef)
{
	_orig->OMSetStencilRef(StencilRef);
}
void STDMETHODCALLTYPE D3D12CommandList::SetPipelineState(ID3D12PipelineState *pPipelineState)
{
	_orig->SetPipelineState(pPipelineState);
}
void STDMETHODCALLTYPE D3D12CommandList::ResourceBarrier(UINT NumBarriers, const D3D12_RESOURCE_BARRIER *pBarriers)
{
	_orig->ResourceBarrier(NumBarriers, pBarriers);
}
void STDMETHODCALLTYPE D3D12CommandList::ExecuteBundle(ID3D12GraphicsCommandList *pCommandList)
{
	_orig->ExecuteBundle(pCommandList);
}
void STDMETHODCALLTYPE D3D12CommandList::SetDescriptorHeaps(UINT NumDescriptorHeaps, ID3D12DescriptorHeap *const *ppDescriptorHeaps)
{
	_orig->SetDescriptorHeaps(NumDescriptorHeaps, ppDescriptorHeaps);
}
void STDMETHODCALLTYPE D3D12CommandList::SetComputeRootSignature(ID3D12RootSignature *pRootSignature)
{
	_orig->SetComputeRootSignature(pRootSignature);
}
void STDMETHODCALLTYPE D3D12CommandList::SetGraphicsRootSignature(ID3D12RootSignature *pRootSignature)
{
	_orig->SetGraphicsRootSignature(pRootSignature);
}
void STDMETHODCALLTYPE D3D12CommandList::SetComputeRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
{
	_orig->SetComputeRootDescriptorTable(RootParameterIndex, BaseDescriptor);
}
void STDMETHODCALLTYPE D3D12CommandList::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
{
	_orig->SetGraphicsRootDescriptorTable(RootParameterIndex, BaseDescriptor);
}
void STDMETHODCALLTYPE D3D12CommandList::SetComputeRoot32BitConstant(UINT RootParameterIndex, UINT SrcData, UINT DestOffsetIn32BitValues)
{
	_orig->SetComputeRoot32BitConstant(RootParameterIndex, SrcData, DestOffsetIn32BitValues);
}
void STDMETHODCALLTYPE D3D12CommandList::SetGraphicsRoot32BitConstant(UINT RootParameterIndex, UINT SrcData, UINT DestOffsetIn32BitValues)
{
	_orig->SetGraphicsRoot32BitConstant(RootParameterIndex, SrcData, DestOffsetIn32BitValues);
}
void STDMETHODCALLTYPE D3D12CommandList::SetComputeRoot32BitConstants(UINT RootParameterIndex, UINT Num32BitValuesToSet, const void *pSrcData, UINT DestOffsetIn32BitValues)
{
	_orig->SetComputeRoot32BitConstants(RootParameterIndex, Num32BitValuesToSet, pSrcData, DestOffsetIn32BitValues);
}
void STDMETHODCALLTYPE D3D12CommandList::SetGraphicsRoot32BitConstants(UINT RootParameterIndex, UINT Num32BitValuesToSet, const void *pSrcData, UINT DestOffsetIn32BitValues)
{
	_orig->SetGraphicsRoot32BitConstants(RootParameterIndex, Num32BitValuesToSet, pSrcData, DestOffsetIn32BitValues);
}
void STDMETHODCALLTYPE D3D12CommandList::SetComputeRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
{
	_orig->SetComputeRootConstantBufferView(RootParameterIndex, BufferLocation);
}
void STDMETHODCALLTYPE D3D12CommandList::SetGraphicsRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
{
	_orig->SetGraphicsRootConstantBufferView(RootParameterIndex, BufferLocation);
}
void STDMETHODCALLTYPE D3D12CommandList::SetComputeRootShaderResourceView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
{
	_orig->SetComputeRootShaderResourceView(RootParameterIndex, BufferLocation);
}
void STDMETHODCALLTYPE D3D12CommandList::SetGraphicsRootShaderResourceView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
{
	_orig->SetGraphicsRootShaderResourceView(RootParameterIndex, BufferLocation);
}
void STDMETHODCALLTYPE D3D12CommandList::SetComputeRootUnorderedAccessView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
{
	_orig->SetComputeRootUnorderedAccessView(RootParameterIndex, BufferLocation);
}
void STDMETHODCALLTYPE D3D12CommandList::SetGraphicsRootUnorderedAccessView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
{
	_orig->SetGraphicsRootUnorderedAccessView(RootParameterIndex, BufferLocation);
}
void STDMETHODCALLTYPE D3D12CommandList::IASetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW *pView)
{
	_orig->IASetIndexBuffer(pView);
}
void STDMETHODCALLTYPE D3D12CommandList::IASetVertexBuffers(UINT StartSlot, UINT NumViews, const D3D12_VERTEX_BUFFER_VIEW *pViews)
{
	_orig->IASetVertexBuffers(StartSlot, NumViews, pViews);
}
void STDMETHODCALLTYPE D3D12CommandList::SOSetTargets(UINT StartSlot, UINT NumViews, const D3D12_STREAM_OUTPUT_BUFFER_VIEW *pViews)
{
	_orig->SOSetTargets(StartSlot, NumViews, pViews);
}
void STDMETHODCALLTYPE D3D12CommandList::OMSetRenderTargets(UINT NumRenderTargetDescriptors, const D3D12_CPU_DESCRIPTOR_HANDLE *pRenderTargetDescriptors, BOOL RTsSingleHandleToDescriptorRange, D3D12_CPU_DESCRIPTOR_HANDLE const *pDepthStencilDescriptor)
{
	_orig->OMSetRenderTargets(NumRenderTargetDescriptors, pRenderTargetDescriptors, RTsSingleHandleToDescriptorRange, pDepthStencilDescriptor);
}
void STDMETHODCALLTYPE D3D12CommandList::ClearDepthStencilView(D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView, D3D12_CLEAR_FLAGS ClearFlags, FLOAT Depth, UINT8 Stencil, UINT NumRects, const D3D12_RECT *pRects)
{
	_orig->ClearDepthStencilView(DepthStencilView, ClearFlags, Depth, Stencil, NumRects, pRects);
}
void STDMETHODCALLTYPE D3D12CommandList::ClearRenderTargetView(D3D12_CPU_DESCRIPTOR_HANDLE RenderTargetView, const FLOAT ColorRGBA[4], UINT NumRects, const D3D12_RECT *pRects)
{
	_orig->ClearRenderTargetView(RenderTargetView, ColorRGBA, NumRects, pRects);
}
void STDMETHODCALLTYPE D3D12CommandList::ClearUnorderedAccessViewUint(D3D12_GPU_DESCRIPTOR_HANDLE ViewGPUHandleInCurrentHeap, D3D12_CPU_DESCRIPTOR_HANDLE ViewCPUHandle, ID3D12Resource *pResource, const UINT Values[4], UINT NumRects, const D3D12_RECT *pRects)
{
	_orig->ClearUnorderedAccessViewUint(ViewGPUHandleInCurrentHeap, ViewCPUHandle, pResource, Values, NumRects, pRects);
}
void STDMETHODCALLTYPE D3D12CommandList::ClearUnorderedAccessViewFloat(D3D12_GPU_DESCRIPTOR_HANDLE ViewGPUHandleInCurrentHeap, D3D12_CPU_DESCRIPTOR_HANDLE ViewCPUHandle, ID3D12Resource *pResource, const FLOAT Values[4], UINT NumRects, const D3D12_RECT *pRects)
{
	_orig->ClearUnorderedAccessViewFloat(ViewGPUHandleInCurrentHeap, ViewCPUHandle, pResource, Values, NumRects, pRects);
}
void STDMETHODCALLTYPE D3D12CommandList::DiscardResource(ID3D12Resource *pResource, const D3D12_DISCARD_REGION *pRegion)
{
	_orig->DiscardResource(pResource, pRegion);
}
void STDMETHODCALLTYPE D3D12CommandList::BeginQuery(ID3D12QueryHeap *pQueryHeap, D3D12_QUERY_TYPE Type, UINT Index)
{
	_orig->BeginQuery(pQueryHeap, Type, Index);
}
void STDMETHODCALLTYPE D3D12CommandList::EndQuery(ID3D12QueryHeap *pQueryHeap, D3D12_QUERY_TYPE Type, UINT Index)
{
	_orig->EndQuery(pQueryHeap, Type, Index);
}
void STDMETHODCALLTYPE D3D12CommandList::ResolveQueryData(ID3D12QueryHeap *pQueryHeap, D3D12_QUERY_TYPE Type, UINT StartIndex, UINT NumQueries, ID3D12Resource *pDestinationBuffer, UINT64 AlignedDestinationBufferOffset)
{
	_orig->ResolveQueryData(pQueryHeap, Type, StartIndex, NumQueries, pDestinationBuffer, AlignedDestinationBufferOffset);
}
void STDMETHODCALLTYPE D3D12CommandList::SetPredication(ID3D12Resource *pBuffer, UINT64 AlignedBufferOffset, D3D12_PREDICATION_OP Operation)
{
	_orig->SetPredication(pBuffer, AlignedBufferOffset, Operation);
}
void STDMETHODCALLTYPE D3D12CommandList::SetMarker(UINT Metadata, const void *pData, UINT Size)
{
	_orig->SetMarker(Metadata, pData, Size);
}
void STDMETHODCALLTYPE D3D12CommandList::BeginEvent(UINT Metadata, const void *pData, UINT Size)
{
	_orig->BeginEvent(Metadata, pData, Size);
}
void STDMETHODCALLTYPE D3D12CommandList::EndEvent()
{
	_orig->EndEvent();
}
void STDMETHODCALLTYPE D3D12CommandList::ExecuteIndirect(ID3D12CommandSignature *pCommandSignature, UINT MaxCommandCount, ID3D12Resource *pArgumentBuffer, UINT64 ArgumentBufferOffset, ID3D12Resource *pCountBuffer, UINT64 CountBufferOffset)
{
	_orig->ExecuteIndirect(pCommandSignature, MaxCommandCount, pArgumentBuffer, ArgumentBufferOffset, pCountBuffer, CountBufferOffset);
}

void STDMETHODCALLTYPE D3D12CommandList::AtomicCopyBufferUINT(ID3D12Resource *pDstBuffer, UINT64 DstOffset, ID3D12Resource *pSrcBuffer, UINT64 SrcOffset, UINT Dependencies, ID3D12Resource *const *ppDependentResources, const D3D12_SUBRESOURCE_RANGE_UINT64 *pDependentSubresourceRanges)
{
	assert(_interface_version >= 1);
	static_cast<ID3D12GraphicsCommandList1 *>(_orig)->AtomicCopyBufferUINT(pDstBuffer, DstOffset, pSrcBuffer, SrcOffset, Dependencies, ppDependentResources, pDependentSubresourceRanges);
}
void STDMETHODCALLTYPE D3D12CommandList::AtomicCopyBufferUINT64(ID3D12Resource *pDstBuffer, UINT64 DstOffset, ID3D12Resource *pSrcBuffer, UINT64 SrcOffset, UINT Dependencies, ID3D12Resource *const *ppDependentResources, const D3D12_SUBRESOURCE_RANGE_UINT64 *pDependentSubresourceRanges)
{
	assert(_interface_version >= 1);
	static_cast<ID3D12GraphicsCommandList1 *>(_orig)->AtomicCopyBufferUINT64(pDstBuffer, DstOffset, pSrcBuffer, SrcOffset, Dependencies, ppDependentResources, pDependentSubresourceRanges);
}
void STDMETHODCALLTYPE D3D12CommandList::OMSetDepthBounds(FLOAT Min, FLOAT Max)
{
	assert(_interface_version >= 1);
	static_cast<ID3D12GraphicsCommandList1 *>(_orig)->OMSetDepthBounds(Min, Max);
}
void STDMETHODCALLTYPE D3D12CommandList::SetSamplePositions(UINT NumSamplesPerPixel, UINT NumPixels, D3D12_SAMPLE_POSITION *pSamplePositions)
{
	assert(_interface_version >= 1);
	static_cast<ID3D12GraphicsCommandList1 *>(_orig)->SetSamplePositions(NumSamplesPerPixel, NumPixels, pSamplePositions);
}
void STDMETHODCALLTYPE D3D12CommandList::ResolveSubresourceRegion(ID3D12Resource *pDstResource, UINT DstSubresource, UINT DstX, UINT DstY, ID3D12Resource *pSrcResource, UINT SrcSubresource, D3D12_RECT *pSrcRect, DXGI_FORMAT Format, D3D12_RESOLVE_MODE ResolveMode)
{
	assert(_interface_version >= 1);
	static_cast<ID3D12GraphicsCommandList1 *>(_orig)->ResolveSubresourceRegion(pDstResource, DstSubresource, DstX, DstY, pSrcResource, SrcSubresource, pSrcRect, Format, ResolveMode);
}
void STDMETHODCALLTYPE D3D12CommandList::SetViewInstanceMask(UINT Mask)
{
	assert(_interface_version >= 1);
	static_cast<ID3D12GraphicsCommandList1 *>(_orig)->SetViewInstanceMask(Mask);
}

void STDMETHODCALLTYPE D3D12CommandList::WriteBufferImmediate(UINT Count, const D3D12_WRITEBUFFERIMMEDIATE_PARAMETER *pParams, const D3D12_WRITEBUFFERIMMEDIATE_MODE *pModes)
{
	assert(_interface_version >= 2);
	static_cast<ID3D12GraphicsCommandList2 *>(_orig)->WriteBufferImmediate(Count, pParams, pModes);
}

void STDMETHODCALLTYPE D3D12CommandList::SetProtectedResourceSession(ID3D12ProtectedResourceSession *pProtectedResourceSession)
{
	assert(_interface_version >= 3);
	static_cast<ID3D12GraphicsCommandList3 *>(_orig)->SetProtectedResourceSession(pProtectedResourceSession);
}

void STDMETHODCALLTYPE D3D12CommandList::BeginRenderPass(UINT NumRenderTargets, const D3D12_RENDER_PASS_RENDER_TARGET_DESC *pRenderTargets, const D3D12_RENDER_PASS_DEPTH_STENCIL_DESC *pDepthStencil, D3D12_RENDER_PASS_FLAGS Flags)
{
	assert(_interface_version >= 4);
	static_cast<ID3D12GraphicsCommandList4 *>(_orig)->BeginRenderPass(NumRenderTargets, pRenderTargets, pDepthStencil, Flags);
}
void STDMETHODCALLTYPE D3D12CommandList::EndRenderPass(void)
{
	assert(_interface_version >= 4);
	static_cast<ID3D12GraphicsCommandList4 *>(_orig)->EndRenderPass();
}
void STDMETHODCALLTYPE D3D12CommandList::InitializeMetaCommand(ID3D12MetaCommand *pMetaCommand, const void *pInitializationParametersData, SIZE_T InitializationParametersDataSizeInBytes)
{
	assert(_interface_version >= 4);
	static_cast<ID3D12GraphicsCommandList4 *>(_orig)->InitializeMetaCommand(pMetaCommand, pInitializationParametersData, InitializationParametersDataSizeInBytes);
}
void STDMETHODCALLTYPE D3D12CommandList::ExecuteMetaCommand(ID3D12MetaCommand *pMetaCommand, const void *pExecutionParametersData, SIZE_T ExecutionParametersDataSizeInBytes)
{
	assert(_interface_version >= 4);
	static_cast<ID3D12GraphicsCommandList4 *>(_orig)->ExecuteMetaCommand(pMetaCommand, pExecutionParametersData, ExecutionParametersDataSizeInBytes);
}
void STDMETHODCALLTYPE D3D12CommandList::BuildRaytracingAccelerationStructure(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC *pDesc, UINT NumPostbuildInfoDescs, const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_DESC *pPostbuildInfoDescs)
{
	assert(_interface_version >= 4);
	static_cast<ID3D12GraphicsCommandList4 *>(_orig)->BuildRaytracingAccelerationStructure(pDesc, NumPostbuildInfoDescs, pPostbuildInfoDescs);
}
void STDMETHODCALLTYPE D3D12CommandList::EmitRaytracingAccelerationStructurePostbuildInfo(const D3D12_RAYTRACING_ACCELERATION_STRUCTURE_POSTBUILD_INFO_DESC *pDesc, UINT NumSourceAccelerationStructures, const D3D12_GPU_VIRTUAL_ADDRESS *pSourceAccelerationStructureData)
{
	assert(_interface_version >= 4);
	static_cast<ID3D12GraphicsCommandList4 *>(_orig)->EmitRaytracingAccelerationStructurePostbuildInfo(pDesc, NumSourceAccelerationStructures, pSourceAccelerationStructureData);
}
void STDMETHODCALLTYPE D3D12CommandList::CopyRaytracingAccelerationStructure(D3D12_GPU_VIRTUAL_ADDRESS DestAccelerationStructureData, D3D12_GPU_VIRTUAL_ADDRESS SourceAccelerationStructureData, D3D12_RAYTRACING_ACCELERATION_STRUCTURE_COPY_MODE Mode)
{
	assert(_interface_version >= 4);
	static_cast<ID3D12GraphicsCommandList4 *>(_orig)->CopyRaytracingAccelerationStructure(DestAccelerationStructureData, SourceAccelerationStructureData, Mode);
}
void STDMETHODCALLTYPE D3D12CommandList::SetPipelineState1(ID3D12StateObject *pStateObject)
{
	assert(_interface_version >= 4);
	static_cast<ID3D12GraphicsCommandList4 *>(_orig)->SetPipelineState1(pStateObject);
}
void STDMETHODCALLTYPE D3D12CommandList::DispatchRays(const D3D12_DISPATCH_RAYS_DESC *pDesc)
{
	assert(_interface_version >= 4);
	static_cast<ID3D12GraphicsCommandList4 *>(_orig)->DispatchRays(pDesc);
}

void STDMETHODCALLTYPE D3D12CommandList::RSSetShadingRate(D3D12_SHADING_RATE baseShadingRate, const D3D12_SHADING_RATE_COMBINER *combiners)
{
	assert(_interface_version >= 5);
	static_cast<ID3D12GraphicsCommandList5 *>(_orig)->RSSetShadingRate(baseShadingRate, combiners);
}
void STDMETHODCALLTYPE D3D12CommandList::RSSetShadingRateImage(ID3D12Resource *shadingRateImage)
{
	assert(_interface_version >= 5);
	static_cast<ID3D12GraphicsCommandList5 *>(_orig)->RSSetShadingRateImage(shadingRateImage);
}
