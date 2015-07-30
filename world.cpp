#include "common.h"
#include "world.h"

void World::InitScene(Context* context_, String sceneFileNam, EWORLDS worldType_) 
{
    context = context_;
    scene = SharedPtr<Scene>(new Scene(context_));
    ResourceCache* cache = context_->GetSubsystem<ResourceCache>();
    SharedPtr<File> sceneFile = cache->GetFile("Scenes/" + sceneFileNam);
    scene->LoadXML(*sceneFile);
    isActive = false;
    worldType = worldType_;
}

void World::CreateNamedRT(String worldName) 
{
    Graphics* g = context->GetSubsystem<Graphics>();
    int w = g->GetWidth();
    int h = g->GetHeight();


    renderTexture = SharedPtr<Texture2D>(new Texture2D(context));

    
    if (renderTexture) 
    {
        renderTexture->SetSize(w, h, Graphics::GetRGBFormat(), TEXTURE_RENDERTARGET);
        renderTexture->SetFilterMode(FILTER_NEAREST);
        renderTexture->SetName(worldName);

        ResourceCache* cache = context->GetSubsystem<ResourceCache>();
        cache->AddManualResource(renderTexture);
        

        RenderSurface* surface = renderTexture->GetRenderSurface();
        surface->SetUpdateMode(RenderSurfaceUpdateMode::SURFACE_UPDATEALWAYS);
        surface->SetViewport(0, camera.vp);
        
    }
}

void World::SaveFrame(String fileName)
{
    
    if (renderTexture) 
    {
        Renderer* renderer = context->GetSubsystem<Renderer>();
        Viewport* prevVp = renderer->GetViewport(3);
        renderer->SetViewport(0, camera.vp);
        Graphics* graphic = context->GetSubsystem<Graphics>();
        
        SharedPtr<Image> img = SharedPtr<Image>(new Image(context));
        img->SetSize(graphic->GetWidth(), graphic->GetHeight(), 3);
        
        
        Engine* eng = context->GetSubsystem<Engine>();
        eng->RunFrame();
        eng->Render();

        graphic->TakeScreenShot((*img));

    
        img->SavePNG(context->GetSubsystem<FileSystem>()->GetProgramDir() + fileName);

        renderer->SetViewport(0, prevVp);

    }


}

void World::SetupPlayer() 
{
    camera.node = scene->GetChild("camera", true);
    if (camera.node)
    {
        camera.camera = camera.node->GetComponent<Camera>();
        if (camera.camera)
        {
            camera.vp = SharedPtr<Viewport>(new Viewport(context, scene, camera.camera));
        }
    }

    player.node = scene->GetChild("player", true);
    player.body = player.node->GetComponent<RigidBody>();
}
void World::CopyPlayerTransforms(World& w)
{
    if (player.node)
    {
        player.node->SetWorldPosition(w.player.node->GetWorldPosition());
        player.node->SetWorldRotation(w.player.node->GetWorldRotation());
        camera.node->SetWorldPosition(w.camera.node->GetWorldPosition());
        camera.node->SetWorldRotation(w.camera.node->GetWorldRotation());
    }
}


void World::FillPortalsWorldWithVisibleObstaclesFrom(World& w)
{
    ResourceCache* cache = context->GetSubsystem<ResourceCache>();

    Material* black = cache->GetResource<Material>("Materials/Black.xml"); // notexture unlit black mat (for override white portals on screen)
    
    if (black)
    if (w.scene)
    {
        PODVector<Node*> nodes;
        w.scene->GetChildrenWithComponent<StaticModel>(nodes, true);
        
        for (int i = 0; i < nodes.Size(); i++) 
        {
            Node* n = nodes[i];
            if (n) 
            {
                StaticModel* model = n->GetComponent<StaticModel>();
                if (model) 
                {
                    Node* newNode = scene->CreateChild(n->GetName(), LOCAL);
                    StaticModel* newModel = newNode->CreateComponent<StaticModel>();
                    newModel->SetModel(model->GetModel());
                    newModel->SetMaterial(black);
                    newNode->SetWorldPosition(n->GetWorldPosition());
                    newNode->SetWorldRotation(n->GetWorldRotation());
                    newNode->SetWorldScale(n->GetWorldScale());
                }

            }
        }
    
    }
}

void World::MoveCamera(float timeStep) 
{
    static float yaw_ = 0.0f;
    static float pitch_ = 0.0f;


    Input* input = context->GetSubsystem<Input>();

    const float MOVE_SPEED = 20.0f;
    const float MOUSE_SENSITIVITY = 0.1f;

    IntVector2 mouseMove = input->GetMouseMove();
    yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
    pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
    pitch_ = Clamp(pitch_, -90.0f, 90.0f);

    camera.node->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

    Quaternion camRotation = camera.node->GetWorldRotation();
    camRotation.z_ = 0.0f;

    Vector3 dir = Vector3::ZERO;


    if (input->GetKeyDown('W'))
        dir += Vector3::FORWARD;

    if (input->GetKeyDown('S'))
        dir += Vector3::BACK;

    if (input->GetKeyDown('A'))
        dir += Vector3::LEFT;

    if (input->GetKeyDown('D'))
        dir += Vector3::RIGHT;

    if (input->GetKeyDown(KEY_SPACE))
    {
        dir += Vector3::UP;
    }

    if (dir.Length() > 0)
    {
        Vector3 curVel = player.body->GetLinearVelocity();
        if (curVel.Length() < 5.0f)
        {
            float multipler = 1.0f;
            //if (input->GetQualifierDown(KEY_SHIFT)) 
            if (input->GetKeyDown(KEY_LSHIFT))
            {
                multipler = 2.0f;
            }
            player.body->SetLinearVelocity(multipler * 200.0f * (camRotation * dir) * timeStep);
        }
    }
}