#include "route.h"
#include "lib_record.h"
#include "lp_lib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stack>
#include <time.h>
#include <sys/timeb.h>
#include <iostream>
#define MAX_VERTEX_NUM 600
#define MAX_EDGE_NUM 4800
using namespace std;

Vertex vertexs[MAX_VERTEX_NUM];
int edges[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
int edges_id[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
int edge_to_colno[MAX_VERTEX_NUM][MAX_VERTEX_NUM];
Edge colno_to_edge[MAX_EDGE_NUM+1];
int start_vertex,end_vertex;
int result_edge_num = 0;
int result_edge[MAX_EDGE_NUM];

void load_data(char *topo[5000],int edge_num,char *demand)
{
	memset(edges,0,sizeof(edges));
	char *res = NULL;
	int link_id;
	int source_id;
	int destination_id;
	int cost;
	for(int i = 0; i < edge_num; i++){
		res = strtok(topo[i],",");
		link_id = atoi(res);
		res = strtok(NULL,",");
		source_id = atoi(res);
		res = strtok(NULL,",");
		destination_id = atoi(res);
		res = strtok(NULL,",");
		cost = atoi(res);
		if(edges[source_id][destination_id] == 0)
		{
			edges[source_id][destination_id] = cost;
			edges_id[source_id][destination_id] = link_id;
		}
		else if(cost < edges[source_id][destination_id])
		{
			edges[source_id][destination_id] = cost;
			edges_id[source_id][destination_id] = link_id;
		}
		vertexs[source_id].type = NOT_MUST_PASS_VERTEX;
		vertexs[destination_id].type = NOT_MUST_PASS_VERTEX;
	}
	
	start_vertex = atoi(strtok(demand,","));
	vertexs[start_vertex].type = START_VERTEX;
	end_vertex = atoi(strtok(NULL,","));
	vertexs[end_vertex].type = END_VERTEX;
	res =strtok(NULL,"|");
	while(res != NULL){
		vertexs[atoi(res)].type = MUST_PASS_VERTEX;
		res = strtok(NULL,"|");
	}
}

int get_Ncolno_and_init_edge_to_colno()
{
	int colno = 1;
	for (unsigned int i = 0; i < MAX_VERTEX_NUM; i += 1)
	{
		for (unsigned int j = 0; j < MAX_VERTEX_NUM; j += 1)
		{
			if(edges[i][j] != 0)
			{
				edge_to_colno[i][j] = colno++;
				colno_to_edge[colno-1].source_id = i;
				colno_to_edge[colno-1].destination_id = j;
			}
		}
	}
	return colno-1;
}

bool is_correct_solution(REAL *row,int Ncol)
{
	int count = 0;
	int adj_vertex[MAX_VERTEX_NUM];
	for (int i = 0; i < Ncol; i += 1)
	{
		if((int)row[i] == 1)
		{
			count++;
			adj_vertex[colno_to_edge[i+1].source_id] = colno_to_edge[i+1].destination_id;
		}
	}
	int current_vertex = start_vertex;
	int len = 0;
	result_edge_num = 0;
	while(current_vertex != end_vertex)
	{
		len++;
		result_edge[result_edge_num++] = edges_id[current_vertex][adj_vertex[current_vertex]];
		current_vertex = adj_vertex[current_vertex];
		
	}
//	if(len == count) cout<<"success!"<<endl;
	return len == count;
}

long time_interval(struct timeb t_begin,struct timeb t_end)
{
	return (t_end.time - t_begin.time)*1000 + (t_end.millitm - t_begin.millitm);
}

int find_path()
{
  lprec *lp;
  int ret = 0,*colno = NULL,j;
  REAL *row = NULL;
  int Ncol = get_Ncolno_and_init_edge_to_colno();
  lp = make_lp(0,Ncol);
  struct timeb t_begin,t_end;
  ftime(&t_begin);
  for (int i = 1; i <= Ncol; i += 1)
  {
  	set_binary(lp,i,TRUE);
  }
  if(ret == 0)
  {
  	colno = (int*)malloc(Ncol*sizeof(*colno));
  	row = (REAL*)malloc(Ncol*sizeof(*row));
  	if(colno == NULL || row == NULL)
  	{
  		ret = 2;
  		return ret;
  	}
  }
  set_add_rowmode(lp,TRUE);
  for (int i = 0; i < MAX_VERTEX_NUM; i += 1)
  {
	switch(vertexs[i].type)
	{
		case NOT_VERTEX: break; 
		case NOT_MUST_PASS_VERTEX:
				j = 0;
				for (int k = 0; k < MAX_VERTEX_NUM; k += 1)
				{
					if(edges[i][k] != 0)
					{
						colno[j] = edge_to_colno[i][k];
						row[j++] = 1;
					}
				}
				for (int k = 0; k < MAX_VERTEX_NUM; k += 1)
				{
					if(edges[k][i] != 0)
					{
						colno[j] = edge_to_colno[k][i];
						row[j++] = -1;
					}
				}
				add_constraintex(lp,j,row,colno,EQ,0);
				break;
		case MUST_PASS_VERTEX:
				j = 0;
				for (int k = 0; k < MAX_VERTEX_NUM; k += 1)
				{
					if(edges[i][k] != 0)
					{
						colno[j] = edge_to_colno[i][k];
						row[j++] = 1;
					}
				}
				add_constraintex(lp,j,row,colno,EQ,1);
				j = 0;
				for (int k = 0; k < MAX_VERTEX_NUM; k += 1)
				{
					if(edges[k][i] != 0)
					{
						colno[j] = edge_to_colno[k][i];
						row[j++] = 1;
					}
				}
				add_constraintex(lp,j,row,colno,EQ,1);
				break;
		case START_VERTEX:
				j = 0;
				for (int k = 0; k < MAX_VERTEX_NUM; k += 1)
				{
					if(edges[i][k] != 0)
					{
						colno[j] = edge_to_colno[i][k];
						row[j++] = 1;
					}
				}
				add_constraintex(lp,j,row,colno,EQ,1);
				j = 0;
				for (int k = 0; k < MAX_VERTEX_NUM; k += 1)
				{
					if(edges[k][i] != 0)
					{
						colno[j] = edge_to_colno[k][i];
						row[j++] = 1;
					}
				}
				add_constraintex(lp,j,row,colno,EQ,0);
				break;
		case END_VERTEX:
				j = 0;
				for (int k = 0; k < MAX_VERTEX_NUM; k += 1)
				{
					if(edges[i][k] != 0)
					{
						colno[j] = edge_to_colno[i][k];
						row[j++] = 1;
					}
				}
				add_constraintex(lp,j,row,colno,EQ,0);
				j = 0;
				for (int k = 0; k < MAX_VERTEX_NUM; k += 1)
				{
					if(edges[k][i] != 0)
					{
						colno[j] = edge_to_colno[k][i];
						row[j++] = 1;
					}
				}
				add_constraintex(lp,j,row,colno,EQ,1);
				break;
		default : break;
	}  	
  }
  j = 0;
  for (int i = 0; i < MAX_VERTEX_NUM; i += 1)
  {
  	for (int k = 0; k < MAX_VERTEX_NUM; k += 1)
  	{
  		if(edges[i][k] != 0)
  		{
  			colno[j] = edge_to_colno[i][k];
  			row[j++] = edges[i][k];
  		}
  	}
  }
  add_constraintex(lp,j,row,colno,GE,0);
  if(ret == 0)
  {
  	set_add_rowmode(lp,FALSE);
  	set_timeout(lp,1);
  	j = 0;
  	for (int i = 0; i < MAX_VERTEX_NUM; i += 1)
  	{
  		for (int k = 0; k < MAX_VERTEX_NUM; k += 1)
  		{
  			if(edges[i][k] != 0)
  			{
  				colno[j] = edge_to_colno[i][k];
  				row[j++] = edges[i][k];
  			}
  		}
  	}
  	if(!set_obj_fnex(lp,j,row,colno))
  	{
  		ret == 4;
  	}
  }
  if(ret == 0)
  {
  	set_minim(lp);
  	set_verbose(lp,IMPORTANT);
  	ret = solve(lp);
  	if(ret == OPTIMAL)
  		ret = 0;
  	else ret = 5;
  } 
  if(ret == 0)
  {
  	int result = (int)get_objective(lp);
  	get_variables(lp, row);
	while(!is_correct_solution(row,Ncol))
	{
	  del_constraint(lp,get_Nrows(lp));
	  j = 0;
	  for (int i = 0; i < MAX_VERTEX_NUM; i += 1)
	  {
	  	for (int k = 0; k < MAX_VERTEX_NUM; k += 1)
	  	{
	  		if(edges[i][k] != 0)
	  		{
	  			colno[j] = edge_to_colno[i][k];
	  			row[j++] = edges[i][k];
	  		}
	  	}
	  }
	  ftime(&t_end);
	  if(time_interval(t_begin,t_end) > 8000) return -1;
	  add_constraintex(lp,j,row,colno,GE,result+1);
	  set_timeout(lp,1);
	  solve(lp);
	  result = (int)get_objective(lp);
	  get_variables(lp,row);
    }
    return 0;   
  }
  
 if(row != NULL)
    free(row);
 if(colno != NULL)
    free(colno);
 if(lp != NULL) {
    delete_lp(lp);
  }	
 
  return ret;
}

//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
   load_data(topo,edge_num,demand);
   int ret = find_path();
   if(ret == -1) result_edge_num = 0;
   for (int i = 0; i < result_edge_num; i += 1)
   {
   		record_result(result_edge[i]);
   }
}







