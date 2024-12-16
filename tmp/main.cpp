//// 走迷宫
//
//#include <iostream>
//#include <algorithm>
//#include <vector>
//#include <cmath>
//#include <functional>
//#include <climits>
//#include <cstring>
//#include <unordered_map>
//#include <unordered_set>
//#include <map>
//#include <set>
//#include <queue>
//#include <stack>
//#include <deque>
//
//#define quickio ios::sync_with_stdio(false),cin.tie(0),cout.tie(0);
//#define endl "\n"
//
//using namespace std;
//typedef long long ll;
//
//const int N = 1e3, M = 1e3;
//char board[N][M];
//char book[N][M];
//int dx[4] = { 0, 0, 1, -1 };
//int dy[4] = { 1, -1, 0, 0 };
//int n, m, xs, xt, ys, yt;
//
//int bfs()
//{
//    queue<pair<int, int>>q;
//    q.push({ xs, ys });
//    book[xs][ys] = 1;
//    int ans = -1;
//    while (q.size())
//    {
//        int sz = q.size();
//        ans++;
//        while (sz--)
//        {
//            //auto& [x, y] = q.front();
//            int x = q.front().first, y = q.front().second;
//            q.pop();
//            //book[x][y] = 0;
//            if (x == xt && y == yt)
//                return ans;
//            int a, b;
//            for (int i = 0; i < 4; i++)
//            {
//                a = x + dx[i];
//                b = y + dy[i];
//
//                if (a >= 0 && a < n && b >= 0 && b < m && !book[a][b] && board[a][b] == '.')
//                {
//                    if (a == xt && b == yt)
//                        return ans + 1;
//                    q.push({ a, b });
//                    book[a][b] = 1;
//                }
//            }
//        }
//    }
//    return -1;
//}
//
//int main()
//{
//    cin >> n >> m;
//    cin >> xs >> ys >> xt >> yt;
//    xs--, ys--, xt--, yt--;
//    for (int i = 0; i < n; i++)
//        for (int j = 0; j < m; j++)
//            cin >> board[i][j];
//
//    cout << bfs();
//    return 0;
//}


/*没写出来*/
// 主持人调度（二）
// https://www.nowcoder.com/practice/4edf6e6d01554870a12f218c94e8a299?tpId=196&tqId=37562&ru=/exam/oj

// 贪心 --> 感觉和最少箭数射气球一样
// 先按左边排序，再找每段重合的最小右端点，看有多少条线重合

#include <iostream>

int main()
{
    printf("%d",'\n');
    return 0;
}