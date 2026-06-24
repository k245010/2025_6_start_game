#include "motionData.h"

MotionLoad::MotionLoad(SceneBase* _scene):LoadBase(_scene)
{
	
}

MotionLoad::~MotionLoad()
{
	DeleteHandle(loadMap, MV1DeleteModel);
}

void MotionLoad::Load()
{
	for (auto& motion : MOTION) {

		int handle = MV1LoadModel(motion.path.c_str());
		AddHandleToMap(loadMap, motion.key, handle);
		
	}
	
}
