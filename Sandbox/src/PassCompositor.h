#include <vector>

#include "IRenderPass.h"
#include "MainRenderPass.h"
#include "UIRenderPass.h"

class PassCompositor
{
private:
	std::vector<IRenderPass*> passes;

public:
	void Init();
	void Shutdown();
	void Draw();
	void PrepareFrame();
	PassCompositor();
	~PassCompositor();
};
