#include "open9xinterface.h"
void include_open9x_arm_binaries(FirmwareInfo * open9x) {
open9x->add_option(new Open9xFirmware("open9x-arm-en", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-en.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-fr", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-fr.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-se", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-se.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-imperial-en", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-imperial-en.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-imperial-fr", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-imperial-fr.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-imperial-se", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-imperial-se.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-templates-en", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-templates-en.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-templates-fr", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-templates-fr.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-templates-se", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-templates-se.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-templates-imperial-en", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-templates-imperial-en.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-templates-imperial-fr", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-templates-imperial-fr.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-templates-imperial-se", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-templates-imperial-se.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-en", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-en.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-fr", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-fr.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-se", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-se.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-imperial-en", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-imperial-en.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-imperial-fr", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-imperial-fr.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-imperial-se", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-imperial-se.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-templates-en", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-templates-en.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-templates-fr", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-templates-fr.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-templates-se", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-templates-se.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-templates-imperial-en", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-templates-imperial-en.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-templates-imperial-fr", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-templates-imperial-fr.bin", OPEN9X_ARM_STAMP));
open9x->add_option(new Open9xFirmware("open9x-arm-heli-templates-imperial-se", new Open9xInterface(BOARD_ERSKY9X), OPEN9X_BIN_URL "open9x-arm-heli-templates-imperial-se.bin", OPEN9X_ARM_STAMP));
}
