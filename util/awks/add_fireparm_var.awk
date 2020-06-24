{a = 0;}
($2 == "landuse_parm_ID") {printf("%f	%s\n1.0      fire_parm_ID\n",$1,$2); a=1;}
(a == 0) {printf("%s	%s\n",$1,$2);}
