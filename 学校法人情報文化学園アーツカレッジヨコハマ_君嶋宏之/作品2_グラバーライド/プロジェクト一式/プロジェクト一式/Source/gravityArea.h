#pragma once  
#include "stageObject.h"  
#include "effectManager.h" // Add this include to resolve the incomplete type error  

class GravityArea : public StageObject {  
public:  
   GravityArea(SceneBase* _scene);  
   ~GravityArea();  
   void Init(int Model, VECTOR pos, VECTOR scale, VECTOR rotation, int type, int _collisionId, int _gravityAreaType);  
   int GetGravityAreaType() const { return graviyAreaType; };  
   void Draw()override;
private:  
   int graviyAreaType;  
 
};
