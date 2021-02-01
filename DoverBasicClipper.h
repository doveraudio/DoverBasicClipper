#pragma once
#include "IControls.h"
#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  kClip,
  kPost,
  kNumParams
};

enum EControlTags
{
  kCtrlTagMeter = 0,
  kNumCtrlTags
};
using namespace iplug;
using namespace igraphics;

class DoverBasicClipper final : public Plugin
{
public:
  DoverBasicClipper(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnIdle() override;
private:
  IPeakSender<2> mMeterSender;
#endif
};
