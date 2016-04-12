#ifndef __ROUTE_H__
#define __ROUTE_H__

#define NOT_VERTEX 0
#define NOT_MUST_PASS_VERTEX 1
#define MUST_PASS_VERTEX 2
#define START_VERTEX 3
#define END_VERTEX 4

struct Vertex{
	int type;
	Vertex(){
		type = NOT_VERTEX;
	};
};
typedef struct
{
	int source_id;
	int destination_id;
}Edge;

void load_data(char *graph[5000], int edge_num,char *condition);
void search_route(char *graph[5000], int edge_num, char *condition);

#endif
