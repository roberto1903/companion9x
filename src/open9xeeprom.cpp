#include <stdlib.h>
#include <algorithm>
#include "helpers.h"
#include "open9xeeprom.h"
#include <QObject>
// #include <QMessageBox>

int8_t open9xV4209FromSource(RawSource source);
int8_t open9x209FromSource(RawSource source);
RawSource open9xV4209ToSource(int8_t value);
RawSource open9x209ToSource(int8_t value);
int8_t open9xFromSwitch(const RawSwitch & sw);
RawSwitch open9xToSwitch(int8_t sw);

class TimerModeV212: public TransformedField {
  public:
    TimerModeV212(TimerMode & mode):
      TransformedField(internalField),
      internalField(_mode),
      mode(mode)
    {
    }

    virtual void beforeExport()
    {
      if (mode >= TMRMODE_OFF && mode <= TMRMODE_THt)
        _mode = 0+mode-TMRMODE_OFF;
      else if (mode >= TMRMODE_FIRST_MOMENT_SWITCH)
        _mode = 26+mode-TMRMODE_FIRST_MOMENT_SWITCH;
      else if (mode >= TMRMODE_FIRST_SWITCH)
        _mode = 5+mode-TMRMODE_FIRST_SWITCH;
      else if (mode <= TMRMODE_FIRST_NEG_MOMENT_SWITCH)
        _mode = -22+mode-TMRMODE_FIRST_NEG_MOMENT_SWITCH;
      else if (mode <= TMRMODE_FIRST_NEG_SWITCH)
        _mode = -1+mode-TMRMODE_FIRST_NEG_SWITCH;
      else
        _mode = 0;
    }

    virtual void afterImport()
    {
      if (_mode <= -22)
        mode = TimerMode(TMRMODE_FIRST_NEG_MOMENT_SWITCH+(_mode+22));
      else if (_mode <= -1)
        mode = TimerMode(TMRMODE_FIRST_NEG_SWITCH+(_mode+1));
      else if (_mode < 5)
        mode = TimerMode(_mode);
      else if (_mode < 5+21)
        mode = TimerMode(TMRMODE_FIRST_SWITCH+(_mode-5));
      else
        mode = TimerMode(TMRMODE_FIRST_MOMENT_SWITCH+(_mode-5-21));
    }

  protected:
    SignedField<8> internalField;
    TimerMode & mode;
    int _mode;
};

template <int N>
class SwitchField: public TransformedField {
  public:
    SwitchField(RawSwitch & sw):
      TransformedField(internalField),
      internalField(_sw),
      sw(sw)
    {
    }

    virtual void beforeExport()
    {
      _sw = open9xFromSwitch(sw);
    }

    virtual void afterImport()
    {
      sw = open9xToSwitch(_sw);
    }

  protected:
    SignedField<N> internalField;
    int _sw;
    RawSwitch & sw;
};

class PhaseField: public TransformedField {
  public:
    PhaseField(PhaseData & phase, BoardEnum board):
      TransformedField(internalField),
      phase(phase),
      board(board)
    {
      if (board == BOARD_STOCK) {
        // On stock we use 10bits per trim
        for (int i=0; i<NUM_STICKS; i++)
          internalField.Append(new SignedField<8>(trimBase[i]));
        for (int i=0; i<NUM_STICKS; i++)
          internalField.Append(new SignedField<2>(trimExt[i]));
      }
      else {
        for (int i=0; i<NUM_STICKS; i++)
          internalField.Append(new SignedField<16>(phase.trim[i]));
      }

      internalField.Append(new SwitchField<8>(phase.swtch));
      internalField.Append(new ZCharField<6>(phase.name));
      internalField.Append(new UnsignedField<4>(phase.fadeIn));
      internalField.Append(new UnsignedField<4>(phase.fadeOut));

      if (board == BOARD_GRUVIN9X) {
        for (int i=0; i<2; i++) {
          internalField.Append(new SignedField<16>(phase.rotaryEncoders[i]));
        }
      }

      if (board != BOARD_STOCK) {
        for (int i=0; i<O9X_MAX_GVARS; i++) {
          internalField.Append(new SignedField<16>(phase.gvars[i]));
        }
      }

      // Dump("Phase");
    }

    virtual void beforeExport()
    {
      if (board == BOARD_STOCK) {
        for (int i=0; i<NUM_STICKS; i++) {
          trimBase[i] = phase.trim[i] >> 2;
          trimExt[i] = (phase.trim[i] & 0x03);
        }
      }
    }

    virtual void afterImport()
    {
      if (board == BOARD_STOCK) {
        for (int i=0; i<NUM_STICKS; i++)
          phase.trim[i] = ((trimBase[i]) << 2) + (trimExt[i] & 0x03);
      }
    }

  protected:
    StructField internalField;
    PhaseData & phase;
    BoardEnum board;
    int trimBase[NUM_STICKS];
    int trimExt[NUM_STICKS];
};

template <int N>
class MixSourceField: public TransformedField {
  public:
    MixSourceField(RawSource & source, BoardEnum board):
      TransformedField(internalField),
      internalField(param),
      source(source),
      board(board),
      param(0),
      rotencCount(board == BOARD_GRUVIN9X ? 2 : 0)
    {
    }

    virtual void beforeExport()
    {
      if (source.type == SOURCE_TYPE_NONE) {
        param = 0;
      }
      else if (source.type == SOURCE_TYPE_STICK) {
        param = 1 + source.index;
      }
      else if (source.type == SOURCE_TYPE_ROTARY_ENCODER) {
        if (board == BOARD_GRUVIN9X) {
          param = 8 + source.index;
        }
        else {
          EEPROMWarnings += ::QObject::tr("Open9x on this board doesn't have Rotary Encoders") + "\n";
          param = 5 + source.index; // use pots instead
        }
      }
      else if (source.type == SOURCE_TYPE_TRIM) {
        param = 8 + rotencCount + source.index; // use pots instead
      }
      else if (source.type == SOURCE_TYPE_MAX) {
        param = 12 + rotencCount; // MAX
      }
      else if (source.type == SOURCE_TYPE_3POS) {
        param = 13 + rotencCount;
      }
      else if (source.type == SOURCE_TYPE_SWITCH) {
        param = 13 + rotencCount + open9xFromSwitch(RawSwitch(source.index));
      }
      else if (source.type == SOURCE_TYPE_CYC) {
        param = 35 + rotencCount + source.index;
      }
      else if (source.type == SOURCE_TYPE_PPM) {
        param = 38 + rotencCount + source.index;
      }
      else if (source.type == SOURCE_TYPE_CH) {
        param = 46 + rotencCount + source.index;
      }
    }

    virtual void afterImport()
    {
      if (param == 0) {
        source = RawSource(SOURCE_TYPE_NONE);
      }
      else if (param <= 7) {
        source = RawSource(SOURCE_TYPE_STICK, param-1);
      }
      else if (param <= 7 + rotencCount) {
        source = RawSource(SOURCE_TYPE_STICK, param-8);
      }
      else if (param <= 11 + rotencCount) {
        source = RawSource(SOURCE_TYPE_TRIM, param-8-rotencCount);
      }
      else if (param == 12 + rotencCount) {
        source = RawSource(SOURCE_TYPE_MAX);
      }
      else if (param == 13 + rotencCount) {
        source = RawSource(SOURCE_TYPE_3POS);
      }
      else if (param <= 34 + rotencCount) {
        source = RawSource(SOURCE_TYPE_SWITCH, open9xToSwitch(param-13-rotencCount).toValue());
      }
      else if (param <= 37 + rotencCount) {
        source = RawSource(SOURCE_TYPE_CYC, param-35-rotencCount);
      }
      else if (param <= 45 + rotencCount) {
        source = RawSource(SOURCE_TYPE_PPM, param-38-rotencCount);
      }
      else {
        source = RawSource(SOURCE_TYPE_CH, param-46-rotencCount);
      }
    }

  protected:
    UnsignedField<N> internalField;
    RawSource & source;
    BoardEnum board;
    unsigned int param;
    unsigned int rotencCount;
};

class HeliField: public StructField {
  public:
    HeliField(SwashRingData & heli, BoardEnum board)
    {
      Append(new BoolField<1>(heli.invertELE));
      Append(new BoolField<1>(heli.invertAIL));
      Append(new BoolField<1>(heli.invertCOL));
      Append(new UnsignedField<5>(heli.type));
      Append(new MixSourceField<8>(heli.collectiveSource, board));
      Append(new UnsignedField<8>(heli.value));
    }
};

template <int N>
class MixCurveParamField: public SignedField<N> {
  public:
    MixCurveParamField(int & curve, int & differential):
      SignedField<N>(param),
      curve(curve),
      differential(differential)
    {
    }

    virtual void ExportBits(QBitArray & output)
    {
      param = (curve ? curve : differential);
      SignedField<N>::ExportBits(output);
    }

    virtual void ImportBits(QBitArray & input)
    {
      // NOTA: screen.type should have been imported first!
      SignedField<N>::ImportBits(input);
      if (curve)
        curve = param;
      else
        differential = param;
    }

  protected:
    int & curve;
    int & differential;
    int param;
};

class MixField: public TransformedField {
  public:
    MixField(MixData & mix, BoardEnum board):
      TransformedField(internalField),
      mix(mix)
    {
      internalField.Append(new UnsignedField<4>(_mix.destCh));
      internalField.Append(new BoolField<1>(_curve));
      internalField.Append(new BoolField<1>(_mix.noExpo));
      internalField.Append(new SpareBitsField<2>());
      internalField.Append(new SignedField<8>(_mix.weight));
      internalField.Append(new SwitchField<6>(_mix.swtch));
      internalField.Append(new UnsignedField<2>((unsigned int &)_mix.mltpx));
      internalField.Append(new UnsignedField<5>(_mix.phases));
      internalField.Append(new SignedField<3>(_mix.carryTrim));
      internalField.Append(new MixSourceField<6>(_mix.srcRaw, board));
      internalField.Append(new UnsignedField<2>(_mix.mixWarn));
      internalField.Append(new UnsignedField<4>(_mix.delayUp));
      internalField.Append(new UnsignedField<4>(_mix.delayDown));
      internalField.Append(new UnsignedField<4>(_mix.speedUp));
      internalField.Append(new UnsignedField<4>(_mix.speedDown));
      internalField.Append(new MixCurveParamField<8>(_mix.curve, _mix.differential));
      internalField.Append(new SignedField<8>(_mix.sOffset));

      // Dump("Mix");
    }

    virtual void beforeExport()
    {
      if (mix.destCh && mix.srcRaw.type != SOURCE_TYPE_NONE) {
        _mix = mix;
        _mix.destCh -= 1;
        _curve = mix.curve;
      }
    }

    virtual void afterImport()
    {
      if (_mix.srcRaw.type != SOURCE_TYPE_NONE) {
        mix = _mix;
        mix.destCh += 1;
      }
    }

  protected:
    StructField internalField;
    MixData & mix;
    MixData _mix;
    bool _curve;
};

class ExpoFieldV211: public TransformedField {
  public:
    ExpoFieldV211(ExpoData & expo):
      TransformedField(internalField),
      expo(expo)
    {
      internalField.Append(new UnsignedField<2>(expo.mode));
      internalField.Append(new SwitchField<6>(expo.swtch));
      internalField.Append(new UnsignedField<2>(expo.chn));
      internalField.Append(new UnsignedField<5>(expo.phases));
      internalField.Append(new BoolField<1>(_curveMode));
      internalField.Append(new SignedField<8>(expo.weight));
      internalField.Append(new SignedField<8>(expo.curveParam));

      // Dump("Expo");
    }

    virtual void beforeExport()
    {
      _curveMode = (expo.curveMode && expo.curveParam);
    }

    virtual void afterImport()
    {
      expo.curveMode = _curveMode;
    }

  protected:
    StructField internalField;
    ExpoData & expo;
    bool _curveMode;
};

class LimitFieldV211: public StructField {
  public:
    LimitFieldV211(LimitData & limit)
    {
      Append(new ConversionField< SignedField<8> >(limit.min, +100));
      Append(new ConversionField< SignedField<8> >(limit.max, -100));
      Append(new SignedField<8>(limit.ppmCenter));
      Append(new SignedField<14>(limit.offset));
      Append(new BoolField<1>(limit.symetrical));
      Append(new BoolField<1>(limit.revert));

      // Dump("Limit");
    }
};

class CurvesField: public TransformedField {
  public:
    CurvesField(CurveData * curves):
      TransformedField(internalField),
      curves(curves)
    {
      for (int i=0; i<O9X_MAX_CURVES; i++)
        internalField.Append(new SignedField<8>(_curves[i]));
      for (int i=0; i<O9X_NUM_POINTS; i++)
        internalField.Append(new SignedField<8>(_points[i]));

      // Dump("Curves");
    }

    virtual void beforeExport()
    {
      memset(_points, 0, sizeof(_points));

      int * cur = &_points[0];
      int offset = 0;
      for (int i=0; i<O9X_MAX_CURVES; i++) {
        CurveData *curve = &curves[i];
        offset += (curve->custom ? curve->count * 2 - 2 : curve->count) - 5;
        if (offset > O9X_NUM_POINTS - 5 * O9X_MAX_CURVES) {
          EEPROMWarnings += ::QObject::tr("open9x only accepts %1 points in all curves").arg(O9X_NUM_POINTS) + "\n";
          break;
        }
        _curves[i] = offset;
        for (int j=0; j<curve->count; j++) {
          *cur++ = curve->points[j].y;
        }
        if (curve->custom) {
          for (int j=1; j<curve->count-1; j++) {
            *cur++ = curve->points[j].x;
          }
        }
      }
    }

    virtual void afterImport()
    {
      for (int i=0; i<O9X_MAX_CURVES; i++) {
        CurveData *curve = &curves[i];
        int * cur = &_points[i==0 ? 0 : 5*i + _curves[i-1]];
        int * next = &_points[(i+1) + _curves[i]];
        int size = next - cur;
        if (size % 2 == 0) {
          curve->count = (size / 2) + 1;
          curve->custom = true;
        }
        else {
          curve->count = size;
          curve->custom = false;
        }
        for (int j=0; j<curve->count; j++)
          curve->points[j].y = cur[j];
        if (curve->custom) {
          curve->points[0].x = -100;
          for (int j=1; j<curve->count-1; j++)
            curve->points[j].x = cur[curve->count+j-1];
          curve->points[curve->count-1].x = +100;
        }
        else {
          for (int j=0; j<curve->count; j++)
            curve->points[j].x = -100 + (200*i) / (curve->count-1);
        }
      }
    }

  protected:
    StructField internalField;
    CurveData *curves;
    int _curves[O9X_MAX_CURVES];
    int _points[O9X_NUM_POINTS];
};

inline int8_t open9xFromSource(RawSource source, BoardEnum board)
{
  if (board == BOARD_GRUVIN9X)
    return open9xV4209FromSource(source);
  else
    return open9x209FromSource(source);
}

inline RawSource open9xToSource(int8_t value, BoardEnum board)
{
  if (board == BOARD_GRUVIN9X)
      return open9xV4209ToSource(value);
  else
    return open9x209ToSource(value);
}

class CustomSwitchField: public TransformedField {
  public:
    CustomSwitchField(CustomSwData & csw, BoardEnum board):
      TransformedField(internalField),
      board(board),
      csw(csw)
    {
      internalField.Append(new SignedField<8>(v1));
      internalField.Append(new SignedField<8>(v2));
      internalField.Append(new UnsignedField<8>(func));

      // Dump("CustomSwitch");
    }

    virtual void beforeExport()
    {
      func = csw.func;
      v1 = csw.val1;
      v2 = csw.val2;

      if ((csw.func >= CS_VPOS && csw.func <= CS_ANEG) || csw.func >= CS_EQUAL) {
        v1 = open9xFromSource(RawSource(csw.val1), board);
      }

      if (csw.func >= CS_EQUAL && csw.func <= CS_ELESS) {
        v2 = open9xFromSource(RawSource(csw.val2), board);
      }

      if (csw.func >= CS_AND && csw.func <= CS_XOR) {
        v1 = open9xFromSwitch(RawSwitch(csw.val1));
        v2 = open9xFromSwitch(RawSwitch(csw.val2));
      }
    }

    virtual void afterImport()
    {
      csw.func = func;
      csw.val1 = v1;
      csw.val2 = v2;

      if ((csw.func >= CS_VPOS && csw.func <= CS_ANEG) || csw.func >= CS_EQUAL) {
        csw.val1 = open9xToSource(v1, board).toValue();
      }

      if (csw.func >= CS_EQUAL && csw.func <= CS_ELESS) {
        csw.val2 = open9xToSource(v2, board).toValue();
      }

      if (csw.func >= CS_AND && csw.func <= CS_XOR) {
        csw.val1 = open9xToSwitch(v1).toValue();
        csw.val2 = open9xToSwitch(v2).toValue();
      }
    }

  protected:
    StructField internalField;
    BoardEnum board;
    CustomSwData & csw;
    int v1;
    int v2;
    unsigned int func;
};

const int stockFunctionsConversion[] = {
    0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,
    FuncTrainer, 16, FuncTrainerRUD, 17, FuncTrainerELE, 18, FuncTrainerTHR, 19, FuncTrainerAIL, 20,
    FuncInstantTrim, 21,
    FuncPlaySound, 22,
    FuncPlayHaptic, 23,
    FuncReset, 24,
    FuncVario, 25,
    FuncPlayPrompt, 26,
    FuncPlayValue, 27,
    FuncBacklight, 28,
    FuncAdjustGV1, 29,
    FuncAdjustGV2, 30,
    FuncAdjustGV3, 31,
    FuncAdjustGV4, 32,
    FuncAdjustGV5, 33
};

const int gruvin9xFunctionsConversion[] = {
    0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,
    FuncTrainer, 16, FuncTrainerRUD, 17, FuncTrainerELE, 18, FuncTrainerTHR, 19, FuncTrainerAIL, 20,
    FuncInstantTrim, 21,
    FuncPlaySound, 22,
    FuncPlayHaptic, 23,
    FuncReset, 24,
    FuncVario, 25,
    FuncPlayPrompt, 26,
    FuncPlayValue, 27,
    FuncLogs, 28,
    FuncBacklight, 29,
    FuncAdjustGV1, 30,
    FuncAdjustGV2, 31,
    FuncAdjustGV3, 32,
    FuncAdjustGV4, 33,
    FuncAdjustGV5, 34
};

class CustomFunctionField: public StructField {
  public:
    CustomFunctionField(FuncSwData & fn, BoardEnum board)
    {
      Append(new SwitchField<8>(fn.swtch));
      if (board == BOARD_GRUVIN9X)
        Append(new ConversionField< UnsignedField<7> >((unsigned int &)fn.func, TABLE_CONVERSION(gruvin9xFunctionsConversion), ::QObject::tr("Open9x on gruvin9x board doesn't accept this function")));
      else
        Append(new ConversionField< UnsignedField<7> >((unsigned int &)fn.func, TABLE_CONVERSION(stockFunctionsConversion), ::QObject::tr("Open9x on stock board doesn't accept this function")));
      Append(new BoolField<1>(fn.enabled));
      Append(new UnsignedField<8>(fn.param));

      // Dump("CustomFunction");
    }
};

class FrskyScreenField: public DataField {
  public:
    FrskyScreenField(FrSkyScreenData & screen):
      screen(screen)
    {
      for (int i=0; i<4; i++) {
        bars.Append(new UnsignedField<8>(screen.body.bars[i].source));
        bars.Append(new UnsignedField<8>(screen.body.bars[i].barMin));
        bars.Append(new UnsignedField<8>(screen.body.bars[i].barMax));
      }

      for (int i=0; i<8; i++) {
        numbers.Append(new UnsignedField<8>(screen.body.cells[i]));
      }
      for (int i=0; i<4; i++) {
        numbers.Append(new SpareBitsField<8>());
      }
    }

    virtual void ExportBits(QBitArray & output)
    {
      if (screen.type == 0)
        numbers.ExportBits(output);
      else
        bars.ExportBits(output);
    }

    virtual void ImportBits(QBitArray & input)
    {
      // NOTA: screen.type should have been imported first!
      if (screen.type == 0)
        numbers.ImportBits(input);
      else
        bars.ImportBits(input);
    }

    virtual unsigned int size()
    {
      // NOTA: screen.type should have been imported first!
      if (screen.type == 0)
        return numbers.size();
      else
        return bars.size();
    }

  protected:
    FrSkyScreenData & screen;
    StructField bars;
    StructField numbers;
};

const int rssiLevelConversion[2][8] = { {0, 2, 1, 3, 2, 0, 3, 1}, {0, 1, 1, 2, 2, 3, 3, 4} };

class FrskyField: public StructField {
  public:
    FrskyField(FrSkyData & frsky)
    {
      for (int i=0; i<2; i++) {
        Append(new UnsignedField<8>(frsky.channels[i].ratio));
        Append(new SignedField<12>(frsky.channels[i].offset));
        Append(new UnsignedField<4>(frsky.channels[i].type));
        for (int j=0; j<2; j++)
          Append(new UnsignedField<8>(frsky.channels[i].alarms[j].value));
        for (int j=0; j<2; j++)
          Append(new UnsignedField<2>(frsky.channels[i].alarms[j].level));
        for (int j=0; j<2; j++)
          Append(new UnsignedField<1>(frsky.channels[i].alarms[j].greater));
        Append(new UnsignedField<2>(frsky.channels[i].multiplier));
      }

      Append(new UnsignedField<2>(frsky.usrProto));
      Append(new UnsignedField<2>(frsky.blades));
      Append(new SpareBitsField<4>());
      Append(new UnsignedField<3>(frsky.voltsSource));
      Append(new UnsignedField<3>(frsky.currentSource));
      Append(new UnsignedField<1>(frsky.screens[0].type));
      Append(new UnsignedField<1>(frsky.screens[1].type));
      for (int i=0; i<2; i++) {
        Append(new ConversionField< UnsignedField<2> >(frsky.rssiAlarms[i].level, 4, rssiLevelConversion[i]));
        Append(new ConversionField< UnsignedField<6> >(frsky.rssiAlarms[i].value, -50));
      }
      for (int i=0; i<2; i++) {
        Append(new FrskyScreenField(frsky.screens[i]));
      }
      Append(new UnsignedField<3>(frsky.varioSource));
      Append(new UnsignedField<5>(frsky.varioSpeedUpMin));
      Append(new UnsignedField<8>(frsky.varioSpeedDownMin));

      // Dump("FrSky");
    }
};

const int protocolConversion[] = {PPM, 0, PPM16, 1, PPMSIM, 2, PXX, 3, DSM2, 4};
const int channelsConversion[] = {4, -2, 6, -1, 8, 0, 10, 1, 12, 2, 14, 3, 16, 4};
int exportPpmDelay(int delay) { return (delay - 300) / 50; }
int importPpmDelay(int delay) { return 300 + 50 * delay; }

Open9xModelDataNew::Open9xModelDataNew(ModelData & modelData, BoardEnum board, unsigned int variant):
  board(board),
  variant(variant)
{
  Append(new ZCharField<10>(modelData.name));

  for (int i=0; i<O9X_MAX_TIMERS; i++) {
    Append(new TimerModeV212(modelData.timers[i].mode));
    Append(new UnsignedField<16>(modelData.timers[i].val));
    if (board == BOARD_GRUVIN9X) {
      Append(new BoolField<1>(modelData.timers[i].persistent));
      Append(new SpareBitsField<15>());
    }
  }

  Append(new ConversionField< SignedField<3> >(modelData.protocol, TABLE_CONVERSION(protocolConversion), ::QObject::tr("Open9x doesn't accept this protocol")));
  Append(new BoolField<1>(modelData.thrTrim));
  Append(new ConversionField< SignedField<4> >(modelData.ppmNCH, TABLE_CONVERSION(channelsConversion), ::QObject::tr("Open9x doesn't allow this number of channels")));
  Append(new UnsignedField<3>(modelData.trimInc));
  Append(new BoolField<1>(modelData.disableThrottleWarning));
  Append(new BoolField<1>(modelData.pulsePol));
  Append(new BoolField<1>(modelData.extendedLimits));
  Append(new BoolField<1>(modelData.extendedTrims));
  Append(new SpareBitsField<1>());
  Append(new ConversionField< SignedField<8> >(modelData.ppmDelay, exportPpmDelay, importPpmDelay));
  Append(new UnsignedField<8>(modelData.beepANACenter));

  for (int i=0; i<O9X_MAX_MIXERS; i++)
    Append(new MixField(modelData.mixData[i], board));
  for (int i=0; i<O9X_NUM_CHNOUT; i++)
    Append(new LimitFieldV211(modelData.limitData[i]));
  for (int i=0; i<O9X_MAX_EXPOS; i++)
    Append(new ExpoFieldV211(modelData.expoData[i]));
  Append(new CurvesField(modelData.curves));
  for (int i=0; i<O9X_NUM_CSW; i++)
    Append(new CustomSwitchField(modelData.customSw[i], board));
  for (int i=0; i<O9X_NUM_FSW; i++)
    Append(new CustomFunctionField(modelData.funcSw[i], board));
  Append(new HeliField(modelData.swashRingData, board));
  for (int i=0; i<O9X_MAX_PHASES; i++)
    Append(new PhaseField(modelData.phaseData[i], board));
  Append(new SignedField<8>(modelData.ppmFrameLength));
  Append(new UnsignedField<8>(modelData.thrTraceSrc));
  Append(new UnsignedField<8>(modelData.modelId));
  Append(new UnsignedField<8>(modelData.switchWarningStates));

  if (board == BOARD_STOCK && (variant & GVARS_VARIANT)) {
    for (int i=0; i<O9X_MAX_GVARS; i++) {
      // on M64 GVARS are common to all phases, and there is no name
      Append(new SignedField<16>(modelData.phaseData[0].gvars[i]));
    }
  }

  if (board != BOARD_STOCK) {
    for (int i=0; i<O9X_MAX_GVARS; i++) {
      Append(new ZCharField<6>(modelData.gvars_names[i]));
    }
  }

  if (board != BOARD_STOCK || (variant & FRSKY_VARIANT)) {
    Append(new FrskyField(modelData.frsky));
  }
}
