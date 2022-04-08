#pragma once

#include "Common.h"

#include "Cube/BaseCube.h"
#include "Renderer/DataTypes.h"
#include "Renderer/Renderable.h"

class BigCube : public BaseCube
{
public:
	BigCube() = default;
	~BigCube() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
};