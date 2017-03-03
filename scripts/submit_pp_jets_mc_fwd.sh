B#!/bin/bash
1;4601;0c
USERNAME=ykulinic
CONFIG=config/configJetsFwd_pp_mc.cfg
DATE=`date --utc '+20%y%m%d.%H%M'`
BLACKLIST=

OUTPUTS=myOut.root

INLIST=( mc15_5TeV.420021.PowhegPythia8EvtGen_A14_NNPDF23LO_CT10ME_jetjet_JZ1R04.merge.AOD.e4109_s2860_r7792_r7676 mc15_5TeV.420022.PowhegPythia8EvtGen_A14_NNPDF23LO_CT10ME_jetjet_JZ2R04.merge.AOD.e4109_s2860_r7792_r7676 mc15_5TeV.420023.PowhegPythia8EvtGen_A14_NNPDF23LO_CT10ME_jetjet_JZ3R04.merge.AOD.e5067_s2860_r7792_r7676 mc15_5TeV.420024.PowhegPythia8EvtGen_A14_NNPDF23LO_CT10ME_jetjet_JZ4R04.merge.AOD.e5067_s2860_r7792_r7676 mc15_5TeV.420025.PowhegPythia8EvtGen_A14_NNPDF23LO_CT10ME_jetjet_JZ5R04.merge.AOD.e5067_s2860_r7792_r7676 )

now=$(date +"%T")
echo "Current time : $now"

for fn in ${INLIST[@]} ; do

  outFn=${fn//EvtGen_A14_NNPDF23LO_CT10ME_jetjet_/_}
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
