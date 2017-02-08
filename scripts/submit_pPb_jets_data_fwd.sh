#!/bin/bash

USERNAME=ykulinic
CONFIG=config/configJetsFwd_pPb.cfg
DATE=`date --utc '+20%y%m%d.%H%M'`
BLACKLIST=

OUTPUTS=myOut.root

INLIST=( data16_hip5TeV.00314199.physics_Main.recon.AOD.f781_m1741 data16_hip5TeV.00312968.physics_Main.recon.AOD.f774_m1736 data16_hip5TeV.00312945.physics_Main.recon.AOD.f774_m1736 data16_hip5TeV.00312937.physics_Main.recon.AOD.f774_m1736 data16_hip5TeV.00312837.physics_Main.recon.AOD.f774_m1736  data16_hip5TeV.00312796.physics_Main.recon.AOD.f784_m1741 data16_hip5TeV.00312649.physics_Main.recon.AOD.f784_m1741 )

now=$(date +"%T")
echo "Current time : $now"

for fn in ${INLIST[@]} ; do

  outFn=${fn}
  INDS=$fn
  OUTDS=$outFn

	echo "Submitting to "${INDS}
  echo "Output DS name: "user.${USERNAME}.${OUTDS}.${DATE}

  prun --inDS=${INDS}\
       --outDS=user.${USERNAME}.${OUTDS}.${DATE}\
       --outputs=${OUTPUTS}\
       --exec="echo %IN > inputFiles.txt; runAnalysis ${CONFIG}"\
       --useRootCore\
       --excludedSite=${BLACKLIST}\
       --nGBPerJob=10\
       --tmpDir=/tmp\
       --mergeOutput

	 now=$(date +"%T")
	 echo "Current time : $now"
done                  #
