/*--------------------------------------------------------------*/
/* 											*/
/*					compute_stream_routing			*/
/*											*/
/*	compute_stream_routing.c - creates a patch object				*/
/*											*/
/*	NAME										*/
/*	compute_stream_routing.c - creates a patch object				*/
/*											*/
/*	SYNOPSIS									*/
/*	struct routing_list_object compute_stream_routing( 				*/
/*							struct command_line_object command */
/*							struct stream_network_object *network)	*/
/*							int num_reaches,
/*							struct date *current_date)	*/
/*											*/
/* 											*/
/*											*/
/*	OPTIONS										*/
/*											*/
/*											*/
/*	DESCRIPTION									*/
/*											*/
/* 	computes reach scale stream routing using nonlinear kimetic wave					*/
/*											*/
/*											*/
/*											*/
/*	PROGRAMMER NOTES								*/
/*											*/
/*			                                   */
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"
#include <math.h>








double  compute_stream_routing(struct command_line_object *command_line,
						 struct stream_network_object *stream_network,
						 int  num_reaches,
						 struct	date	current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.				*/
	/*--------------------------------------------------------------*/
	
    double nonlinear_kimetic_wave(
                        double ,
                        double , 
                        double ,
			            double ,
			            double ,
                        double , 
                        double );
	double reservoir_operation(struct reservoir_object *,
                                  double ,
                                  double ,
                                  struct date);
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/

	int i;
	int j;
	int k;
    int downstream_neighbour;
    double alfa;
    double tangent;
    double stagelow;
    double manning_new;
    double dt;
    double xarea;
    double lateral_input_flow,streamflow;
	double Qout,Qin,previous_lateral_input,length,initial_flow,temp,sum;
	

	struct patch_object *patch;
	struct hillslope_object *hillslope;

	/*--------------------------------------------------------------*/
	/* route water from top to bottom				*/
	/*--------------------------------------------------------------*/

	dt=86400.0;
	streamflow=0.0;
	sum=0.0;
	for (i = 0; i < num_reaches; i++) {
	/* calculate total lateral input from patches */
	   lateral_input_flow = 0.0;
		Qout=0.0;
		Qin=0.0;
		previous_lateral_input=0.0;
		length=0.0;
		initial_flow=0.0;
	   for (j=0; j <stream_network[i].num_lateral_inputs; j++) {
	            patch=stream_network[i].lateral_inputs[j];
		   if (patch[0].drainage_type == STREAM  ){
	      lateral_input_flow += (patch[0].streamflow)*patch[0].area/(3600*24*stream_network[i].length); //unit:m2/s
			   sum+= (patch[0].streamflow)*patch[0].area;}
		   
	
	}
		for (j=0; j <stream_network[i].num_neighbour_hills; j++) {
			hillslope=stream_network[i].neighbour_hill[j];
			lateral_input_flow += (hillslope[0].base_flow)*hillslope[0].area/(3600*24*stream_network[i].length); //unit:m2/s
			sum+= (hillslope[0].base_flow)*hillslope[0].area;
						
		}
          
	   /*calulate alfa from manning conductivity, wetperimeter, and streamslope*/
           if(stream_network[i].stream_slope <=0 ) stream_network[i].stream_slope=0.01;
	   alfa = pow(stream_network[i].manning*pow(stream_network[i].bottom_width,(2.0/3.0))*pow((1/stream_network[i].stream_slope),-0.5),0.6);
	   tangent = (stream_network[i].top_width-stream_network[i].bottom_width)/(2*stream_network[i].max_height);
	   if(tangent <= 0.0) tangent=0.0001;
	   alfa = alfa*pow((1+2*sqrt(1+tangent*tangent)*stream_network[i].water_depth/stream_network[i].bottom_width),0.4);
        

	    /*consider variation of manning N when water level rise*/
	   stagelow = 0.5;
	   if(stream_network[i].water_depth > stagelow*stream_network[i].max_height) 
	       manning_new = stream_network[i].manning*2.3;
	   else
	       manning_new = stream_network[i].manning;
		alfa = alfa*pow((manning_new/stream_network[i].manning),0.6);
            
          
        /*calulate stream flow by using nonlinear kimetic wave */
		Qin=stream_network[i].Qin;
		initial_flow=stream_network[i].initial_flow;
		previous_lateral_input=stream_network[i].previous_lateral_input;
		length=stream_network[i].length;
		Qout=nonlinear_kimetic_wave(alfa,Qin,initial_flow,lateral_input_flow,previous_lateral_input,length,dt);
        stream_network[i].Qout=Qout;
		

		/*calulate water depth for next time step */
		xarea=alfa*pow(stream_network[i].Qin,0.6);
		stream_network[i].water_depth=(-stream_network[i].bottom_width+sqrt(abs(stream_network[i].bottom_width*stream_network[i].bottom_width+4*tangent*xarea)))/(2*tangent);
        	
		
		/*If there is a reservoir in this reach, do reservoir operation */
	
		if(stream_network[i].reservoir_ID!=0){
			   stream_network[i].Qout=reservoir_operation(&(stream_network[i].reservoir),stream_network[i].Qout,dt,current_date);
			 
					}

		/*calulate initial flow and previous lateral input for next time step */
		stream_network[i].initial_flow=Qout;
		stream_network[i].previous_lateral_input=lateral_input_flow;
		stream_network[i].Qin=0.0;
		
        /*calulate income flow  for downstream neighbours */
	     for (j=0; j< stream_network[i].num_downstream_neighbours; j++) {
			 downstream_neighbour=stream_network[i].downstream_neighbours[j];
                            for(k=i;k<num_reaches;k++)
                               if(stream_network[k].reach_ID == downstream_neighbour){
                                    stream_network[k].Qin += Qout/stream_network[i].num_downstream_neighbours;
								   temp=stream_network[k].Qin;
					break;			
	}	
	}
	}
 
    streamflow=stream_network[num_reaches-1].Qout;
	return(streamflow);

} /*end compute_stream_routing.c*/


double nonlinear_kimetic_wave(double alfa,double Qin,double initial_flow,double lateral_input,double previous_lateral_input,double dx,double dt)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.				*/
	/*--------------------------------------------------------------*/

	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	
	int mlm;
	int k;
	int ilm;
	double beta,Qout;
	double epsi0;
	double qk;
	double qk1;
	double up;
	double down;
	double c;
	double epsi;
	double f1;
	double fk;
	double alam;
	double f;


    /*--------------------------------------------------------------*/
	/*INITIAL ESTIMATE OF QT BY LINEAR KINEMATIC SCHEME*/
    /*--------------------------------------------------------------*/
        beta=0.6;
	epsi0=0.001;
	mlm=5; 
	k = 0;
	 if(Qin <= 4.5e-308 && initial_flow <= 4.5e-308)
		 qk=0.5*(lateral_input+previous_lateral_input)*dx;
	 else
	 {up=(dt/dx)*Qin+alfa*beta*initial_flow*pow((0.5*(Qin+initial_flow)),(beta-1))+dt*0.5*(lateral_input+previous_lateral_input);
		 down=(dt/dx)+alfa*beta*pow((0.5*(Qin+initial_flow)),(beta-1));
		 qk=up/down;
    
	 }
	 if(qk<0){
		 Qout=0;
	         return(Qout);}
    /*--------------------------------------------------------------*/
	/*Downhill Newton method*/
    /*--------------------------------------------------------------*/
	 c=(dt/dx)*Qin+alfa*pow(initial_flow,beta)+dt*0.5*(lateral_input+previous_lateral_input);
	
         epsi=0.00001*c;
	 do{
		 fk=(dt/dx)*qk+alfa*pow(qk,beta)-c;
	 f1=(dt/dx)+alfa*beta*pow(qk,(beta-1));
	 qk1=qk-fk/f1;
	 k=k+1;
       
	 if(qk1<=0)
		 qk1=qk*0.00000001;

	 for(ilm=1;ilm<=mlm;ilm++){
		 alam=1.0/pow(2.0,(ilm-1));
		 Qout=alam*qk1+(1-alam)*qk;
		 f=(dt/dx)*Qout+alfa*pow(Qout,beta)-c;
        
		 if(abs(f)<=epsi || abs(f)<=epsi0)
                    goto _jumpout;
		 if(abs(f)<abs(fk))
			 break;
	 }
	 qk=Qout;
	 if(k>25)
		 break;}while(abs(f)>epsi);
         _jumpout:
         return(Qout);


}

	
	double reservoir_operation(struct reservoir_object *current_reservoir,double inflow,double dt,struct date current_date)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.				*/
	/*--------------------------------------------------------------*/

	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	
	
	double storage;
	double outflow;

	storage=current_reservoir->initial_storage;
     
	
	outflow=current_reservoir->min_outflow;
	
	storage=current_reservoir->initial_storage+(inflow-outflow)*dt*86400;
	
        if(storage>current_reservoir->month_max_storage[current_date.month-1]){
		
            outflow=outflow+(storage-current_reservoir->month_max_storage[current_date.month-1])/(dt*86400);
		
		storage=current_reservoir->month_max_storage[current_date.month-1];
		
	}
	if(storage<current_reservoir->min_storage){
		if(current_reservoir->flag_min_flow_storage==0 && storage<0)/*min_flow has higher priority*/{
			storage=0;
			outflow=(current_reservoir->initial_storage-storage)/(dt*86400)+inflow;
		}
			
		
		if(current_reservoir->flag_min_flow_storage!=0) /*min_storage has higher priority*/{
			storage=current_reservoir->min_storage;
			outflow=(current_reservoir->initial_storage-storage)/(dt*86400)+inflow;
		}
			
		
	}
	current_reservoir->initial_storage=storage;
	
	return(outflow);





	 }

