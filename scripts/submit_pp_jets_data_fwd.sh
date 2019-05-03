#!/bin/bash

USERNAME=ykulinic
CONFIG=config/configJetsFwd_pp.cfg
DATE=`date --utc '+20%y%m%d.%H%M'`
BLACKLIST=

OUTPUTS=myOut.root

# INLIST=(data15_5TeV.00286282.physics_Main.recon.AOD.r9623 data15_5TeV.00286361.physics_Main.recon.AOD.r9582 data15_5TeV.00286364.physics_Main.recon.AOD.r9582 data15_5TeV.00286367.physics_Main.recon.AOD.r9582 data15_5TeV.00286411.physics_Main.recon.AOD.r9582 data15_5TeV.00286474.physics_Main.recon.AOD.r9582)
INLIST=(data15_5TeV.00286364.physics_Main.recon.AOD.r9582 data15_5TeV.00286411.physics_Main.recon.AOD.r9582)

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
       --nGBPerJob=MAX\
       --nFilesPerJob=15\
       --tmpDir=/tmp\
       --mergeOutput

	 now=$(date +"%T")
	 echo "Current time : $now"
done
