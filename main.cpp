#include "AIController.h"
#include <utility>
#include <vector>
#include <queue>
#include <unordered_map>
//#pragma GCC optimize("Ofast")
using std::cerr;
using std::endl;
using std::make_pair;
using std::pair;
using std::queue;
using std::min;
using std::max;
using std::unordered_map;
using std::vector;

#define SEARCH_LAYER 3 //11
#define INFINITY 10000000
#define my_rate (-2)
#define other_rate 1
#define board_row_length 4
#define board_col_length 4
#define board_cnt_rate 3
#define my_board_rate_low 0.5
#define my_board_rate_high 3

extern int ai_side;
std::string ai_name = "Sora Ginko";

class PIC
{
private:
	char board_status[8][8];
public:
	friend class DISSIONTREE;
	int my_board=0,other_board=0;
	pair<int,int> my_pos;
	pair<int,int> other_pos;

	PIC(){
		for(int i=0;i<8;i++)
			for(int j=0;j<8;j++){
				board_status[i][j]='n';
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
	    if(nrow<0||nrow>8||ncol<0||ncol>8)return false;
        if(row<nrow){
            if((col<8&&board_status[row][col]=='2')||(col-1>=0&&board_status[row][col-1]=='2'))
                return false;
        }else if(row>nrow){
            if((col<8&&board_status[row-1][col]=='2')||(col-1>=0&&board_status[row-1][col-1]=='2'))
                return false;
        }else if(col<ncol){
            if((row>0&&board_status[row-1][col]=='1')||(row<8&&board_status[row][col]=='1'))
                return false;
        }else{
            if((row>0&&board_status[row-1][col-1]=='1')||(row<8&&board_status[row][col-1]=='1'))
                return false;
        }
	    return true;
    }

    static bool finish(pair<int,int>pos,bool check_my) {
        if(check_my){
            if(pos.first==((ai_side==0)?0:8))
                return true;
            else return false;
        }else{
            if(pos.first==((ai_side==1)?0:8))
                return true;
            else return false;
        }
	}

	int measureLength(pair<int,int>pos,bool check_my)const{
	    bool field[9][9]={false};
	    queue<pair<int,pair<int,int>>> waitList;
	    waitList.push(make_pair(0,pos));
	    field[pos.first][pos.second]=true;
	    while(!waitList.empty()){
	        auto now=waitList.front();
	        waitList.pop();
	        if(finish(now.second,check_my)){
	            return now.first;
	        }
	        int d_row[]={0,0,-1,1};
	        int d_col[]={-1,1,0,0};
	        for(int i=0;i<4;i++){
	            if(can_walk(now.second,now.second.first+d_row[i],now.second.second+d_col[i])&&(!field[now.second.first+d_row[i]][now.second.second+d_col[i]])){
	                waitList.push(make_pair(now.first+1, make_pair(now.second.first+d_row[i],now.second.second+d_col[i])));
	                field[now.second.first+d_row[i]][now.second.second+d_col[i]]=true;
	            }
	        }
	    }
	    return -1;
	}

	inline int hash(){
	    int zs=100000429;
	    int store=0;
	    for(int i=0;i<8;i++)
	        for(int j=0;j<8;j++)
	            store+=board_status[i][j];
	    int ans=store%zs;
	    ans+=10000019*(my_pos.first+3967*my_pos.second+877*my_board);
	    ans+=other_pos.first+3967*other_pos.second+877*other_board;
	    return ans;
	}
}pic;

class DISSIONTREE{
public:
	static double evaluate(const PIC& cal_pic,bool my_side){
        int my_length=cal_pic.measureLength(cal_pic.my_pos,true);
        int other_length=cal_pic.measureLength(cal_pic.other_pos,false);
        if(my_length==-1)return -1000000;
        if(other_length==0)return -10000;
        if(my_length==0)return 10000;
        else{
            double score=0;
            if(cal_pic.my_board<=cal_pic.other_board){
                score=my_rate*my_length+other_rate*other_length-board_cnt_rate*(cal_pic.my_board*my_board_rate_low-cal_pic.other_board);
            }else{
                score=my_rate*my_length+other_rate*other_length-board_cnt_rate*(cal_pic.my_board*my_board_rate_high-cal_pic.other_board);
            }
            return score;
        }
	}
private:
	static void apply_action(PIC& now_status,pair<int,pair<int,int>>loc,bool is_my){
        if(is_my){
            switch (loc.first){
                case -1:
                    break;
                case 0:
                    now_status.my_pos=loc.second;
                    break;
                case 1:
                    now_status.board_status[loc.second.first][loc.second.second]='1';
                    now_status.my_board++;
                    break;
                case 2:
                    now_status.board_status[loc.second.first][loc.second.second]='2';
                    now_status.my_board++;
                    break;
            }
        }else{
            switch (loc.first){
                case -1:
                    break;
                case 0:
                    now_status.other_pos=loc.second;
                    break;
                case 1:
                    now_status.board_status[loc.second.first][loc.second.second]='1';
                    now_status.other_board++;
                    break;
                case 2:
                    now_status.board_status[loc.second.first][loc.second.second]='2';
                    now_status.other_board++;
                    break;
            }
        }
    }

    void roll_back_action(PIC& now_status,pair<int,pair<int,int>>loc,bool is_my){
        if(is_my){
            switch (loc.first){
                case -1:
                    break;
                case 0:
                    now_status.my_pos=loc.second;
                    break;
                case 1:
                    now_status.board_status[loc.second.first][loc.second.second]='n';
                    now_status.my_board--;
                    break;
                case 2:
                    now_status.board_status[loc.second.first][loc.second.second]='n';
                    now_status.my_board--;
                    break;
            }
        }else{
            switch (loc.first){
                case -1:
                    break;
                case 0:
                    now_status.other_pos=loc.second;
                    break;
                case 1:
                    now_status.board_status[loc.second.first][loc.second.second]='n';
                    now_status.other_board--;
                    break;
                case 2:
                    now_status.board_status[loc.second.first][loc.second.second]='n';
                    now_status.other_board--;
                    break;
            }
        }
    }

	int search_limit=SEARCH_LAYER;
public:
	void readIN(std::pair<int, std::pair<int, int> > loc,bool my_action){
        apply_action(pic,loc, my_action);
	}

	inline vector<pair<int,pair<int,int>>> build_next_layer(PIC& now_status,bool guess_my){
        vector<pair<int,pair<int,int>>> ansList;
        if(guess_my){
            //??????????????????
            int d_row[]={0,0,-1,1};
            int d_col[]={-1,1,0,0};
            for(int i=0;i<4;i++){
                int n_row=now_status.my_pos.first+d_row[i];
                int n_col=now_status.my_pos.second+d_col[i];
                if(n_row==now_status.other_pos.first && n_col==now_status.other_pos.second && now_status.can_walk(now_status.my_pos,n_row,n_col)){
                    //????????????
                    int nn_row=now_status.my_pos.first+2*d_row[i];
                    int nn_col=now_status.my_pos.second+2*d_col[i];
                    if(now_status.can_walk(make_pair(n_row,n_col),nn_row,nn_col)){
                        ansList.emplace_back(0, make_pair(nn_row,nn_col));
                    }else{
                        int dd_beg;
                        if(i>=0&&i<=1)dd_beg=2;
                        else dd_beg=0;
                        for(int j=dd_beg;j<dd_beg+2;j++){
                            nn_row=n_row+d_row[j];
                            nn_col=n_col+d_col[j];
                            if(now_status.can_walk(make_pair(n_row,n_col),nn_row,nn_col)){
                                ansList.emplace_back(0, make_pair(nn_row,nn_col));
                            }
                        }
                    }
                }else{
                    //?????????
                    if(now_status.can_walk(now_status.my_pos,n_row,n_col)){
                        ansList.emplace_back(0, make_pair(n_row,n_col));
                    }
                }
            }
        }else{
            int d_row[]={0,0,-1,1};
            int d_col[]={-1,1,0,0};
            for(int i=0;i<4;i++){
                int n_row=now_status.other_pos.first+d_row[i];
                int n_col=now_status.other_pos.second+d_col[i];
                if(n_row==now_status.my_pos.first && n_col==now_status.my_pos.second && now_status.can_walk(now_status.other_pos,n_row,n_col)){
                    //????????????
                    int nn_row=now_status.other_pos.first+2*d_row[i];
                    int nn_col=now_status.other_pos.second+2*d_col[i];
                    if(now_status.can_walk(make_pair(n_row,n_col),nn_row,nn_col)){
                        ansList.emplace_back(0, make_pair(nn_row,nn_col));
                    }else{
                        int dd_beg;
                        if(i>=0&&i<=1)dd_beg=2;
                        else dd_beg=0;
                        for(int j=dd_beg;j<dd_beg+2;j++){
                            nn_row=n_row+d_row[j];
                            nn_col=n_col+d_col[j];
                            if(now_status.can_walk(make_pair(n_row,n_col),nn_row,nn_col)){
                                ansList.emplace_back(0, make_pair(nn_row,nn_col));
                            }
                        }
                    }
                }else{
                    //?????????
                    if(now_status.can_walk(now_status.other_pos,n_row,n_col)){
                        ansList.emplace_back(0, make_pair(n_row,n_col));
                    }
                }
            }
        }

        if((guess_my&&now_status.my_board<10)||(!guess_my&&now_status.other_board<10)){
            //operator 1 place board ??????
            for(int i=0;i<8;i++)
                for(int j=0;j<8;j++){
                    if((abs(i-now_status.my_pos.first)<=board_row_length&&abs(j-now_status.my_pos.second)<=board_col_length)||(abs(i-now_status.other_pos.first)<=board_row_length&&abs(j-now_status.other_pos.second)<=board_col_length)){
                        if(now_status.board_status[i][j]=='n'&&(i-1<0||now_status.board_status[i-1][j]!='1')&&(i+1>=8||now_status.board_status[i+1][j]!='1')){
                            apply_action(now_status, make_pair(1, make_pair(i, j)),guess_my);
                            if(now_status.measureLength(now_status.other_pos, false)!=-1&&now_status.measureLength(now_status.my_pos, true)!=-1)
                                ansList.emplace_back(1, make_pair(i, j));
                            roll_back_action(now_status, make_pair(1, make_pair(i, j)),guess_my);
                        }
                    }
                }

            //operator 2 place board ??????
            for(int i=0;i<8;i++)
                for(int j=0;j<8;j++){
                    if((abs(i-now_status.my_pos.first)<=board_row_length&&abs(j-now_status.my_pos.second)<=board_col_length)||(abs(i-now_status.other_pos.first)<=board_row_length&&abs(j-now_status.other_pos.second)<=board_col_length)){
                        if(now_status.board_status[i][j]=='n'&&(j-1<0||now_status.board_status[i][j-1]!='2')&&(j+1>=8||now_status.board_status[i][j+1]!='2')){
                            apply_action(now_status, make_pair(2, make_pair(i, j)),guess_my);
                            if(now_status.measureLength(now_status.other_pos, false)!=-1&&now_status.measureLength(now_status.my_pos, true)!=-1)
                                ansList.emplace_back(2, make_pair(i, j));
                            roll_back_action(now_status, make_pair(2, make_pair(i, j)),guess_my);
                        }
                    }
                }
        }
        return ansList;
	}

	pair<double,pair<double,double>> dfs_decide(PIC& now_status,int now_layer,double alpha,double beta,bool next_is_my){
		if(now_layer!=search_limit){
            vector<pair<int,pair<int,int>>> actionList=build_next_layer(now_status,next_is_my);
            double ans=-INFINITY;
			if(next_is_my){
				//????????????
				for(auto act:actionList){
				    auto his_pos=now_status.my_pos;
                    apply_action(now_status,act, true);

                    //for(int kk=0;kk<now_layer;kk++)std::cerr<<' ';
                    //std::cerr<<act.first<<' '<<act.second.first<<' '<<act.second.second<<std::endl;

                    bool finish_flag=false;
                    if(now_status.finish(now_status.my_pos,true))finish_flag=true;
                    pair<double,pair<double,double>> res;
                    if(!finish_flag)res=dfs_decide(now_status,now_layer+1,alpha,beta,!next_is_my);
                    else res=make_pair(10000-now_layer, make_pair(10000-now_layer,10000-now_layer));
                    if(ans==-INFINITY||res.first>ans){
                        ans=res.first;
                    }
                    alpha=max(alpha,max(res.second.first,res.second.second));

                    if(act.first==0)roll_back_action(now_status, make_pair(0,his_pos), true);
                    else roll_back_action(now_status,act, true);
                    if(alpha>=beta)break;
				}
				return make_pair(ans, make_pair(alpha,beta));
			}else{
				//????????????
                for(auto act:actionList){
                    auto his_pos=now_status.other_pos;
                    apply_action(now_status,act, false);

                    //for(int kk=0;kk<now_layer;kk++)std::cerr<<' ';
                    //std::cerr<<act.first<<' '<<act.second.first<<' '<<act.second.second<<std::endl;

                    bool finish_flag=false;
                    if(now_status.finish(now_status.other_pos,false))finish_flag=true;
                    pair<double,pair<double,double>> res;
                    if(!finish_flag)res=dfs_decide(now_status,now_layer+1,alpha,beta,!next_is_my);
                    else res=make_pair(-10000-now_layer, make_pair(-10000-now_layer,-10000-now_layer));
                    if(ans==-INFINITY||res.first<ans){
                        ans=res.first;
                    }
                    beta=min(beta,min(res.second.first,res.second.second));

                    if(act.first==0)roll_back_action(now_status, make_pair(0,his_pos), false);
                    else roll_back_action(now_status,act, false);
                    if(alpha>=beta)break;
                }
                return make_pair(ans, make_pair(alpha,beta));
			}
		}else{
			double score=evaluate(now_status,next_is_my)-now_layer;
            //for(int kk=0;kk<now_layer;kk++)std::cerr<<' ';
            //std::cerr<<"leave "<<score<<std::endl;
			return make_pair(score, make_pair(score,score));
		}
	}

	inline pair<int,pair<int,int>> decide(){
	    //std::cerr<<"===="<<std::endl;
	    double alpha=-INFINITY;
	    double beta=INFINITY;
        vector<pair<int,pair<int,int>>> actionList=build_next_layer(pic,true);
        double ans=-INFINITY;
        pair<int,pair<int,int>> choose_act;
        for(auto act:actionList){
            auto his_pos=pic.my_pos;
            apply_action(pic,act, true);

            //std::cerr<<act.first<<' '<<act.second.first<<' '<<act.second.second<<std::endl;
            if(pic.finish(pic.my_pos,true)){
                choose_act=act;
                return choose_act;
            }
            auto res=dfs_decide(pic,1,alpha,beta, false);
            if(res.first>ans){
                ans=res.first;
                choose_act=act;
            }
            alpha=max(alpha,max(res.second.first,res.second.second));
            //std::cerr<<act.first<<' '<<act.second.first<<' '<<act.second.second<<' '<<res.first<<std::endl;

            if(act.first==0)roll_back_action(pic, make_pair(0,his_pos), true);
            else roll_back_action(pic,act, true);
            //std::cerr<<"!!"<<res.first<<std::endl;
            if(alpha>=beta)break;
        }

        readIN(choose_act, true);
		return choose_act;
	}
}decisionTree;

void init() {
	std::ios::sync_with_stdio(false);
	pic.init();
}

std::pair<int, std::pair<int, int> > action(std::pair<int, std::pair<int, int> > loc) {
    std::cerr<<loc.first<<' '<<loc.second.first<<' '<<loc.second.second<<std::endl;
	decisionTree.readIN(loc, false);
	auto decision=decisionTree.decide();
	return decision;
}
