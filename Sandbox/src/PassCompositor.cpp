#include "PassCompositor.h"
#include "IRenderPass.h"
#include "MainRenderPass.h"
#include "UIRenderPass.h"


PassCompositor::PassCompositor(){

}

PassCompositor::~PassCompositor(){
	for (auto pass : passes)
	{
		pass->Shutdown();
		delete pass;
    }
}

void PassCompositor::Init(){
    IRenderPass* system = new MainRenderPass();
    passes.push_back(system);
    system = new UIRenderPass();
    passes.push_back(system);


}

void PassCompositor::PrepareFrame(){
 for(auto pass: passes){
        pass->PrepareFrame();
    }
}

void PassCompositor::Draw(){
for(auto pass: passes){
        pass->Draw();
    }
}

void PassCompositor::Shutdown(){
    for(auto pass: passes){
        pass->Shutdown();
    }
}
