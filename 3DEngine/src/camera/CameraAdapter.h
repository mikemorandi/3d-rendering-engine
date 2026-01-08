#pragma once

#include <memory>

class CameraAdapter;
using CameraAdapterPtr = std::shared_ptr<CameraAdapter>;
using CameraAdapterConstPtr = std::shared_ptr<const CameraAdapter>;
using CameraAdapterWeakPtr = std::weak_ptr<CameraAdapter>;
class Camera;
using CameraPtr = std::shared_ptr<Camera>;
using CameraConstPtr = std::shared_ptr<const Camera>;
using CameraWeakPtr = std::weak_ptr<Camera>;

class CameraAdapter
{

protected:
	CameraAdapter(const CameraPtr& cam) 
		: cam(cam) {};

	CameraPtr cam;

};

