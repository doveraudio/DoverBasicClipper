#include "DoverBasicClipper.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

DoverBasicClipper::DoverBasicClipper(const InstanceInfo& info)
  : Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kGain)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");
  GetParam(kClip)->InitDouble("Clip", 100., 0., 100.0, 0.01, "%");
  GetParam(kPost)->InitDouble("Post", 50., 0., 100.0, 0.01, "%");


#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);


    const IRECT b = pGraphics->GetBounds();

    const IVStyle style{
      true, // Show label
      true, // Show value
      {
        DEFAULT_BGCOLOR, // Background
        DEFAULT_FGCOLOR, // Foreground
        DEFAULT_PRCOLOR, // Pressed
        COLOR_BLACK, // Frame
        DEFAULT_HLCOLOR, // Highlight
        DEFAULT_SHCOLOR, // Shadow
        COLOR_BLACK, // Extra 1
        DEFAULT_X2COLOR, // Extra 2
        DEFAULT_X3COLOR  // Extra 3
      }, // Colors
      IText(22.f, EAlign::Center) // Label text
    };

    const int nRows = 5;
    const int nCols = 8;

    int cellIdx = -1;

    auto nextCell = [&]() {
      return b.GetGridCell(++cellIdx, nRows, nCols).GetPadded(-5.);
    };

    auto sameCell = [&]() {
      return b.GetGridCell(cellIdx, nRows, nCols).GetPadded(-5.);
    };

    auto addLabel = [&](const char* label) {
      pGraphics->AttachControl(new ITextControl(nextCell().GetFromTop(20.f), label, style.labelText));
    };

    pGraphics->AttachControl(new IVLEDMeterControl<2>(b.GetFromRight(100).GetPadded(-30).GetVShifted(20)), kCtrlTagMeter);
    pGraphics->AttachControl(new ITextControl(b.GetFromBLHC(300, 45), "Basic Clipper", IText(50)));
    pGraphics->AttachControl(new IVKnobControl(nextCell().GetCentredInside(110.).GetVShifted(75).GetHShifted(25), kGain, "Gain", style, true));
    pGraphics->AttachControl(new IVKnobControl(nextCell().GetCentredInside(110.).GetVShifted(75).GetHShifted(125), kClip, "Clip", style, true));
    pGraphics->AttachControl(new IVKnobControl(nextCell().GetCentredInside(110.).GetVShifted(75).GetHShifted(225), kPost, "Post", style, true));
  };
#endif
}

#if IPLUG_DSP
void DoverBasicClipper::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double gain = GetParam(kGain)->Value() / 50.;
  const double post = GetParam(kPost)->Value() / 50.;
  const double clip = GetParam(kClip)->Value() / 100.;
  double sample = 0.f;
  double cursample = 0.f;

  const int nChans = NOutChansConnected();
  mMeterSender.ProcessBlock(outputs, nFrames, kCtrlTagMeter);
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      if (inputs[c][s] > 0) {
        cursample = inputs[c][s];

      }
      else {
        cursample = -inputs[c][s];
      }

      if ((cursample * gain) > clip) {
        sample = clip * post;
        //sample = 0.f;
      }
      else {
        sample = (cursample * gain) * post;

      }
      if (inputs[c][s] < 0)
      {
        sample = -sample;
      }
      outputs[c][s] = sample;

    }
  }
}

void DoverBasicClipper::OnIdle() {
  mMeterSender.TransmitData(*this);
}
#endif
