#include "AIController.h"
#include <utility>
#pragma GCC optimize("Ofast")
using std::cerr;
using std::endl;
using std::make_pair;
using std::pair;

extern int ai_side;
std::string ai_name = "Sora Ginko";

class PIC
{
private:
	char board_statue[8][8];
public:
	friend class DISSIONTREE;
	int my_board=0,other_board=0;
	pair<int,int> my_pos;
	pair<int,int> other_pos;

	PIC(){
		for(int i=0;i<8;i++)
			for(int j=0;j<8;j++){
				board_statue[i][j]='n';
			}
	}

	void init(){
		if(ai_side==0){
			my_pos=make_pair(8,4);
			other_pos=make_pair(0,4);
		}else{
			my_pos=make_pair(0,4);
			other_pos=make_pair(8,4);
		}
	}

	bool can_walk(pair<int,int> pos,int nrow,int ncol)const
    {
	    int row=pos.first;
	    int col=pos.second;
	    if(abs(nrow-row)+abs(ncol-col)<=1){
            if(nrow<0||nrow>8||ncol<0||ncol>8)return false;
            if(row<nrow){
                if((col<8&&board_statue[row][col]=='2')||(col-1>=0&&board_statue[row][col-1]=='2'))
                    return false;
            }else if(row>nrow){
                if((col<8&&board_statue[row-1][col]=='2')||(col-1>=0&&board_statue[row-1][col-1]=='2'))
                    return false;
            }else if(col<ncol){
                if((row>0&&board_statue[row-1][col]=='1')||(row<8&&board_statue[row][col]=='1'))
                    return false;
            }else{
                if((row>0&&board_statue[row-1][col-1]=='1')||(row<8&&board_statue[row][col-1]=='1'))
                    return false;
            }
        }
	    return true;
    }

    bool finish(){
	    if(my_pos.first==(ai_side==0?0:8))
	        return true;
	    else return false;
	}
}pic;

class DISSIONTREE{
private:
	static double evaluate(const PIC& cal_pic){
		if(ai_side){
			return cal_pic.my_pos.first-8;
		}else{
			return -cal_pic.my_pos.first;
		}
	}

	struct node{
		PIC status;
		node* son=nullptr;
		node* brother=nullptr;
		pair<int,pair<int,int>>add_action;
		int layer_cnt;
		double score;
        void init(){
            score=evaluate(status);
        }
        void read_action(pair<int,pair<int,int>> loc){
            if(layer_cnt%2==0){
                switch (loc.first){
                    case -1:
                        init();
                        break;
                    case 0:
                        status.my_pos=loc.second;
                        init();
                        break;
                    case 1:
                        status.board_statue[loc.second.first][loc.second.second]='1';
                        status.my_board++;
                        init();
                        break;
                    case 2:
                        status.board_statue[loc.second.first][loc.second.second]='2';
                        status.my_board++;
                        init();
                        break;
                }
            }else{
                switch (loc.first){
                    case -1:
                        init();
                        break;
                    case 0:
                        status.other_pos=loc.second;
                        init();
                        break;
                    case 1:
                        status.board_statue[loc.second.first][loc.second.second]='1';
                        status.other_board++;
                        init();
                        break;
                    case 2:
                        status.board_statue[loc.second.first][loc.second.second]='2';
                        status.other_board++;
                        init();
                        break;
                }
            }
        }
		node(const PIC& pre_status,const pair<int,pair<int,int>>& _add_action,int _layer_cnt): status(pre_status), add_action(_add_action), layer_cnt(_layer_cnt){
            read_action(_add_action);
            init();
        }
		~node(){
			if(brother!=nullptr)delete brother;
			if(son!=nullptr)delete son;
		}
	};
	int search_limit=2;
	node* root=nullptr;
public:
	void readIN(std::pair<int, std::pair<int, int> > loc){
		if(root==nullptr){
			root=new node(pic,loc,1);
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
        //if(now_pos->status.finish())return;
		if(now_pos->son==nullptr){
			//build son layor
			//operator 0 move
			node* build_node=nullptr;
			if((now_pos->layer_cnt+1)%2==0){
				//推测己方决策
				int d_row[]={0,0,-1,1};
				int d_col[]={-1,1,0,0};
				for(int i=0;i<4;i++){
				    int n_row=now_pos->status.my_pos.first+d_row[i];
				    int n_col=now_pos->status.my_pos.second+d_col[i];
				    if(n_row==now_pos->status.other_pos.first && n_col==now_pos->status.other_pos.second){
				        //需要跳跃
                        int nn_row=now_pos->status.my_pos.first+2*d_row[i];
                        int nn_col=now_pos->status.my_pos.second+2*d_col[i];
                        if(now_pos->status.can_walk(make_pair(n_row,n_col),nn_row,nn_col)){
                            node* tem_node=new node(now_pos->status, make_pair(0, make_pair(nn_row,nn_col)),now_pos->layer_cnt+1);
                            if(build_node==nullptr)
                                build_node=tem_node;
                            else{
                                tem_node->brother=build_node;
                                build_node=tem_node;
                            }
                        }else{
                            int dd_beg;
                            if(i>=0&&i<=1)dd_beg=2;
                            else dd_beg=0;
                            for(int j=dd_beg;j<dd_beg+2;j++){
                                nn_row=n_row+d_row[j];
                                nn_col=n_col+d_col[j];
                                if(now_pos->status.can_walk(make_pair(n_row,n_col),nn_row,nn_col)){
                                    node* tem_node=new node(now_pos->status, make_pair(0, make_pair(nn_row,nn_col)),now_pos->layer_cnt+1);
                                    if(build_node==nullptr)
                                        build_node=tem_node;
                                    else{
                                        tem_node->brother=build_node;
                                        build_node=tem_node;
                                    }
                                }
                            }
                        }
				    }else{
				        //直接走
				        if(now_pos->status.can_walk(now_pos->status.my_pos,n_row,n_col)){
				            node* tem_node=new node(now_pos->status, make_pair(0, make_pair(n_row,n_col)),now_pos->layer_cnt+1);
				            if(build_node==nullptr)
				                build_node=tem_node;
				            else{
				                tem_node->brother=build_node;
				                build_node=tem_node;
				            }
				        }
				    }
                }
			}else{
				//推测对方决策
                int d_row[]={0,0,-1,1};
                int d_col[]={-1,1,0,0};
                for(int i=0;i<4;i++){
                    int n_row=now_pos->status.other_pos.first+d_row[i];
                    int n_col=now_pos->status.other_pos.second+d_col[i];
                    if(n_row==now_pos->status.my_pos.first && n_col==now_pos->status.my_pos.second){
                        //需要跳跃
                        int nn_row=now_pos->status.other_pos.first+2*d_row[i];
                        int nn_col=now_pos->status.other_pos.second+2*d_col[i];
                        if(now_pos->status.can_walk(make_pair(n_row,n_col),nn_row,nn_col)){
                            node* tem_node=new node(now_pos->status, make_pair(0, make_pair(nn_row,nn_col)),now_pos->layer_cnt+1);
                            if(build_node==nullptr)
                                build_node=tem_node;
                            else{
                                tem_node->brother=build_node;
                                build_node=tem_node;
                            }
                        }else{
                            int dd_beg;
                            if(i>=0&&i<=1)dd_beg=2;
                            else dd_beg=0;
                            for(int j=dd_beg;j<dd_beg+2;j++){
                                nn_row=n_row+d_row[j];
                                nn_col=n_col+d_col[j];
                                if(now_pos->status.can_walk(make_pair(n_row,n_col),nn_row,nn_col)){
                                    node* tem_node=new node(now_pos->status, make_pair(0, make_pair(nn_row,nn_col)),now_pos->layer_cnt+1);
                                    if(build_node==nullptr)
                                        build_node=tem_node;
                                    else{
                                        tem_node->brother=build_node;
                                        build_node=tem_node;
                                    }
                                }
                            }
                        }
                    }else{
                        //直接走
                        if(now_pos->status.can_walk(now_pos->status.other_pos,n_row,n_col)){
                            node* tem_node=new node(now_pos->status, make_pair(0, make_pair(n_row,n_col)),now_pos->layer_cnt+1);
                            if(build_node==nullptr)
                                build_node=tem_node;
                            else{
                                tem_node->brother=build_node;
                                build_node=tem_node;
                            }
                        }
                    }
                }
			}

			//todo

			if(((now_pos->layer_cnt+1)%2==0&&now_pos->status.my_board<10)||((now_pos->layer_cnt+1)%2==1&&now_pos->status.other_board<10)){
				//operator 1 place board 垂直
				for(int i=0;i<8;i++)
					for(int j=0;j<8;j++){
						if(now_pos->status.board_statue[i][j]=='n'&&(i-1<0||now_pos->status.board_statue[i-1][j]!='1')&&(i+1>=8||now_pos->status.board_statue[i+1][j]!='1')){
							node* tem_node=new node(now_pos->status, make_pair(1, make_pair(i, j)), now_pos->layer_cnt+1);
						    if(build_node==nullptr)
						        build_node=tem_node;
						    else{
								tem_node->brother=build_node;
								build_node=tem_node;
							}
						}
					}
			
				//operator 2 place board 水平
				for(int i=0;i<8;i++)
					for(int j=0;j<8;j++){
						if(now_pos->status.board_statue[i][j]=='n'&&(j-1<0||now_pos->status.board_statue[i][j-1]!='2')&&(j+1>=8||now_pos->status.board_statue[i][j+1]!='2')){
                            node* tem_node=new node(now_pos->status, make_pair(2, make_pair(i, j)), now_pos->layer_cnt+1);
                            if(build_node==nullptr)
                                build_node=tem_node;
                            else{
                                tem_node->brother=build_node;
                                build_node=tem_node;
                            }
						}
					}
			}


			now_pos->son=build_node;
		}
	}

	double dfs_decide(node* now_pos,int father_layer){
		if(now_pos->layer_cnt!=father_layer+search_limit){
			build_next_layor(now_pos);
			auto ans=dfs_decide(now_pos->son, father_layer);
			double choose_score=ans;
			node* it=now_pos->son->brother;
			if(now_pos->son->layer_cnt%2==0){
				//己方决策
				while(it!=nullptr){
					auto tem_ans=dfs_decide(it, father_layer);
					if(tem_ans>choose_score){
						choose_score=tem_ans;
					}
					it=it->brother;
				}
				return choose_score;
			}else{
				//对方决策
				while(it!=nullptr){
					auto tem_ans=dfs_decide(it, father_layer);
					if(tem_ans<choose_score){
						choose_score=tem_ans;
					}
					it=it->brother;
				}
				return choose_score;
			}
		}else{
			double choose_score=now_pos->score;
			node* it=now_pos->brother;
			if(now_pos->layer_cnt%2==0){
				while(it!=nullptr){
					if(it->score>choose_score){
						choose_score=it->score;
					}
					it=it->brother;
				}
				return choose_score;
			}else{
				while(it!=nullptr){
					if(it->score<choose_score){
						choose_score=it->score;
					}
					it=it->brother;
				}
				return choose_score;
			}
		}
	}

	inline pair<int,pair<int,int>> decide(){
        build_next_layor(root);
        auto it=root->son;
        pair<double,node*> decide_res;
        decide_res.second=nullptr;
        while(it!=nullptr){
            auto tem_ans=dfs_decide(it,root->layer_cnt);
            //std::cerr<<it->add_action.first<<' '<<it->add_action.second.first<<' '<<it->add_action.second.second<<' '<<it->score<<' '<<tem_ans<<std::endl;
            if(decide_res.second==nullptr||decide_res.first<tem_ans){
                decide_res.first=tem_ans;
                decide_res.second=it;
            }
            it=it->brother;
        }
        readIN(decide_res.second->add_action);
		return decide_res.second->add_action;
	}
}decisionTree;

void init() {
	std::ios::sync_with_stdio(false);
	pic.init();
}

std::pair<int, std::pair<int, int> > action(std::pair<int, std::pair<int, int> > loc) {
	decisionTree.readIN(loc);
	auto decision=decisionTree.decide();
	return decision;
}
