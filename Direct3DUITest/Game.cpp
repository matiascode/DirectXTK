//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
 
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 240.0);  //run update loop at 240Hz
    
}

// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	//should have elapsed time = 1/240
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;
}

// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    auto context = m_deviceResources->GetD3DDeviceContext();
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext=m_deviceResources->GetD3DDeviceContext();
    // TODO: Add your rendering code here.
	m_d3dContext->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	m_d3dContext->OMSetDepthStencilState(m_states->DepthNone(), 0);
	m_d3dContext->RSSetState(m_raster.Get());

	m_effect->SetWorld(m_world);

	m_effect->Apply(m_d3dContext.Get());

	m_d3dContext->IASetInputLayout(m_inputLayout.Get());
	//render a grid on x-y plane with origin 0,0,0
	RenderWorldGrid();

    context;

    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    // Clear the views
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);
}

// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1280;
    height = 720;
}

// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
	//   m_d3dDevice == m_deviceResources->GetD3DDevice()
    // TODO: Initialize device dependent objects here (independent of window size).
	m_states = std::make_unique<CommonStates>(m_deviceResources->GetD3DDevice());
	
	m_effect = std::make_unique<BasicEffect>(m_deviceResources->GetD3DDevice());
	m_effect->SetVertexColorEnabled(true);

	void const* shaderByteCode;
	size_t byteCodeLength;

	m_effect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

	DX::ThrowIfFailed(
		m_deviceResources->GetD3DDevice()->CreateInputLayout(VertexPositionColor::InputElements,
			VertexPositionColor::InputElementCount,
			shaderByteCode, byteCodeLength,
			m_inputLayout.ReleaseAndGetAddressOf()));

	m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(m_deviceResources->GetD3DDeviceContext());
	CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE,
		TRUE, FALSE);
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rastDesc,
		m_raster.ReleaseAndGetAddressOf()));
	
 device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.	
	m_world = DirectX::SimpleMath::Matrix::Identity;
	m_view = DirectX::SimpleMath::Matrix::CreateLookAt(DirectX::SimpleMath::Vector3(2.f, 2.f, 2.f),
		DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::UnitY);
	m_proj = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		float(m_deviceResources->GetScreenViewport().Width) / float(m_deviceResources->GetScreenViewport().Height), 0.1f, 10.f);

	m_effect->SetView(m_view);
	m_effect->SetProjection(m_proj);
 
}

void Game::RenderWorldGrid()
{
	m_batch->Begin();

	DirectX::SimpleMath::Vector3 xaxis(2.f, 0.f, 0.f);
	DirectX::SimpleMath::Vector3 yaxis(0.f, 0.f, 2.f);
	DirectX::SimpleMath::Vector3 origin = DirectX::SimpleMath::Vector3::Zero;

	size_t divisions = 20;

	for (size_t i = 0; i <= divisions; ++i)
	{
		float fPercent = float(i) / float(divisions);
		fPercent = (fPercent * 2.0f) - 1.0f;

		DirectX::SimpleMath::Vector3 scale = xaxis * fPercent + origin;

		VertexPositionColor v1(scale - yaxis, Colors::LightSteelBlue);
		VertexPositionColor v2(scale + yaxis, Colors::BlanchedAlmond);
		m_batch->DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= divisions; i++)
	{
		float fPercent = float(i) / float(divisions);
		fPercent = (fPercent * 2.0f) - 1.0f;

		DirectX::SimpleMath::Vector3 scale = yaxis * fPercent + origin;

		VertexPositionColor v1(scale - xaxis, Colors::LightSteelBlue);
		VertexPositionColor v2(scale + xaxis, Colors::BlanchedAlmond);
		m_batch->DrawLine(v1, v2);
	}

	m_batch->End();
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
	m_raster.Reset();
	m_states.reset();
	m_effect.reset();
	m_batch.reset();
	m_inputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}