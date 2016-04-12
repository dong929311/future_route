#include "route.h"
#include "lib_record.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/timeb.h>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#define MAX_VERTEX_NUM 600
#define MAX_EDGE_NUM 4800
#define MAX_INT 60000
using namespace std;

Edge edges[MAX_EDGE_NUM];
Vertex vertexs[MAX_VERTEX_NUM];
DemandVertexs demand_vertexs;
int start_vertex,end_vertex;
vector<int> open;
stack<int> path;
bool visited[MAX_VERTEX_NUM];
int dis[MAX_VERTEX_NUM];
int pre[MAX_VERTEX_NUM];

int result_path_v[MAX_VERTEX_NUM];
int result_path_v_num = 0;
int result_edge[MAX_VERTEX_NUM-1];
int result_edge_num = 0;
int tmp_total_cost = 0;
int result_total_cost = MAX_INT;
priority_queue<Node,vector<Node>,MyPriority> p_queue;

int min_cost_count = 0;
class ComGreater{
public: bool operator()(const int &a,const int &b)
	{
		return dis[a] > dis[b];
	}
};

void load_data(char *topo[5000],int edge_num,char *demand)
{
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
		edges[link_id].source_id = source_id;
		edges[link_id].destination_id = destination_id;
		edges[link_id].cost = cost;
		vertexs[source_id].out_edge_index[vertexs[source_id].out_edge_num] = link_id;
		vertexs[source_id].out_edge_num++;
	}
	
	start_vertex = atoi(strtok(demand,","));
	end_vertex = atoi(strtok(NULL,","));

	res =strtok(NULL,"|");
	while(res != NULL){
		vertexs[atoi(res)].is_demand = true;
		demand_vertexs.vertexs[demand_vertexs.len] = atoi(res);
		demand_vertexs.len++;
		res = strtok(NULL,"|");
	}
}

void dijkstra(int s,int d)
{
	for (int i = 0; i < MAX_VERTEX_NUM; i += 1)
	{
		dis[i] = MAX_INT;
	}
	open.clear();
	open.push_back(s);
	dis[s] = 0;
	pre[s] = 0;
	int cur_vertex_id;
	int cur_edge_id;
	vector<int> ::iterator it;
	while(open.size() != 0 ){
		cur_vertex_id = open.back();
		if(cur_vertex_id == d) return;
		open.erase(open.end()-1);
		for (int i = 0; i < vertexs[cur_vertex_id].out_edge_num; i += 1)
		{
			cur_edge_id = vertexs[cur_vertex_id].out_edge_index[i];
			if (!visited[edges[cur_edge_id].destination_id])
			{
				it = find(open.begin(),open.end(),edges[cur_edge_id].destination_id);
				if(it == open.end()){
					open.push_back(edges[cur_edge_id].destination_id);
					dis[edges[cur_edge_id].destination_id] = dis[cur_vertex_id] + edges[cur_edge_id].cost;
					pre[edges[cur_edge_id].destination_id] = cur_vertex_id;
				}
				else{
					if(dis[*it] > dis[cur_vertex_id] + edges[cur_edge_id].cost){
						dis[*it] = dis[cur_vertex_id] + edges[cur_edge_id].cost;
						pre[*it] = cur_vertex_id;
					}
				}
			}
			visited[cur_vertex_id] = true;
			sort(open.begin(),open.end(),ComGreater());
		}
	}
} 

void get_path(int s,int d)
{
	int root = d;
	while(root != s){
		path.push(root);
		root = pre[root];
	}
	path.push(s);
}

void swap(int *a,int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

long time_interval(struct timeb t_begin,struct timeb t_end)
{
	return (t_end.time - t_begin.time)*1000 + (t_end.millitm - t_begin.millitm);
}

void find_method1(int tmp_demand[])
{
	int path_v[MAX_VERTEX_NUM];
	int path_v_num = 0;
	tmp_total_cost = 0;
	while(!path.empty())
	{
		path.pop();
	}
	memset(visited,false,sizeof(visited));
	int s,d;
	s = start_vertex;
	visited[end_vertex] = true;
	int i = 0;
	for(; i<= demand_vertexs.len; i++)
	{
		if (i != demand_vertexs.len && !visited[tmp_demand[i]])
		{	
			d = tmp_demand[i];
			dijkstra(s,d);
			if(dis[d] == MAX_INT) break;
			get_path(s,d);
			tmp_total_cost += dis[d];
			while(!path.empty())
			{
				path_v[path_v_num++] = path.top();
				path.pop();
			}
			--path_v_num;
			memset(visited,false,sizeof(visited));
			visited[end_vertex] = true;
			for(int i = 0; i < path_v_num;i++)
			{
				visited[path_v[i]] = true;
			}
			visited[d] = false;
			s = d;
		}
		else if(i == demand_vertexs.len)
		{
			d = end_vertex;
			visited[end_vertex] = false;
			dijkstra(s,d);
			if(dis[d] == MAX_INT) break;
			get_path(s,d);
			tmp_total_cost +=dis[d];
			while(!path.empty())
			{
				path_v[path_v_num++] = path.top();
				path.pop();
			}
		}
			
	}
	if(i == demand_vertexs.len+1) 
	{
			
		if(result_total_cost > tmp_total_cost )
		{
			result_total_cost = tmp_total_cost;
			memcpy(result_path_v,path_v,path_v_num*sizeof(int));
			result_path_v_num = path_v_num;
			min_cost_count = 0;
		}
		else min_cost_count ++;
	}
}

void find_method2(int tmp_demand[])
{
	int path_start_v[MAX_VERTEX_NUM];
	int path_middle_v[MAX_VERTEX_NUM];
	int path_end_v[MAX_VERTEX_NUM];
	int path_start_v_num = 0;
	int path_middle_v_num = 0;
	int path_end_v_num = 0;
	int middle_total_cost = 0;
	tmp_total_cost	= 0;
	while(!path.empty()){
		path.pop();
	}
	memset(visited,false,sizeof(visited));
	int s,d;
	bool flag = false;
	visited[start_vertex] = true;
	visited[end_vertex] = true;
	s = tmp_demand[0];
	for (int i = 1; i < demand_vertexs.len; i += 1)
	{
		if(!visited[tmp_demand[i]])
		{
			d = tmp_demand[i];
			dijkstra(s,d);
			if(dis[d] == MAX_INT)
			{
				flag = true;
				break;
			}
			get_path(s,d);
			tmp_total_cost += dis[d];
			while(!path.empty())
			{
				path_middle_v[path_middle_v_num++] = path.top();
				path.pop();
			}
			--path_middle_v_num;
			memset(visited,false,sizeof(visited));
			visited[start_vertex] = true;
			visited[end_vertex] = true;
			for (int j = 0; j < path_middle_v_num; j += 1)
			{
				visited[path_middle_v[j]] = true;
			}
			visited[d] = false;
			s = d; 
		}
	}
	if(flag) return ;
	middle_total_cost = tmp_total_cost;
	s = start_vertex;
	d = path_middle_v[0];
	visited[start_vertex] = false;
	visited[d] = false;
	visited[path_middle_v[path_middle_v_num]] = true;
	dijkstra(s,d);
	if(dis[d] == MAX_INT) return;
	get_path(s,d);
	tmp_total_cost += dis[d];
	while(!path.empty())
	{
		path_start_v[path_start_v_num++] = path.top();
		path.pop();
	}
	--path_start_v_num;
	
	memset(visited,false,sizeof(visited));
	for ( int i = 0; i < path_start_v_num; i += 1)
	{
		visited[path_start_v[i]] = true; 
	}
	for ( int i = 0; i < path_middle_v_num; i += 1)
	{
		visited[path_middle_v[i]] = true;
	}
	s = path_middle_v[path_middle_v_num];
	d = end_vertex;
	dijkstra(s,d);
	if(dis[d] == MAX_INT) return;
	get_path(s,d);
	tmp_total_cost += dis[d];
	while(!path.empty())
	{
		path_end_v[path_end_v_num++] = path.top();
		path.pop();
	}
	if(result_total_cost > tmp_total_cost)
	{
		result_total_cost = tmp_total_cost;
		memcpy(result_path_v,path_start_v,path_start_v_num*sizeof(int));
		result_path_v_num = path_start_v_num;
		memcpy(result_path_v+result_path_v_num,path_middle_v,path_middle_v_num*sizeof(int));
		result_path_v_num += path_middle_v_num;
		memcpy(result_path_v+result_path_v_num,path_end_v,path_end_v_num*sizeof(int));
		result_path_v_num += path_end_v_num;
		min_cost_count = 0;
	}
	else min_cost_count++;
	
	path_start_v_num = 0;
	path_end_v_num = 0;
	tmp_total_cost = middle_total_cost;
	memset(visited,false,sizeof(visited));
	visited[start_vertex] = true;
	for (int i = 0; i < path_middle_v_num; i += 1)
	{
		visited[path_middle_v[i]] = true;
	}
	s = path_middle_v[path_middle_v_num];
	d = end_vertex;
	visited[s] = false;
	dijkstra(s,d);
	if(dis[d] == MAX_INT) return;
	get_path(s,d);
	tmp_total_cost += dis[d];
	while(!path.empty())
	{
		path_end_v[path_end_v_num++] = path.top();
		path.pop();
	}
	memset(visited,false,sizeof(visited));
	for (int i = 0; i < path_middle_v_num; i += 1)
	{
		visited[path_middle_v[i]] = true;
	}
	for ( int i = 0; i < path_end_v_num; i += 1)
	{
		visited[path_end_v[i]] = true;
	}
	s = start_vertex;
	d = path_middle_v[0];
	visited[d] = false;
	dijkstra(s,d);
	if(dis[d] == MAX_INT) return;
	get_path(s,d);
	tmp_total_cost += dis[d];
	while(!path.empty())
	{
		path_start_v[path_start_v_num++] = path.top();
		path.pop();
	}
	--path_start_v_num;
	if(result_total_cost > tmp_total_cost)
	{
		result_total_cost = tmp_total_cost;
		memcpy(result_path_v,path_start_v,path_start_v_num*sizeof(int));
		result_path_v_num = path_start_v_num;
		memcpy(result_path_v+result_path_v_num,path_middle_v,path_middle_v_num*sizeof(int));
		result_path_v_num += path_middle_v_num;
		memcpy(result_path_v+result_path_v_num,path_end_v,path_end_v_num*sizeof(int));
		result_path_v_num += path_end_v_num;
		min_cost_count = 0;
	}
	else min_cost_count++;
}	

void find_way()
{   
	int tmp_demand[demand_vertexs.len];
	memcpy(tmp_demand,demand_vertexs.vertexs,demand_vertexs.len*sizeof(int));
	srand((unsigned)time(NULL));
	struct timeb t_begin,t_end;
	ftime(&t_begin);
	while(true)
	{	
		ftime(&t_end);
		if(time_interval(t_begin,t_end) > 8000*2) return ;
		for(int i = 0;i < demand_vertexs.len;i++)
		{
			swap(&tmp_demand[i],&tmp_demand[rand()%(demand_vertexs.len-i)+i]);
		}
		find_method1(tmp_demand);
		find_method2(tmp_demand);
		if(min_cost_count == 400) return ;
	}
	
}

bool get_result()
{
	if(result_total_cost == MAX_INT)
	{
		return false;
	}
	else
	{
		int tmp_edge;
		for (int i = 0; i < result_path_v_num-1; i += 1)
		{	
			tmp_edge = MAX_INT;
			for (int j = 0; j < vertexs[result_path_v[i]].out_edge_num; j += 1)
			{
				if (edges[vertexs[result_path_v[i]].out_edge_index[j]].destination_id == result_path_v[i+1])
				{
					if(tmp_edge == MAX_INT) 
						tmp_edge = vertexs[result_path_v[i]].out_edge_index[j];
					else if(edges[tmp_edge].cost > edges[vertexs[result_path_v[i]].out_edge_index[j]].cost)
						tmp_edge = vertexs[result_path_v[i]].out_edge_index[j];
				}
			}
			result_edge[i] = tmp_edge;
		}
		result_edge_num = result_path_v_num -1;
	}
	return true;
}


bool init_queue()
{
	memset(visited,false,sizeof(visited));
	dijkstra(start_vertex,end_vertex);
	if(dis[end_vertex] == MAX_INT) return false;
	Node start_node; 
	start_node.vertex_id = start_vertex;
	start_node.parent = NULL;
	start_node.current_path_demand_num = 0;
	start_node.current_path_total_cost = 0;
	start_node.estimate_next_path_demand_num = 0;
	start_node.estimate_next_path_total_cost = 0;
	start_node.estimate_total_demand_num = 0;
	start_node.estimate_total_cost = 0;
	p_queue.push(start_node);
	return true;	
}

class MyCom
{
public:	bool operator()(const int &a,const int &b)
		{
			if(edges[a].destination_id == edges[b].destination_id)
				return edges[a].cost < edges[b].cost;
			else return edges[a].destination_id < edges[b].destination_id;
		}
};

void delete_repeat_elem(Node &current_node,int arc_edges[],int &arc_edges_len,bool flag[])
{
	arc_edges_len = vertexs[current_node.vertex_id].out_edge_num;
	if (arc_edges_len == 0) return ;
	for (int i = 0; i < vertexs[current_node.vertex_id].out_edge_num; i += 1)
	{
		arc_edges[i] = vertexs[current_node.vertex_id].out_edge_index[i];			
	}
	sort(arc_edges,arc_edges+arc_edges_len,MyCom());
	memset(flag,false,arc_edges_len*sizeof(bool));
	for (int i = 1; i < arc_edges_len; i += 1)
	{
		if(edges[arc_edges[i]].destination_id == edges[arc_edges[i-1]].destination_id)
		{
			flag[i] = true;
		}
	}
	return ;
}

void get_current_path_info(pNode current_pnode,int start_to_current_path[],int &start_to_curent_path_len)
{
	pNode p = current_pnode;
	while(!path.empty())
	{
		path.pop();
	}
	while(p!=NULL)
	{
		path.push(p->vertex_id);
		p = p->parent;
	}
	start_to_curent_path_len = 0;
	while(!path.empty())
	{
		start_to_current_path[start_to_curent_path_len++] = path.top();
		path.pop();
	}
	return ;
}

void copy_node(pNode current_pnode,Node &current_node)
{
	current_pnode->vertex_id = current_node.vertex_id;
	current_pnode->parent = current_node.parent;
	current_pnode->current_path_demand_num = current_node.current_path_demand_num;
	current_pnode->current_path_total_cost = current_node.current_path_total_cost;
	current_pnode->estimate_next_path_demand_num = current_node.estimate_next_path_demand_num;
	current_pnode->estimate_next_path_total_cost = current_node.estimate_next_path_total_cost;
	current_pnode->estimate_total_demand_num = current_node.estimate_total_demand_num;
	current_pnode->estimate_total_cost = current_node.estimate_total_cost;
}

bool has_loop(int start_to_current_path[],int &start_to_curent_path_len,int child_vertex_id)
{
	for (int i = 0; i < start_to_curent_path_len; i += 1)
	{
		if(start_to_current_path[i] == child_vertex_id)  return true;
	}
	return false;
}

void close_current_path(int start_to_current_path[],int &start_to_curent_path_len)
{
	memset(visited,false,sizeof(visited));
	for (int i = 0; i < start_to_curent_path_len; i += 1)
	{
		visited[start_to_current_path[i]] = true;
	}
}

int count_estimate_demand_num(int s,int d)
{
	int cnt = 0;
	int root = d;
	while(root != s){
		if(vertexs[root].is_demand) ++cnt;
		root = pre[root];
	}
	return cnt;
}

void search_graph()
{
	
	if(!init_queue()) return;
	struct timeb t_begin,t_end;
	ftime(&t_begin);
	Node current_node;
	Node child_node;
	int arc_edges[MAX_OUTEDGE_LEN];
	int arc_edges_len;
	int start_to_current_path[MAX_VERTEX_NUM];
	int start_to_curent_path_len;
	bool flag[MAX_OUTEDGE_LEN];
	while(!p_queue.empty())
	{
		ftime(&t_end);
		if(time_interval(t_begin,t_end) > 9000*2) return ;
		current_node = p_queue.top();
		p_queue.pop();
		pNode current_pnode = (pNode)malloc(sizeof(Node));
		copy_node(current_pnode,current_node);		
		delete_repeat_elem(current_node,arc_edges,arc_edges_len,flag);
		get_current_path_info(current_pnode,start_to_current_path,start_to_curent_path_len);
		for (int i = 0; i < arc_edges_len; i += 1)
		{
			if(!flag[i]&&!has_loop(start_to_current_path,start_to_curent_path_len,edges[arc_edges[i]].destination_id))
			{
				if(edges[arc_edges[i]].destination_id == end_vertex)
				{
					if(current_pnode->current_path_demand_num == demand_vertexs.len)
					{
						if(current_pnode->current_path_total_cost + edges[arc_edges[i]].cost < result_total_cost)
						{
							memcpy(result_path_v,start_to_current_path,start_to_curent_path_len*sizeof(int));
							result_path_v[start_to_curent_path_len] = end_vertex;
							result_path_v_num = start_to_curent_path_len + 1;
							result_total_cost = current_pnode->current_path_total_cost + edges[arc_edges[i]].cost;
						}
						else continue;
						
					}
					else continue;
				}
				close_current_path(start_to_current_path,start_to_curent_path_len);
				dijkstra(edges[arc_edges[i]].destination_id,end_vertex);
				if(dis[end_vertex] != MAX_INT && current_pnode->current_path_total_cost + dis[end_vertex] < result_total_cost)
				{
					child_node.vertex_id = edges[arc_edges[i]].destination_id;
					child_node.parent = current_pnode;
					if(vertexs[child_node.vertex_id].is_demand)
					{
						child_node.current_path_demand_num = current_pnode->current_path_demand_num + 1;
					}
					else   child_node.current_path_demand_num = current_pnode->current_path_demand_num;
					child_node.current_path_total_cost = current_pnode->current_path_total_cost + edges[arc_edges[i]].cost;
					child_node.estimate_next_path_demand_num = count_estimate_demand_num(child_node.vertex_id,end_vertex);
					child_node.estimate_next_path_total_cost = dis[end_vertex];
					child_node.estimate_total_cost = child_node.current_path_total_cost + child_node.estimate_next_path_total_cost;
					child_node.estimate_total_demand_num = child_node.current_path_demand_num + child_node.estimate_next_path_demand_num;
					p_queue.push(child_node);
//					if(!vertexs[edges[arc_edges[i]].destination_id].is_demand)
//					{
//						for (unsigned int i = 0; i < 6; i += 1)
//						{
//							find_method(current_pnode,start_to_current_path,start_to_curent_path_len,arc_edges[i]);
//						}
//						
//					}
				}
				
				
			}
		}
	}
	return;
}


//你要完成的功能总入口
void search_route(char *topo[5000], int edge_num, char *demand)
{
   load_data(topo,edge_num,demand);
   memset(visited,false,sizeof(visited));
   if(edge_num < 100) search_graph();
   else find_way();
   printf("total_cost = %d ",result_total_cost);
   for ( int i = 0; i < result_path_v_num; i += 1)
   {
   		printf("%d->",result_path_v[i]);
   }
   if(!get_result()) return ;
   for (int i = 0; i < result_edge_num; i += 1)
   {
   		record_result(result_edge[i]);
   }
}







