#ifndef MODELEDIT_H
#define MODELEDIT_H

#include <QDialog>
#include <QtGui>
#include <QPen>
#include "eeprominterface.h"
#include "mixerslist.h"

namespace Ui {
    class ModelEdit;
}

class ModelEdit : public QDialog
{
    Q_OBJECT

public:
    explicit ModelEdit(RadioData &radioData, uint8_t id, QWidget *parent = 0);
    ~ModelEdit();

    void applyBaseTemplate();
    bool redrawCurve;

private:
    Ui::ModelEdit *ui;

    QSpinBox* spn5[MAX_CURVE5][5];
    QSpinBox* spn9[MAX_CURVE9][9];

    MixersList *ExposlistWidget;

    MixersList *MixerlistWidget;

    RadioData &radioData;
    int       id_model;
    ModelData g_model;
    GeneralSettings g_eeGeneral;
    bool protocolEditLock;
    bool switchEditLock;
    bool heliEditLock;
    bool phasesLock;
    bool telemetryLock;

    bool plot_curve[16];

    QSpinBox  * cswitchOffset[NUM_CSW];
    QComboBox * cswitchSource1[NUM_CSW];
    QComboBox * cswitchSource2[NUM_CSW];

    QComboBox * fswtchSwtch[NUM_FSW];
    QComboBox * fswtchFunc[NUM_FSW];
    QSpinBox * fswtchParam[NUM_FSW];
    
    QSpinBox  * safetySwitchValue[NUM_CHNOUT];
    QComboBox * safetySwitchSwtch[NUM_CHNOUT];

    void setupExposListWidget();
    void setupMixerListWidget();

    void updateSettings();
    void tabModelEditSetup();
    void tabPhases();
    void tabExpos();
    void tabMixes();
    void tabHeli();
    void tabLimits();
    void tabCurves();
    void tabCustomSwitches();
    void tabSafetySwitches();
    void tabFunctionSwitches();
    void tabTelemetry();
    void tabTemplates();
    void updateCurvesTab();
    void setSwitchWidgetVisibility(int i);
    void setLimitMinMax();
    void updateSwitchesTab();
    void updateHeliTab();
    void updateA1Fields();
    void updateA2Fields();

    void launchSimulation();
    void resizeEvent(QResizeEvent *event  = 0);

    void drawCurve();
    int currentCurve;
    void setCurrentCurve(int curveId);

    QSpinBox *getNodeSB(int i);

    int getExpoIndex(int dch);
    bool gm_insertExpo(int idx);
    void gm_deleteExpo(int index);
    void gm_openExpo(int index);
    int gm_moveExpo(int idx, bool dir);
    void exposDeleteList(QList<int> list);
    QList<int> createExpoListFromSelected();
    void setSelectedByExpoList(QList<int> list);

    int getMixerIndex(int dch);
    bool gm_insertMix(int idx);
    void gm_deleteMix(int index);
    void gm_openMix(int index);
    int gm_moveMix(int idx, bool dir);
    void mixersDeleteList(QList<int> list);
    QList<int> createMixListFromSelected();
    void setSelectedByMixList(QList<int> list);

    void applyTemplate(uint8_t idx);
    MixData* setDest(uint8_t dch);
    void setCurve(uint8_t c, int8_t ar[]);
    void setSwitch(uint8_t idx, uint8_t func, int8_t v1, int8_t v2);

    void on_phaseName_editingFinished(unsigned int phase, QLineEdit *edit);
    void on_phaseSwitch_currentIndexChanged(unsigned int phase, int index);
    void on_phaseFadeIn_valueChanged(unsigned int phase, int value);
    void on_phaseFadeOut_valueChanged(unsigned int phase, int value);
    void on_phaseTrim_valueChanged(unsigned int phase, unsigned int stick, int value);
    void on_phaseTrimUse_currentIndexChanged(unsigned int phase, unsigned int stick, int index, QSpinBox *trim, QSlider *slider);
    void displayOnePhaseOneTrim(unsigned int phase_idx, unsigned int trim_idx, QComboBox *trimUse, QSpinBox *trimVal, QSlider *trimSlider);
    void displayOnePhase(unsigned int phase_idx, QLineEdit *name, QComboBox *sw, QSpinBox *fadeIn, QSpinBox *fadeOut, QComboBox *trim1Use, QSpinBox *trim1, QLabel *trim1Label, QSlider *trim1Slider, QComboBox *trim2Use, QSpinBox *trim2, QLabel *trim2Label, QSlider *trim2Slider, QComboBox *trim3Use, QSpinBox *trim3, QLabel *trim3Label, QSlider *trim3Slider, QComboBox *trim4Use, QSpinBox *trim4, QLabel *trim4Label, QSlider *trim4Slider);
    void incSubtrim(uint8_t idx, int16_t inc);

signals:
    void modelValuesChanged();

private slots:
    void clearExpos(bool ask=true);
    void clearMixes(bool ask=true);
    void clearCurves(bool ask=true);
    void on_T2ThrTrgChkB_toggled(bool checked);
    void on_TrainerChkB_toggled(bool checked);
    void on_ppmFrameLengthDSB_editingFinished();
    void on_DSM_Type_currentIndexChanged(int index);
    void on_pxxRxNum_editingFinished();
    void on_extendedLimitsChkB_toggled(bool checked);
    void on_extendedTrimsChkB_toggled(bool checked);
    void resetCurve();
    void editCurve();
    void plotCurve(bool checked);

    void mimeMixerDropped(int index, const QMimeData *data, Qt::DropAction action);
    void pasteMixerMimeData(const QMimeData * mimeData, int destIdx);

    void mimeExpoDropped(int index, const QMimeData *data, Qt::DropAction action);
    void pasteExpoMimeData(const QMimeData * mimeData, int destIdx);

    void on_pushButton_clicked();

    void exposDelete(bool ask=true);
    void exposCut();
    void exposCopy();
    void exposPaste();
    void exposDuplicate();
    void expoOpen(QListWidgetItem *item = NULL);
    void expoAdd();
    void moveExpoUp();
    void moveExpoDown();

    void mixersDelete(bool ask=true);
    void mixersCut();
    void mixersCopy();
    void mixersPaste();
    void mixersDuplicate();
    void mixerOpen();
    void mixerAdd();
    void moveMixUp();
    void moveMixDown();

    void expolistWidget_customContextMenuRequested(QPoint pos);
    void expolistWidget_doubleClicked(QModelIndex index);
    void expolistWidget_KeyPress(QKeyEvent *event);


    void mixerlistWidget_customContextMenuRequested(QPoint pos);
    void mixerlistWidget_doubleClicked(QModelIndex index);
    void mixerlistWidget_KeyPress(QKeyEvent *event);

    void curvePointEdited();
    void limitOffsetEdited();
    void limitEdited();
    void limitInvEdited();
    void customSwitchesEdited();
    void safetySwitchesEdited();
    void functionSwitchesEdited();
    void exposEdited();
    void mixesEdited();
    void heliEdited();

    void on_bcRUDChkB_toggled(bool checked);
    void on_bcELEChkB_toggled(bool checked);
    void on_bcTHRChkB_toggled(bool checked);
    void on_bcAILChkB_toggled(bool checked);
    void on_bcP1ChkB_toggled(bool checked);
    void on_bcP2ChkB_toggled(bool checked);
    void on_bcP3ChkB_toggled(bool checked);

    void on_thrExpoChkB_toggled(bool checked);
    void on_thrTrimChkB_toggled(bool checked);
    void on_ppmDelaySB_editingFinished();
    void on_a1UnitCB_currentIndexChanged(int index);
    void on_a2UnitCB_currentIndexChanged(int index);
    void on_a1RatioSB_editingFinished();
    void on_a1CalibSB_editingFinished();
    void on_a11LevelCB_currentIndexChanged(int index);
    void on_a11GreaterCB_currentIndexChanged(int index);
    void on_a11ValueSB_editingFinished();
    void on_a12LevelCB_currentIndexChanged(int index);
    void on_a12GreaterCB_currentIndexChanged(int index);
    void on_a12ValueSB_editingFinished();
    void on_a2RatioSB_editingFinished();
    void on_a2CalibSB_editingFinished();
    void on_a21LevelCB_currentIndexChanged(int index);
    void on_a21GreaterCB_currentIndexChanged(int index);
    void on_a21ValueSB_editingFinished();
    void on_a22LevelCB_currentIndexChanged(int index);
    void on_a22GreaterCB_currentIndexChanged(int index);
    void on_a22ValueSB_editingFinished();
    void on_frskyProtoCB_currentIndexChanged(int index);
    void on_frskyUnitsCB_currentIndexChanged(int index);
    void on_frskyBladesCB_currentIndexChanged(int index);
    void on_telBarCB_1_currentIndexChanged(int index);
    void on_telBarCB_2_currentIndexChanged(int index);
    void on_telBarCB_3_currentIndexChanged(int index);
    void on_telBarCB_4_currentIndexChanged(int index);
    void on_telMinSB_1_editingFinished();
    void on_telMinSB_2_editingFinished();
    void on_telMinSB_3_editingFinished();
    void on_telMinSB_4_editingFinished();
    void on_telMaxSB_1_editingFinished();
    void on_telMaxSB_2_editingFinished();
    void on_telMaxSB_3_editingFinished();
    void on_telMaxSB_4_editingFinished();
    void on_numChannelsSB_editingFinished();
    void on_protocolCB_currentIndexChanged(int index);
    void on_pulsePolCB_currentIndexChanged(int index);
    // TODO void on_trimSWCB_currentIndexChanged(int index);
    void on_trimIncCB_currentIndexChanged(int index);
    void on_timer1DirCB_currentIndexChanged(int index);
    void on_timer1ModeCB_currentIndexChanged(int index);
    void on_timer1ValTE_editingFinished();
    void on_timer2DirCB_currentIndexChanged(int index);
    void on_timer2ModeCB_currentIndexChanged(int index);
    void on_timer2ValTE_editingFinished();
    void on_modelNameLE_editingFinished();

    void on_phase0Name_editingFinished();
    void on_phase1Name_editingFinished();
    void on_phase2Name_editingFinished();
    void on_phase3Name_editingFinished();
    void on_phase4Name_editingFinished();

    void on_phase1Switch_currentIndexChanged(int index);
    void on_phase2Switch_currentIndexChanged(int index);
    void on_phase3Switch_currentIndexChanged(int index);
    void on_phase4Switch_currentIndexChanged(int index);

    void on_phase0FadeIn_valueChanged(int value);
    void on_phase0FadeOut_valueChanged(int value);
    void on_phase1FadeIn_valueChanged(int value);
    void on_phase1FadeOut_valueChanged(int value);
    void on_phase2FadeIn_valueChanged(int value);
    void on_phase2FadeOut_valueChanged(int value);
    void on_phase3FadeIn_valueChanged(int value);
    void on_phase3FadeOut_valueChanged(int value);
    void on_phase4FadeIn_valueChanged(int value);
    void on_phase4FadeOut_valueChanged(int value);

    void on_phase0Trim1_valueChanged(int value);
    void on_phase0Trim2_valueChanged(int value);
    void on_phase0Trim3_valueChanged(int value);
    void on_phase0Trim4_valueChanged(int value);
    void on_phase1Trim1Value_valueChanged(int value);
    void on_phase1Trim2Value_valueChanged(int value);
    void on_phase1Trim3Value_valueChanged(int value);
    void on_phase1Trim4Value_valueChanged(int value);
    void on_phase2Trim1Value_valueChanged(int value);
    void on_phase2Trim2Value_valueChanged(int value);
    void on_phase2Trim3Value_valueChanged(int value);
    void on_phase2Trim4Value_valueChanged(int value);
    void on_phase3Trim1Value_valueChanged(int value);
    void on_phase3Trim2Value_valueChanged(int value);
    void on_phase3Trim3Value_valueChanged(int value);
    void on_phase3Trim4Value_valueChanged(int value);
    void on_phase4Trim1Value_valueChanged(int value);
    void on_phase4Trim2Value_valueChanged(int value);
    void on_phase4Trim3Value_valueChanged(int value);
    void on_phase4Trim4Value_valueChanged(int value);

    void on_phase1Trim1Use_currentIndexChanged(int index);
    void on_phase1Trim2Use_currentIndexChanged(int index);
    void on_phase1Trim3Use_currentIndexChanged(int index);
    void on_phase1Trim4Use_currentIndexChanged(int index);
    void on_phase2Trim1Use_currentIndexChanged(int index);
    void on_phase2Trim2Use_currentIndexChanged(int index);
    void on_phase2Trim3Use_currentIndexChanged(int index);
    void on_phase2Trim4Use_currentIndexChanged(int index);
    void on_phase3Trim1Use_currentIndexChanged(int index);
    void on_phase3Trim2Use_currentIndexChanged(int index);
    void on_phase3Trim3Use_currentIndexChanged(int index);
    void on_phase3Trim4Use_currentIndexChanged(int index);
    void on_phase4Trim1Use_currentIndexChanged(int index);
    void on_phase4Trim2Use_currentIndexChanged(int index);
    void on_phase4Trim3Use_currentIndexChanged(int index);
    void on_phase4Trim4Use_currentIndexChanged(int index);

    void on_phases_currentChanged(int index);
    void on_tabWidget_currentChanged(int index);
    void on_templateList_doubleClicked(QModelIndex index);
    void ControlCurveSignal(bool flag);
    
};

#endif // MODELEDIT_H
