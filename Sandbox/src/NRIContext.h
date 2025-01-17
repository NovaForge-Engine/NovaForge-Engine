#pragma once
#include <Extensions/NRIHelper.h>
#include <Extensions/NRIStreamer.h>
#include <Extensions/NRISwapChain.h>
#include <NRI.h>

struct NRIInterface : public nri::CoreInterface,
					  public nri::HelperInterface,
					  public nri::StreamerInterface,
					  public nri::SwapChainInterface
{
};
