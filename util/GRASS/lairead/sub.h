	void	input_prompt(int *, int *, char *, char *, char *,  char *, char *,
				char *, char *, char *, int );
  

	void	input_ascii_int(int *, char *, int, int, int);
	void	input_ascii_float(float *, char *, int, int, int, float);
	void	input_ascii_sint(short int *, char *, int, int, int);
	void	link_patch_veg(struct flow_struct *, struct allom_struct *, int, int);
	void	change_world(FILE *, FILE *, struct flow_struct *, int);
	int	read_allom_table(FILE *, int, struct allom_struct *);
	void	print_flow_table(int, struct flow_struct *, char *, char *);
	int	build_flow_table(FILE *, struct flow_struct *, 
			int *, float *, int *, int *, int *, int, int);



int funcnumber(char *funcname);
void msgnoloop(char *msg);
int varnumber( char *vname, char **VARNAME);
void space_output(FILE *outfile, int lev);
void readheader(FILE *templatefile , struct headerstruct  *header );
void outputheader(FILE *templatefile, struct headerstruct *header);
void error(char *msg);
