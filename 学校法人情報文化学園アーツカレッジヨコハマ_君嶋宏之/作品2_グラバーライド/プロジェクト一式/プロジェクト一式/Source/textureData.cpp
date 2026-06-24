#include "textureData.h"

TextureData::TextureData(SceneBase* _scene) :LoadBase(_scene)
{

}

TextureData::~TextureData()
{
}

void TextureData::Load()
{
	for (const auto& t : TEXTURE) {
		int handle = LoadGraph(t.path.c_str());
		AddHandleToMap(loadMap, t.key, handle);
	}
}
