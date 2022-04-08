#pragma once

#include "Common.h"

#include "Cube/BaseCube.h"
#include "Renderer/DataTypes.h"
#include "Renderer/Renderable.h"

class SmallCube : public BaseCube
{
public:
	SmallCube() = default;
	~SmallCube() = default;

	virtual void Update(_In_ FLOAT deltaTime) override;
};