#!/bin/bash

rhessys_bin=$1
world=$2
tec=$3
flow=$4
args=$5
sty=$6
stm=$7
std=$8
sth=$9
edy=${10}
edm=${11}
edd=${12}
edh=${13}

cat <<EOF
BEGIN {line=1;}
(line==1) { 
	printf("mkdir %s/out/cal/%s\n", dir, name);
	printf("touch %s/out/cal/%s/par%d\n", dir, name, tag);
	printf("touch %s/out/cal/%s/str%d\n", dir, name, tag); 
	printf("touch %s/out/cal/%s/trans%d\n", dir, name, tag); 
	printf("touch %s/out/cal/%s/evap%d\n", dir, name, tag); 
	printf("touch %s/out/cal/%s/psn%d\n", dir, name, tag);
	line=2}
{

printf("cd %s/worldfiles\n", dir);
printf("$rhessys_bin -t ../tec/$tec -w $world -b -p $args "); 
printf("-r ../flow/$flow -pre ../out/cal/%s/sen%d -st $sty $stm $std $sth -ed $edy $edm $edd $edh ", name, tag);
printf("-s %f %f -sv %f %f -gw %f %f\n",\$1,\$2,\$1,\$2,\$3,\$4);

printf("cd ../out/cal/%s\n", name);
printf("echo %f %f %f %f %f > tpar%d\n", \$1,\$2,\$3,\$4,\$5,tag);
printf("cat par%d tpar%d > newpar%d\n", tag, tag, tag);
printf("mv newpar%d par%d\n", tag, tag);

printf("awk '{printf(\"%%lf\\\\n\", \$col)}' col=19 < sen%d_basin.daily > tstr%d\n", tag, tag);
printf("paste str%d tstr%d > newstr%d\n", tag, tag, tag);
printf("mv newstr%d str%d\n", tag, tag);

printf("awk '{printf(\"%%lf\\\\n\", \$col)}' col=16 < sen%d_basin.daily > ttrans%d\n", tag, tag);
printf("paste trans%d ttrans%d > newtrans%d\n", tag, tag, tag);
printf("mv newtrans%d trans%d\n", tag, tag);

printf("awk '{printf(\"%%lf\\\\n\", \$col)}' col=14 < sen%d_basin.daily > tevap%d\n", tag, tag);
printf("paste evap%d tevap%d > newevap%d\n", tag, tag, tag);
printf("mv newevap%d evap%d\n", tag, tag);

printf("awk '{printf(\"%%lf\\\\n\", \$col)}' col=20 < sen%d_basin.daily > tpsn%d\n", tag, tag);
printf("paste psn%d tpsn%d > newpsn%d\n", tag, tag, tag);
printf("mv newpsn%d psn%d\n", tag, tag);

printf("rm tpar%d\n", tag);
printf("rm tstr%d\n",tag);
printf("rm ttrans%d\n", tag);
printf("rm tevap%d\n", tag);
printf("rm tpsn%d\n", tag);
}
EOF

