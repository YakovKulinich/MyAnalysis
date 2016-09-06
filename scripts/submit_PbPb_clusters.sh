#!/bin/bash

USERNAME=ykulinic
CONFIG=config/configClustersGrid.cfg
DATE=`date --utc '+20%y%m%d.%H%M'`
BLACKLIST=

OUTPUTS=myOut.root

INLIST=(data15_hi.00287866.physics_MinBias.merge.AOD.r7874_p2580)

now=$(date +"%T")
echo "Current time : $now"

for fn in ${INLIST[@]} ; do

  outFn=${fn//merge.AOD/Fluctuations}
  INDS=$fn
  OUTDS=user.${USERNAME}.${outFn}.${DATE}.121

	echo "Submitting to "${INDS}
  echo "Output DS name: "${OUTDS}

  prun --inDS=${INDS}\
       --outDS=${OUTDS}\
       --outputs=${OUTPUTS}\
       --exec="echo %IN > inputFiles.txt; runAnalysis ${CONFIG}"\
       --useRootCore\
       --excludedSite=${BLACKLIST}\
       --nGBPerJob=10\
       --tmpDir=/tmp\
       --mergeOutput

	 now=$(date +"%T")
	 echo "Current time : $now"
done
