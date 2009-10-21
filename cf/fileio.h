void header_help(int *maxr, int *maxc, char* fndem);
int find_stream( struct flow_struct *, int, int *);
void find_max_flna( struct flow_struct *flow_table,int curr, int *str_inx);
int	find_patch(int , struct flow_struct *, int , int , int );
int	check_neighbours(int er,int ec, int *patch, int *zone, int *hill, int *stream, 
					 struct flow_struct *flow_entry, int num_adj,FILE *f1,
				int maxr, int maxc,int sc_flag, double cell); 
void pause();
