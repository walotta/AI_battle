#include "AIController.h"
#include <utility>
#pragma GCC optimize("Ofast")
using std::cerr;
using std::endl;
using std::make_pair;
using std::pair;

extern int ai_side;
std::string ai_name = "Sora Ginko";

class DISSIONTREE;
class PIC
{
private:
	bool field[9][9][2];//0 up 1 right
	char board_statue[8][8];
public:
	friend class DISSIONTREE;
	int my_board=0,other_board=0;
	pair<int,int> my_pos;
	pair<int,int> other_pos;

	PIC(){
		for(int i=0;i<9;i++)
			for(int j=0;j<9;j++){
				for(int k=0;k<2;k++)
					field[i][j][k]=true;
			}
		for(int i=0;i<8;i++)
			for(int j=0;j<8;j++){
				board_statue[i][j]='n';
			}
	}

	void init(){
		if(ai_side){
			my_pos=make_pair(8,4);
			other_pos=make_pair(0,4);
		}else{
			my_pos=make_pair(0,4);
			other_pos=make_pair(8,4);
		}
	}
}pic;

class DISSIONTREE{
private:
	static double evaluate(const PIC& cal_pic){
		if(ai_side){
			return cal_pic.my_pos.second-8;
		}else{
			return -cal_pic.my_pos.second;
		}
	}

	struct node{
		PIC space;
		node* son=nullptr;
		node* brother=nullptr;
		pair<int,pair<int,int>>add_action;
		int layer_cnt=0;
		double score;
		node(const PIC& _space,pair<int,pair<int,int>> _add_action,int _layer_cnt):space(_space),add_action(_add_action),layer_cnt(_layer_cnt){}
		~node(){
			if(son->brother!=nullptr)delete son->brother;
			if(son!=nullptr)delete son;
		}
		void init(){
			score=evaluate(space);
		}
		void read_action(pair<int,pair<int,int>> loc){
			if(layer_cnt%2==0){
				switch (loc.first){
					case -1:
						init();
						break;
					case 0:
						space.my_pos=loc.second;
						init();
						break;
					case 1:
						space.field[loc.second.first][loc.second.second][1]=false;
						space.field[loc.second.first+1][loc.second.second][1]=false;
						space.board_statue[loc.second.first][loc.second.second]='1';
						space.my_board++;
						init();
						break;
					case 2:
						space.field[loc.second.first+1][loc.second.second][0]=false;
						space.field[loc.second.first+1][loc.second.second+1][0]=false;
						space.board_statue[loc.second.first][loc.second.second]='2';
						space.my_board++;
						init();
						break;
				}
			}else{
				switch (loc.first){
					case -1:
						init();
						break;
					case 0:
						space.other_pos=loc.second;
						init();
						break;
					case 1:
						space.field[loc.second.first][loc.second.second][1]=false;
						space.field[loc.second.first+1][loc.second.second][1]=false;
						space.board_statue[loc.second.first][loc.second.second]='1';
						space.other_board++;
						init();
						break;
					case 2:
						space.field[loc.second.first+1][loc.second.second][0]=false;
						space.field[loc.second.first+1][loc.second.second+1][0]=false;
						space.board_statue[loc.second.first][loc.second.second]='2';
						space.other_board++;
						init();
						break;
				}
			}
		}
	};
	int search_limit=2;
	node* root=nullptr;
public:
	void readIN(std::pair<int, std::pair<int, int> > loc){
		if(root==nullptr){
			switch (loc.first){
				case -1:
					root=new node(pic,loc,0);
					root->init();
					break;
				case 0:
					pic.other_pos=loc.second;
					root=new node(pic,loc,1);
					root->init();
					break;
				case 1:
					pic.field[loc.second.first][loc.second.second][1]=false;
					pic.field[loc.second.first+1][loc.second.second][1]=false;
					pic.board_statue[loc.second.first][loc.second.second]='1';
					pic.other_board++;
					root=new node(pic,loc,1);
					root->init();
					break;
				case 2:
					pic.field[loc.second.first+1][loc.second.second][0]=false;
					pic.field[loc.second.first+1][loc.second.second+1][0]=false;
					pic.board_statue[loc.second.first][loc.second.second]='2';
					pic.other_board++;
					root=new node(pic,loc,1);
					root->init();
					break;
			}
		}else{
			auto it=root->son;
			if(it->add_action==loc){
				auto new_root=it;
				root->son=it->brother;
				delete root;
				root=new_root;
				root->brother=nullptr;
			}else{
				while(it->brother->add_action!=loc)
					it=it->brother;
				auto new_root=it->brother;
				it->brother=it->brother->brother;
				delete root;
				root=new_root;
				root->brother=nullptr;
			}
		}
	}

	inline void build_next_layor(node* now_pos){
		//todo
		if(now_pos->son==nullptr){
			//build son layor
			//operator 0 move
			node* build_node=nullptr;
			if((now_pos->layer_cnt+1)%2==0){
				//推测己方决策
				if(now_pos->space.field[now_pos->space.my_pos.first][now_pos->space.my_pos.second][0]&&now_pos->space.my_pos.second+1<=8){
					if(build_node==nullptr){
						build_node=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.my_pos.first,now_pos->space.my_pos.second+1)),now_pos->layer_cnt+1);
						build_node->read_action(make_pair(0,make_pair(now_pos->space.my_pos.first,now_pos->space.my_pos.second+1)));
					}else{
						build_node->brother=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.my_pos.first,now_pos->space.my_pos.second+1)),now_pos->layer_cnt+1);
						build_node=build_node->brother;
						build_node->read_action(make_pair(0,make_pair(now_pos->space.my_pos.first,now_pos->space.my_pos.second+1)));
					}
				}
				if(now_pos->space.field[now_pos->space.my_pos.first][now_pos->space.my_pos.second][1]&&now_pos->space.my_pos.first+1<=8){
					if(build_node==nullptr){
						build_node=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.my_pos.first+1,now_pos->space.my_pos.second)),now_pos->layer_cnt+1);
						build_node->read_action(make_pair(0,make_pair(now_pos->space.my_pos.first+1,now_pos->space.my_pos.second)));
					}else{
						build_node->brother=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.my_pos.first+1,now_pos->space.my_pos.second)),now_pos->layer_cnt+1);
						build_node=build_node->brother;
						build_node->read_action(make_pair(0,make_pair(now_pos->space.my_pos.first+1,now_pos->space.my_pos.second)));
					}
				}
				if(now_pos->space.my_pos.second-1>=0&&now_pos->space.field[now_pos->space.my_pos.first][now_pos->space.my_pos.second-1][0]){
					if(build_node==nullptr){
						build_node=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.my_pos.first,now_pos->space.my_pos.second-1)),now_pos->layer_cnt+1);
						build_node->read_action(make_pair(0,make_pair(now_pos->space.my_pos.first,now_pos->space.my_pos.second-1)));
					}else{
						build_node->brother=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.my_pos.first,now_pos->space.my_pos.second-1)),now_pos->layer_cnt+1);
						build_node=build_node->brother;
						build_node->read_action(make_pair(0,make_pair(now_pos->space.my_pos.first,now_pos->space.my_pos.second-1)));
					}
				}
				if(now_pos->space.my_pos.first-1>=0&&now_pos->space.field[now_pos->space.my_pos.first-1][now_pos->space.my_pos.second][0]){
					if(build_node==nullptr){
						build_node=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.my_pos.first-1,now_pos->space.my_pos.second)),now_pos->layer_cnt+1);
						build_node->read_action(make_pair(0,make_pair(now_pos->space.my_pos.first-1,now_pos->space.my_pos.second)));
					}else{
						build_node->brother=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.my_pos.first-1,now_pos->space.my_pos.second)),now_pos->layer_cnt+1);
						build_node=build_node->brother;
						build_node->read_action(make_pair(0,make_pair(now_pos->space.my_pos.first-1,now_pos->space.my_pos.second)));
					}
				}
			}else{
				//推测对方决策
				if(now_pos->space.field[now_pos->space.other_pos.first][now_pos->space.other_pos.second][0]&&now_pos->space.other_pos.second+1<=8){
					if(build_node==nullptr){
						build_node=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.other_pos.first,now_pos->space.other_pos.second+1)),now_pos->layer_cnt+1);
						build_node->read_action(make_pair(0,make_pair(now_pos->space.other_pos.first,now_pos->space.other_pos.second+1)));
					}else{
						build_node->brother=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.other_pos.first,now_pos->space.other_pos.second+1)),now_pos->layer_cnt+1);
						build_node=build_node->brother;
						build_node->read_action(make_pair(0,make_pair(now_pos->space.other_pos.first,now_pos->space.other_pos.second+1)));
					}
				}
				if(now_pos->space.field[now_pos->space.other_pos.first][now_pos->space.other_pos.second][1]&&now_pos->space.other_pos.first+1<=8){
					if(build_node==nullptr){
						build_node=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.other_pos.first+1,now_pos->space.other_pos.second)),now_pos->layer_cnt+1);
						build_node->read_action(make_pair(0,make_pair(now_pos->space.other_pos.first+1,now_pos->space.other_pos.second)));
					}else{
						build_node->brother=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.other_pos.first+1,now_pos->space.other_pos.second)),now_pos->layer_cnt+1);
						build_node=build_node->brother;
						build_node->read_action(make_pair(0,make_pair(now_pos->space.other_pos.first+1,now_pos->space.other_pos.second)));
					}
				}
				if(now_pos->space.other_pos.second-1>=0&&now_pos->space.field[now_pos->space.other_pos.first][now_pos->space.other_pos.second-1][0]){
					if(build_node==nullptr){
						build_node=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.other_pos.first,now_pos->space.other_pos.second-1)),now_pos->layer_cnt+1);
						build_node->read_action(make_pair(0,make_pair(now_pos->space.other_pos.first,now_pos->space.other_pos.second-1)));
					}else{
						build_node->brother=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.other_pos.first,now_pos->space.other_pos.second-1)),now_pos->layer_cnt+1);
						build_node=build_node->brother;
						build_node->read_action(make_pair(0,make_pair(now_pos->space.other_pos.first,now_pos->space.other_pos.second-1)));
					}
				}
				if(now_pos->space.other_pos.first-1>=0&&now_pos->space.field[now_pos->space.other_pos.first-1][now_pos->space.other_pos.second][0]){
					if(build_node==nullptr){
						build_node=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.other_pos.first-1,now_pos->space.other_pos.second)),now_pos->layer_cnt+1);
						build_node->read_action(make_pair(0,make_pair(now_pos->space.other_pos.first-1,now_pos->space.other_pos.second)));
					}else{
						build_node->brother=new node(now_pos->space,make_pair(0,make_pair(now_pos->space.other_pos.first-1,now_pos->space.other_pos.second)),now_pos->layer_cnt+1);
						build_node=build_node->brother;
						build_node->read_action(make_pair(0,make_pair(now_pos->space.other_pos.first-1,now_pos->space.other_pos.second)));
					}
				}
			}
			if(((now_pos->layer_cnt+1)%2==0&&now_pos->space.my_board<10)||((now_pos->layer_cnt+1)%2==1&&now_pos->space.other_board<10)){
				//operator 1 place board 垂直
				for(int i=0;i<8;i++)
					for(int j=0;j<8;j++){
						if(now_pos->space.board_statue[i][j]=='n'&&(j-1>=0&&now_pos->space.board_statue[i][j-1]!='1')&&(j+1<8&&now_pos->space.board_statue[i][j+1]!='1')){
							if(build_node==nullptr){
								build_node=new node(now_pos->space,make_pair(1,make_pair(i,j)),now_pos->layer_cnt+1);
								build_node->read_action(make_pair(1,make_pair(i,j)));
							}else{
								build_node->brother=new node(now_pos->space,make_pair(1,make_pair(i,j)),now_pos->layer_cnt+1);
								build_node=build_node->brother;
								build_node->read_action(make_pair(1,make_pair(i,j)));
							}
						}
					}
			
				//operator 2 place board 水平
				for(int i=0;i<8;i++)
					for(int j=0;j<8;j++){
						if(now_pos->space.board_statue[i][j]=='n'&&(i-1>=0&&now_pos->space.board_statue[i-1][j]!='2')&&(i+1<8&&now_pos->space.board_statue[i+1][j]!='2')){
							if(build_node==nullptr){
								build_node=new node(now_pos->space,make_pair(2,make_pair(i,j)),now_pos->layer_cnt+1);
								build_node->read_action(make_pair(2,make_pair(i,j)));
							}else{
								build_node->brother=new node(now_pos->space,make_pair(2,make_pair(i,j)),now_pos->layer_cnt+1);
								build_node=build_node->brother;
								build_node->read_action(make_pair(2,make_pair(i,j)));
							}
						}
					}
			}
		}
	}

	pair<double,node*> dfs_decide(node* now_pos,int father_layor){
		if(now_pos->layer_cnt!=father_layor+search_limit){
			build_next_layor(now_pos);
			auto ans=dfs_decide(now_pos->son,father_layor);
			double choose_score=ans.first;
			node* choose_node=now_pos;
			node* it=now_pos->brother;
			if(now_pos->layer_cnt%2==0){
				//己方决策
				while(it!=nullptr){
					auto tem_ans=dfs_decide(it,father_layor);
					if(tem_ans.first>choose_score){
						choose_score=tem_ans.first;
						choose_node=it;
					}
					it=it->brother;
				}
				return make_pair(choose_score,choose_node);
			}else{
				//对方决策
				while(it!=nullptr){
					auto tem_ans=dfs_decide(it,father_layor);
					if(tem_ans.first<choose_score){
						choose_score=tem_ans.first;
						choose_node=it;
					}
					it=it->brother;
				}
				return make_pair(choose_score,choose_node);
			}
		}else{
			double choose_score=now_pos->score;
			node* choose_node=now_pos;
			node* it=now_pos->brother;
			if(now_pos->layer_cnt%2==0){
				while(it!=nullptr){
					if(it->score>choose_score){
						choose_score=it->score;
						choose_node=it;
					}
					it=it->brother;
				}
				return make_pair(choose_score,choose_node);
			}else{
				while(it!=nullptr){
					if(it->score<choose_score){
						choose_score=it->score;
						choose_node=it;
					}
					it=it->brother;
				}
				return make_pair(choose_score,choose_node);
			}
		}
	}

	inline pair<int,pair<int,int>> decide(){
		auto decide_res=dfs_decide(root,root->layer_cnt);
		return decide_res.second->add_action;
	}
}disstionTree;

void init() {
	std::ios::sync_with_stdio(false);
	pic.init();
}

std::pair<int, std::pair<int, int> > action(std::pair<int, std::pair<int, int> > loc) {
	disstionTree.readIN(loc);
	return disstionTree.decide();
}
