#pragma once
#include <NRI.h>
#include <Extensions/NRIHelper.h>
#include <Extensions/NRIStreamer.h>
#include <Extensions/NRISwapChain.h>


struct NRIInterface : public nri::CoreInterface,
					  public nri::HelperInterface,
					  public nri::StreamerInterface,
					  public nri::SwapChainInterface
{
};
