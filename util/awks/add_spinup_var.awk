{a = 0;}
($2 == "veg_parm_ID") {printf("%f	%s\n1.0      spinup_default_ID\n",$1,$2); a=1;}
(a == 0) {printf("%s	%s\n",$1,$2);}
