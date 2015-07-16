//#include "common.h"

enum EWORLDS { WORLD_A, WORLD_B, WORLD_P };

struct World
{
	SharedPtr<Scene> scene;
	SharedPtr<Context> context;
	bool isActive;
	EWORLDS worldType;

	struct
	{
		SharedPtr<Node> node;
		SharedPtr<Camera> camera;
		SharedPtr<Viewport> vp;
	} camera;

	struct
	{
		SharedPtr<Node> node;
		SharedPtr<RigidBody> body;
	} player;

	SharedPtr<Texture2D> renderTexture;

	void InitScene(Context* context_, String sceneFileNam, EWORLDS worldType_);
	void SetupPlayer();
	//void SetCurrent();
	void CopyPlayerTransforms(World& w);
	void MoveCamera(float timeStep);
	void CreateNamedRT(String worldName);
	void SaveFrame(String fileName);
	void FillPortalsWorldWithVisibleObstaclesFrom(World& w);


};
