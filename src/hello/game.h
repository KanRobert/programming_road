#include<string>            
#include<cstdio>
using std::string;
using std::printf;

void Game(const string &question){
    if(question.size()<6) printf("我不回答这种弱智问题。");
    else if(*question.rbegin()!='?' && *question.rbegin()!='~') printf("请端正你
的态度。");
    else printf("随缘。");
}
