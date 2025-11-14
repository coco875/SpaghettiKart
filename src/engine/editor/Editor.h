#ifndef __EDITOR_H__
#define __EDITOR_H__

#include <libultraship/libultraship.h>
#include <libultra/gbi.h>
#include "GameObject.h"


#ifdef __cplusplus
extern "C" {
#include "camera.h"
}

#include "ObjectPicker.h"
namespace Editor {
    class ObjectPicker;

    class Editor {
public:
    Editor();
    ~Editor();

    ObjectPicker eObjectPicker;
    std::vector<GameObject*> eGameObjects;

    void Tick();
    void Draw();
	void Load();
    void GenerateCollision();
    GameObject* AddObject(FVector pos, IRotator rot, FVector scale, const char* model, float collScale, GameObject::CollisionType collision, float boundingBoxSize);
    void AddLight(const char* name, FVector* pos, s8* rot);
    void ClearObjects();
    void ResetGizmo();
    void RemoveObject();
    void SelectObjectFromSceneExplorer(std::variant<AActor*, OObject*, GameObject*> object);
    void SetLevelDimensions(s16 minX, s16 maxX, s16 minZ, s16 maxZ, s16 minY, s16 maxY);
    void ClearMatrixPool();
    void DeleteObject();
    bool bEditorEnabled = false;

    Camera* eCamera = &cameras[0];
private:
    bool _draw = false;
    Vec3f _ray;

    s32 Inverse(MtxF* src, MtxF* dest);
    void Copy(MtxF* src, MtxF* dest);
    void Clear(MtxF* mf);
};
}
#endif

void SetLevelDimensions(s16 minX, s16 maxX, s16 minZ, s16 maxZ, s16 minY, s16 maxY);

#endif // __EDITOR_H__
