/* 方法一：二进制枚举 + 哈希
思路与算法

我们可以采取最朴素的思路，即枚举出所有的子序列，然后判断当前的子序列是否是非严格递增的。
那么我们可以用什么办法来枚举所有的子序列呢？我们需要从原序列中选出一些数，来形成新的序列，即原序列的子序列。
对于原序列的每一个数来说，都有两种可能的状态，即被选中或者不被选中。如果我们用 1 代表被选中，0 代表不被选中，假设一个序列长度为 3，那么选出的子序列就对应着下面的八种状态：
[0,0,0]
[0,0,1]
[0,1,0]
[0,1,1]
[1,0,0]
[1,0,1]
[1,1,0]
[1,1,1]
由此可见长度为 nn 的序列选择子序列一共会有 2^n 种情况，这 2^n 中情况就是区间 [0，2^n−1] 的所有整数的二进制表示。
我们可以枚举区间中的每一个数，然后对它做二进制数位拆分，我们会得到一个 0/1 序列，接着可以构造出这个 0/1 序列对应的子序列，然后再检查这个序列是否是非严格递增的。
当然，我们还需要解决子序列去重的问题。对于序列去重，我们可以使用串哈希算法（即 Rabin-Karp 编码，这里不了解的同学可以参考 「官方题解 - 1392. 最长快乐前缀」），
即对于一个序列 a_0, a_1,..., a_{n - 1} ，我们可以认为是一个 max{a_i}+1 进制的数，这个数的数值等于（记 b=max{a_i}+1）：
f(vec(a)) = \sum_{i=0}^{n-1} b^i *a_i
每次我们找到一个合法序列的时候，都去计算这个序列的哈希值，用一个哈希表来记录已有的哈希值，如果该值已经出现在哈希表中，就舍弃这个序列，否则就把这个序列加入到答案中。
在实现的过程中，我们发现这个哈希值可能非常大，我们可以将它模一个大素数 PP，将这个值映射到 \rm intint 的范围内。所以实际上这里的哈希函数是：
f(vec(a)) = \sum_{i=0}^{n-1} b^i *a_i (mod P)
而这里的 b 也未必是 max{a_i}+1，它可以任意选一个大于 max{a_i}+1的数。

假设序列的长度是 nn。

时间复杂度：O(2^n \cdot n)。这里枚举所有子序列的时间代价是 O(2^n)，每次检测序列是否合法和获取哈希值的时间代价都是 O(n)，故渐近时间复杂度为 O(2^n \cdot n)
空间复杂度：O(2^n)。最坏情况下整个序列都是递增的，每个长度大于等于 2 的子序列都要加入答案，这里哈希表中要加入 2^n 个元素，空间代价为 O(2^n)；
用一个临时的数组来存当前答案，空间代价为 O(n) */


class Solution {
public:
    vector<int> temp;
    vector<vector<int>> ans;
    unordered_set<int> s;
    int n;

    void findSubsequences(int mask, vector<int>& nums) {
        temp.clear();
        for (int i = 0; i < n; ++i) {
            if (mask & 1) {
                temp.push_back(nums[i]);
            }
            mask >>= 1;
        }
    }

    bool check() {
        for (int i = 1; i < temp.size(); ++i) {
            if (temp[i] < temp[i - 1]) {
                return false;
            }
        }
        return temp.size() >= 2;
    }

    int getHash(int base, int mod) {
        int hashValue = 0;
        for (const auto &x: temp) {
            hashValue = 1LL * hashValue * base % mod + (x + 101);
            hashValue %= mod;
        }
        return hashValue;
    }

    vector<vector<int>> findSubsequences(vector<int>& nums) {
        n = nums.size();
        for (int i = 0; i < (1 << n); ++i) {
            findSubsequences(i, nums);
            int hashValue = getHash(263, int(1E9) + 7);
            if (check() && s.find(hashValue) == s.end()) {
                ans.push_back(temp);
                s.insert(hashValue);
            }
        }
        return ans;
    }
};
/* 
方法二：递归枚举 + 减枝
思路与算法

我们也可以用递归的方法实现二进制枚举，像「方法一」那样枚举出所有的子序列，然后判断是否合法。直接把方法一变成递归形式，我们可以得到这样的代码： */
vector<vector<int>> ans;
vector<int> temp;
void dfs(int cur, vector<int>& nums) {
    if (cur == nums.size()) {
        // 判断是否合法，如果合法判断是否重复，将满足条件的加入答案
        if (isValid() && notVisited()) {
            ans.push_back(temp);
        }
        return;
    }
    // 如果选择当前元素
    temp.push_back(nums[cur]);
    dfs(cur + 1, nums);
    temp.pop_back();
    // 如果不选择当前元素
    dfs(cur + 1, nums);
}

/* 这是一个递归枚举子序列的通用模板，即用一个临时数组 temp 来保存当前选出的子序列，使用 cur 来表示当前位置的下标，在 dfs(cur, nums) 开始之前，
[0,cur−1] 这个区间内的所有元素都已经被考虑过，而 [cur,n] 这个区间内的元素还未被考虑。
在执行 dfs(cur, nums) 时，我们考虑cur 这个位置选或者不选，如果选择当前元素，那么把当前元素加入到 temp 中，然后递归下一个位置，在递归结束后，应当把 temp 的最后一个元素删除进行回溯；
如果不选当前的元素，直接递归下一个位置。

当然，如果我们简单地这样枚举，对于每一个子序列，我们还需要做一次 O(n) 的合法性检查和哈希判重复，
在执行整个程序的过程中，我们还需要使用一个空间代价 O(2^n)的哈希表来维护已经出现的子序列的哈希值。
我们可以对选择和不选择做一些简单的限定，就可以让枚举出来的都是合法的并且不重复：

1. 使序列合法的办法非常简单，即给「选择」做一个限定条件，只有当前的元素大于等于上一个选择的元素的时候才能选择这个元素，这样枚举出来的所有元素都是合法的

2. 那如何保证没有重复呢？我们需要给「不选择」做一个限定条件，只有当当前的元素不等于上一个选择的元素的时候，才考虑不选择当前元素，直接递归后面的元素。
   因为如果有两个相同的元素，我们会考虑这样四种情况：
   1)前者被选择，后者被选择
   2)前者被选择，后者不被选择
   3)前者不被选择，后者被选择
   4)前者不被选择，后者不被选择
   其中第二种情况和第三种情况其实是等价的，我们这样限制之后，舍弃了第二种，保留了第三种，于是达到了去重的目的。

时间复杂度：O(2^n \cdot n)。仍然需要对子序列做二进制枚举，枚举出的序列虽然省去了判断合法性和哈希的过程，但是仍然需要 O(n) 的时间添加到答案中。
空间复杂度：O(n)。这里临时数组的空间代价是 O(n)，递归使用的栈空间的空间代价也是 O(n)。 */

class Solution {
public:
    vector<int> temp; 
    vector<vector<int>> ans;

    void dfs(int cur, int last, vector<int>& nums) {
        if (cur == nums.size()) {
            if (temp.size() >= 2) {
                ans.push_back(temp);
            }
            return;
        }
        if (nums[cur] >= last) {
            temp.push_back(nums[cur]);
            dfs(cur + 1, nums[cur], nums);
            temp.pop_back();
        }
        //因为只有2）、3）种情况会产生重复，和last相等时不考虑当前元素直接跳过的话就相当于删去了第二种情况，于是去重
        if (nums[cur] != last) {
            dfs(cur + 1, last, nums);
        }
    }

    vector<vector<int>> findSubsequences(vector<int>& nums) {
        dfs(0, INT_MIN, nums);
        return ans;
    }
};


/* 另一种判重方法：
考虑一下去重问题，我使用了 O(n) 的判重方法：

设 stack 中最后一个值的位置为 last。如果 stack 为空，则 last = -1。
设当前正在处理的位置为 pos。
如果在 nums 的子区间 [last+1, pos) 中，存在和 nums[pos] 相同的值，则当前 nums[pos] 必须丢弃，不然会产生重复的子序列。
链接：https://leetcode-cn.com/problems/increasing-subsequences/solution/di-gui-gao-ding-liu-xing-dai-ma-pan-zhong-by-time-/ */

class Solution {
public:
    // 判重代码；
    bool is_first(const vector<int> &num, int last, int pos) {
        for(int i = last+1; i < pos; i++) {
            if(num[i] == num[pos]) {
                return false;
            }
        }
        return true;
    }
    void dfs(const vector<int> &nums, int last, int pos, vector<int> &stack, vector<vector<int>> &anw) {
        if(nums.size() == pos) { return; } //到达末尾，直接范围吧
        // 检查 nums[pos] 是否符合要求
        if((stack.empty() || nums[pos] >= stack.back()) && is_first(nums, last, pos)) {
            stack.push_back(nums[pos]);
            if(stack.size() >= 2) { //大于 2 了，那就放进去吧
                anw.push_back(stack);
            } 
            dfs(nums, pos, pos+1, stack, anw); // 继续处理下一个。
            stack.pop_back(); // 将当前放入这个吐出来。
        }
        dfs(nums, last, pos+1, stack, anw);
    }
    vector<vector<int>> findSubsequences(vector<int>& nums) {
        vector<vector<int>> anw;
        vector<int> stack;
        dfs(nums, -1, 0, stack, anw);
        return anw;
    }
};






