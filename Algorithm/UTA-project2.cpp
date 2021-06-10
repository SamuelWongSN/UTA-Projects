#include <iostream>
#include <vector>
#include <cstring>
using namespace std;

string longest_Common_Subsequence(string text1, string text2) {
    //保存两个字符串的长度
    int m = text1.size();
    int n = text2.size();
    if(!m || !n)
    return "";
    //保存公共子序列长度的矩阵
    //设置为m+1、n+1是为了计算第一行的空集
    int dp[m+1][n+1][2];
    //初始化矩阵
    memset(dp,0,sizeof(dp));
    // 对角1 左2 右边3
    //遍历两个字符串
    for(int i=1;i<=m;i++){
        for(int j=1;j<=n;j++){
            //如果字符串相同，则该位置更新为对角的前一个位置+1
            if(text1[i-1] == text2[j-1])
            {
                dp[i][j][0] = dp[i-1][j-1][0]+1;
                dp[i][j][1] = 1;
            }
                
            //如果字符不相同，则该位置置为相邻得上一个或左侧中较大的一个数
            else
            {
                dp[i][j][0] = max(dp[i][j-1][0],dp[i-1][j][0]);
                dp[i][j][1] = dp[i][j-1][0] < dp[i-1][j][0] ? 2 : 3;
            }
        }
    }
    string result = "";
    for(int i = m, j = n; i > 0 && j > 0;)
    {
        if(1 == dp[i][j][1]){
            result =  string(1,text1[i-1]) + result;
            i--;j--;
        }
        else if(2 == dp[i][j][1])
        {
            i--;
        }
        else if(3 == dp[i][j][1])
        {
            j--;
        }
    }
    return result;
    
//    //读取共同子序列
//     int len = dp[m][n];
//     int i = m,j = n;
//     vector<char> result(len);

//     while(len){
//         if(text1[i-1] == text2[j-1]){
//             result[len-1] = text1[i-1];
//             i--;
//             j--;
//             len--;
//         }
//         else if(dp[i-1][j] == len && text1[i-1-1] == text2[j-1]){
//             i = i - 1;
//             result[len-1] = text2[j-1];
//             i--;
//             j--;
//             len--;
//         }
//         else if(dp[i][j-1] == len && text1[i-1] == text2[j-1-1]){
//             j = j - 1;
//             result[len-1] = text1[i-1];
//             i--;
//             j--;
//             len--;
//         }
//         else{
//             if(dp[i-1][j] == len){
//                 i = i - 1;
//             }
//             else if(dp[i][j-1] == len){
//                 j = j - 1;
//             }
//         }
//     }


//     // //返回公共子序列
//     // return result;
//     string ans;
//     ans.assign(result.begin(),result.end());
//     return ans;
}

int main() {
    int len1 = 0, len2 = 0;
    string s1, s2;
    cout << "please input the length of first string:";
    cin >> len1;
    cout << "please input the first string:";
    cin >> s1;

    cout << "please input the length of second string:";
    cin >> len2;
    cout << "please input the second string:";
    cin >> s2;

    string result = longest_Common_Subsequence(s1,s2);
    cout << endl <<"---------------------------------------"<<endl;
    cout << "the length of LCS: "<<result.size()<<endl;
    cout << "LCS: "<< result <<endl;

    getchar();
	return 0;
}