#include <stdlib.h>
#include <algorithm>
#include "helpers.h"
#include "open9xeeprom.h"
#include <QObject>
// #include <QMessageBox>

int8_t open9xStock209FromSource(RawSource source);
RawSource open9xStock209ToSource(int8_t value);
int8_t open9xStockFromSwitch(const RawSwitch & sw);
RawSwitch open9xStockToSwitch(int8_t sw);
int8_t open9xV4209FromSource(RawSource source);
RawSource open9xV4209ToSource(int8_t value);
int8_t open9xArmFromSwitch(const RawSwitch & sw);
RawSwitch open9xArmToSwitch(int8_t sw);
RawSource open9xArm210ToSource(int8_t value);
int8_t open9xArm210FromSource(RawSource source);

#define HAS_PERSISTENT_TIMERS(board) (IS_ARM(board) || board == BOARD_GRUVIN9X)
#define MAX_CUSTOM_SWITCHES(board)   (IS_ARM(board) ? 32 : 12)
#define HAS_LARGE_LCD(board)         (board == BOARD_X9DA || board == BOARD_ACT)
#define MAX_VIEWS(board)             (HAS_LARGE_LCD(board) ? 2 : 256)

inline int8_t open9xFromSource(RawSource source, BoardEnum board)
{
  if (board == BOARD_SKY9X)
    return open9xArm210FromSource(source);
  else if (board == BOARD_GRUVIN9X)
    return open9xV4209FromSource(source);
  else
    return open9xStock209FromSource(source);
}

inline RawSource open9xToSource(int8_t value, BoardEnum board)
{
  if (board == BOARD_SKY9X)
    return open9xArm210ToSource(value);
  else if (board == BOARD_GRUVIN9X)
    return open9xV4209ToSource(value);
  else
    return open9xStock209ToSource(value);
}

inline int8_t open9xFromSwitch(const RawSwitch & sw, BoardEnum board)
{
  if (board == BOARD_SKY9X)
    return open9xArmFromSwitch(sw);
  else
    return open9xStockFromSwitch(sw);
}

inline RawSwitch open9xToSwitch(int8_t sw, BoardEnum board)
{
  if (board == BOARD_SKY9X)
    return open9xArmToSwitch(sw);
  else
    return open9xStockToSwitch(sw);
}

class TimerModeField: public TransformedField {
  public:
    TimerModeField(TimerMode & mode, BoardEnum board):
      TransformedField(internalField),
      internalField(_mode),
      mode(mode),
      board(board),
      numCSW(MAX_CUSTOM_SWITCHES(board)),
      _mode(0)
    {
    }

    virtual void beforeExport()
    {
      if (mode >= TMRMODE_OFF && mode <= TMRMODE_THt)
        _mode = 0+mode-TMRMODE_OFF;
      else if (mode >= TMRMODE_FIRST_MOMENT_SWITCH)
        _mode = numCSW+14+mode-TMRMODE_FIRST_MOMENT_SWITCH;
      else if (mode >= TMRMODE_FIRST_SWITCH)
        _mode = 5+mode-TMRMODE_FIRST_SWITCH;
      else if (mode <= TMRMODE_FIRST_NEG_MOMENT_SWITCH)
        _mode = -numCSW-10+mode-TMRMODE_FIRST_NEG_MOMENT_SWITCH;
      else if (mode <= TMRMODE_FIRST_NEG_SWITCH)
        _mode = -1+mode-TMRMODE_FIRST_NEG_SWITCH;
      else
        _mode = 0;
    }

    virtual void afterImport()
    {
      if (_mode <= -numCSW-10)
        mode = TimerMode(TMRMODE_FIRST_NEG_MOMENT_SWITCH+(_mode+numCSW+10));
      else if (_mode <= -1)
        mode = TimerMode(TMRMODE_FIRST_NEG_SWITCH+(_mode+1));
      else if (_mode < 5)
        mode = TimerMode(_mode);
      else if (_mode < numCSW+14)
        mode = TimerMode(TMRMODE_FIRST_SWITCH+(_mode-5));
      else
        mode = TimerMode(TMRMODE_FIRST_MOMENT_SWITCH+(_mode-numCSW-14));
    }

  protected:
    SignedField<8> internalField;
    TimerMode & mode;
    BoardEnum board;
    int numCSW;
    int _mode;
};

template <int N>
class SwitchField: public TransformedField {
  public:
    SwitchField(RawSwitch & sw, BoardEnum board):
      TransformedField(internalField),
      internalField(_sw),
      sw(sw),
      board(board)
    {
    }

    virtual void beforeExport()
    {
      _sw = open9xFromSwitch(sw, board);
    }

    virtual void afterImport()
    {
      sw = open9xToSwitch(_sw, board);
    }

  protected:
    SignedField<N> internalField;
    int _sw;
    RawSwitch & sw;
    BoardEnum board;
};

class PhaseField: public TransformedField {
  public:
    PhaseField(PhaseData & phase, int index, BoardEnum board):
      TransformedField(internalField),
      internalField("Phase"),
      phase(phase),
      index(index),
      board(board),
      rotencCount(IS_ARM(board) ? 1 : (board == BOARD_GRUVIN9X ? 2 : 0))
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
          internalField.Append(new SignedField<16>(trimBase[i]));
      }

      internalField.Append(new SwitchField<8>(phase.swtch, board));
      if (HAS_LARGE_LCD(board))
        internalField.Append(new ZCharField<10>(phase.name));
      else
        internalField.Append(new ZCharField<6>(phase.name));
      internalField.Append(new UnsignedField<4>(phase.fadeIn));
      internalField.Append(new UnsignedField<4>(phase.fadeOut));

      for (int i=0; i<rotencCount; i++) {
        internalField.Append(new SignedField<16>(phase.rotaryEncoders[i]));
      }

      if (board != BOARD_STOCK) {
        for (int i=0; i<O9X_MAX_GVARS; i++) {
          internalField.Append(new SignedField<16>(phase.gvars[i]));
        }
      }
    }

    virtual void beforeExport()
    {
      for (int i=0; i<NUM_STICKS; i++) {
        int trim;
        if (phase.trimRef[i] >= 0) {
          trim = 501 + phase.trimRef[i] - (phase.trimRef[i] >= index ? 1 : 0);
        }
        else {
          trim = std::max(-500, std::min(500, phase.trim[i]));
        }
        if (board == BOARD_STOCK) {
          trimBase[i] = trim >> 2;
          trimExt[i] = (trim & 0x03);
        }
        else {
          trimBase[i] = trim;
        }
      }
    }

    virtual void afterImport()
    {
      for (int i=0; i<NUM_STICKS; i++) {
        int trim;
        if (board == BOARD_STOCK)
          trim = ((trimBase[i]) << 2) + (trimExt[i] & 0x03);
        else
          trim = trimBase[i];
        if (trim > 500) {
          phase.trimRef[i] = trim - 501;
          if (phase.trimRef[i] >= index)
            phase.trimRef[i] += 1;
          phase.trim[i] = 0;
        }
        else {
          phase.trim[i] = trim;
        }

      }
    }

  protected:
    StructField internalField;
    PhaseData & phase;
    int index;
    BoardEnum board;
    int rotencCount;
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
      rotencCount(board == BOARD_SKY9X ? 1 : (board == BOARD_GRUVIN9X ? 2 : 0)),
      customSwitchesCount(MAX_CUSTOM_SWITCHES(board))
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
        param = 13 + rotencCount + open9xFromSwitch(RawSwitch(source.index), board);
      }
      else if (source.type == SOURCE_TYPE_CYC) {
        param = 23 + customSwitchesCount + rotencCount + source.index;
      }
      else if (source.type == SOURCE_TYPE_PPM) {
        param = 26 + customSwitchesCount + rotencCount + source.index;
      }
      else if (source.type == SOURCE_TYPE_CH) {
        param = 34 + customSwitchesCount + rotencCount + source.index;
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
      else if (param <= 22 + customSwitchesCount + rotencCount) {
        source = RawSource(SOURCE_TYPE_SWITCH, open9xToSwitch(param-13-rotencCount, board).toValue());
      }
      else if (param <= 25 + customSwitchesCount + rotencCount) {
        source = RawSource(SOURCE_TYPE_CYC, param-23-customSwitchesCount-rotencCount);
      }
      else if (param <= 33 + customSwitchesCount + rotencCount) {
        source = RawSource(SOURCE_TYPE_PPM, param-26-customSwitchesCount-rotencCount);
      }
      else {
        source = RawSource(SOURCE_TYPE_CH, param-34-customSwitchesCount-rotencCount);
      }
    }

  protected:
    UnsignedField<N> internalField;
    RawSource & source;
    BoardEnum board;
    unsigned int param;
    unsigned int rotencCount;
    unsigned int customSwitchesCount;
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

class MixField: public TransformedField {
  public:
    MixField(MixData & mix, BoardEnum board):
      TransformedField(internalField),
      internalField("Mix"),
      mix(mix)
    {
      if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>(_destCh));
        internalField.Append(new UnsignedField<16>(mix.phases));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new BoolField<1>(mix.noExpo));
        internalField.Append(new SignedField<3>(mix.carryTrim));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        internalField.Append(new SpareBitsField<1>());
        internalField.Append(new SignedField<16>(mix.weight));
        internalField.Append(new SwitchField<8>(mix.swtch, board));
        internalField.Append(new SignedField<8>(_curveParam));
        internalField.Append(new UnsignedField<8>(mix.mixWarn));
        internalField.Append(new UnsignedField<8>(mix.delayUp));
        internalField.Append(new UnsignedField<8>(mix.delayDown));
        internalField.Append(new UnsignedField<8>(mix.speedUp));
        internalField.Append(new UnsignedField<8>(mix.speedDown));
        internalField.Append(new MixSourceField<8>(mix.srcRaw, board));
        internalField.Append(new SignedField<8>(mix.sOffset));
        if (HAS_LARGE_LCD(board))
          internalField.Append(new ZCharField<10>(mix.name));
        else
          internalField.Append(new ZCharField<6>(mix.name));
      }
      else {
        internalField.Append(new UnsignedField<4>(_destCh));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new BoolField<1>(mix.noExpo));
        internalField.Append(new SpareBitsField<2>());
        internalField.Append(new SignedField<8>(mix.weight));
        internalField.Append(new SwitchField<6>(mix.swtch, board));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        internalField.Append(new UnsignedField<5>(mix.phases));
        internalField.Append(new SignedField<3>(mix.carryTrim));
        internalField.Append(new MixSourceField<6>(mix.srcRaw, board));
        internalField.Append(new UnsignedField<2>(mix.mixWarn));
        internalField.Append(new UnsignedField<4>(mix.delayUp));
        internalField.Append(new UnsignedField<4>(mix.delayDown));
        internalField.Append(new UnsignedField<4>(mix.speedUp));
        internalField.Append(new UnsignedField<4>(mix.speedDown));
        internalField.Append(new SignedField<8>(_curveParam));
        internalField.Append(new SignedField<8>(mix.sOffset));
      }
    }

    virtual void beforeExport()
    {
      if (mix.destCh && mix.srcRaw.type != SOURCE_TYPE_NONE) {
        _destCh = mix.destCh - 1;
        _curveMode = mix.curve;
        _curveParam = (mix.curve ? mix.curve : mix.differential);
      }
      else {
        mix.clear();
        _destCh = 0;
        _curveMode = 0;
        _curveParam = 0;
      }
    }

    virtual void afterImport()
    {
      if (mix.srcRaw.type != SOURCE_TYPE_NONE) {
        mix.destCh = _destCh + 1;
        mix.curve = 0;
        mix.differential = 0;
        if (_curveMode)
          mix.curve = _curveParam;
        else
          mix.differential = _curveParam;
      }
    }

  protected:
    StructField internalField;
    MixData & mix;
    unsigned int _destCh;
    bool _curveMode;
    int _curveParam;
};

class ExpoField: public TransformedField {
  public:
    ExpoField(ExpoData & expo, BoardEnum board):
      TransformedField(internalField),
      internalField("Expo"),
      expo(expo),
      board(board)
    {
      if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>(expo.mode));
        internalField.Append(new UnsignedField<8>(expo.chn));
        internalField.Append(new SwitchField<8>(expo.swtch, board));
        internalField.Append(new UnsignedField<16>(expo.phases));
        internalField.Append(new SignedField<8>(expo.weight));
        internalField.Append(new BoolField<8>(_curveMode));
        if (HAS_LARGE_LCD(board))
          internalField.Append(new ZCharField<10>(expo.name));
        else
          internalField.Append(new ZCharField<6>(expo.name));
        internalField.Append(new SignedField<8>(expo.curveParam));
      }
      else {
        internalField.Append(new UnsignedField<2>(expo.mode));
        internalField.Append(new SwitchField<6>(expo.swtch, board));
        internalField.Append(new UnsignedField<2>(expo.chn));
        internalField.Append(new UnsignedField<5>(expo.phases));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new SignedField<8>(expo.weight));
        internalField.Append(new SignedField<8>(expo.curveParam));
      }
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
    BoardEnum board;
    bool _curveMode;
};

class LimitField: public StructField {
  public:
    LimitField(LimitData & limit, BoardEnum board):
      StructField("Limit")
    {
      Append(new ConversionField< SignedField<8> >(limit.min, +100));
      Append(new ConversionField< SignedField<8> >(limit.max, -100));
      Append(new SignedField<8>(limit.ppmCenter));
      Append(new SignedField<14>(limit.offset));
      Append(new BoolField<1>(limit.symetrical));
      Append(new BoolField<1>(limit.revert));
      if (HAS_LARGE_LCD(board))
        Append(new ZCharField<6>(limit.name));
    }
};

class CurvesField: public TransformedField {
  public:
    CurvesField(CurveData * curves, BoardEnum board):
      TransformedField(internalField),
      internalField("Curves"),
      curves(curves),
      board(board),
      maxCurves(IS_ARM(board) ? O9X_ARM_MAX_CURVES : O9X_MAX_CURVES),
      maxPoints(IS_ARM(board) ? O9X_ARM_NUM_POINTS : O9X_NUM_POINTS)
    {
      for (int i=0; i<maxCurves; i++) {
        if (IS_ARM(board))
          internalField.Append(new SignedField<16>(_curves[i]));
        else
          internalField.Append(new SignedField<8>(_curves[i]));
      }
      for (int i=0; i<maxPoints; i++) {
        internalField.Append(new SignedField<8>(_points[i]));
      }
    }

    virtual void beforeExport()
    {
      memset(_points, 0, sizeof(_points));

      int * cur = &_points[0];
      int offset = 0;
      for (int i=0; i<maxCurves; i++) {
        CurveData *curve = &curves[i];
        offset += (curve->custom ? curve->count * 2 - 2 : curve->count) - 5;
        if (offset > maxPoints - 5 * maxCurves) {
          EEPROMWarnings += ::QObject::tr("open9x only accepts %1 points in all curves").arg(maxPoints) + "\n";
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
      for (int i=0; i<maxCurves; i++) {
        CurveData *curve = &curves[i];
        int * cur = &_points[i==0 ? 0 : 5*i + _curves[i-1]];
        int * next = &_points[5*(i+1) + _curves[i]];
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
    BoardEnum board;
    int maxCurves;
    int maxPoints;
    int _curves[O9X_ARM_MAX_CURVES];
    int _points[O9X_ARM_NUM_POINTS];
};

class CustomSwitchField: public TransformedField {
  public:
    CustomSwitchField(CustomSwData & csw, BoardEnum board):
      TransformedField(internalField),
      internalField("CustomSwitch"),
      board(board),
      csw(csw)
    {
      internalField.Append(new SignedField<8>(v1));
      internalField.Append(new SignedField<8>(v2));
      internalField.Append(new UnsignedField<8>(func));
      if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>(csw.delay));
        internalField.Append(new UnsignedField<8>(csw.duration));
      }
    }

    virtual void beforeExport()
    {
      func = csw.func;
      v1 = csw.val1;
      v2 = csw.val2;

      if ((csw.func >= CS_FN_VPOS && csw.func <= CS_FN_ANEG) || csw.func >= CS_FN_EQUAL) {
        v1 = open9xFromSource(RawSource(csw.val1), board);
      }

      if (csw.func >= CS_FN_EQUAL && csw.func <= CS_FN_ELESS) {
        v2 = open9xFromSource(RawSource(csw.val2), board);
      }

      if (csw.func >= CS_FN_AND && csw.func <= CS_FN_XOR) {
        v1 = open9xFromSwitch(RawSwitch(csw.val1), board);
        v2 = open9xFromSwitch(RawSwitch(csw.val2), board);
      }
    }

    virtual void afterImport()
    {
      csw.func = func;
      csw.val1 = v1;
      csw.val2 = v2;

      if ((csw.func >= CS_FN_VPOS && csw.func <= CS_FN_ANEG) || csw.func >= CS_FN_EQUAL) {
        csw.val1 = open9xToSource(v1, board).toValue();
      }

      if (csw.func >= CS_FN_EQUAL && csw.func <= CS_FN_ELESS) {
        csw.val2 = open9xToSource(v2, board).toValue();
      }

      if (csw.func >= CS_FN_AND && csw.func <= CS_FN_XOR) {
        csw.val1 = open9xToSwitch(v1, board).toValue();
        csw.val2 = open9xToSwitch(v2, board).toValue();
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

class CustomFunctionField: public TransformedField {
  public:
    CustomFunctionField(FuncSwData & fn, BoardEnum board):
      TransformedField(internalField),
      internalField("CustomFunction"),
      fn(fn),
      board(board),
      _delay(0)
    {
      internalField.Append(new SwitchField<8>(fn.swtch, board));
      if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>((unsigned int &)fn.func));
        internalField.Append(new CharField<6>(_param));
        internalField.Append(new UnsignedField<8>(_delay));
        internalField.Append(new SpareBitsField<8>());
      }
      else {
        if (board == BOARD_GRUVIN9X)
          internalField.Append(new ConversionField< UnsignedField<7> >((unsigned int &)fn.func, TABLE_CONVERSION(gruvin9xFunctionsConversion), ::QObject::tr("Open9x on gruvin9x board doesn't accept this function")));
        else
          internalField.Append(new ConversionField< UnsignedField<7> >((unsigned int &)fn.func, TABLE_CONVERSION(stockFunctionsConversion), ::QObject::tr("Open9x on stock board doesn't accept this function")));
        internalField.Append(new BoolField<1>(fn.enabled));
        internalField.Append(new UnsignedField<8>(fn.param));
      }
    }

    virtual void beforeExport()
    {
      if (IS_ARM(board)) {
        if (fn.func <= FuncInstantTrim) {
          *((uint32_t *)_param) = fn.param;
          _delay = (fn.enabled & 0x01);
        }
        else if (fn.func == FuncPlayPrompt || fn.func == FuncBackgroundMusic) {
          memcpy(_param, fn.paramarm, sizeof(_param));
        }
        else {
          unsigned int value = fn.param;
          if ((fn.func == FuncPlayValue || fn.func == FuncVolume || (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGV5)) && value > 7) {
            value--;
          }
          *((uint32_t *)_param) = value;
        }
      }
    }

    virtual void afterImport()
    {
      if (IS_ARM(board)) {
        unsigned int value = *((uint32_t *)_param);
        if (fn.func <= FuncInstantTrim) {
          fn.enabled = (_delay & 0x01);
          fn.param = value;
        }
        else if (fn.func == FuncPlayPrompt || fn.func == FuncBackgroundMusic) {
          memcpy(fn.paramarm, _param, sizeof(fn.paramarm));
        }
        else {
          fn.param = value;
          if ((fn.func == FuncPlayValue || fn.func == FuncVolume || (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGV5)) && value > 7) {
            fn.param++;
          }
        }
      }
    }

  protected:
    StructField internalField;
    FuncSwData & fn;
    BoardEnum board;
    char _param[6];
    unsigned int _delay;
};

class FrskyScreenField: public DataField {
  public:
    FrskyScreenField(FrSkyScreenData & screen):
      DataField("Frsky Screen"),
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

const int rssiLevelConversion[2][8] = { {0, 2, 1, 3, 2, 0, 3, 1}, {0, 1, 1, 2, 2, 3, 3, 0} };

class FrskyField: public StructField {
  public:
    FrskyField(FrSkyData & frsky, BoardEnum board):
      StructField("FrSky")
    {
      for (int i=0; i<2; i++) {
        Append(new UnsignedField<8>(frsky.channels[i].ratio, "Ratio"));
        Append(new SignedField<12>(frsky.channels[i].offset, "Offset"));
        Append(new UnsignedField<4>(frsky.channels[i].type, "Type"));
        for (int j=0; j<2; j++)
          Append(new UnsignedField<8>(frsky.channels[i].alarms[j].value, "Alarm value"));
        for (int j=0; j<2; j++)
          Append(new UnsignedField<2>(frsky.channels[i].alarms[j].level));
        for (int j=0; j<2; j++)
          Append(new UnsignedField<1>(frsky.channels[i].alarms[j].greater));
        Append(new UnsignedField<2>(frsky.channels[i].multiplier, "Multiplier"));
      }

      if (IS_ARM(board)) {
        for (int i=0; i<2; i++) {
          Append(new ConversionField< UnsignedField<2> >(frsky.rssiAlarms[i].level, 4, rssiLevelConversion[i]));
          Append(new ConversionField< SignedField<6> >(frsky.rssiAlarms[i].value, -50));
        }
        Append(new UnsignedField<8>(frsky.usrProto));
        Append(new UnsignedField<8>(frsky.voltsSource));
        Append(new UnsignedField<8>(frsky.blades));
        Append(new UnsignedField<8>(frsky.currentSource));

        Append(new UnsignedField<1>(frsky.screens[0].type));
        Append(new UnsignedField<1>(frsky.screens[1].type));
        Append(new UnsignedField<1>(frsky.screens[2].type));
        Append(new SpareBitsField<5>());

        for (int i=0; i<3; i++) {
          Append(new FrskyScreenField(frsky.screens[i]));
        }
        Append(new UnsignedField<8>(frsky.varioSource));
        Append(new SignedField<8>(frsky.varioCenterMin));
        Append(new SignedField<8>(frsky.varioCenterMax));
        Append(new SignedField<8>(frsky.varioMin));
        Append(new SignedField<8>(frsky.varioMax));
        Append(new SpareBitsField<8>());
        Append(new SpareBitsField<8>());
      }
      else {
        Append(new UnsignedField<2>(frsky.usrProto));
        Append(new UnsignedField<2>(frsky.blades));
        Append(new UnsignedField<1>(frsky.screens[0].type));
        Append(new UnsignedField<1>(frsky.screens[1].type));
        Append(new UnsignedField<2>(frsky.voltsSource));
        Append(new SignedField<4>(frsky.varioMin));
        Append(new SignedField<4>(frsky.varioMax));
        for (int i=0; i<2; i++) {
          Append(new ConversionField< UnsignedField<2> >(frsky.rssiAlarms[i].level, 4, rssiLevelConversion[i]));
          Append(new ConversionField< SignedField<6> >(frsky.rssiAlarms[i].value, -50, 0, 100, "RSSI value"));
        }
        for (int i=0; i<2; i++) {
          Append(new FrskyScreenField(frsky.screens[i]));
        }
        Append(new UnsignedField<3>(frsky.varioSource));
        Append(new SignedField<5>(frsky.varioCenterMin));
        Append(new UnsignedField<3>(frsky.currentSource));
        Append(new SignedField<8>(frsky.varioCenterMax));
      }
    }
};

const int protocolConversion[] = {PPM, 0, PPM16, 1, PPMSIM, 2, PXX, 3, DSM2, 4};
const int channelsConversion[] = {4, -2, 6, -1, 8, 0, 10, 1, 12, 2, 14, 3, 16, 4};
int exportPpmDelay(int delay) { return (delay - 300) / 50; }
int importPpmDelay(int delay) { return 300 + 50 * delay; }

Open9xModelDataNew::Open9xModelDataNew(ModelData & modelData, BoardEnum board, unsigned int variant):
  StructField(),
  board(board),
  variant(variant),
  maxPhases(IS_ARM(board) ? O9X_ARM_MAX_PHASES : O9X_MAX_PHASES),
  maxMixers(IS_ARM(board) ? O9X_ARM_MAX_MIXERS : O9X_MAX_MIXERS),
  maxChannels(IS_ARM(board) ? O9X_ARM_NUM_CHNOUT : O9X_NUM_CHNOUT),
  maxExpos(IS_ARM(board) ? O9X_ARM_MAX_EXPOS : O9X_MAX_EXPOS),
  maxCustomSwitches(IS_ARM(board) ? O9X_ARM_NUM_CSW : O9X_NUM_CSW),
  maxCustomFunctions(IS_ARM(board) ? O9X_ARM_NUM_FSW : O9X_NUM_FSW)
{
  sprintf(name, "Model %s", modelData.name);

  if (HAS_LARGE_LCD(board))
    Append(new ZCharField<12>(modelData.name));
  else
    Append(new ZCharField<10>(modelData.name));

  for (int i=0; i<O9X_MAX_TIMERS; i++) {
    Append(new TimerModeField(modelData.timers[i].mode, board));
    Append(new UnsignedField<16>(modelData.timers[i].val));
    if (HAS_PERSISTENT_TIMERS(board)) {
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

  if (IS_ARM(board))
    Append(new UnsignedField<16>(modelData.beepANACenter));
  else
    Append(new UnsignedField<8>(modelData.beepANACenter));

  for (int i=0; i<maxMixers; i++)
    Append(new MixField(modelData.mixData[i], board));
  for (int i=0; i<maxChannels; i++)
    Append(new LimitField(modelData.limitData[i]));
  for (int i=0; i<maxExpos; i++)
    Append(new ExpoField(modelData.expoData[i], board));
  Append(new CurvesField(modelData.curves, board));
  for (int i=0; i<maxCustomSwitches; i++)
    Append(new CustomSwitchField(modelData.customSw[i], board));
  for (int i=0; i<maxCustomFunctions; i++)
    Append(new CustomFunctionField(modelData.funcSw[i], board));
  Append(new HeliField(modelData.swashRingData, board));
  for (int i=0; i<maxPhases; i++)
    Append(new PhaseField(modelData.phaseData[i], i, board));
  Append(new SignedField<8>(modelData.ppmFrameLength));
  Append(new UnsignedField<8>(modelData.thrTraceSrc));
  Append(new UnsignedField<8>(modelData.modelId));

  if (board == BOARD_X9DA || board == BOARD_ACT)
    Append(new UnsignedField<16>(modelData.switchWarningStates));
  else
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
    Append(new FrskyField(modelData.frsky, board));
  }

  if (HAS_LARGE_LCD(board))
    Append(new CharField<10>(modelData.bitmap));

}

Open9xGeneralDataNew::Open9xGeneralDataNew(GeneralSettings & generalData, BoardEnum board, unsigned int version, unsigned int variant):
  TransformedField(internalField),
  internalField("General Settings"),
  generalData(generalData),
  board(board),
  inputsCount(board == BOARD_X9DA ? 8 : 7)
{
  generalData.version = version;
  generalData.variant = variant;

  internalField.Append(new UnsignedField<8>(generalData.version));
  if (version >= 213 || (!IS_ARM(board) && version >= 212))
    internalField.Append(new UnsignedField<16>(generalData.variant));

  for (int i=0; i<inputsCount; i++)
    internalField.Append(new SignedField<16>(generalData.calibMid[i]));
  for (int i=0; i<inputsCount; i++)
    internalField.Append(new SignedField<16>(generalData.calibSpanNeg[i]));
  for (int i=0; i<inputsCount; i++)
    internalField.Append(new SignedField<16>(generalData.calibSpanPos[i]));

  internalField.Append(new UnsignedField<16>(chkSum));
  internalField.Append(new UnsignedField<8>(generalData.currModel));
  internalField.Append(new UnsignedField<8>(generalData.contrast));
  internalField.Append(new UnsignedField<8>(generalData.vBatWarn));
  internalField.Append(new SignedField<8>(generalData.vBatCalib));
  internalField.Append(new SignedField<8>(generalData.backlightMode));

  for (int i=0; i<NUM_STICKS; i++)
    internalField.Append(new SignedField<16>(generalData.trainer.calib[i]));
  for (int i=0; i<NUM_STICKS; i++) {
    internalField.Append(new UnsignedField<6>(generalData.trainer.mix[i].src));
    internalField.Append(new UnsignedField<2>(generalData.trainer.mix[i].mode));
    internalField.Append(new SignedField<8>(generalData.trainer.mix[i].weight));
  }

  internalField.Append(new UnsignedField<8>(generalData.view, 0, MAX_VIEWS(board)-1));

  internalField.Append(new SpareBitsField<3>());
  internalField.Append(new SignedField<2>((int &)generalData.beeperMode));
  internalField.Append(new BoolField<1>(generalData.flashBeep));
  internalField.Append(new BoolField<1>(generalData.disableMemoryWarning));
  internalField.Append(new BoolField<1>(generalData.disableAlarmWarning));

  internalField.Append(new UnsignedField<2>(generalData.stickMode));
  internalField.Append(new SignedField<5>(generalData.timezone));
  internalField.Append(new SpareBitsField<1>());

  internalField.Append(new UnsignedField<8>(generalData.inactivityTimer));

  internalField.Append(new BoolField<1>(generalData.throttleReversed));
  internalField.Append(new BoolField<1>(generalData.minuteBeep));
  internalField.Append(new BoolField<1>(generalData.preBeep));
  if (version >= 213 || (!IS_ARM(board) && version >= 212))
    internalField.Append(new UnsignedField<3>(generalData.splashMode)); // TODO
  else
    internalField.Append(new SpareBitsField<3>());
  internalField.Append(new SignedField<2>((int &)generalData.hapticMode));

  internalField.Append(new SpareBitsField<8>());
  internalField.Append(new UnsignedField<8>(generalData.backlightDelay));
  internalField.Append(new UnsignedField<8>(generalData.templateSetup));
  internalField.Append(new SignedField<8>(generalData.PPM_Multiplier));
  internalField.Append(new SignedField<8>(generalData.hapticLength));
  internalField.Append(new UnsignedField<8>(generalData.reNavigation));

  internalField.Append(new SignedField<3>(generalData.beeperLength));
  internalField.Append(new UnsignedField<3>(generalData.hapticStrength));
  internalField.Append(new UnsignedField<1>(generalData.gpsFormat));
  internalField.Append(new SpareBitsField<1>()); // unexpectedShutdown

  internalField.Append(new UnsignedField<8>(generalData.speakerPitch));

  if (IS_ARM(board))
    internalField.Append(new ConversionField< SignedField<8> >(generalData.speakerVolume, -12, 0, 23, "Volume"));
  else
    internalField.Append(new ConversionField< SignedField<8> >(generalData.speakerVolume, -7, 0, 7, "Volume"));

  if (IS_ARM(board)) {
    internalField.Append(new UnsignedField<8>(generalData.backlightBright));
    internalField.Append(new SignedField<8>(generalData.currentCalib));
    if (version >= 213) {
      internalField.Append(new SignedField<8>(generalData.temperatureWarn)); // TODO
      internalField.Append(new UnsignedField<8>(generalData.mAhWarn));
      internalField.Append(new SpareBitsField<16>()); // mAhUsed
      internalField.Append(new SpareBitsField<32>()); // globalTimer
      internalField.Append(new SignedField<8>(generalData.temperatureCalib)); // TODO
      internalField.Append(new UnsignedField<8>(generalData.btBaudrate)); // TODO
      internalField.Append(new BoolField<8>(generalData.optrexDisplay)); // TODO
      internalField.Append(new UnsignedField<8>(generalData.sticksGain)); // TODO
    }
  }
}

void Open9xGeneralDataNew::beforeExport()
{
  uint16_t sum = 0;
  for (int i=0; i<inputsCount; i++)
    sum += generalData.calibMid[i];
  for (int i=0; i<5; i++)
    sum += generalData.calibSpanNeg[i];
  chkSum = sum;
}

void Open9xGeneralDataNew::afterImport()
{
}

