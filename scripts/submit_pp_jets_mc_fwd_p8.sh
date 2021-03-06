#!/bin/bash

USERNAME=ykulinic
CONFIG=config/configJetsFwd_pp_mc.cfg
DATE=`date --utc '+20%y%m%d.%H%M'`
BLACKLIST=

OUTPUTS=myOut.root

INLIST=( mc15_5TeV.420009.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ04R04.merge.AOD.e4947_s2860_r7792_r7676 mc15_5TeV.420011.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ1R04.merge.AOD.e4108_s2860_r7792_r7676 mc15_5TeV.420012.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2R04.merge.AOD.e4108_s2860_r7792_r7676 )

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
