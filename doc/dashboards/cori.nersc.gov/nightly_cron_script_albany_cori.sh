#!/bin/csh

BASE_DIR=/project/projectdirs/piscees/nightlyCoriCDash
EXE_DIR=/project/projectdirs/piscees/nightlyCoriCDashExe

cd $BASE_DIR

source cori_modules.sh >& modules.out 
export CRAYPE_LINK_TYPE=STATIC

cp $BASE_DIR/repos/Albany/doc/dashboards/cori.nersc.gov/nightly_cron_script_cismAlbany_cori_submit.sh $BASE_DIR


LOG_FILE=$BASE_DIR/nightly_log_coriAlbany.txt

eval "env  TEST_DIRECTORY=$BASE_DIR SCRIPT_DIRECTORY=$BASE_DIR ctest -VV -S $BASE_DIR/ctest_nightly_albany.cmake" > $LOG_FILE 2>&1

cp -r build/AlbanyInstall/bin/* $EXE_DIR
chmod -R 0755 $EXE_DIR

