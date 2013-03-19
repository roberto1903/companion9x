#include <stdlib.h>
#include <algorithm>
#include "helpers.h"
#include "open9xeeprom.h"
#include <QObject>
// #include <QMessageBox>

#define HAS_PERSISTENT_TIMERS(board)         (IS_ARM(board) || board == BOARD_GRUVIN9X)
#define HAS_LARGE_LCD(board)                 (board == BOARD_X9DA)
#define MAX_VIEWS(board)                     (HAS_LARGE_LCD(board) ? 2 : 256)
#define MAX_ROTARY_ENCODERS(board)           (board==BOARD_GRUVIN9X ? 2 : (board==BOARD_SKY9X ? 1 : 0))
#define MAX_PHASES(board, version)           (IS_ARM(board) ? 9 :  ((IS_DBLEEPROM(board) && version >= 213) ? 6 :  5))
#define MAX_MIXERS(board, version)           (IS_ARM(board) ? 64 : 32)
#define MAX_CHANNELS(board, version)         (IS_ARM(board) ? 32 : 16)
#define MAX_EXPOS(board, version)            (IS_ARM(board) ? 32 : ((IS_DBLEEPROM(board) && version >= 213) ? 16 : 14))
#define MAX_CUSTOM_SWITCHES(board, version)  (IS_ARM(board) ? 32 : ((IS_DBLEEPROM(board) && version >= 213) ? 15 : 12))
#define MAX_CUSTOM_FUNCTIONS(board, version) (IS_ARM(board) ? 32 : ((IS_DBLEEPROM(board) && version >= 213) ? 24 : 16))

inline int switchIndex(int i, BoardEnum board, unsigned int version)
{
  bool recent = (version >= 214 || (!IS_ARM(board) && version >= 213));
  if (recent)
    return (i<=3 ? i+3 : (i<=6 ? i-3 : i));
  else
    return i;
}

class SwitchesConversionTable: public ConversionTable {

  public:
    SwitchesConversionTable(BoardEnum board, unsigned int version, unsigned long flags=0)
    {
      int val=0;

      addConversion(RawSwitch(SWITCH_TYPE_NONE), val++);

      for (int i=1; i<=9; i++) {
        int s = switchIndex(i, board, version);
        addConversion(RawSwitch(SWITCH_TYPE_SWITCH, -s), -val);
        addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
      }

      for (int i=1; i<=MAX_CUSTOM_SWITCHES(board, version); i++) {
        addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, -i), -val);
        addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
      }

      addConversion(RawSwitch(SWITCH_TYPE_OFF), -val);
      addConversion(RawSwitch(SWITCH_TYPE_ON), val++);

      for (int i=1; i<=9; i++) {
        int s = switchIndex(i, board, version);
        addConversion(RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, -s), -val);
        addConversion(RawSwitch(SWITCH_TYPE_MOMENT_SWITCH, s), val++);
      }

      for (int i=1; i<=MAX_CUSTOM_SWITCHES(board, version); i++) {
        addConversion(RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, -i), -val);
        addConversion(RawSwitch(SWITCH_TYPE_MOMENT_VIRTUAL, i), val++);
      }

      addConversion(RawSwitch(SWITCH_TYPE_ONM), val++);
    }

  protected:

    // TODO const RawSwitch & sw
    void addConversion(RawSwitch sw, int b)
    {
      ConversionTable::addConversion(sw.toValue(), b);
    }
};

#define FLAG_NONONE       0x01
#define FLAG_NOSWITCHES   0x02
#define FLAG_NOTELEMETRY  0x04

class SourcesConversionTable: public ConversionTable {

  public:
    SourcesConversionTable(BoardEnum board, unsigned int version, unsigned long flags=0)
    {
      bool recent = (version >= 214 || (!IS_ARM(board) && version >= 213));

      int val=0;

      if (!(flags & FLAG_NONONE)) {
        addConversion(RawSource(SOURCE_TYPE_NONE), val++);
      }

      for (int i=0; i<7; i++)
        addConversion(RawSource(SOURCE_TYPE_STICK, i), val++);

      for (int i=0; i<MAX_ROTARY_ENCODERS(board); i++)
        addConversion(RawSource(SOURCE_TYPE_ROTARY_ENCODER, 0), val++);

      if (!recent) {
        for (int i=0; i<NUM_STICKS; i++)
          addConversion(RawSource(SOURCE_TYPE_TRIM, i), val++);
      }

      addConversion(RawSource(SOURCE_TYPE_MAX), val++);

      if (recent) {
        for (int i=0; i<3; i++)
          addConversion(RawSource(SOURCE_TYPE_CYC, i), val++);
      }

      if (recent) {
        for (int i=0; i<NUM_STICKS; i++)
          addConversion(RawSource(SOURCE_TYPE_TRIM, i), val++);
      }

      addConversion(RawSource(SOURCE_TYPE_3POS), val++);

      if (!(flags & FLAG_NOSWITCHES)) {
        if (recent) {
          for (int i=1; i<=3; i++)
            addConversion(RawSource(SOURCE_TYPE_SWITCH, RawSwitch(SWITCH_TYPE_SWITCH, switchIndex(i, board, version)).toValue()), val++);
          for (int i=7; i<=9; i++)
            addConversion(RawSource(SOURCE_TYPE_SWITCH, RawSwitch(SWITCH_TYPE_SWITCH, switchIndex(i, board, version)).toValue()), val++);
        }
        else {
          for (int i=1; i<=9; i++)
            addConversion(RawSource(SOURCE_TYPE_SWITCH, RawSwitch(SWITCH_TYPE_SWITCH, switchIndex(i, board, version)).toValue()), val++);
        }
        for (int i=1; i<=MAX_CUSTOM_SWITCHES(board, version); i++)
          addConversion(RawSource(SOURCE_TYPE_SWITCH, RawSwitch(SWITCH_TYPE_VIRTUAL, i).toValue()), val++);
      }

      if (!recent) {
        for (int i=0; i<3; i++)
          addConversion(RawSource(SOURCE_TYPE_CYC, i), val++);
      }

      for (int i=0; i<8; i++)
        addConversion(RawSource(SOURCE_TYPE_PPM, i), val++);

      for (int i=0; i<MAX_CHANNELS(board, version); i++)
        addConversion(RawSource(SOURCE_TYPE_CH, i), val++);

      if (!(flags & FLAG_NOTELEMETRY)) {
        if (recent) {
          for (int i=0; i<5; i++)
            addConversion(RawSource(SOURCE_TYPE_GVAR, i), val++);
        }

        if (recent)
          addConversion(RawSource(SOURCE_TYPE_TELEMETRY, 0), val++);

        for (int i=1; i<TELEMETRY_SOURCES_COUNT; i++)
          addConversion(RawSource(SOURCE_TYPE_TELEMETRY, i), val++);
      }
    }

  protected:

    // TODO const RawSource & source
    void addConversion(RawSource source, int b)
    {
      ConversionTable::addConversion(source.toValue(), b);
    }
};

class TimerModeField: public TransformedField {
  public:
    TimerModeField(TimerMode & mode, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField(_mode),
      mode(mode),
      board(board),
      numCSW(MAX_CUSTOM_SWITCHES(board, version)),
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
class SwitchField: public ConversionField< SignedField<N> > {
  public:
    SwitchField(RawSwitch & sw, BoardEnum board, unsigned int version, unsigned long flags=0):
      ConversionField< SignedField<N> >(_switch, &switchesConversionTable, "Switch"),
      switchesConversionTable(board, version, flags),
      sw(sw),
      _switch(0)
    {
    }

    virtual void beforeExport()
    {
      _switch = sw.toValue();
      ConversionField< SignedField<N> >::beforeExport();
    }
    
    virtual void afterImport()
    {
      ConversionField< SignedField<N> >::afterImport();	
      sw = RawSwitch(_switch);
    }    
    
  protected:
    SwitchesConversionTable switchesConversionTable;
    RawSwitch & sw;
    int _switch;
};

template <int N>
class SourceField: public ConversionField< UnsignedField<N> > {
  public:
    SourceField(RawSource & source, BoardEnum board, unsigned int version, unsigned long flags=0):
      ConversionField< UnsignedField<N> >(_source, &sourcesConversionTable, "Source"),
      sourcesConversionTable(board, version, flags),
      source(source),
      _source(0)
    {
    }

    virtual void beforeExport()
    {
      _source = source.toValue();
      ConversionField< UnsignedField<N> >::beforeExport();
    }
    
    virtual void afterImport()
    {
      ConversionField< UnsignedField<N> >::afterImport();	
      source = RawSource(_source);
    }    

  protected:
    SourcesConversionTable sourcesConversionTable;
    RawSource & source;
    unsigned int _source;
};

class HeliField: public StructField {
  public:
    HeliField(SwashRingData & heli, BoardEnum board, unsigned int version)
    {
      Append(new BoolField<1>(heli.invertELE));
      Append(new BoolField<1>(heli.invertAIL));
      Append(new BoolField<1>(heli.invertCOL));
      Append(new UnsignedField<5>(heli.type));
      Append(new SourceField<8>(heli.collectiveSource, board, version, FLAG_NOSWITCHES));
      Append(new UnsignedField<8>(heli.value));
    }
};

class PhaseField: public TransformedField {
  public:
    PhaseField(PhaseData & phase, int index, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("Phase"),
      phase(phase),
      index(index),
      board(board),
      version(version),
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

      internalField.Append(new SwitchField<8>(phase.swtch, board, version));
      if (HAS_LARGE_LCD(board))
        internalField.Append(new ZCharField<10>(phase.name));
      else
        internalField.Append(new ZCharField<6>(phase.name));

      if (IS_ARM(board) && version >= 214) {
        internalField.Append(new UnsignedField<8>(phase.fadeIn));
        internalField.Append(new UnsignedField<8>(phase.fadeOut));
      }
      else {
        internalField.Append(new UnsignedField<4>(phase.fadeIn));
        internalField.Append(new UnsignedField<4>(phase.fadeOut));
      }

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
    unsigned int version;
    int rotencCount;
    int trimBase[NUM_STICKS];
    int trimExt[NUM_STICKS];
};

void splitGvarParam(const int gvar, int & _gvar, unsigned int & _gvarParam, int version)
{
  if (version>=213) {
      if (gvar < -125) {
        _gvarParam = 0;
        _gvar = 256+ gvar + 125;
      }
      else if (gvar > 125) {
        _gvarParam = 1;
        _gvar = gvar - 126;
      }
      else {
        if (gvar<0) _gvarParam=1;
        else _gvarParam = 0;
        _gvar = gvar;
      }
  } else { 
	  if (gvar < -125) {
		_gvarParam = 1;
		_gvar = gvar + 125;
	  }
	  else if (gvar > 125) {
		_gvarParam = 1;
		_gvar = gvar - 126;
	  }
	  else {
		_gvarParam = 0;
		_gvar = gvar;
	  }
  }
}

void concatGvarParam(int & gvar, const int _gvar, const unsigned int _gvarParam)
{
  if (_gvarParam == 0) {
    gvar = _gvar;
  }
  else if (_gvar >= 0) {
    gvar = 126 + _gvar;
  }
  else {
    gvar = -125 + _gvar;
  }
}

void exportGvarParam(const int gvar, int & _gvar)
{
  if (gvar < -125) {
    _gvar = 1024 + gvar + 125;
  }
  else if (gvar > 125) {
    _gvar = 1024 + gvar - 126;
  }
  else {
    _gvar = gvar;
  }
}

void importGvarParam(int & gvar, const int _gvar)
{
  if (_gvar >= 1024) {
    gvar = 126 + _gvar - 1024;
  }
  else if (gvar >= 1019) {
    gvar = -126 + 1019 - _gvar;
  }
  else {
    gvar = _gvar;
  }
}

class MixField: public TransformedField {
  public:
    MixField(MixData & mix, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("Mix"),
      mix(mix),
      board(board),
      version(version)
    {
      if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>(_destCh));
        internalField.Append(new UnsignedField<16>(mix.phases));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new BoolField<1>(mix.noExpo));
        internalField.Append(new SignedField<3>(mix.carryTrim));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        if (version >= 214)
          internalField.Append(new SpareBitsField<1>());
        else
          internalField.Append(new UnsignedField<1>(_offsetMode));
        internalField.Append(new SignedField<16>(_weight));
        internalField.Append(new SwitchField<8>(mix.swtch, board, version));
        internalField.Append(new SignedField<8>(_curveParam));
        if (version >= 214) {
          internalField.Append(new UnsignedField<4>(mix.mixWarn));
          internalField.Append(new UnsignedField<4>(mix.srcVariant));
        }
        else {
          internalField.Append(new UnsignedField<8>(mix.mixWarn));
        }
        internalField.Append(new UnsignedField<8>(mix.delayUp));
        internalField.Append(new UnsignedField<8>(mix.delayDown));
        internalField.Append(new UnsignedField<8>(mix.speedUp));
        internalField.Append(new UnsignedField<8>(mix.speedDown));
        internalField.Append(new SourceField<8>(mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        if (version >= 214)
          internalField.Append(new SignedField<16>(_offset));
        else
          internalField.Append(new SignedField<8>(_offset));
        if (HAS_LARGE_LCD(board))
          internalField.Append(new ZCharField<10>(mix.name));
        else
          internalField.Append(new ZCharField<6>(mix.name));
      }
      else {
        internalField.Append(new UnsignedField<4>(_destCh));
        internalField.Append(new BoolField<1>(_curveMode));
        internalField.Append(new BoolField<1>(mix.noExpo));
        internalField.Append(new UnsignedField<1>(_weightMode));
        internalField.Append(new UnsignedField<1>(_offsetMode));
        internalField.Append(new SignedField<8>(_weight));
        internalField.Append(new SwitchField<6>(mix.swtch, board, version));
        internalField.Append(new UnsignedField<2>((unsigned int &)mix.mltpx));
        internalField.Append(new UnsignedField<5>(mix.phases));
        internalField.Append(new SignedField<3>(mix.carryTrim));
        internalField.Append(new SourceField<6>(mix.srcRaw, board, version, FLAG_NOTELEMETRY));
        internalField.Append(new UnsignedField<2>(mix.mixWarn));
        internalField.Append(new UnsignedField<4>(mix.delayUp));
        internalField.Append(new UnsignedField<4>(mix.delayDown));
        internalField.Append(new UnsignedField<4>(mix.speedUp));
        internalField.Append(new UnsignedField<4>(mix.speedDown));
        internalField.Append(new SignedField<8>(_curveParam));
        internalField.Append(new SignedField<8>(_offset));
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

      if (IS_ARM(board)) {
        exportGvarParam(mix.weight, _weight);
        if (version >= 214)
          exportGvarParam(mix.sOffset, _offset);
        else
          splitGvarParam(mix.sOffset, _offset, _offsetMode, version);
      }
      else {
        splitGvarParam(mix.weight, _weight, _weightMode, version);
        splitGvarParam(mix.sOffset, _offset, _offsetMode, version);
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

      if (IS_ARM(board)) {
        importGvarParam(mix.weight, _weight);
        if (version >= 214)
          importGvarParam(mix.sOffset, _offset);
        else
          concatGvarParam(mix.sOffset, _offset, _offsetMode);
      }
      else {
        concatGvarParam(mix.weight, _weight, _weightMode);
        concatGvarParam(mix.sOffset, _offset, _offsetMode);
      }
    }

  protected:
    StructField internalField;
    MixData & mix;
    BoardEnum board;
    unsigned int version;
    unsigned int _destCh;
    bool _curveMode;
    int _curveParam;
    int _weight;
    int _offset;
    unsigned int _weightMode;
    unsigned int _offsetMode;
};

class ExpoField: public TransformedField {
  public:
    ExpoField(ExpoData & expo, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("Expo"),
      expo(expo),
      board(board)
    {
      if (IS_ARM(board)) {
        internalField.Append(new UnsignedField<8>(expo.mode));
        internalField.Append(new UnsignedField<8>(expo.chn));
        internalField.Append(new SwitchField<8>(expo.swtch, board, version));
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
        internalField.Append(new SwitchField<6>(expo.swtch, board, version));
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

class CustomSwitchesFunctionsTable: public ConversionTable {

  public:
    CustomSwitchesFunctionsTable(BoardEnum board, unsigned int version)
    {
      int val=0;
      bool recent = (version >= 214 || (!IS_ARM(board) && version >= 213));
      addConversion(CS_FN_OFF, val++);
      if (recent)
        addConversion(CS_FN_VEQUAL, val++);
      addConversion(CS_FN_VPOS, val++);
      addConversion(CS_FN_VNEG, val++);
      addConversion(CS_FN_APOS, val++);
      addConversion(CS_FN_ANEG, val++);
      addConversion(CS_FN_AND, val++);
      addConversion(CS_FN_OR, val++);
      addConversion(CS_FN_XOR, val++);
      addConversion(CS_FN_EQUAL, val++);
      if (!recent)
        addConversion(CS_FN_NEQUAL, val++);
      addConversion(CS_FN_GREATER, val++);
      addConversion(CS_FN_LESS, val++);
      if (!recent) {
        addConversion(CS_FN_EGREATER, val++);
        addConversion(CS_FN_ELESS, val++);
      }
      addConversion(CS_FN_DPOS, val++);
      addConversion(CS_FN_DAPOS, val++);
    }
};

class CustomSwitchesAndSwitchesConversionTable: public ConversionTable {

  public:
    CustomSwitchesAndSwitchesConversionTable()
    {
      int val=0;

      addConversion(RawSwitch(SWITCH_TYPE_NONE), val++);

      for (int i=1; i<=8; i++) {
        int s = switchIndex(i, BOARD_STOCK, 214);
        addConversion(RawSwitch(SWITCH_TYPE_SWITCH, s), val++);
      }

      for (int i=3; i<=9; i++) {
        addConversion(RawSwitch(SWITCH_TYPE_VIRTUAL, i), val++);
      }
    }

  protected:

    // TODO const RawSwitch & sw
    void addConversion(RawSwitch sw, int b)
    {
      ConversionTable::addConversion(sw.toValue(), b);
    }
};

class CustomSwitchField: public TransformedField {
  public:
    CustomSwitchField(CustomSwData & csw, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("CustomSwitch"),
      csw(csw),
      board(board),
      version(version),
      functionsConversionTable(board, version),
      sourcesConversionTable(board, version, (version >= 214 || (!IS_ARM(board) && version >= 213)) ? 0 : FLAG_NOSWITCHES),
      switchesConversionTable(board, version)
    {
      internalField.Append(new SignedField<8>(v1));
      internalField.Append(new SignedField<8>(v2));

      if (IS_ARM(board)) {
        internalField.Append(new ConversionField< UnsignedField<8> >(csw.func, &functionsConversionTable, "Function"));
        internalField.Append(new UnsignedField<8>(csw.delay));
        internalField.Append(new UnsignedField<8>(csw.duration));
        if (version >= 214) {
          internalField.Append(new ConversionField< UnsignedField<8> >(csw.andsw, &andswitchesConversionTable, "AND switch"));
        }
      }
      else {
        if (version >= 213) {
          internalField.Append(new ConversionField< UnsignedField<4> >(csw.func, &functionsConversionTable, "Function"));
          internalField.Append(new ConversionField< UnsignedField<4> >(csw.andsw, &andswitchesConversionTable, "AND switch"));
        }
        else {
          internalField.Append(new ConversionField< UnsignedField<8> >(csw.func, &functionsConversionTable, "Function"));
        }
      }
    }

    virtual void beforeExport()
    {
      v1 = csw.val1;
      v2 = csw.val2;

      if ((csw.func >= CS_FN_VPOS && csw.func <= CS_FN_ANEG) || csw.func >= CS_FN_EQUAL) {
        sourcesConversionTable.exportValue(csw.val1, v1);
      }

      if (csw.func >= CS_FN_EQUAL && csw.func <= CS_FN_ELESS) {
        sourcesConversionTable.exportValue(csw.val2, v2);
      }

      if (csw.func >= CS_FN_AND && csw.func <= CS_FN_XOR) {
        switchesConversionTable.exportValue(csw.val1, v1);
        switchesConversionTable.exportValue(csw.val2, v2);
      }
    }

    virtual void afterImport()
    {
      csw.val1 = v1;
      csw.val2 = v2;

      if ((csw.func >= CS_FN_VPOS && csw.func <= CS_FN_ANEG) || csw.func >= CS_FN_EQUAL) {
        sourcesConversionTable.importValue(v1, csw.val1);
      }

      if (csw.func >= CS_FN_EQUAL && csw.func <= CS_FN_ELESS) {
        sourcesConversionTable.importValue(v2, csw.val2);
      }

      if (csw.func >= CS_FN_AND && csw.func <= CS_FN_XOR) {
        switchesConversionTable.importValue(v1, csw.val1);
        switchesConversionTable.importValue(v2, csw.val2);
      }
    }

  protected:
    StructField internalField;
    CustomSwData & csw;
    BoardEnum board;
    unsigned int version;
    CustomSwitchesFunctionsTable functionsConversionTable;
    SourcesConversionTable sourcesConversionTable;
    SwitchesConversionTable switchesConversionTable;
    CustomSwitchesAndSwitchesConversionTable andswitchesConversionTable;
    int v1;
    int v2;
};

class CustomFunctionsConversionTable: public ConversionTable {

  public:
    CustomFunctionsConversionTable(BoardEnum board, unsigned int version)
    {
      int val=0;

      if (IS_ARM(board) || version < 213) {
        for (int i=0; i<16; i++) {
          addConversion(val, val);
          val++;
        }
      }
      else {
        for (int i=0; i<16; i++) {
          addConversion(i, i / 4);
        }
        val+=4;
      }
      addConversion(FuncTrainer, val++);
      addConversion(FuncTrainerRUD, val++);
      addConversion(FuncTrainerELE, val++);
      addConversion(FuncTrainerTHR, val++);
      addConversion(FuncTrainerAIL, val++);
      addConversion(FuncInstantTrim, val++);
      addConversion(FuncPlaySound, val++);
      addConversion(FuncPlayHaptic, val++);
      addConversion(FuncReset, val++);
      addConversion(FuncVario, val++);
      addConversion(FuncPlayPrompt, val++);
      if (version >= 213 && !IS_ARM(board))
        addConversion(FuncPlayBoth, val++);
      addConversion(FuncPlayValue, val++);
      if (board == BOARD_GRUVIN9X || board == BOARD_SKY9X)
        addConversion(FuncLogs, val++);
      if (IS_ARM(board))
        addConversion(FuncVolume, val++);
      addConversion(FuncBacklight, val++);
      if (IS_ARM(board)) {
        addConversion(FuncBackgroundMusic, val++);
        addConversion(FuncBackgroundMusicPause, val++);
      }
      for (int i=0; i<5; i++)
        addConversion(FuncAdjustGV1+i, val++);
    }
};

template <int N>
class SwitchesWarningField: public TransformedField {
  public:
    SwitchesWarningField(unsigned int & sw, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField(_sw, "SwitchesWarning"),
      sw(sw),
      board(board),
      version(version)
    {
    }

    virtual void beforeExport()
    {
      bool recent = (version >= 214 || (!IS_ARM(board) && version >= 213));
      if (recent) {
        _sw = (sw & 0b11000001) + ((sw & 0b110000) >> 3) + ((sw & 0b001110) << 2);
      }
      else {
        _sw = sw;
      }
    }

    virtual void afterImport()
    {
      bool recent = (version >= 214 || (!IS_ARM(board) && version >= 213));
      if (recent) {
        sw = (_sw & 0b11000001) + ((_sw & 0b111000) >> 2) + ((_sw & 0b000110) << 3);
      }
      else {
        sw = _sw;
      }
    }

  protected:
    UnsignedField<N> internalField;
    unsigned int &sw;
    unsigned int _sw;
    BoardEnum board;
    unsigned int version;
};

class CustomFunctionField: public TransformedField {
  public:
    CustomFunctionField(FuncSwData & fn, BoardEnum board, unsigned int version):
      TransformedField(internalField),
      internalField("CustomFunction"),
      fn(fn),
      board(board),
      version(version),
      functionsConversionTable(board, version),
      sourcesConversionTable(board, version, FLAG_NONONE),
      _param(0),
      _delay(0),
      _union_param(0)
    {
      internalField.Append(new SwitchField<8>(fn.swtch, board, version));
      if (IS_ARM(board)) {
        internalField.Append(new ConversionField< UnsignedField<8> >((unsigned int &)fn.func, &functionsConversionTable, "Function", ::QObject::tr("Open9x on this board doesn't accept this function")));
        if (board == BOARD_X9DA)
          internalField.Append(new CharField<10>(_arm_param));
        else
          internalField.Append(new CharField<6>(_arm_param));
        if (version >= 214) {
          internalField.Append(new UnsignedField<2>(_mode));
          internalField.Append(new UnsignedField<6>(_delay));
        }
        else {
          internalField.Append(new UnsignedField<8>(_delay));
        }
        if (version < 214)
          internalField.Append(new SpareBitsField<8>());
      }
      else {
        if (version >= 213) {
          internalField.Append(new UnsignedField<3>(_union_param));
          internalField.Append(new ConversionField< UnsignedField<5> >((unsigned int &)fn.func, &functionsConversionTable, "Function", ::QObject::tr("Open9x on this board doesn't accept this function")));
        }
        else {
          internalField.Append(new ConversionField< UnsignedField<7> >((unsigned int &)fn.func, &functionsConversionTable, "Function", ::QObject::tr("Open9x on this board doesn't accept this function")));
          internalField.Append(new BoolField<1>((bool &)fn.enabled));
        }
        internalField.Append(new UnsignedField<8>(_param));
      }
    }

    virtual void beforeExport()
    {
      if (IS_ARM(board)) {
        _mode = 0;
        if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayValue)
          _delay = fn.repeatParam / 5;
        else
          _delay = (fn.enabled ? 1 : 0);
          
        if (fn.func <= FuncInstantTrim) {
          *((uint32_t *)_arm_param) = fn.param;
        }
        else if (fn.func == FuncPlayPrompt || fn.func == FuncBackgroundMusic) {
          memcpy(_arm_param, fn.paramarm, sizeof(_arm_param));
        }
        else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGV5) {
          unsigned int value;
          if (version >= 214) {
            _mode = fn.adjustMode;
            if (fn.adjustMode == 1)
              sourcesConversionTable.exportValue(fn.param, (int &)value);
            else if (fn.adjustMode == 2)
              value = RawSource(fn.param).index;
            else
              value = fn.param;
          }
          else {
            unsigned int value;
            sourcesConversionTable.exportValue(fn.param, (int &)value);
          }
          *((uint32_t *)_arm_param) = value;
        }
        else if (fn.func == FuncPlayValue || fn.func == FuncVolume) {
          unsigned int value;
          sourcesConversionTable.exportValue(fn.param, (int &)value);
          *((uint32_t *)_arm_param) = value;
        }
        else {
          *((uint32_t *)_arm_param) = fn.param;
        }
      }
      else {
        /* the default behaviour */
        _param = fn.param;
        _union_param = (fn.enabled ? 1 : 0);
        if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGV5) {
          if (version >= 213) {
            _union_param += (fn.adjustMode << 1);
            if (fn.adjustMode == 1)
              sourcesConversionTable.exportValue(fn.param, (int &)_param);
            else if (fn.adjustMode == 2)
              _param = RawSource(fn.param).index;
          }
          else {
            sourcesConversionTable.exportValue(fn.param, (int &)_param);
          }
        }
        else if (fn.func == FuncPlayValue) {
          if (version >= 213) {
            _union_param = fn.repeatParam / 10;
            sourcesConversionTable.exportValue(fn.param, (int &)_param);
          }
          else {
            SourcesConversionTable(board, version, FLAG_NONONE|FLAG_NOSWITCHES).exportValue(fn.param, (int &)_param);
          }
        }
        else if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayBoth) {
          if (version >= 213)
            _union_param = fn.repeatParam / 10;
        }
        else if (fn.func <= FuncSafetyCh16) {
          if (version >= 213)
            _union_param += ((fn.func % 4) << 1);
        }
      }
    }

    virtual void afterImport()
    {
      if (IS_ARM(board)) {
        if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayValue)
          fn.repeatParam = _delay * 5;
        else
          fn.enabled = (_delay & 0x01);

        unsigned int value = *((uint32_t *)_arm_param);
        if (fn.func <= FuncInstantTrim) {
          fn.param = value;
        }
        else if (fn.func == FuncPlayPrompt || fn.func == FuncBackgroundMusic) {
          memcpy(fn.paramarm, _arm_param, sizeof(fn.paramarm));
        }
        else if (fn.func == FuncVolume) {
          sourcesConversionTable.importValue(value, (int &)fn.param);
        }
        else if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGV5) {
          if (version >= 214) {
            fn.adjustMode = _mode;
            if (fn.adjustMode == 1)
              sourcesConversionTable.importValue(value, (int &)fn.param);
            else if (fn.adjustMode == 2)
              fn.param = RawSource(SOURCE_TYPE_GVAR, value).toValue();
            else
              fn.param = value;
          }
          else {
            sourcesConversionTable.importValue(value, (int &)fn.param);
          }
        }
        else if (fn.func == FuncPlayValue) {
          if (version >= 213) {
            fn.repeatParam = _union_param * 10;
            sourcesConversionTable.importValue(value, (int &)fn.param);
          }
          else {
            SourcesConversionTable(board, version, FLAG_NONONE|FLAG_NOSWITCHES).importValue(value, (int &)fn.param);
          }
        }
        else {
          fn.param = value;
        }
      }
      else {
        fn.param = _param;
        if (version >= 213) {
          fn.enabled = (_union_param & 0x01);
        }
        if (fn.func >= FuncAdjustGV1 && fn.func <= FuncAdjustGV5) {
          if (version >= 213) {
            fn.adjustMode = ((_union_param >> 1) & 0x03);
            if (fn.adjustMode == 1)
              sourcesConversionTable.importValue(_param, (int &)fn.param);
            else if (fn.adjustMode == 2)
              fn.param = RawSource(SOURCE_TYPE_GVAR, _param).toValue();
          }
          else {
            sourcesConversionTable.importValue(_param, (int &)fn.param);
          }
        }
        else if (fn.func == FuncPlayValue) {
          if (version >= 213) {
            fn.repeatParam = _union_param * 10;
            sourcesConversionTable.importValue(_param, (int &)fn.param);
          }
          else {
            SourcesConversionTable(board, version, FLAG_NONONE|FLAG_NOSWITCHES).importValue(_param, (int &)fn.param);
          }
        }
        else if (fn.func == FuncPlaySound || fn.func == FuncPlayPrompt || fn.func == FuncPlayBoth) {
          if (version >= 213)
            fn.repeatParam = _union_param * 10;
        }
        else if (fn.func <= FuncSafetyCh16) {
          if (version >= 213) {
            fn.func = AssignFunc(((fn.func >> 2) << 2) + ((_union_param >> 1) & 0x03));
          }
          fn.param = (int8_t)fn.param;
        }
      }
    }

  protected:
    StructField internalField;
    FuncSwData & fn;
    BoardEnum board;
    unsigned int version;
    CustomFunctionsConversionTable functionsConversionTable;
    SourcesConversionTable sourcesConversionTable;
    char _arm_param[6];
    unsigned int _param;
    unsigned int _delay;
    unsigned int _mode;
    unsigned int _union_param;
};

class FrskyScreenField: public DataField {
  public:
    FrskyScreenField(FrSkyScreenData & screen, BoardEnum board, unsigned int version):
      DataField("Frsky Screen"),
      screen(screen),
      board(board),
      version(version)
    {
      for (int i=0; i<4; i++) {
        bars.Append(new UnsignedField<8>(_screen.body.bars[i].source));
        bars.Append(new UnsignedField<8>(_screen.body.bars[i].barMin));
        bars.Append(new UnsignedField<8>(_screen.body.bars[i].barMax));
      }
      for (int i=0; i<8; i++) {
        numbers.Append(new UnsignedField<8>(_screen.body.cells[i]));
      }
      for (int i=0; i<4; i++) {
        numbers.Append(new SpareBitsField<8>());
      }
    }

    virtual void ExportBits(QBitArray & output)
    {
      _screen = screen;

      bool recent = (version >= 214 || (!IS_ARM(board) && version >= 213));
      if (!recent) {
        for (int i=0; i<4; i++) {
          if (_screen.body.bars[i].source > 0)
            _screen.body.bars[i].source--;
        }
        for (int i=0; i<8; i++) {
          if (_screen.body.cells[i] > 0)
            _screen.body.cells[i]--;
        }
      }

      if (screen.type == 0)
        numbers.ExportBits(output);
      else
        bars.ExportBits(output);
    }

    virtual void ImportBits(QBitArray & input)
    {
      _screen = screen;

      bool recent = (version >= 214 || (!IS_ARM(board) && version >= 213));

      // NOTA: screen.type should have been imported first!
      if (screen.type == 0) {
        numbers.ImportBits(input);
        if (!recent) {
          for (int i=0; i<8; i++) {
            if (_screen.body.cells[i] > 0) {
              _screen.body.cells[i]++;
            }
          }
        }
      }
      else {
        bars.ImportBits(input);
        if (!recent) {
          for (int i=0; i<4; i++) {
            if (_screen.body.bars[i].source > 0)
              _screen.body.bars[i].source++;
          }
        }
      }

      screen = _screen;
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
    FrSkyScreenData _screen;
    BoardEnum board;
    unsigned int version;
    StructField bars;
    StructField numbers;
};

class RSSIConversionTable: public ConversionTable
{
  public:
    RSSIConversionTable(int index)
    {
      addConversion(0, 2-index);
      addConversion(1, 3-index);
      addConversion(2, index ? 3 : 0);
      addConversion(3, 1-index);
    }

    RSSIConversionTable()
    {
    }
};

class FrskyField: public StructField {
  public:
    FrskyField(FrSkyData & frsky, BoardEnum board, unsigned int version):
      StructField("FrSky")
    {
      rssiConversionTable[0] = RSSIConversionTable(0);
      rssiConversionTable[1] = RSSIConversionTable(1);

      if (IS_ARM(board)) {
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
          Append(new SpareBitsField<2>());
          Append(new UnsignedField<8>(frsky.channels[i].multiplier, 0, 5, "Multiplier"));
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
          Append(new FrskyScreenField(frsky.screens[i], board, version));
        }
        Append(new UnsignedField<8>(frsky.varioSource));
        Append(new SignedField<8>(frsky.varioCenterMax));
        Append(new SignedField<8>(frsky.varioCenterMin));
        Append(new SignedField<8>(frsky.varioMin));
        Append(new SignedField<8>(frsky.varioMax));
        for (int i=0; i<2; i++) {
          Append(new ConversionField< UnsignedField<2> >(frsky.rssiAlarms[i].level, &rssiConversionTable[i], "RSSI"));
          Append(new ConversionField< SignedField<6> >(frsky.rssiAlarms[i].value, -50));
        }
      }
      else {
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
          Append(new UnsignedField<2>(frsky.channels[i].multiplier, 0, 3, "Multiplier"));
        }
        Append(new UnsignedField<2>(frsky.usrProto));
        Append(new UnsignedField<2>(frsky.blades));
        Append(new UnsignedField<1>(frsky.screens[0].type));
        Append(new UnsignedField<1>(frsky.screens[1].type));
        Append(new UnsignedField<2>(frsky.voltsSource));
        Append(new SignedField<4>(frsky.varioMin));
        Append(new SignedField<4>(frsky.varioMax));
        for (int i=0; i<2; i++) {
          Append(new ConversionField< UnsignedField<2> >(frsky.rssiAlarms[i].level, &rssiConversionTable[i], "RSSI level"));
          Append(new ConversionField< SignedField<6> >(frsky.rssiAlarms[i].value, -50, 0, 100, "RSSI value"));
        }
        for (int i=0; i<2; i++) {
          Append(new FrskyScreenField(frsky.screens[i], board, version));
        }
        Append(new UnsignedField<3>(frsky.varioSource));
        Append(new SignedField<5>(frsky.varioCenterMin));
        Append(new UnsignedField<3>(frsky.currentSource));
        Append(new SignedField<8>(frsky.varioCenterMax));
      }
    }

  protected:
    RSSIConversionTable rssiConversionTable[2];
};

int exportPpmDelay(int delay) { return (delay - 300) / 50; }
int importPpmDelay(int delay) { return 300 + 50 * delay; }

Open9xModelDataNew::Open9xModelDataNew(ModelData & modelData, BoardEnum board, unsigned int version, unsigned int variant):
  StructField(),
  board(board),
  variant(variant),
  protocolsConversionTable(board)
{
  sprintf(name, "Model %s", modelData.name);

  if (HAS_LARGE_LCD(board))
    Append(new ZCharField<12>(modelData.name));
  else
    Append(new ZCharField<10>(modelData.name));

  bool recent = (version >= 214 || (!IS_ARM(board) && version >= 213));

  if (recent)
    Append(new UnsignedField<8>(modelData.modelId));

  for (int i=0; i<O9X_MAX_TIMERS; i++) {
    Append(new TimerModeField(modelData.timers[i].mode, board, version));
    Append(new UnsignedField<16>(modelData.timers[i].val));
    if (HAS_PERSISTENT_TIMERS(board)) {
      Append(new BoolField<1>(modelData.timers[i].persistent));
      Append(new SpareBitsField<15>());
    }
  }

  Append(new ConversionField< SignedField<3> >(modelData.protocol, &protocolsConversionTable, "Protocol", ::QObject::tr("Open9x doesn't accept this protocol")));
  Append(new BoolField<1>(modelData.thrTrim));
  Append(new ConversionField< SignedField<4> >(modelData.ppmNCH, &channelsConversionTable, "Channels number", ::QObject::tr("Open9x doesn't allow this number of channels")));
  Append(new UnsignedField<3>(modelData.trimInc));
  Append(new BoolField<1>(modelData.disableThrottleWarning));
  Append(new BoolField<1>(modelData.pulsePol));
  Append(new BoolField<1>(modelData.extendedLimits));
  Append(new BoolField<1>(modelData.extendedTrims));
  Append(new SpareBitsField<1>());
  Append(new ConversionField< SignedField<8> >(modelData.ppmDelay, exportPpmDelay, importPpmDelay));

  if (IS_ARM(board) || board==BOARD_GRUVIN9X)
    Append(new UnsignedField<16>(modelData.beepANACenter));
  else
    Append(new UnsignedField<8>(modelData.beepANACenter));

  for (int i=0; i<MAX_MIXERS(board, version); i++)
    Append(new MixField(modelData.mixData[i], board, version));
  for (int i=0; i<MAX_CHANNELS(board, version); i++)
    Append(new LimitField(modelData.limitData[i], board));
  for (int i=0; i<MAX_EXPOS(board, version); i++)
    Append(new ExpoField(modelData.expoData[i], board, version));
  Append(new CurvesField(modelData.curves, board));
  for (int i=0; i<MAX_CUSTOM_SWITCHES(board, version); i++)
    Append(new CustomSwitchField(modelData.customSw[i], board, version));
  for (int i=0; i<MAX_CUSTOM_FUNCTIONS(board, version); i++)
    Append(new CustomFunctionField(modelData.funcSw[i], board, version));
  Append(new HeliField(modelData.swashRingData, board, version));
  for (int i=0; i<MAX_PHASES(board, version); i++)
    Append(new PhaseField(modelData.phaseData[i], i, board, version));
  Append(new SignedField<8>(modelData.ppmFrameLength));
  Append(new UnsignedField<8>(modelData.thrTraceSrc));

  if (!recent)
    Append(new UnsignedField<8>(modelData.modelId));

  if (board == BOARD_X9DA)
    Append(new UnsignedField<16>(modelData.switchWarningStates));
  else
    Append(new SwitchesWarningField<8>(modelData.switchWarningStates, board, version));

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
    Append(new FrskyField(modelData.frsky, board, version));
  }

  if (HAS_LARGE_LCD(board)) {
    Append(new CharField<10>(modelData.bitmap));
  }

  if (board == BOARD_SKY9X) {
    Append(new UnsignedField<8>(modelData.ppmSCH));
    Append(new UnsignedField<8>(modelData.ppm2SCH));
    Append(new ConversionField< SignedField<8> >(modelData.ppm2NCH, &channelsConversionTable, "Channels number", ::QObject::tr("Open9x doesn't allow this number of channels for PPM2")));
  }
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

  if (version >= 214 || (!IS_ARM(board) && version >= 213)) {
    internalField.Append(new SignedField<8>(generalData.vBatMin));
    internalField.Append(new SignedField<8>(generalData.vBatMax));
  }

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
