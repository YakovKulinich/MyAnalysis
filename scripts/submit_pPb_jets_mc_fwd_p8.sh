#!/bin/bash

USERNAME=ykulinic
CONFIG=config/configJetsFwd_pPb_mc.cfg
DATE=`date --utc '+20%y%m%d.%H%M'`
BLACKLIST=

OUTPUTS=myOut.root

INLIST=( mc15_5TeV.420018.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1R04_MaxEta_m3p0.merge.AOD.e6114_s3178_s3153_r9622_r9006 mc15_5TeV.420019.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2R04_MaxEta_m3p0.merge.AOD.e6114_s3178_s3153_r9622_r9006 )

now=$(date +"%T")
echo "Current time : $now"

for fn in ${INLIST[@]} ; do

  outFn=${fn//EvtGen_A14NNPDF23LO_jetjet_/_}
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
