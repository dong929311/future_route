#ifndef __ROUTE_H__
#define __ROUTE_H__

#define MAX_OUTEDGE_LEN 8
#define MAX_DEMAND_VERTEX 50
#define TRUE 1
#define FALSE 0
typedef struct {	
	int source_id;
	int destination_id;
	int cost;
}Edge;

struct Vertex{
	int out_edge_index[MAX_OUTEDGE_LEN];
	int out_edge_num;
	bool is_demand;
	Vertex(){
		out_edge_num = 0;
		is_demand = false;
	};
};

struct DemandVertexs{
	int vertexs[MAX_DEMAND_VERTEX];
	int len;
	DemandVertexs(){
		len = 0;
	};
};

typedef struct node{
	int vertex_id;
	struct node *parent;
	int current_path_demand_num;
	int current_path_total_cost;
	int estimate_next_path_demand_num;
	int estimate_next_path_total_cost;
	int estimate_total_demand_num;
	int estimate_total_cost;
}Node,*pNode;

struct MyPriority
{
	bool operator()(const Node &node1,const Node &node2)
	{
		if(node1.estimate_total_demand_num == node2.estimate_total_demand_num)
			return node1.current_path_total_cost > node2.current_path_total_cost ; // small first
		else return node1.estimate_total_demand_num < node2.estimate_total_demand_num; //large first
	}
};

void dijkstra(int s,int d);
void get_path(int s,int d);
long time_interval(struct timeb t_begin,struct timeb t_end);
void find_way();
bool get_result();
void load_data(char *graph[5000], int edge_num,char *condition);
void search_route(char *graph[5000], int edge_num, char *condition);

#endif
