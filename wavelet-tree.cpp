#include<vector>
#include<algorithm>
#include <iostream>
#include "bitmap.hpp"
#include "wavelet-tree.hpp"
using namespace std;

// vector<Bitmap> r0; int n, s;
// vector<vector<int>> r0; int n, s;

template<class Bitmap>
void WaveTree<Bitmap>::build(iter b, iter e, int l, int r, int u) {
  if (l == r)
    return;
  int m = (l+r)/2;
  // r0[u].reserve(e-b+1); r0[u].push_back(0);
  // for (iter it = b; it != e; ++it)
  //   r0[u].push_back(r0[u].back() + (*it<=m));

  r0[u].resize(e-b+1);
  int i; iter it;
  for (it = b, i=0; it !=e; ++it, ++i) {
    r0[u].set(i, *it<=m);
  }
  r0[u].build_rank();

  iter p = stable_partition(b, e, [=](int i){
      return i<=m;});

  build(b, p, l, m, u*2);
  build(p, e, m+1, r, u*2+1);
}

template<class Bitmap>
int WaveTree<Bitmap>::memory() {
  int mem = 0;
  mem += 2*sizeof(int);

  mem += sizeof(vector<Bitmap>);
  for (int i = 0; i < (int)r0.size(); ++i)
    mem += r0[i].memory();
  return mem;
}

template<class Bitmap>
WaveTree<Bitmap>::WaveTree(vector<int> &arr, int sigma) {
  n = arr.size(); s = sigma;
  r0.resize(s*2);
  build(arr.begin(), arr.end(), 0, s-1, 1);
}

//k in [1,n], [i,j] is 0-indexed, -1 if error
template<class Bitmap>
int WaveTree<Bitmap>::quantile(int k, int i, int j) {
  j++;
  // if (k < 1 or k > j-i)
  //   return -1;
  int l = 0, r = s-1, u = 1, m, ri, rj;
  while (l != r) {
    m = (l+r)/2;
    // ri = r0[u][i]; rj = r0[u][j]; u*=2;
    ri = r0[u].rank1(i-1); rj = r0[u].rank1(j-1); u*=2;
    if (k <= rj-ri)
      i = ri, j = rj, r = m;
    else
      k -= rj-ri, i -= ri, j -= rj,
        l = m+1, ++u;
  }
  return r;
}

//counts numbers in [a,b] in positions [i,j]
template<class Bitmap>
int WaveTree<Bitmap>::range(int i, int j, int a, int b) {
  if (b < a or j < i)
    return 0;
  q = a; w = b;
  return range(i, j+1, 0, s-1, 1);
}

template<class Bitmap>
int WaveTree<Bitmap>::range(int i, int j, int a, int b, int u) {
  if (b < q or w < a)
    return 0;
  if (q <= a and b <= w)
    return j-i;
  // int m = (a+b)/2, ri = r0[u][i], rj = r0[u][j];
  int m = (a+b)/2, ri = r0[u].rank1(i-1), rj = r0[u].rank1(j-1);
  return range(ri, rj, a, m, u*2) +
    range(i-ri, j-rj, m+1, b, u*2+1);
}

//count occurrences of x in positions [0,i]
template<class Bitmap>
int WaveTree<Bitmap>::rank(int x, int i) {
  i++;
  int l = 0, r = s-1, u = 1, m, z;
  while (l != r) {
    m = (l+r)/2;
    // z = r0[u][i]; u*=2;
    z = r0[u].rank1(i-1); u*=2;
    if (x <= m)
      i = z, r = m;
    else
      i -= z, l = m+1, ++u;
  }
  return i;
}
template class WaveTree<BitmapRank>;
template class WaveTree<BitmapRankVec>;
