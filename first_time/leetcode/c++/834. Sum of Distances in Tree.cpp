class Solution {
public:
    vector<int> ans, sz, dp;
    vector<vector<int>> graph;

    //以u为根的树，u节点的和
    void dfs(int u, int f) {
        //初始化当前节点
        sz[u] = 1;
        dp[u] = 0;
        //当前节点的每个子节点求和
        for (auto& v: graph[u]) {
            //表示遍历到了进递归时的那条边了，即进来时u->v,现在到了v->u，要跳过
            if (v == f) {
                continue;
            }
            //dfs先求得子节点的和
            dfs(v, u);
            dp[u] += dp[v] + sz[v];
            sz[u] += sz[v];
        }
    }

    void dfs2(int u, int f) {
        ans[u] = dp[u];
        for (auto& v: graph[u]) {
            if (v == f) {
                continue;
            }
            int pu = dp[u], pv = dp[v];
            int su = sz[u], sv = sz[v];

            dp[u] -= dp[v] + sz[v];
            sz[u] -= sz[v];
            dp[v] += dp[u] + sz[u];
            sz[v] += sz[u];

            dfs2(v, u);

            dp[u] = pu, dp[v] = pv;
            sz[u] = su, sz[v] = sv;
        }
    }

    vector<int> sumOfDistancesInTree(int N, vector<vector<int>>& edges) {
        ans.resize(N, 0);
        sz.resize(N, 0);
        dp.resize(N, 0);
        graph.resize(N, {});
        //得到每一个节点的相连节点
        for (auto& edge: edges) {
            int u = edge[0], v = edge[1];
            graph[u].emplace_back(v);
            graph[v].emplace_back(u);
        }
        //先来一次树形dp，使每个节点获得当前树的对应dp和sz
        dfs(0, -1);
        //dfs换根，对当前节点和每个子节点进行调换，加上或减去节点的贡献
        dfs2(0, -1);
        return ans;
    }
};
//https://leetcode-cn.com/problems/sum-of-distances-in-tree/solution/shu-zhong-ju-chi-zhi-he-by-leetcode-solution/