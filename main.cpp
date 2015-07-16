#include "common.h"
#include "world.h"

using namespace Urho3D;

class MyApp : public Application
{
public:

	World worlds[3];
	SharedPtr<RenderPath> rp;
	SharedPtr<Camera> mainCamera;
	SharedPtr<Viewport> mainViewport;
	SharedPtr<Scene> mainScene;
	

    MyApp(Context* context) :
        Application(context)
    {

    }

    virtual void Setup()
    {
        // Called before engine initialization. engineParameters_ member variable can be modified here
        engineParameters_["WindowTitle"] = GetTypeName();
        engineParameters_["FullScreen"] = false;
        engineParameters_["Headless"] = false;
        engineParameters_["WindowWidth"] = 1280;
		engineParameters_["WindowHeight"] = 720;
		//engineParameters_["RenderPath"] = "Bin/CoreData/RenderPaths/Forward.xml";
    }

	void InitMainView() 
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		Renderer* renderer = GetSubsystem<Renderer>();

		mainScene = SharedPtr<Scene>(new Scene(context_));
		mainScene->CreateComponent<Octree>();


		mainCamera = mainScene->CreateComponent<Camera>();
		mainViewport = SharedPtr<Viewport>(new Viewport(context_, mainScene, mainCamera));


		//rp = SharedPtr<RenderPath>(new RenderPath());
		//rp->Load(cache->GetResource<XMLFile>("PostProcess/Portal.xml"));
		
		rp = mainViewport->GetRenderPath()->Clone();
		rp->Append(cache->GetResource<XMLFile>("PostProcess/Portal.xml"));
		mainViewport->SetRenderPath(rp);
		
		renderer->SetViewport(0, mainViewport);


	}

    virtual void Start()
    {
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		Renderer* renderer = GetSubsystem<Renderer>();
		renderer->SetNumViewports(0);


        // Called after engine initialization. Setup application & subscribe to events here
        SubscribeToEvent(E_KEYDOWN, HANDLER(MyApp, HandleKeyDown));
		SubscribeToEvent(E_UPDATE, HANDLER(MyApp, HandleUpdate));
		SubscribeToEvent(E_MOUSEBUTTONDOWN, HANDLER(MyApp, HandleMouseDown));
		SubscribeToEvent(E_NODECOLLISION, HANDLER(MyApp, HandleCollision));

		

		// usial worlds
		worlds[WORLD_A].InitScene(context_, "WorldA.xml", WORLD_A);
		worlds[WORLD_A].SetupPlayer();
		worlds[WORLD_A].CreateNamedRT("WorldA");

		worlds[WORLD_B].InitScene(context_, "WorldB.xml", WORLD_B);
		worlds[WORLD_B].SetupPlayer();
		worlds[WORLD_B].CreateNamedRT("WorldB");

		// world with portal mesh
		worlds[WORLD_P].InitScene(context_, "WorldP.xml", WORLD_P);
		worlds[WORLD_P].SetupPlayer();
		worlds[WORLD_P].CreateNamedRT("WorldP");
		worlds[WORLD_P].FillPortalsWorldWithVisibleObstaclesFrom(worlds[WORLD_A]);

		InitMainView();


    }

    virtual void Stop()
    {
        // Perform optional cleanup after main loop has terminated
    }

    void HandleKeyDown(StringHash eventType, VariantMap& eventData)
    {
        using namespace KeyDown;

        // Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
        int key = eventData[P_KEY].GetInt();
        if (key == KEY_ESC)
            engine_->Exit();


		if (key == KEY_F1)
		{

			worlds[WORLD_A].SaveFrame("A.png");
			worlds[WORLD_P].SaveFrame("P.png");
			worlds[WORLD_B].SaveFrame("B.png");


		}

    }

	void HandleMouseDown(StringHash eventType, VariantMap& eventData)
	{
		using namespace MouseButtonDown;

		int key = eventData[P_BUTTON].GetInt();

		if (key == MOUSEB_MIDDLE)
		{

		}



	}

	void HandleUpdate(StringHash eventType, VariantMap& eventData)
	{
		using namespace Update;

		// Take the frame time step, which is stored as a float
		float timeStep = eventData[P_TIMESTEP].GetFloat();

		worlds[WORLD_A].MoveCamera(timeStep);
		//worlds[WORLD_B].MoveCamera(timeStep);

		worlds[WORLD_B].CopyPlayerTransforms(worlds[WORLD_A]);
		worlds[WORLD_P].CopyPlayerTransforms(worlds[WORLD_A]);

	}

	void HandleCollision(StringHash eventType, VariantMap& eventData)
	{
		using namespace NodeCollision;
		Node* otherNode = (Node*)eventData[P_OTHERNODE].GetPtr();


	}

};

DEFINE_APPLICATION_MAIN(MyApp)