#pragma once
#include "pch.h"
#include "Effects.h"
class BasicRenderObject
{
protected:
	//simple view
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	std::unique_ptr<DirectX::GeometricPrimitive> m_shape;
	//full model view
	bool m_hasFullModelView;
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
	std::unique_ptr<DirectX::Model> m_model;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
public:
	BasicRenderObject(Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext) : 
		m_hasFullModelView(false)
	{
		m_effect = std::make_unique<DirectX::BasicEffect>(m_d3dDevice.Get());
		m_effect->SetTextureEnabled(true);
		m_effect->SetPerPixelLighting(true);
		m_effect->SetLightingEnabled(true);
		m_effect->SetLightEnabled(0, true);
		m_effect->SetLightDiffuseColor(0, DirectX::Colors::White);
		m_effect->SetLightDirection(0, -DirectX::SimpleMath::Vector3::UnitZ);

		m_shape = DirectX::GeometricPrimitive::CreateSphere(m_d3dContext.Get());
		m_shape->CreateInputLayout(m_effect.get(),
			m_inputLayout.ReleaseAndGetAddressOf());

		DX::ThrowIfFailed(
			DirectX::CreateWICTextureFromFile(m_d3dDevice.Get(), L"earth.bmp", nullptr,
				m_texture.ReleaseAndGetAddressOf()));

		m_effect->SetTexture(m_texture.Get());

	};

};