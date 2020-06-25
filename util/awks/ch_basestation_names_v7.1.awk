BEGIN {h=0;}
{a = 0; }
($2 == "basin_ID") { h=1;}
($2 == "n_basestations") && (h==1) {printf("%f	%s\n",$1,$2="basin_n_basestations"); a=1;}
($2 == "hillslope_ID") { h=2;}
($2 == "n_basestations") && (h==2) {printf("%f	%s\n",$1,$2="hillslope_n_basestations"); a=1;}
($2 == "zone_ID") { h=3;}
($2 == "n_basestations") && (h==3) {printf("%f	%s\n",$1,$2="zone_n_basestations"); a=1;}
($2 == "patch_ID") { h=4;}
($2 == "n_basestations") && (h==4) {printf("%f	%s\n",$1,$2="patch_n_basestations"); a=1;}
($2 == "canopy_strata_ID") { h=5;}
($2 == "n_basestations") && (h==5) {printf("%f	%s\n",$1,$2="canopy_strata_n_basestations"); a=1; h=0;}
($2 == "zone_base_station_ID") {printf("%f %s\n",$1,$2="zone_basestation_ID"); a=1;}
(a == 0) {printf("%s	%s\n",$1,$2);}

