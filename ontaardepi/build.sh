#!/bin/bash
set -e

if [ -z "$1" ]; then 
  echo "ERROR: specify path to project"
  exit 1
fi

TMP_DIR="/tmp/ob_singular"

create_singular_project() {
  rm -rf $TMP_DIR
  mkdir $TMP_DIR
  cd $TMP_DIR
  ano init
  cd -
  cd $1
  # create singular ino file
  cat Kerstboom022.ino \
    BT_code.ino \
    DuoPompRegeling.ino \
    Laagwater.ino \
    Lees_Hum_en_Temp.ino \
    LeesLight.ino \
    SD_card_modules.ino \
    SMS_afhandeling.ino \
    VerwerkSensoren.ino \
    > $TMP_DIR/src/sketch.ino
  cp -r ../libraries $TMP_DIR/lib
  cd -
}

build_project() {
  cd $TMP_DIR
  ano build -m mega2560
  ano upload -m mega2560 -p /dev/ttyACM0
  cd -
}

create_singular_project $1
build_project
#ano build -s ~/ontaardeboom/Kerstboom022 -m mega2560 -f ${GPP_INCLUDES}
