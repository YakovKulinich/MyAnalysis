#!/bin/bash

USERNAME=ykulinic
CONFIG=config/configJetsFwd_pPb_8TeV.cfg
DATE=`date --utc '+20%y%m%d.%H%M'`
BLACKLIST=

OUTPUTS=myOut.root

INLIST=( data16_hip8TeV.00314170.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00314157.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00314112.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00314105.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00314077.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00314014.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00313984.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00313935.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00313929.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00313878.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00313833.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00313695.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00313688.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00313630.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00313629.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00313603.physics_Main.recon.AOD.f781_m1741 data16_hip8TeV.00313575.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313574.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313572.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313435.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313333.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313295.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313285.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313259.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313187.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313136.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313107.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313100.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313067.physics_Main.recon.AOD.f774_m1736 data16_hip8TeV.00313063.physics_Main.recon.AOD.f774_m1736 )

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
       --nFilesPerJob=20\       
       --tmpDir=/tmp\
       --mergeOutput

	 now=$(date +"%T")
	 echo "Current time : $now"
done                  #
