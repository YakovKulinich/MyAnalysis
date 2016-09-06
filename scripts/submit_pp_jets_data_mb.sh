#!/bin/bash

USERNAME=ykulinic
CONFIG=config/configJetsGridMB.cfg
DATE=`date --utc '+20%y%m%d.%H%M'`
BLACKLIST=

OUTPUTS=myOut.root
#data15_5TeV.00286474.physics_MinBias.merge.AOD.f652_m1533 data15_5TeV.00286411.physics_MinBias.merge.AOD.f652_m1533 data15_5TeV.00286367.physics_MinBias.merge.AOD.f652_m1528 data15_5TeV.00286361.physics_MinBias.merge.AOD.f652_m1528 data15_5TeV.00286328.physics_MinBias.merge.AOD.f652_m1528 data15_5TeV.00286364.physics_MinBias.merge.AOD.f652_m1528 data15_5TeV.00286282.physics_MinBias.merge.AOD.f652_m1528 
INLIST=( data15_5TeV.00286367.physics_MinBias.merge.AOD.f652_m1528 )

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
