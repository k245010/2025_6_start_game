#pragma once
#include "../Library/gameObject.h"
#include <vector>

class CommonFind : public GameObject
{
public:
	CommonFind(SceneBase* _scene);
	~CommonFind();
	
	/// <summary>
	/// よく使うclassを纏めたcommonFindListからクラスを取り出すためのもの
	/// FindGameObjectのCommon版で使い方は一緒
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <returns></returns>
	template<class T>
	T* CommonFindObject()const;
private:
	//LoadManager* loadManager;
	//SlowMostionManager* slowMostionManager;
	//InputManager* inputManager;
	//EffekseerData* effekseerData;
	
	std::vector<GameObject*> commonFind;
};
template<class T>
inline T* CommonFind::CommonFindObject() const
{
	if (commonFind.empty()) return nullptr; // 追加: ベクタが空ならnullptr返却

	for (auto& com : commonFind) {
		if (auto ptr = dynamic_cast<T*>(com)) {
			return ptr;
		}
	}
	return nullptr;
}

